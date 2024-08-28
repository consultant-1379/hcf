#include <linux/limits.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string>

#include "protocol_primitives.h"
#include "protocol_handler.h"
#include "constants.h"

#include "hcf/core/utils.h"

#include "cmd_option_agent.h"

#include "hcf/trace/com_ericsson_common_hcf_hcrsfm.h"
#include "hcf/trace/tracing_macros.h"

HCF_CC_FUNCTION_TRACER_CLASS_DEFINE

void print_command_usage (const char * program_name, const char * error_text);

int main (int argc, char * argv[]) {
	HCF_CC_TRACE_FUNCTION;

	const uid_t uid = ::getuid();
	HCF_CC_TRACE(User, "User executing HCRSFM command:", uid);

	if (uid != HCF_NS_HCF_CMD::CMD_AUTHORAZIED_USER) {
		::fprintf(::stderr, "User is not authorized to executed the command.\n\n");
		return HCF_NS_HCF_CMD::CMD_EXIT_NOT_AUTHORIZED_USER;
	}

	HCF_NS_HCF_CMD::cmd_option_agent cmd_opt_agent(argc, argv);

	if (cmd_opt_agent.parse() < 0) { // Error parsing command line
		print_command_usage(cmd_opt_agent.program_name(), cmd_opt_agent.error_text());
		return HCF_NS_HCF_CMD::CMD_EXIT_INCORRECT_USAGE;
	}

	if (!cmd_opt_agent.is_cmd_line_valid()) {	// Invalid option set provided
		print_command_usage(cmd_opt_agent.program_name(), cmd_opt_agent.error_text());
		return HCF_NS_HCF_CMD::CMD_EXIT_INCORRECT_USAGE;
	}

	const char * cmd_line_argument = 0;
	char absolute_path[PATH_MAX + 1];

	if (!cmd_opt_agent.list()) {	// All options (excluding '-l' have a cmd-line argument)
		HCF_CC_TRACE(FileArgument, cmd_opt_agent.argv()[2]);

		if (cmd_opt_agent.erase()) {
			// Only option '-e' doesn't require a path resolution (since it takes the RSF ID)
			cmd_line_argument = cmd_opt_agent.argv()[2];
		}
		else {
			// All the other options require to resolve the provided path (that can be relative)
			const char * const argument = cmd_opt_agent.argv()[2];

			errno = 0;
			char * return_ptr = ::realpath(argument, absolute_path);
			const int errno_save = errno;

			// If the call to 'realpath' failed, try to understand why
			if ((!return_ptr) && (errno_save != ENOENT)) {
				// If the returned pointer is NULL and the errno isn't
				// 'No such file or directory', a general error happened
				HCF_CC_TRACE(FileArgumentError, "Generic error occurred when trying to get the absolute path for the file.", argument);
				::fprintf(::stderr, "Error when executing.\n\n");
				return HCF_NS_HCF_CMD::CMD_EXIT_GENERAL_ERROR;
			}
			else if (errno_save == ENOENT) {
				// Otherwise, if the errno is 'No such file or directory' the provided file doesn't exist
				HCF_CC_TRACE(FileArgumentError, "The file does not exist, errno == ENOENT!", argument);
				::fprintf(::stderr, "The file '%s' does not exist!\n\n", argument);
				return HCF_NS_HCF_CMD::CMD_EXIT_NOT_EXISTING_FILE;
			}
			else {
				struct stat file_info;

				if (!::stat(absolute_path, &file_info)) {
					if (file_info.st_mode & S_IFDIR) {
						HCF_CC_TRACE(FileArgumentError, "The file is not a regular file (probably it's a directory)!", argument);
						::fprintf(::stderr, "The file '%s' is not a regular file!\n\n", argument);
						return HCF_NS_HCF_CMD::CMD_EXIT_NOT_REGULAR_FILE;
					}
				}
				else {
					HCF_CC_TRACE(FileArgumentError, "Generic error when trying to retrieve file information. "
							"The file seems to exist, but call to 'stat' function is failed!", argument);
					::fprintf(::stderr, "Error when executing.\n\n");
					return HCF_NS_HCF_CMD::CMD_EXIT_GENERAL_ERROR;
				}
			}

			cmd_line_argument = absolute_path;
		}
	}

	// After completing the command line parsing, build the primitive to be sent to the HCF server
	uint32_t primitive_to_send = 0;
	std::string rsf_installing_path;

	if (cmd_opt_agent.erase()) {
		primitive_to_send = TSL_NS_HCF_INTERNALAPI::HCF_RULESET_FILE_DELETE_REQUEST_ID;

		std::string rsf_installed_path = TSL_NS_HCF_INTERNALAPI::internalapi_constants::get_rsf_installed_path() +
										 std::string("/") +
										 std::string(const_cast<char *>(cmd_line_argument)) +
										 std::string(".xml");

		std::string rsf_deleting_path = TSL_NS_HCF_INTERNALAPI::internalapi_constants::get_rsf_deleting_path() +
										std::string("/") +
										std::string(const_cast<char *>(cmd_line_argument)) +
										std::string(".xml");

		if (::access(rsf_installed_path.c_str(), F_OK)) {
			HCF_CC_TRACE(FileArgumentError, "Requested to delete a RSF that is not installed. "
					"The following file should be present, but it isn't!", rsf_installed_path.c_str());
			::fprintf(::stderr, "Rule Set File not installed\n\n");
			return HCF_NS_HCF_CMD::CMD_EXIT_NOT_EXISTING_FILE;
		}

		HCF_CC_TRACE(FileArgument, rsf_installed_path.c_str());
		if (TSL_NS_HCF_CORE::utils::copy(rsf_installed_path.c_str(), rsf_deleting_path.c_str())) {
			HCF_CC_TRACE(FileArgumentError, "Failed to order the RSF deletion. The copy operation from "
					"installed folder to deleting one is failed!", rsf_deleting_path.c_str());
			::fprintf(::stderr, "Generic error: Failed to copy the Rule Set File into the internal HCF folder\n\n");
			return HCF_NS_HCF_CMD::CMD_EXIT_RSF_MOVE_FAILED;
		}
	}
	else if (cmd_opt_agent.install()){
		primitive_to_send = TSL_NS_HCF_INTERNALAPI::HCF_RULESET_FILE_INSTALL_REQUEST_ID;

		rsf_installing_path = TSL_NS_HCF_INTERNALAPI::internalapi_constants::get_rsf_installing_path() +
							  std::string("/") +
							  std::string(::basename(const_cast<char *>(cmd_line_argument)));

		HCF_CC_TRACE(FileArgument, rsf_installing_path.c_str());
		if (TSL_NS_HCF_CORE::utils::copy(cmd_line_argument, rsf_installing_path.c_str())) {
			HCF_CC_TRACE(FileArgumentError, "Failed to order the RSF installation. The copy operation from "
					"the current folder to the installing folder is failed!", rsf_installing_path.c_str());
			::fprintf(::stderr, "Generic error: Failed to move the Rule Set File into the internal HCF folder\n\n");
			return HCF_NS_HCF_CMD::CMD_EXIT_RSF_MOVE_FAILED;
		}

		cmd_line_argument = rsf_installing_path.c_str();
	}
	else if (cmd_opt_agent.list())
		primitive_to_send = TSL_NS_HCF_INTERNALAPI::HCF_RULESET_FILE_LIST_REQUEST_ID;
	else if (cmd_opt_agent.upgrade()){
		primitive_to_send = TSL_NS_HCF_INTERNALAPI::HCF_RULESET_FILE_UPGRADE_REQUEST_ID;

		rsf_installing_path = TSL_NS_HCF_INTERNALAPI::internalapi_constants::get_rsf_installing_path() +
							  std::string("/") +
							  std::string("u_") +
							  std::string(::basename(const_cast<char *>(cmd_line_argument)));

		HCF_CC_TRACE(FileArgument, rsf_installing_path.c_str());
		if (TSL_NS_HCF_CORE::utils::copy(cmd_line_argument, rsf_installing_path.c_str())) {
			HCF_CC_TRACE(FileArgumentError, "Failed to order the RSF upgrade. The copy operation from "
					"the current folder to the installing folder is failed!", rsf_installing_path.c_str());
			::fprintf(::stderr, "Generic error: Failed to move the Rule Set File into the internal HCF folder\n\n");
			return HCF_NS_HCF_CMD::CMD_EXIT_RSF_MOVE_FAILED;
		}

		cmd_line_argument = rsf_installing_path.c_str();
	}
	else {	// cmd_opt_agent.validate()
		primitive_to_send = TSL_NS_HCF_INTERNALAPI::HCF_RULESET_FILE_VALIDATE_REQUEST_ID;

		// In case of validation copy for a while the RSF under HCF main folder
		rsf_installing_path = TSL_NS_HCF_INTERNALAPI::internalapi_constants::get_hcf_cluster_nobackup_path() +
							  std::string("/") + std::string(::basename(const_cast<char *>(cmd_line_argument)));

		HCF_CC_TRACE(FileArgument, rsf_installing_path.c_str());
		if (TSL_NS_HCF_CORE::utils::copy(cmd_line_argument, rsf_installing_path.c_str())) {
			HCF_CC_TRACE(FileArgumentError, "Failed to order the RSF validation. The copy operation from "
					"the current folder to the HCF main folder is failed!", rsf_installing_path.c_str());
			::fprintf(::stderr, "Generic error: Failed to move the Rule Set File into the internal HCF folder\n\n");
			return HCF_NS_HCF_CMD::CMD_EXIT_RSF_MOVE_FAILED;
		}

		cmd_line_argument = rsf_installing_path.c_str();
	}

	// Open the file storing the information about the address and port on which the server is waiting
	std::string file_path = TSL_NS_HCF_INTERNALAPI::internalapi_constants::get_cmd_server_address_file_path();

	int cmd_server_address_file_fd = -1;
	if ((cmd_server_address_file_fd = ::open(file_path.c_str(), O_RDONLY)) < 0) {
		const int errno_save = errno;
		::fprintf(::stderr, "Generic error: Failed to get the server address (error == %d)\n\n", errno_save);
		return HCF_NS_HCF_CMD::CMD_EXIT_NOT_EXISTING_FILE;
	}

	// After successful opening, read from the file the info about the address and the port
	char buffer[2048];
	ssize_t bytes_read = -1;
	if ((bytes_read = ::read(cmd_server_address_file_fd, buffer, TSL_ARRAY_SIZE(buffer))) < 0) {
		const int errno_save = errno;
		::fprintf(::stderr, "Generic error: Failed to read the server address (error == %d)\n\n", errno_save);
		::close(cmd_server_address_file_fd);
		return HCF_NS_HCF_CMD::CMD_EXIT_NOT_EXISTING_FILE;
	}
	buffer[bytes_read] = 0;
	HCF_CC_TRACE(ServerConnection, "The HCF server is available at the following address.", buffer);

	// Parse the read buffer to extract the address and the port values
	std::string tmp = buffer;
	char ip[16];
	char port[8];
	::snprintf(ip, TSL_ARRAY_SIZE(ip), "%s", tmp.substr(0, tmp.find_first_of(':')).c_str());
	::snprintf(port, TSL_ARRAY_SIZE(port), "%s", tmp.substr(tmp.find_first_of(':') + 1).c_str());

	// Prepare the structure storing the server address
	in_addr addr;
	inet_pton(AF_INET, ip, &addr.s_addr);

	// Create the socket to contact the HCF server
	HCF_CC_TRACE(ServerConnection, "Creating a socket to connect to HCF server.", buffer);
	int socket_fd = -1;
	errno = 0;
	if ((socket_fd = ::socket(AF_INET, SOCK_STREAM, 0)) < 0) { // Failed to create the socket to contact HCF server
		const int errno_save = errno;
		HCF_CC_TRACE(ServerConnectionError, "Failed to create a socket to connect to HCF server.", errno_save);
		::fprintf(::stderr, "Generic error: Failed to create the socket to contact the server (error == %d)\n\n", errno_save);
		return HCF_NS_HCF_CMD::CMD_EXIT_CONNECTION_FAILED;
	}

	// Prepare the structure to be used to connect to the HCF server
	struct sockaddr_in server_addr;
	::memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = addr.s_addr;
	server_addr.sin_port = htons(::atoi(port));

	// Connect to the HCF server
	HCF_CC_TRACE(ServerConnection, "Connecting to HCF server.", buffer);
	errno = 0;
	if (::connect(socket_fd, (struct sockaddr *)(&server_addr), sizeof(server_addr))) {
		const int errno_save = errno;
		::close(socket_fd);
		HCF_CC_TRACE(ServerConnectionError, "Failed to connect to HCF server. If error == ECONNREFUSED(111), the operation "
				"will be executed later, otherwise, there is an error!", errno_save);

		if ((errno_save == ECONNREFUSED) && !((primitive_to_send == TSL_NS_HCF_INTERNALAPI::HCF_RULESET_FILE_LIST_REQUEST_ID) || (primitive_to_send == TSL_NS_HCF_INTERNALAPI::HCF_RULESET_FILE_VALIDATE_REQUEST_ID))) {
			::fprintf(::stderr, "HCF server not reachable, the operation has been ordered\n\n");
			return HCF_NS_HCF_CMD::CMD_EXIT_OK;
		}
		else {
			::fprintf(::stderr, "Generic error: Failed to contact the server (error == %d)\n\n", errno_save);
			TSL_NS_HCF_CORE::utils::remove(cmd_line_argument);
			return HCF_NS_HCF_CMD::CMD_EXIT_CONNECTION_FAILED;
		}
	}

	// Create the protocol handler object, passing to it the created socket
	TSL_NS_HCF_INTERNALAPI::protocol_handler prot_handler(socket_fd);
	uint32_t cmd_line_argument_length = (cmd_line_argument) ? ::strlen(cmd_line_argument) : 0;

	HCF_CC_TRACE(Primitive, "Preparing the primitive to send:", primitive_to_send, cmd_line_argument_length, ((cmd_line_argument) ? cmd_line_argument : ""));
	if (prot_handler.make_primitive(primitive_to_send, cmd_line_argument_length, cmd_line_argument) < 0) {
		// Failed to build the primitive with the given parameters
		HCF_CC_TRACE(PrimitiveError, "Failed to build the request primitive. Removing the file previously "
				"copied under HCF private folders.", prot_handler.error(), prot_handler.error_text());
		::fprintf(::stderr, "Generic error: Failed to build the request primitive (error == %d, error_message == '%s')\n\n",
				prot_handler.error(), prot_handler.error_text());
		::close(socket_fd);
		TSL_NS_HCF_CORE::utils::remove(cmd_line_argument);
		return HCF_NS_HCF_CMD::CMD_EXIT_BUILD_PRIMITIVE_FAILED;
	}

	// After the primitive has been correctly built, sent it to the HCF server
	HCF_CC_TRACE(Primitive, "Sending the request primitive:", primitive_to_send, cmd_line_argument_length, ((cmd_line_argument) ? cmd_line_argument : ""));
	if (prot_handler.send_primitive() < 0) {	// Failed to send the primitive to the HCF server
		HCF_CC_TRACE(PrimitiveError, "Failed to send the request primitive. Removing the file previously "
				"copied under HCF private folders.", prot_handler.error(), prot_handler.error_text());
		::fprintf(::stderr, "Generic error: Failed to send the request primitive (error == %d, error_message == '%s')\n\n",
				prot_handler.error(), prot_handler.error_text());
		::close(socket_fd);
		TSL_NS_HCF_CORE::utils::remove(cmd_line_argument);
		return HCF_NS_HCF_CMD::CMD_EXIT_SEND_PRIMITIVE_FAILED;
	}

	// After the primitive has been correctly sent, wait for the response primitive from HCF server
	HCF_CC_TRACE(Primitive, "Receiving a primitive as response to the request:", primitive_to_send, cmd_line_argument_length, ((cmd_line_argument) ? cmd_line_argument : ""));
	if (prot_handler.recv_primitive() < 0) {	// Failed to receive the primitive from the HCF server
		HCF_CC_TRACE(PrimitiveError, "Failed to receive the response primitive. Removing the file previously "
				"copied under HCF private folders.", prot_handler.error(), prot_handler.error_text());
		::fprintf(::stderr, "Generic error: Failed to receive the response primitive (error == %d, error_message == '%s')\n\n",
				prot_handler.error(), prot_handler.error_text());
		::close(socket_fd);
		TSL_NS_HCF_CORE::utils::remove(cmd_line_argument);
		return HCF_NS_HCF_CMD::CMD_EXIT_RECV_PRIMITIVE_FAILED;
	}
	::close(socket_fd);

	// After receiving correctly the primitive, unpack it
	uint32_t response_primitive;
	char primitive_msg[4096] = {0};

	HCF_CC_TRACE(Primitive, "Unpacking the response to the request:", primitive_to_send, cmd_line_argument_length, ((cmd_line_argument) ? cmd_line_argument : ""));
	if (prot_handler.unpack_primitive(response_primitive, primitive_msg, TSL_ARRAY_SIZE(primitive_msg)) < 0) {	// Failed to unpack the received primitive
		HCF_CC_TRACE(PrimitiveError, "Failed to unpack the response primitive. Removing the file previously "
				"copied under HCF private folders.", prot_handler.error(), prot_handler.error_text());
		::fprintf(::stderr, "Generic error: Failed to unpack the response primitive (error == %d, error_message == '%s')\n\n",
				prot_handler.error(), prot_handler.error_text());
		TSL_NS_HCF_CORE::utils::remove(cmd_line_argument);
		return HCF_NS_HCF_CMD::CMD_EXIT_UNPACK_PRIMITIVE_FAILED;
	}
	HCF_CC_TRACE(Primitive, "Response primitive:", response_primitive, ::strlen(primitive_msg), primitive_msg);

	/*
	 * After unpacking the primitive check that the correct primitive has been received.
	 * A correct response primitive has an ID equal to the ID of the request primitive plus 1.
	 */

	int cmd_exit_code = HCF_NS_HCF_CMD::CMD_EXIT_OK;

	if (response_primitive == primitive_to_send + TSL_NS_HCF_INTERNALAPI::HCF_RULESET_FILE_OPERATION_FAILURE){
		cmd_exit_code = HCF_NS_HCF_CMD::CMD_EXIT_RFS_ALREADY_INSTALLED;
	}
	else if (response_primitive != primitive_to_send + TSL_NS_HCF_INTERNALAPI::HCF_RULESET_FILE_OPERATION_SUCCESS){
		HCF_CC_TRACE(CmdExitCode, "An error occurred when receiving the response primitive. The received ID is "
				"different from the request primitive one plus 1. Removing the file previously copied under HCF"
				"private folders.", HCF_NS_HCF_CMD::CMD_EXIT_UNEXPECTED_PRIMITIVE_RECEIVED);
		::fprintf(::stderr, "Generic error: Received an unexpected primitive (with id %u instead of %u)\n\n",
				response_primitive, (primitive_to_send + 1));
		TSL_NS_HCF_CORE::utils::remove(cmd_line_argument);
		return HCF_NS_HCF_CMD::CMD_EXIT_UNEXPECTED_PRIMITIVE_RECEIVED;
	}

	// After all the checks, print the received message
	HCF_CC_TRACE(CmdExitCode, "The command exits with the following exit code.", cmd_exit_code);
	::printf("%s\n\n", primitive_msg);
	return cmd_exit_code;
}

void print_command_usage (const char * program_name, const char * error_text) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(CmdUsageError, "Incorrect usage for hcrsfm command!", error_text);

	::fprintf(stderr, "Incorrect usage: %s\n", error_text);
	::fprintf(stderr, "Usage:\n");
	::fprintf(stderr, "%s -e | --erase rsf_id\n", program_name);
	::fprintf(stderr, "%s -i | --install rsf_file_name\n", program_name);
	::fprintf(stderr, "%s -l | --list\n", program_name);
	::fprintf(stderr, "%s -u | --upgrade rsf_file_name\n", program_name);
	::fprintf(stderr, "%s -v | --validate rsf_file_name\n\n", program_name);
}
