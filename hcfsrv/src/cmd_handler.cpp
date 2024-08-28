#include <linux/limits.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>

#include "hcf/core/logger.h"
#include "hcf/core/utils.h"

#include "hcf/imm/imm_config_class.h"

#include "protocol_handler.h"
#include "protocol_primitives.h"
#include "imm_object_hcrule.h"
#include "imm_object_hcrule_parameter.h"
#include "configurator.h"

#include "hcf/trace/com_ericsson_common_hcf_srv_cmdhandler.h"
#include "hcf/trace/tracing_macros.h"

#include "cmd_handler.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_SRV_CMD_HANDLER__

// Extern objects
extern TSL_NS_HCF_IMM::imm_config_class<HCF_NS_HCF_SRV::imm_object_hcrule> hcrule_class_handler;
extern TSL_NS_HCF_IMM::imm_config_class<HCF_NS_HCF_SRV::imm_object_hcrule_parameter> hcrule_parameter_class_handler;

HCF_NS_HCF_SRV_BEGIN

HCF_CC_FUNCTION_TRACER_CLASS_DEFINE

int __TSL_CLASS_NAME__::handle_input () {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_LOG(LEVEL_NOTICE, "A new connection is available for the Command Handler, dispatching it!");

	// First, accept the incoming connection
	int session_fd = -1;
	HCF_CC_TRACE(HandleInput, "Accepting connection on the command handler socket!", -1, "");
	if ((session_fd = ::accept(_server_socket_fd, 0, 0)) < 0) {
		const int errno_save = errno;
		HCF_CC_TRACE(HandleInputErr, "Call 'accept' failed.", errno_save, ::strerror(errno_save));
		HCF_CC_LOG(LEVEL_ERROR, "Call 'accept' failed, errno == %d", errno_save);
		return 0;
	}
	HCF_CC_TRACE(HandleInput, "Connection accepted, reading the primitive!", -1, "");

	// Then receive the protocol primitive from the command
	TSL_NS_HCF_INTERNALAPI::protocol_handler prot_handler(session_fd);
	if (prot_handler.recv_primitive() < 0) {
		// Failed to receive the primitive from the HCF server
		HCF_CC_TRACE(HandleInputErr, "Call 'prot_handler.recv_primitive' failed.", prot_handler.error(), prot_handler.error_text());
		HCF_CC_LOG(LEVEL_ERROR, "Failed to receive the request primitive: error == %d, error_message == '%s'", prot_handler.error(), prot_handler.error_text());
		::close(session_fd);
		return 0;
	}
	HCF_CC_TRACE(HandleInput, "Primitive received, unpacking it!", -1, "");

	// After receiving the primitive, unpack it to extract the received data
	uint32_t request_primitive;
	char primitive_msg[4096] = {0};
	if (prot_handler.unpack_primitive(request_primitive, primitive_msg, TSL_ARRAY_SIZE(primitive_msg)) < 0) {
		// Failed to unpack the received primitive
		HCF_CC_TRACE(HandleInputErr, "Call 'prot_handler.unpack_primitive' failed.", prot_handler.error(), prot_handler.error_text());
		HCF_CC_LOG(LEVEL_ERROR, "Failed to unpack the request primitive: error == %d, error_message == '%s'", prot_handler.error(), prot_handler.error_text());
		::close(session_fd);
		return 0;
	}
	HCF_CC_TRACE(HandleInput, "Primitive correctly unpacked, handling it!", request_primitive, primitive_msg);
	HCF_CC_LOG(LEVEL_NOTICE, "Received the the primitive having ID == %u, handling it!", request_primitive);
	int call_result = 0;
	uint32_t reply_primitive;

	// Handle the primitive according to its ID
	switch (request_primitive) {

	case TSL_NS_HCF_INTERNALAPI::HCF_RULESET_FILE_LIST_REQUEST_ID:
		call_result = handle_listing_primitive(reply_primitive, primitive_msg, TSL_ARRAY_SIZE(primitive_msg));
		break;

	case TSL_NS_HCF_INTERNALAPI::HCF_RULESET_FILE_VALIDATE_REQUEST_ID:
		call_result = handle_validation_primitive(reply_primitive, primitive_msg, TSL_ARRAY_SIZE(primitive_msg));
		break;

	case TSL_NS_HCF_INTERNALAPI::HCF_RULESET_FILE_INSTALL_REQUEST_ID:
		call_result = handle_install_primitive(reply_primitive, primitive_msg, TSL_ARRAY_SIZE(primitive_msg));
		break;

	case TSL_NS_HCF_INTERNALAPI::HCF_RULESET_FILE_DELETE_REQUEST_ID:
		call_result = handle_delete_primitive(reply_primitive, primitive_msg, TSL_ARRAY_SIZE(primitive_msg));
		break;

	case TSL_NS_HCF_INTERNALAPI::HCF_RULESET_FILE_UPGRADE_REQUEST_ID:
		call_result = handle_update_primitive(reply_primitive, primitive_msg, TSL_ARRAY_SIZE(primitive_msg));
		break;

	default:
		HCF_CC_TRACE(HandleInput, "WARNING: unsupported primitive received!", request_primitive, primitive_msg);
		HCF_CC_LOG(LEVEL_WARNING, "ERROR: Received an unexpected primitive (with id %u)!", request_primitive);
		::close(session_fd);
		return 0;
	}
	HCF_CC_TRACE(HandleInput, "Primitive handling returned:", request_primitive, ((call_result) ? "FAILURE" : "SUCCESS"));
	HCF_CC_LOG(LEVEL_NOTICE, "The handling of the primitive having ID == %u was %s (error_code == %d)!",
			request_primitive, (call_result) ? "FAILED" : "SUCCESSFUL", call_result);

	// After handling the primitive, send back the response to the command: first, build the primitive
	HCF_CC_TRACE(HandleInput, "Primitive correctly handled, making response primitive!", reply_primitive, primitive_msg);
	if (prot_handler.make_primitive(reply_primitive, ::strlen(primitive_msg), primitive_msg)) {
		// Failed to build the primitive with the given parameters
		HCF_CC_TRACE(HandleInputErr, "Call 'prot_handler.make_primitive' failed.", prot_handler.error(), prot_handler.error_text());
		HCF_CC_LOG(LEVEL_ERROR, "Failed to build the response primitive: error == %d, error_message == '%s'", prot_handler.error(), prot_handler.error_text());
		::close(session_fd);
		return 0;
	}
	HCF_CC_TRACE(HandleInput, "Primitive correctly build, sending it!", reply_primitive, primitive_msg);

	// After the primitive has been correctly built, sent it to the command
	if (prot_handler.send_primitive()) {
		// Failed to send the primitive to the command
		HCF_CC_TRACE(HandleInputErr, "Call 'prot_handler.send_primitive' failed.", prot_handler.error(), prot_handler.error_text());
		HCF_CC_LOG(LEVEL_ERROR, "Failed to send the reply primitive: error == %d, error_message == '%s'", prot_handler.error(), prot_handler.error_text());
	}

	// Close the session file descriptor and return
	::close(session_fd);
	return 0;
}

int __TSL_CLASS_NAME__::handle_output () {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(HandleOutput, "Received an output I/O event for the Command Handler, it "
			"shouldn't occur, removing the Event Handler from the reactor");
	HCF_CC_LOG(LEVEL_WARNING, "Received an output I/O event for the Command Handler, it "
			"shouldn't occur, removing the Event Handler from the reactor");
	return -1;
}

int __TSL_CLASS_NAME__::handle_exception (bool error_flag, bool hungup_flag, bool read_hungup_flag) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(HandleException, "Received an error on the Command Handler I/O handle, removing the Event Handler "
			"from the reactor: error_flag == %s, hungup_flag == %s, read_hungup_flag == %s",
			(error_flag ? "TRUE" : "FALSE"), (hungup_flag ? "TRUE" : "FALSE"), (read_hungup_flag ? "TRUE" : "FALSE"));
	HCF_CC_LOG(LEVEL_WARNING, "Received an error on the Command Handler I/O handle, removing the Event Handler "
			"from the reactor: error_flag == %s, hungup_flag == %s, read_hungup_flag == %s",
			(error_flag ? "TRUE" : "FALSE"), (hungup_flag ? "TRUE" : "FALSE"), (read_hungup_flag ? "TRUE" : "FALSE"));
	return -1;
}

int __TSL_CLASS_NAME__::handle_close () {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(HandleClose, "Removing the Command Handler from the reactor!");
	HCF_CC_LOG(LEVEL_NOTICE, "Removing the Command Handler from the reactor!");
	return 0;
}

int __TSL_CLASS_NAME__::initialize () {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_LOG(LEVEL_NOTICE, "Initializing the Command Handler object!");

	// Create the hcrsfm command server socket
	HCF_CC_TRACE(Initialize, "Received a request to initialize the Command Handler object, creating the socket!", "");
	if ((_server_socket_fd = ::socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		// Failed to create the socket to contact HCF command
		const int errno_save = errno;
		HCF_CC_TRACE(InitializeErr, "Call 'socket' failed.", errno_save);
		HCF_CC_LOG(LEVEL_ERROR, "Failed to create the socket for command handling (error == %d)", errno_save);
		return ERR_CMD_HANDLER_INITIALIZE_FAILED;
	}
	HCF_CC_TRACE(Initialize, "Retrieving the name of the current host!", "");

	// Retrieve the name of the current host
	size_t hostname_len = HOST_NAME_MAX;
	char hostname[hostname_len];
	if (configurator::get_current_hostname(hostname, hostname_len)) {
		HCF_CC_TRACE(InitializeErr, "Call 'configurator::get_current_hostname' failed.", ERR_GET_CONFIGURATION_INFO_FAILED);
		HCF_CC_LOG(LEVEL_ERROR, "Call 'configurator::get_current_hostname' failed!");
		::close(_server_socket_fd);
		return ERR_GET_CONFIGURATION_INFO_FAILED;
	}
	HCF_CC_TRACE(Initialize, "Retrieving the IP address for the following host:", hostname);

	// Retrieve the IP address associated to the retrieved host name
	struct sockaddr_in server_addr;
	if (configurator::get_addr_by_hostname(hostname, (struct sockaddr *)(&server_addr))) {
		HCF_CC_TRACE(InitializeErr, "Call 'configurator::get_addr_by_hostname' failed.", ERR_GET_CONFIGURATION_INFO_FAILED);
		HCF_CC_LOG(LEVEL_ERROR, "Call 'configurator::get_addr_by_hostname' failed for hostname == %s", hostname);
		::close(_server_socket_fd);
		return ERR_GET_CONFIGURATION_INFO_FAILED;
	}
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = 0;

	// Bind the created socket to the retrieved address. Let the kernel to choose a port
	HCF_CC_TRACE(Initialize, "Binding the address for the following host:", hostname);
	if (::bind(_server_socket_fd, (struct sockaddr *)(&server_addr), sizeof(server_addr))) {
		const int errno_save = errno;
		HCF_CC_TRACE(InitializeErr, "Call 'bind' failed.", errno_save);
		HCF_CC_LOG(LEVEL_ERROR, "Call 'bind' failed, errno == %d", errno_save);
		::close(_server_socket_fd);
		return ERR_CMD_HANDLER_INITIALIZE_FAILED;
	}
	HCF_CC_TRACE(Initialize, "Retrieving the port number to which the host was bind.", hostname);

	// Query the socket in order to get the port number assigned by the kernel
	socklen_t sock_len = sizeof(server_addr);
	errno = 0;
	if (::getsockname(_server_socket_fd, (struct sockaddr *)(&server_addr), &sock_len)) {
		const int errno_save = errno;
		HCF_CC_TRACE(InitializeErr, "Call 'getsockname' failed.", errno_save);
		HCF_CC_LOG(LEVEL_ERROR, "Call 'getsockname' failed, errno == %d", errno_save);
		::close(_server_socket_fd);
		return ERR_CMD_HANDLER_INITIALIZE_FAILED;
	}

	// Build the path of the file containing the server address and open it
	std::string file_path = TSL_NS_HCF_INTERNALAPI::internalapi_constants::get_cmd_server_address_file_path();
	HCF_CC_TRACE(Initialize, "Opening the Command Handler configuration file:", file_path.c_str());

	int cmd_server_address_file_fd = -1;
	if ((cmd_server_address_file_fd = ::open(file_path.c_str(), O_WRONLY | O_CREAT, 0640)) < 0) {
		const int errno_save = errno;
		HCF_CC_TRACE(InitializeErr, "Call 'open' failed.", errno_save);
		HCF_CC_LOG(LEVEL_ERROR, "Call 'open' failed for file '%s', errno == %d", file_path.c_str(), errno_save);
		::close(_server_socket_fd);
		return ERR_CMD_HANDLER_INITIALIZE_FAILED;
	}

	// Make the IP address as a string
	char ip[16] = {0};
	in_addr addr;
	addr.s_addr = server_addr.sin_addr.s_addr;
	::inet_ntop(AF_INET, &addr, ip, TSL_ARRAY_SIZE(ip));

	// Make the buffer to write into the file
	char buffer[128];
	const int port = ntohs(server_addr.sin_port);
	::snprintf(buffer, TSL_ARRAY_SIZE(buffer), "%s:%d\n", ip, port);
	HCF_CC_LOG(LEVEL_INFO, "The command handler object is listening the host '%s', on address '%s::%d'!", hostname, ip, port);
	HCF_CC_TRACE(Initialize, "The command handler is listening on the current host on the following address:", buffer);

	// Write into the opened file the prepared buffer
	ssize_t bytes_to_write = ::strlen(buffer);
	ssize_t bytes_written = -1;
	if ((bytes_written = ::write(cmd_server_address_file_fd, buffer, bytes_to_write)) < bytes_to_write) {
		const int errno_save = errno;
		HCF_CC_TRACE(InitializeErr, "Call 'write' failed.", errno_save);
		HCF_CC_LOG(LEVEL_ERROR, "Call 'write' failed for file '%s', bytes_written == %lu, bytes_to_write == %lu, errno == %d",
				file_path.c_str(), bytes_written, bytes_to_write, errno_save);
		::close(cmd_server_address_file_fd);
		::close(_server_socket_fd);
		return ERR_CMD_HANDLER_INITIALIZE_FAILED;
	}
	::close(cmd_server_address_file_fd);

	// Listen on the created socket in order to accept incoming connections
	HCF_CC_TRACE(Initialize, "Opening the socket for listening in order to accept new connections!", hostname);
	if (::listen(_server_socket_fd, 10)) {
		const int errno_save = errno;
		HCF_CC_TRACE(InitializeErr, "Call 'listen' failed.", errno_save);
		HCF_CC_LOG(LEVEL_ERROR, "Generic error: Failed to enable the socket to accept connections for command handling (error == %d)", errno_save);
		return ERR_CMD_HANDLER_INITIALIZE_FAILED;
	}

	return ERR_NO_ERROR;
}

int __TSL_CLASS_NAME__::finalize () {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(Finalize, "Received a request to finalize the Command Handler object!");
	HCF_CC_LOG(LEVEL_NOTICE, "Finalizing the Command Handler object!");
	::close(_server_socket_fd);
	return ERR_NO_ERROR;
}

int __TSL_CLASS_NAME__::handle_startup_operations () {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_LOG(LEVEL_NOTICE, "Performing the startup operations for the Command Handler object!");

	// FIRST: Get the list of the the already installed RSFs and re-install them
	std::set<std::string> list_rsf_to_install;
	std::string rsf_installing_path = TSL_NS_HCF_INTERNALAPI::internalapi_constants::get_rsf_installed_path() + std::string("/");
	HCF_CC_TRACE(HandleStartupOperations, "Retrieving the list of the RSFs already installed in the following folder:", rsf_installing_path.c_str());
	if (TSL_NS_HCF_CORE::utils::list(rsf_installing_path.c_str(), list_rsf_to_install)){
		HCF_CC_TRACE(HandleStartupOperationsErr, "Call 'utils::list' failed.", ERR_FILE_SYSTEM_OPERATION_FAILED, "");
		HCF_CC_LOG(LEVEL_ERROR, "Failed to retrieve the list of the installed RSF from '%s'", rsf_installing_path.c_str());
		return ERR_FILE_SYSTEM_OPERATION_FAILED;
	}

	// For each retrieved RSF, re-install it
	uint32_t fake_primitive;
	char fake_primitive_message[PATH_MAX + 1] = {0};

	for (std::set<std::string>::iterator it = list_rsf_to_install.begin(); it != list_rsf_to_install.end(); ++it) {
		::snprintf(fake_primitive_message, TSL_ARRAY_SIZE(fake_primitive_message), "%s", std::string(rsf_installing_path + (*it)).c_str());
		HCF_CC_TRACE(HandleStartupOperations, "Reinstalling the following RSF:", fake_primitive_message);

		if (handle_install_primitive(fake_primitive, fake_primitive_message, TSL_ARRAY_SIZE(fake_primitive_message), false)) {
			HCF_CC_TRACE(HandleStartupOperationsErr, "Call 'handle_install_primitive' failed.", -1, fake_primitive_message);
			HCF_CC_LOG(LEVEL_ERROR, "Failed to install the Rule Set File '%s', error_message == '%s'",
					std::string(rsf_installing_path + (*it)).c_str(), fake_primitive_message);
		}
		::memset(fake_primitive_message, 0, TSL_ARRAY_SIZE(fake_primitive_message));
	}

	// SECOND: Get the list of the RSFs that are scheduled to be installed and install them
	list_rsf_to_install.clear();
	rsf_installing_path = TSL_NS_HCF_INTERNALAPI::internalapi_constants::get_rsf_installing_path() + std::string("/");
	HCF_CC_TRACE(HandleStartupOperations, "Retrieving the list of the RSFs to be installed in the following folder:", rsf_installing_path.c_str());
	if (TSL_NS_HCF_CORE::utils::list(rsf_installing_path.c_str(), list_rsf_to_install)) {
		HCF_CC_TRACE(HandleStartupOperationsErr, "Call 'utils::list' failed.", ERR_FILE_SYSTEM_OPERATION_FAILED, "");
		HCF_CC_LOG(LEVEL_ERROR, "Failed to retrieve the list of the RSF ready to install");
		return ERR_FILE_SYSTEM_OPERATION_FAILED;
	}

	for (std::set<std::string>::iterator it = list_rsf_to_install.begin(); it != list_rsf_to_install.end(); ++it) {
		::snprintf(fake_primitive_message, TSL_ARRAY_SIZE(fake_primitive_message), "%s", std::string(rsf_installing_path + (*it)).c_str());

		// Understand if the file must be installed or updated. A RSF to be updated has the prefix 'u_'
		if (it->find("u_") == std::string::npos){
			HCF_CC_TRACE(HandleStartupOperations, "Installing the following RSF:", fake_primitive_message);
			if (handle_install_primitive(fake_primitive, fake_primitive_message, TSL_ARRAY_SIZE(fake_primitive_message))) {
				HCF_CC_TRACE(HandleStartupOperationsErr, "Call 'handle_install_primitive' failed.", -1, fake_primitive_message);
				HCF_CC_LOG(LEVEL_ERROR, "Failed to install the Rule Set File '%s', error_message == '%s'",
						std::string(rsf_installing_path + (*it)).c_str(), fake_primitive_message);
			}
		}
		else {
			HCF_CC_TRACE(HandleStartupOperations, "Updating the following RSF:", fake_primitive_message);
			if (handle_update_primitive(fake_primitive, fake_primitive_message, TSL_ARRAY_SIZE(fake_primitive_message))) {
				HCF_CC_TRACE(HandleStartupOperationsErr, "Call 'handle_update_primitive' failed.", -1, fake_primitive_message);
				HCF_CC_LOG(LEVEL_ERROR, "Failed to update the Rule Set File '%s', error_message == '%s'",
						std::string(rsf_installing_path + (*it)).c_str(), fake_primitive_message);
			}
		}
		::memset(fake_primitive_message, 0, TSL_ARRAY_SIZE((fake_primitive_message)));
	}

	// THIRD: Get the list of the RSFs that are scheduled to be deleted and delete them
	list_rsf_to_install.clear();
	rsf_installing_path = TSL_NS_HCF_INTERNALAPI::internalapi_constants::get_rsf_deleting_path() + std::string("/");
	HCF_CC_TRACE(HandleStartupOperations, "Retrieving the list of the RSFs to be deleted in the following folder:", rsf_installing_path.c_str());
	if (TSL_NS_HCF_CORE::utils::list(rsf_installing_path.c_str(), list_rsf_to_install)) {
		HCF_CC_TRACE(HandleStartupOperationsErr, "Call 'utils::list' failed.", ERR_FILE_SYSTEM_OPERATION_FAILED, "");
		HCF_CC_LOG(LEVEL_ERROR, "Failed to retrieve the list of the RSF ready to delete");
		return ERR_FILE_SYSTEM_OPERATION_FAILED;
	}

	for (std::set<std::string>::iterator it = list_rsf_to_install.begin(); it != list_rsf_to_install.end(); ++it) {
		::snprintf(fake_primitive_message, TSL_ARRAY_SIZE(fake_primitive_message), "%s", ((*it).substr(0, (*it).find(".xml"))).c_str());
		HCF_CC_TRACE(HandleStartupOperations, "Deleting the following RSF:", fake_primitive_message);

		if (handle_delete_primitive(fake_primitive, fake_primitive_message, TSL_ARRAY_SIZE(fake_primitive_message))) {
			HCF_CC_TRACE(HandleStartupOperationsErr, "Call 'handle_delete_primitive' failed.", -1, fake_primitive_message);
			HCF_CC_LOG(LEVEL_ERROR, "Failed to delete the Rule Set File '%s', error_message == '%s'",
					std::string(rsf_installing_path + (*it)).c_str(), fake_primitive_message);
		}
		::memset(fake_primitive_message, 0, TSL_ARRAY_SIZE(fake_primitive_message));
	}

	// FOURTH: Check if there is some rule without an associated RSF: in that case, those rules must be deleted!
	std::list<imm_object_hcrule *> hcrule_instances_list = hcrule_class_handler.get_all_instances();
	for (std::list<imm_object_hcrule *>::iterator rule = hcrule_instances_list.begin(); rule != hcrule_instances_list.end(); ++rule) {
		const std::pair<std::string, std::string> rule_rsf_info = (*rule)->get_rule_set_file_info();
		if (rule_rsf_info.first.empty()) {	// This rule hasn't any RSF associated: removing it
			HCF_CC_LOG(LEVEL_WARNING, "The HcRule '%s' hasn't no associated RSF: it shouldn't occur!", (*rule)->get_id().c_str());
			if (const int call_result = (*rule)->delete_object()) {
				HCF_CC_LOG(LEVEL_WARNING, "Call 'delete_object' failed for HcRule '%s': call_result == %d", (*rule)->get_id().c_str(), call_result);
			}
		}
	}

	return ERR_NO_ERROR;
}

int __TSL_CLASS_NAME__::handle_install_primitive (uint32_t & reply_primitive, char * reply_message, size_t reply_message_size, bool not_startup_phase) {
	HCF_CC_TRACE_FUNCTION;
	// By default, the primitive handling is successful
	int call_result = 0;
	reply_primitive = TSL_NS_HCF_INTERNALAPI::HCF_RULESET_FILE_INSTALL_REPLY_OK_ID;

	// Build the path of the RSF XSD schema
	const char * rsf_schema_file_path = TSL_NS_HCF_INTERNALAPI::internalapi_constants::get_rsf_schema_file_path();

	// Initialize the RSF parser and get from it the info about the RSF
	ruleset_file_parser rsf_parser;
	std::pair<std::string,std::string> rsf_file_info;
	HCF_CC_TRACE(HandleInstall, "Retrieving the RSF identifier and revision.", reply_primitive, reply_message);

	ruleset_file_parser::initialize();
	if ((call_result = rsf_parser.get_rsf_info(reply_message, rsf_schema_file_path, &rsf_file_info))) {
		HCF_CC_TRACE(HandleInstallErr, "Call 'rsf_parser.get_rsf_info' failed.", rsf_parser.error(), rsf_parser.error_text());
		HCF_CC_LOG(LEVEL_ERROR, "Call 'rsf_parser.get_rsf_info' failed, call_result == %d", call_result);

		// Delete the RSF that is failed to be installed
		if ((not_startup_phase) && (TSL_NS_HCF_CORE::utils::remove(reply_message))) {
			const int errno_save = errno;
			HCF_CC_TRACE(HandleInstallErr, "Call 'utils::remove.", errno_save, ::strerror(errno_save));
			HCF_CC_LOG(LEVEL_ERROR, "Call 'utils::remove' failed for the RSF '%s'", reply_message);
		}

		// Prepare the info to return back to the caller
		reply_primitive = TSL_NS_HCF_INTERNALAPI::HCF_RULESET_FILE_INSTALL_REPLY_NOK_ID;
		::snprintf(reply_message, reply_message_size, "Rule Set File installation failed");
		ruleset_file_parser::finalize();
		return call_result;
	}
	HCF_CC_TRACE(HandleInstall, "Checking if a RSF with the same identifier was already installed.", reply_primitive, reply_message);

	// Check if the RSF hasn't already been installed
	for (std::list<std::pair<std::string,std::string> >::const_iterator it = _installed_rule.begin(); it != _installed_rule.end(); ++it) {
		if (it->first == rsf_file_info.first) {
			reply_primitive = TSL_NS_HCF_INTERNALAPI::HCF_RULESET_FILE_INSTALL_REPLY_NOK_ID;
			HCF_CC_TRACE(HandleInstallErr, "The RSF is already installed!", reply_primitive, "");
			break;
		}
	}

	// Only if the RSF hasn't been installed, install it
	if (reply_primitive == TSL_NS_HCF_INTERNALAPI::HCF_RULESET_FILE_INSTALL_REPLY_OK_ID) {
		HCF_CC_TRACE(HandleInstall, "Parsing the RSF to extract all the needed information.", reply_primitive, reply_message);

		if ((call_result = rsf_parser.parse(reply_message, rsf_schema_file_path, &rsf_file_info, not_startup_phase))) {
			HCF_CC_TRACE(HandleInstallErr, "Call 'rsf_parser.parse' failed.", rsf_parser.error(), rsf_parser.error_text());
			HCF_CC_LOG(LEVEL_ERROR, "Call 'parser.parse' failed!, return_code == %d, error_code == %d, error_message == '%s'",
					call_result, rsf_parser.error(), rsf_parser.error_text());

			// Get all the instances of rules in order to rollback the partial installation
			std::list<imm_object_hcrule *> hcrule_instance_list = hcrule_class_handler.get_all_instances();
			HCF_CC_TRACE(HandleInstallErr, "The RSF installation failed, deleting all the rules!", rsf_parser.error(), rsf_parser.error_text());

			// A rule must be deleted if it belongs to the partially installed RSF
			int return_code = 0;
			for (std::list<imm_object_hcrule *>::iterator rule = hcrule_instance_list.begin(); rule != hcrule_instance_list.end(); ++rule) {
				if ((*rule)->get_rule_set_file_info().first == rsf_file_info.first) {
					if ((return_code = (*rule)->delete_object())) {
						HCF_CC_TRACE(HandleInstallErr, "Failed to delete an health check rule!", return_code, "");
						HCF_CC_LOG(LEVEL_ERROR, "Call 'rule->delete_object' failed, error_code == %d", return_code);
					}
				}
			}

			// Delete the RSF that is failed to be installed
			if ((not_startup_phase) && (TSL_NS_HCF_CORE::utils::remove(reply_message))) {
				const int errno_save = errno;
				HCF_CC_TRACE(HandleInstallErr, "Call 'utils::remove.", errno_save, ::strerror(errno_save));
				HCF_CC_LOG(LEVEL_ERROR, "Call 'utils::remove' failed for the RSF '%s'", reply_message);
			}

			// Prepare the info to return back to the caller
			reply_primitive = TSL_NS_HCF_INTERNALAPI::HCF_RULESET_FILE_INSTALL_REPLY_NOK_ID;
			::snprintf(reply_message, reply_message_size, "Rule Set File installation failed");
			ruleset_file_parser::finalize();
			return call_result;
		}
		else {
			// Add the installed RSF to the list of the installed ones
			ruleset_file_parser::finalize();

			// Move the RSF file from the installing to the installed folder
			std::string rsf_installed_path = TSL_NS_HCF_INTERNALAPI::internalapi_constants::get_rsf_installed_path() +
                                       std::string("/") + rsf_file_info.first + std::string(".xml");
			HCF_CC_TRACE(HandleInstall, "Moving the RSF to the installation folder.", reply_primitive, reply_message);
			if (TSL_NS_HCF_CORE::utils::move(reply_message, rsf_installed_path.c_str(), (not_startup_phase ? true : false))) {
				int errno_save = errno;
				HCF_CC_TRACE(HandleInstallErr, "Call 'utils::move' failed.", errno_save, ::strerror(errno_save));
				HCF_CC_LOG(LEVEL_ERROR, "Call 'utils::move' failed for the RSF '%s'", reply_message);

				// Delete the RSF that is failed to be installed
				if ((not_startup_phase) && (TSL_NS_HCF_CORE::utils::remove(reply_message))) {
					errno_save = errno;
					HCF_CC_TRACE(HandleInstallErr, "Call 'utils::remove' failed.", errno_save, ::strerror(errno_save));
					HCF_CC_LOG(LEVEL_ERROR, "Call 'utils::remove' failed for the RSF '%s'", reply_message);
				}

				// Get all the instances of rules in order to rollback the partial installation
				std::list<imm_object_hcrule *> hcrule_instance_list = hcrule_class_handler.get_all_instances();
				HCF_CC_TRACE(HandleInstallErr, "The RSF installation failed, deleting all the rules!", -1, "");

				// A rule must be deleted if it belongs to the partially installed RSF
				for (std::list<imm_object_hcrule *>::iterator rule = hcrule_instance_list.begin(); rule != hcrule_instance_list.end(); ++rule) {
					if ((*rule)->get_rule_set_file_info().first == rsf_file_info.first) {
						if ((call_result = (*rule)->delete_object())) {
							HCF_CC_TRACE(HandleInstallErr, "Failed to delete an health check rule!", call_result, "");
							HCF_CC_LOG(LEVEL_ERROR, "Call 'rule->delete_object' failed, error_code == %d", call_result);
						}
					}
				}

				// Prepare the info to return back to the caller
				reply_primitive = TSL_NS_HCF_INTERNALAPI::HCF_RULESET_FILE_INSTALL_REPLY_NOK_ID;
				::snprintf(reply_message,reply_message_size,"Rule Set File installation failed");
				return ERR_FILE_SYSTEM_OPERATION_FAILED;
			}

			// Put the information about the installed RSF into the internal structure
			_installed_rule.push_back(std::pair<std::string, std::string>(rsf_file_info.first, rsf_file_info.second));

			// Get all the instances of the user-defined customizable input parameters and load it into the right HcRule instance
			std::list<imm_object_hcrule_parameter *> parameters = hcrule_parameter_class_handler.get_all_instances();

			for (std::list<imm_object_hcrule_parameter *>::iterator p = parameters.begin(); p != parameters.end(); ++p) {
				// FIRST: Get the pointer to the HcRule instance
				imm_object_hcrule_parameter * par = *p;
				imm_object_hcrule * rule;
				if (!(rule = hcrule_class_handler.get_instance(par->get_hcrule_id()))) {
					HCF_CC_TRACE(HandleInstallErr, "Empty instance found for the HcRule, skipping parameter!", -1, "");
					HCF_CC_LOG(LEVEL_ERROR, "A NULL instance has been retrieved for the HcRule == '%s', it will be not possible to add "
							"the parameter (%s, %s)!", par->get_hcrule_id().c_str(), par->get_name().c_str(), par->get_value().c_str());
					continue;
				}
				HCF_CC_TRACE(HandleInstall, "Rule parameter found: adding to the right rule!", reply_primitive, reply_message);

				// SECOND: Add the user-defined parameter to the retrieved HcRule
				if ((call_result = rule->add_rule_attribute(par->get_job(), par->get_name(), par->get_value()))) {
					HCF_CC_TRACE(HandleInstallErr, "Call 'rule->add_rule_attribute' failed.", call_result, "");
					HCF_CC_LOG(LEVEL_ERROR, "Call 'rule->add_rule_attribute' failed (call_result == %d) for HcRule == '%s' and HcJob == '%s':"
							" the parameter (%s, %s) will be not added!", call_result, par->get_hcrule_id().c_str(),
							par->get_job().c_str(), par->get_name().c_str(), par->get_value().c_str());
				}
			}

			// Prepare the info to return back to the caller
			HCF_CC_LOG(LEVEL_NOTICE, "The Rule Set File '%s' has been correctly installed!", reply_message);
			::snprintf(reply_message, reply_message_size, "Rule Set File installed successfully");
			HCF_CC_TRACE(HandleInstall, "The RSF was correctly installed!", reply_primitive, reply_message);
		}
	}
	else {
		// Delete the RSF that is failed to be installed
		if ((not_startup_phase) && (TSL_NS_HCF_CORE::utils::remove(reply_message))) {
			const int errno_save = errno;
			HCF_CC_TRACE(HandleInstallErr, "Call 'utils::remove' failed.", errno_save, ::strerror(errno_save));
			HCF_CC_LOG(LEVEL_ERROR, "Call 'utils::remove' failed for the RSF '%s'", reply_message);
		}

		HCF_CC_LOG(LEVEL_WARNING, "The Rule Set File '%s' is already installed!", reply_message);
		::snprintf(reply_message, reply_message_size, "Rule Set File already installed");
		HCF_CC_TRACE(HandleInstallErr, "The RSF is already installed!", ERR_DUPLICATED_RULE_SET_FILE, reply_message);
		return ERR_DUPLICATED_RULE_SET_FILE;
	}

	return ERR_NO_ERROR;
}

int __TSL_CLASS_NAME__::handle_delete_primitive (uint32_t & reply_primitive, char * reply_message, size_t reply_message_size) {
	HCF_CC_TRACE_FUNCTION;
	// By default, the primitive handling is successful
	int call_result = 0;
	reply_primitive = TSL_NS_HCF_INTERNALAPI::HCF_RULESET_FILE_DELETE_REPLY_NOK_ID;
	HCF_CC_TRACE(HandleDelete, "Checking if the provided RSF is installed!", reply_primitive, reply_message);

	// Check if the RSF having the provided ID is installed
	std::string rsf_to_delete = reply_message;
	std::list<std::pair<std::string, std::string> >::iterator it;
	for (it = _installed_rule.begin(); it != _installed_rule.end(); ++it) {
		if (it->first == rsf_to_delete) {
			reply_primitive = TSL_NS_HCF_INTERNALAPI::HCF_RULESET_FILE_DELETE_REPLY_OK_ID;
			break;
		}
	}

	// Only if the RSF has been installed, install it
	if (reply_primitive == TSL_NS_HCF_INTERNALAPI::HCF_RULESET_FILE_DELETE_REPLY_OK_ID) {
		// Get all the instances of rules in order to understand if some of them must be deleted
		std::list<imm_object_hcrule *> hcrule_instance_list = hcrule_class_handler.get_all_instances();
		HCF_CC_TRACE(HandleDelete, "The RSF is installed, removing all its rules!", reply_primitive, reply_message);

		// A rule must be deleted if it belongs to the deleted RSF
		for (std::list<imm_object_hcrule *>::iterator rule = hcrule_instance_list.begin(); rule != hcrule_instance_list.end(); ++rule) {
			if ((*rule)->get_rule_set_file_info().first == rsf_to_delete) {
				if ((call_result = (*rule)->delete_object())) {
					HCF_CC_TRACE(HandleDeleteErr, "Failed to delete an health check rule!", call_result);
					HCF_CC_LOG(LEVEL_ERROR, "Call 'rule->delete_object' failed, error_code == %d", call_result);

					// Prepare the info to return back to the caller
					reply_primitive = TSL_NS_HCF_INTERNALAPI::HCF_RULESET_FILE_DELETE_REPLY_NOK_ID;
					::snprintf(reply_message, reply_message_size, "Failed to remove the Rule Set File");
					return call_result;
				}
			}
		}

		// Delete the RSF from the list of the installed RSFs
		_installed_rule.erase(it);

		// Remove the deleted RSF from the installed and erasing folder
		std::string rsf_deleting_path = TSL_NS_HCF_INTERNALAPI::internalapi_constants::get_rsf_installed_path() +
                                    std::string("/") + rsf_to_delete + std::string(".xml");
		HCF_CC_TRACE(HandleDelete, "The RSF is installed, removing the XML file from the installed folder!", reply_primitive, reply_message);
		if (TSL_NS_HCF_CORE::utils::remove(rsf_deleting_path.c_str())) {
			const int errno_save = errno;
			HCF_CC_TRACE(HandleDeleteErr, "Call 'utils::remove' failed.", errno_save);
			HCF_CC_LOG(LEVEL_ERROR, "Call 'utils::remove' failed for file '%s'", rsf_deleting_path.c_str());

			// Prepare the info to return back to the caller
			reply_primitive = TSL_NS_HCF_INTERNALAPI::HCF_RULESET_FILE_DELETE_REPLY_NOK_ID;
			::snprintf(reply_message, reply_message_size, "Failed to remove the Rule Set File");
			return ERR_FILE_SYSTEM_OPERATION_FAILED;
		}

		rsf_deleting_path = TSL_NS_HCF_INTERNALAPI::internalapi_constants::get_rsf_deleting_path() +
                        std::string("/") + rsf_to_delete + std::string(".xml");
		HCF_CC_TRACE(HandleDelete, "The RSF is installed, removing the XML file from the deleting folder!", reply_primitive, reply_message);
		if (TSL_NS_HCF_CORE::utils::remove(rsf_deleting_path.c_str())) {
			const int errno_save = errno;
			HCF_CC_TRACE(HandleDeleteErr, "Call 'utils::remove' failed.", errno_save);
			HCF_CC_LOG(LEVEL_ERROR, "Call 'utils::remove' failed for file '%s'", rsf_deleting_path.c_str());

			// Prepare the info to return back to the caller
			reply_primitive = TSL_NS_HCF_INTERNALAPI::HCF_RULESET_FILE_DELETE_REPLY_NOK_ID;
			::snprintf(reply_message, reply_message_size, "Failed to remove the Rule Set File");
			return ERR_FILE_SYSTEM_OPERATION_FAILED;
		}
		::snprintf(reply_message, reply_message_size, "Rule Set File removed successfully");
	}
	else {
		HCF_CC_TRACE(HandleDeleteErr, "The RSF is not installed, cannot delete it!", ERR_UNDEFINED_RULE_SET_FILE);
		HCF_CC_LOG(LEVEL_ERROR, "The Rule Set File '%s' is not installed!", reply_message);
		::snprintf(reply_message, reply_message_size, "Rule Set File not installed");
		return ERR_UNDEFINED_RULE_SET_FILE;
	}

	return ERR_NO_ERROR;
}

int __TSL_CLASS_NAME__::handle_validation_primitive (uint32_t & reply_primitive, char * reply_message, size_t reply_message_size) {
	HCF_CC_TRACE_FUNCTION;
	// By default, the primitive handling is successful
	reply_primitive = TSL_NS_HCF_INTERNALAPI::HCF_RULESET_FILE_VALIDATE_REPLY_OK_ID;

	// Build the path of the RSF XSD schema
	const char * rsf_schema_file_path = TSL_NS_HCF_INTERNALAPI::internalapi_constants::get_rsf_schema_file_path();

	// Initialize the RSF parser and validate the provided RSF
	ruleset_file_parser rsf_parser;
	const std::string rsf_to_validate(reply_message);
	ruleset_file_parser::initialize();

	HCF_CC_TRACE(HandleValidate, "Received a request to validate the RSF!", reply_primitive, reply_message);
	if (rsf_parser.validate(reply_message,rsf_schema_file_path))
		::snprintf(reply_message, reply_message_size, "Validation failed");
	else
		::snprintf(reply_message, reply_message_size, "Validation completed");

	HCF_CC_TRACE(HandleValidate, "RSF validation completed!", reply_primitive, reply_message);
	ruleset_file_parser::finalize();

	// After validating the RSF, try to remove it
	if (TSL_NS_HCF_CORE::utils::remove(rsf_to_validate.c_str())) {
		HCF_CC_LOG(LEVEL_ERROR, "Call 'utils::remove' failed for file '%s'", rsf_to_validate.c_str());

		// Prepare the info to return back to the caller
		reply_primitive = TSL_NS_HCF_INTERNALAPI::HCF_RULESET_FILE_VALIDATE_REPLY_NOK_ID;
		::snprintf(reply_message, reply_message_size, "Failed to remove the Rule Set File");
		return ERR_FILE_SYSTEM_OPERATION_FAILED;
	}

	return ERR_NO_ERROR;
}

int __TSL_CLASS_NAME__::handle_listing_primitive (uint32_t & reply_primitive, char * reply_message, size_t reply_message_size) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(HandleListing, "Received a request to list all the installed RSFs!", reply_primitive, reply_message);

	// By default, the primitive handling is successful
	reply_primitive = TSL_NS_HCF_INTERNALAPI::HCF_RULESET_FILE_LIST_REPLY_OK_ID;

	// Prepare the printout for the caller: first the header
	char * tmp = reply_message;
	tmp += ::snprintf(reply_message, reply_message_size, "RULE SET FILE ID                REVISION");

	// Then add a row for each installed RSF
	for (std::list<std::pair<std::string, std::string> >::const_iterator it = _installed_rule.begin(); it != _installed_rule.end(); ++it) {
		tmp += ::snprintf(tmp, reply_message_size, "\n%-32s" "%s", it->first.c_str(), it->second.c_str());
	}

	return ERR_NO_ERROR;
}

int __TSL_CLASS_NAME__::handle_update_primitive (uint32_t & reply_primitive, char * reply_message, size_t reply_message_size) {
	HCF_CC_TRACE_FUNCTION;
	// Build the path of the RSF XSD schema
	int call_result = 0;
	const char * rsf_schema_file_path = TSL_NS_HCF_INTERNALAPI::internalapi_constants::get_rsf_schema_file_path();

	// Initialize the RSF parser and get from it the info about the RSF
	ruleset_file_parser rsf_parser;
	std::pair<std::string,std::string> rsf_file_info;
	HCF_CC_TRACE(HandleUpdate, "Retrieving the RSF identifier and revision.", reply_primitive, reply_message);

	ruleset_file_parser::initialize();
	if ((call_result = rsf_parser.get_rsf_info(reply_message, rsf_schema_file_path, &rsf_file_info))) {
		HCF_CC_TRACE(HandleUpdateErr, "Call 'rsf_parser.get_rsf_info' failed.", rsf_parser.error(), rsf_parser.error_text());
		HCF_CC_LOG(LEVEL_ERROR, "Call 'rsf_parser.get_rsf_info' failed, call_result == %d", call_result);

		// Delete the RSF that is failed to be updated
		if (TSL_NS_HCF_CORE::utils::remove(reply_message)) {
			const int errno_save = errno;
			HCF_CC_TRACE(HandleUpdateErr, "Call 'utils::remove' failed.", errno_save, ::strerror(errno_save));
			HCF_CC_LOG(LEVEL_ERROR, "Call 'utils::remove' failed for the RSF '%s'", reply_message);
		}

		// Prepare the info to return back to the caller
		reply_primitive = TSL_NS_HCF_INTERNALAPI::HCF_RULESET_FILE_UPGRADE_REPLY_NOK_ID;
		::snprintf(reply_message, reply_message_size, "Rule Set File update failed");
		ruleset_file_parser::finalize();
		return call_result;
	}
	ruleset_file_parser::finalize();
	HCF_CC_TRACE(HandleUpdate, "Checking if the RSF update operation is legal.", reply_primitive, reply_message);

	// An update is handled as a delete and an install: first, get the ID of the RSF to install
	char message_for_delete_primitive[4096] = {0};
	for (std::list<std::pair<std::string, std::string> >::const_iterator it = _installed_rule.begin(); it != _installed_rule.end(); ++it) {

		// If the same RSF is found (same ID and same revision), the update fails
		if ((it->first == rsf_file_info.first) && (it->second == rsf_file_info.second)) {
			HCF_CC_TRACE(HandleUpdateErr, "There is another RSF with the same ID and revision", ERR_DUPLICATED_RULE_SET_FILE, "");
			HCF_CC_LOG(LEVEL_ERROR, "Failed to update the Rule Set File, old and new revision are equal");

			// Delete the RSF that is failed to be updated
			if (TSL_NS_HCF_CORE::utils::remove(reply_message)) {
				const int errno_save = errno;
				HCF_CC_TRACE(HandleUpdateErr, "Call 'utils::remove' failed.", errno_save, ::strerror(errno_save));
				HCF_CC_LOG(LEVEL_ERROR, "Call 'utils::remove' failed for the RSF '%s'", reply_message);
			}

			// Prepare the info to return back to the caller
			reply_primitive = TSL_NS_HCF_INTERNALAPI::HCF_RULESET_FILE_UPGRADE_REPLY_NOK_ID;
			::snprintf(reply_message, reply_message_size, "Rule Set File already installed");
			return ERR_DUPLICATED_RULE_SET_FILE;
		}
		else if (it->first == rsf_file_info.first) {
			// RSF to update found: prepare its ID to be deleted
			::snprintf(message_for_delete_primitive, reply_message_size, "%s", rsf_file_info.first.c_str());
			break;
		}
	}
	HCF_CC_TRACE(HandleUpdate, "RSF found: first, take a backup copy!", reply_primitive, reply_message);

	uint32_t delete_primitive_reply = TSL_NS_HCF_INTERNALAPI::HCF_RULESET_FILE_DELETE_REQUEST_ID;

	// Take a copy of the RSF to be deleted in order to implement a rollback mechanism in case of errors
	std::string backup_rsf_path    = TSL_NS_HCF_INTERNALAPI::internalapi_constants::get_rsf_installing_path() +
                                   std::string("/") + std::string("backup.xml");
	std::string rsf_installed_path = TSL_NS_HCF_INTERNALAPI::internalapi_constants::get_rsf_installed_path() +
                                   std::string("/") + std::string(message_for_delete_primitive) + std::string(".xml");
	if (TSL_NS_HCF_CORE::utils::copy(rsf_installed_path.c_str(), backup_rsf_path.c_str())) {
		const int errno_save = errno;
		HCF_CC_TRACE(HandleUpdateErr, "Call 'utils::copy' failed.", errno_save, ::strerror(errno_save));
		HCF_CC_LOG(LEVEL_WARNING, "Failed to take a backup copy of the RSF to '%s'!", backup_rsf_path.c_str());
		backup_rsf_path.clear();
	}

	if (::strlen(message_for_delete_primitive)) {
		std::string rsf_deleting_path  = TSL_NS_HCF_INTERNALAPI::internalapi_constants::get_rsf_deleting_path() +
                                     std::string("/") + std::string(message_for_delete_primitive) + std::string(".xml");

		// Copy the RSF to be deleted under the deleting folder
		HCF_CC_TRACE(HandleUpdate, "Prepare to delete the old RSF: copy it under deleting folder!", reply_primitive, reply_message);
		if (TSL_NS_HCF_CORE::utils::copy(rsf_installed_path.c_str(), rsf_deleting_path.c_str())) {
			const int errno_save = errno;
			HCF_CC_TRACE(HandleUpdateErr, "Call 'utils::copy' failed.", errno_save, ::strerror(errno_save));
			HCF_CC_LOG(LEVEL_ERROR, "Call 'utils::copy' failed, from == '%s', to == '%s'", rsf_installed_path.c_str(), rsf_deleting_path.c_str());

			// Prepare the info to return back to the caller
			reply_primitive = TSL_NS_HCF_INTERNALAPI::HCF_RULESET_FILE_UPGRADE_REPLY_NOK_ID;
			::snprintf(reply_message,reply_message_size,"Failed to remove the old Rule Set File");
			return ERR_FILE_SYSTEM_OPERATION_FAILED;
		}

		// Delete the old version of the RSF
		HCF_CC_TRACE(HandleUpdate, "Deleting the old RSF!", reply_primitive, reply_message);
		if ((call_result = handle_delete_primitive(delete_primitive_reply, message_for_delete_primitive, reply_message_size))) {
			HCF_CC_TRACE(HandleUpdateErr, "Call 'handle_delete_primitive' failed.", call_result, "");
			HCF_CC_LOG(LEVEL_ERROR, "Failed to delete the Rule Set File '%s'", reply_message);

			// Prepare the info to return back to the caller
			reply_primitive = TSL_NS_HCF_INTERNALAPI::HCF_RULESET_FILE_UPGRADE_REPLY_NOK_ID;
			::snprintf(reply_message, reply_message_size, "Failed to remove the old Rule Set File");
			return call_result;
		}
	}

	// Install the new version of the RSF
	HCF_CC_TRACE(HandleUpdate, "Installing the new RSF!", reply_primitive, reply_message);
	if ((call_result = handle_install_primitive(reply_primitive, reply_message, reply_message_size))) {
		HCF_CC_TRACE(HandleUpdateErr, "Call 'handle_install_primitive' failed.", call_result, "");
		HCF_CC_LOG(LEVEL_ERROR, "Failed to install the Rule Set File, trying to install "
				"again the old one (%s)",  backup_rsf_path.c_str());

		// Try to rollback to the old RSF that was previously removed
		uint32_t fake_primitive;
		const size_t fake_message_size = backup_rsf_path.size() + 1;
		char fake_message[fake_message_size];
		::strncpy(fake_message, backup_rsf_path.c_str(), fake_message_size);
		HCF_CC_TRACE(HandleUpdateErr, "RSF installation failed: reinstalling the old RSF.", 0, fake_message);
		if (const int result = handle_install_primitive(fake_primitive, fake_message, backup_rsf_path.size())) {
			HCF_CC_TRACE(HandleUpdateErr, "Call 'handle_install_primitive' failed.", result, "");
			HCF_CC_LOG(LEVEL_WARNING, "Failed to rollback the RSF installation for the file '%s'!", backup_rsf_path.c_str());
		}

		// Prepare the info to return back to the caller
		reply_primitive = TSL_NS_HCF_INTERNALAPI::HCF_RULESET_FILE_UPGRADE_REPLY_NOK_ID;
		::snprintf(reply_message, reply_message_size, "Failed to install the new Rule Set File");
		return call_result;
	}

	// Remove, if needed, the backup RSF
	if ((!backup_rsf_path.empty()) && (TSL_NS_HCF_CORE::utils::remove(backup_rsf_path.c_str()))) {
		const int errno_save = errno;
		HCF_CC_TRACE(HandleUpdateErr, "Call 'utils::remove' failed.", errno_save, ::strerror(errno_save));
		HCF_CC_LOG(LEVEL_WARNING, "Failed to remove the backup RSF having path '%s'!", backup_rsf_path.c_str());
	}

	// Prepare the info to return back to the caller
	HCF_CC_LOG(LEVEL_NOTICE, "Rule Set File successfully updated!");

	reply_primitive = TSL_NS_HCF_INTERNALAPI::HCF_RULESET_FILE_UPGRADE_REPLY_OK_ID;
	::snprintf(reply_message, reply_message_size, "Rule Set File installed successfully");
	HCF_CC_TRACE(HandleUpdate, "The RSF has been correctly installed!", reply_primitive, reply_message);
	return ERR_NO_ERROR;
}

HCF_NS_HCF_SRV_END
