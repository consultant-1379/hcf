#include <string.h>
#include <fcntl.h>
#include <grp.h>

#include <algorithm>

#include "hcf/core/logger.h"
#include "hcf/core/guard.h"
#include "hcf/core/utils.h"
#include "hcf/imm/om_handler.h"
#include "hcf/imm/constants.h"
#include "hcf/imm/imm_error_constants.h"
#include "internalapi_constants.h"

#include "constants.h"

#include "hcf/trace/com_ericsson_common_hcf_srv_configurator.h"
#include "hcf/trace/tracing_macros.h"

#include "configurator.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_SRV_CLASS_CONFIGURATOR__

HCF_NS_HCF_SRV_BEGIN

HCF_CC_FUNCTION_TRACER_CLASS_DEFINE

// Static fields initialization
const char * const __TSL_CLASS_NAME__::HCF_SYSTEM_CONTROLLER_NODE_PREFIX     = "SC-";
const char * const __TSL_CLASS_NAME__::HCF_PAYLOAD_NODE_PREFIX               = "PL-";
const char * const __TSL_CLASS_NAME__::HCF_ALL_NODES_TARGET_VALUE            = "ALL";
const char * const __TSL_CLASS_NAME__::HCF_ALL_CONTROLLER_NODES_TARGET_VALUE = "SC-ALL";
const char * const __TSL_CLASS_NAME__::HCF_ALL_PAYLOAD_NODES_TARGET_VALUE    = "PL-ALL";
const char * const __TSL_CLASS_NAME__::HCF_AMF_NODE_CLASS_NAME               = "SaAmfNode";
const char * const __TSL_CLASS_NAME__::HCF_CLUSTER_NODES_CONFIG_FILE_PATH    = "/etc/cluster/nodes/all";
const char * const __TSL_CLASS_NAME__::HCF_CLUSTER_NODES_HOSTNAME_FILE_NAME  = "hostname";

TSL_NS_HCF_CORE::thread_mutex_recursive __TSL_CLASS_NAME__::_configuration_sync;
char __TSL_CLASS_NAME__::_managed_element_id[] = {0};
char __TSL_CLASS_NAME__::_internal_output_folder_path[PATH_MAX + 1] = {0};
char __TSL_CLASS_NAME__::_output_folder_path[PATH_MAX + 1] = {0};
std::map<std::string, std::string> __TSL_CLASS_NAME__::_controllers_map;
std::map<std::string, std::string> __TSL_CLASS_NAME__::_payloads_map;

int __TSL_CLASS_NAME__::get_managed_element_id (char * meid_buffer, size_t & buffer_size) {
	HCF_CC_TRACE_FUNCTION;
	TSL_CRITICAL_SECTION_BEGIN(sync_t, _configuration_sync);
		::strncpy(meid_buffer, _managed_element_id, buffer_size);
		buffer_size = ::strlen(_managed_element_id);
	TSL_CRITICAL_SECTION_END;
	return ERR_NO_ERROR;
}

int __TSL_CLASS_NAME__::get_system_controllers_set (std::set<std::string> & sc_set) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(GetSystemControllersSet, "Received a request to retrieve the list of the system controllers nodes in the cluster!");

	TSL_CRITICAL_SECTION_BEGIN(sync_t, _configuration_sync);
		if (!(_controllers_map.size()) && !(_payloads_map.size())) {
			// Info not yet loaded, load it from IMM
			HCF_CC_TRACE(GetSystemControllersSet, "Configuration not yet loaded, querying IMM!");
			if (const int call_result = load_nodes_from_imm()) {
				HCF_CC_TRACE(GetSystemControllersSetErr, "Call 'load_nodes_from_imm' failed.", call_result);
				return call_result;
			}
		}

		// After loading the info from IMM (if not yet done), fill the input set
		HCF_CC_TRACE(GetSystemControllersSet, "Copying the content of internal data structures into input argument!");
		for (std::map<std::string, std::string>::const_iterator it = _controllers_map.begin();
				it != _controllers_map.end(); ++it) sc_set.insert(it->first);
	TSL_CRITICAL_SECTION_END;
	return ERR_NO_ERROR;
}

int __TSL_CLASS_NAME__::get_payloads_set (std::set<std::string> & pl_set) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(GetPayloadsSet, "Received a request to retrieve the list of the payload nodes in the cluster!");

	TSL_CRITICAL_SECTION_BEGIN(sync_t, _configuration_sync);
		if (!(_controllers_map.size()) && !(_payloads_map.size())) {
			// Info not yet loaded, load it from IMM
			HCF_CC_TRACE(GetPayloadsSet, "Configuration not yet loaded, querying IMM!");
			if (const int call_result = load_nodes_from_imm()) {
				HCF_CC_TRACE(GetSystemControllersSetErr, "Call 'load_nodes_from_imm' failed.", call_result);
				return call_result;
			}
		}

		// After loading the info from IMM (if not yet done), fill the input set
		HCF_CC_TRACE(GetPayloadsSet, "Copying the content of internal data structures into input argument!");
		for (std::map<std::string, std::string>::const_iterator it = _payloads_map.begin();
				it != _payloads_map.end(); ++it) pl_set.insert(it->first);
	TSL_CRITICAL_SECTION_END;
	return ERR_NO_ERROR;
}

int __TSL_CLASS_NAME__::get_nodes_set (std::set<std::string> & nodes_set) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(GetNodesSet, "Received a request to retrieve the list of all the nodes in the cluster!");
	/*
	 * To retrieve the set of all the configured nodes in the cluster,
	 * first retrieve the set of the defined controller nodes, and then
	 * retrieve the set of the defined payload nodes.
	 */
	HCF_CC_TRACE(GetNodesSet, "First, retrieve the list of the system controller nodes!");
	if (const int call_result = get_system_controllers_set(nodes_set)) {
		HCF_CC_TRACE(GetNodesSetErr, "Call 'get_system_controllers_set' failed.", call_result);
		return call_result;
	}

	HCF_CC_TRACE(GetNodesSet, "Then, retrieve also the list of the payload nodes!");
	if (const int call_result = get_payloads_set(nodes_set)) {
		/*
		 * In case this second call fails, clear the info previously
		 * loaded in order to avoid to return partial info.
		 */
		HCF_CC_TRACE(GetNodesSetErr, "Call 'get_payloads_set' failed.", call_result);
		nodes_set.clear();
		return call_result;
	}
	return ERR_NO_ERROR;
}

const char * __TSL_CLASS_NAME__::get_node_hostname (const std::string & node) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(GetHostname, "Received a request to retrieve the host name for the following node:", node.c_str());

	TSL_CRITICAL_SECTION_BEGIN(sync_t, _configuration_sync);
		// Based on the type of the provided node, retrieve the info from the correct data structure
		if (node.find(HCF_SYSTEM_CONTROLLER_NODE_PREFIX) != std::string::npos) {
			HCF_CC_TRACE(GetHostname, "The provided node is a system controller, retrieving from its data structure!", node.c_str());
			std::map<std::string, std::string>::const_iterator it = _controllers_map.find(node);
			return (it != _controllers_map.end()) ? it->second.c_str() : 0;
		}
		else if (node.find(HCF_PAYLOAD_NODE_PREFIX) != std::string::npos) {
			HCF_CC_TRACE(GetHostname, "The provided node is a payload, retrieving from its data structure!", node.c_str());
			std::map<std::string, std::string>::const_iterator it = _payloads_map.find(node);
			return (it != _payloads_map.end()) ? it->second.c_str() : 0;
		}

		HCF_CC_TRACE(GetHostname, "The node was not found in the cluster configuration!", node.c_str());
		HCF_CC_LOG(LEVEL_WARNING, "The node having ID == '%s' is UNKNOWN!", node.c_str());
	TSL_CRITICAL_SECTION_END;
	return 0;
}

int __TSL_CLASS_NAME__::get_current_hostname (char * hostname, size_t & length) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(GetHostname, "Received a request to retrieve the current host name!", "");

	if (::gethostname(hostname, length)) {
		const int errno_save = errno;
		HCF_CC_TRACE(GetHostnameErr, "Call 'gethostname' failed.", errno_save);
		HCF_CC_LOG(LEVEL_ERROR, "Call 'gethostname' failed, errno == %d", errno_save);
		return -1;
	}
	length = ::strlen(hostname);
	HCF_CC_TRACE(GetHostname, "The retrieved host name is:", hostname);
	return 0;
}

int __TSL_CLASS_NAME__::get_addr_by_hostname (const char * hostname, struct sockaddr * host_addr) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(GetAddrByHostname, "Received a request to retrieve the IP address of the following host:", hostname);
	struct addrinfo * addr_info;
	struct addrinfo hints;

	// Set to 0 each field in the hints structure and then select to retrieve only IPv4 and TCP addresses
	::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	HCF_CC_TRACE(GetAddrByHostname, "Retrieving the host IP address!", hostname);
	if (const int call_result = ::getaddrinfo(hostname, 0, &hints, &addr_info)) {
		HCF_CC_TRACE(GetAddrByHostnameErr, "Call 'getaddrinfo' failed.", call_result, ::gai_strerror(call_result));
		HCF_CC_LOG(LEVEL_ERROR, "Call 'getaddrinfo' failed, call_result == %d, error_message == '%s'", call_result, ::gai_strerror(call_result));
		return -1;
	}
	::memcpy(host_addr, addr_info->ai_addr, sizeof(struct sockaddr));
	::freeaddrinfo(addr_info);
	return 0;
}

const char * __TSL_CLASS_NAME__::get_internal_output_folder () {
	HCF_CC_TRACE_FUNCTION;
	TSL_CRITICAL_SECTION_BEGIN(sync_t, _configuration_sync);
		if (!(*_internal_output_folder_path)) {
			HCF_CC_TRACE(GetInternalOutputFolder, "The output folder wasn't yet retrieved, calculating it!", "");
			::snprintf(_internal_output_folder_path, TSL_ARRAY_SIZE(_internal_output_folder_path), "%s/%s",
					TSL_NS_HCF_INTERNALAPI::internalapi_constants::get_hcf_cluster_nobackup_path(), HCF_OUTPUT_FOLDER_NAME);
		}
	TSL_CRITICAL_SECTION_END;
	HCF_CC_TRACE(GetInternalOutputFolder, "Returning the value of the HCF internal output folder!", _internal_output_folder_path);
	return _internal_output_folder_path;
}

int __TSL_CLASS_NAME__::create_output_folder () {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(CreateOutputFolder, "Received a request to create the HCF output folder!", get_internal_output_folder());
	if (TSL_NS_HCF_CORE::utils::create_dir_recursive(get_internal_output_folder()) < 0) {
		const int errno_save = errno;
		HCF_CC_TRACE(CreateOutputFolderErr, "Call 'utils::create_dir_recursive' failed.", errno_save);
		HCF_CC_LOG(LEVEL_ERROR, "Call 'utils::create_dir_recursive' failed for folder '%s', errno == %d", get_internal_output_folder(), errno_save);
		return -1;
	}

#ifdef HCF_CC_RUNS_WITH_NO_ROOT_PERMISSIONS
	HCF_CC_TRACE(CreateOutputFolder, "Support for not-root permissions enabled: setting the correct permissions on the folder!", get_internal_output_folder());
	if (::chmod(get_internal_output_folder(), S_ISGID | S_IRWXU | S_IRWXG)) {
		const int errno_save = errno;
		HCF_CC_TRACE(CreateOutputFolderErr, "Call 'chmod' failed.", errno_save);
		HCF_CC_LOG(LEVEL_ERROR, "Call 'chmod' failed for folder '%s', errno == %d", get_internal_output_folder(), errno_save);
		return -2;
	}

	char cmd[1024];
	HCF_CC_TRACE(CreateOutputFolder, "Support for not-root permissions enabled: taking the ownership of the folder!", get_internal_output_folder());
	::snprintf(cmd, TSL_ARRAY_SIZE(cmd), "sudo chown -f %s:%s %s", HCF_SRV_USER_NAME, HCF_SYSTEM_NBI_DATA_GROUP_NAME, get_internal_output_folder());
	::system(cmd);
#endif
	return 0;
}

const char * __TSL_CLASS_NAME__::get_output_folder () {
	HCF_CC_TRACE_FUNCTION;
	TSL_CRITICAL_SECTION_BEGIN(sync_t, _configuration_sync);
		if (!(*_output_folder_path)) {
			/*
			 * Different scenarios are possible:
			 *		(A)	The NBI Filesystem API is provided by LDE (on the node is installed LDE > 2.5):
			 *			1. The NBI path is always available in the '/usr/share/ericsson/cba/nbi-root-dir' configuration file.
			 *
			 *		(B) The NBI Filesystem API is provided by COM (otherwise):
			 *			1. FileM is installed:
			 *				1.1. FileM path defined in the /usr/share/filem/internal_filem_root.conf file.
			 *				1.2. Default FileM path (/var/filem/internal_root)
			 * 			2. FileM is not installed. No FileM path available.
			 *
			 * In order to distinguish between (A) and (B) the existence of (A)'s configuration file could be checked,
			 * since in the (A) case this file is ALWAYS present on the node.
			 */
			*_output_folder_path = 0;	// By default, assuming NBI path cannot be retrieved

			HCF_CC_TRACE(GetOutputFolder, "Trying to open the FileM configuration file:", HCF_FILEM_CONFIGURATION_FILE_PATH);
			int nbi_root_fd = -1;
			if ((nbi_root_fd = ::open(HCF_NBI_API_CONFIGURATION_FILE_PATH, O_RDONLY)) > 0) {
				/*
				 * Configuration file successfully opened: this means that we are in case (A).
				 * Read the NBI root path from the opened configuration file.
				 */
				char buffer[4 * 1024];
				size_t bytes_read = 0;
				HCF_CC_TRACE(GetOutputFolder, "FileM configuration file successfully opened: trying to read its value!", HCF_FILEM_CONFIGURATION_FILE_PATH);
				if ((bytes_read = ::read(nbi_root_fd, buffer, TSL_ARRAY_SIZE(buffer))) > 0) {
					buffer[bytes_read - 1] = 0;
					::snprintf(_output_folder_path, TSL_ARRAY_SIZE(_output_folder_path), "%s/%s", buffer, HCF_OUTPUT_FOLDER_NAME);
					HCF_CC_TRACE(GetOutputFolder, "FileM configuration file successfully read; the output folder path is:", _output_folder_path);
				}
				::close(nbi_root_fd);
			}
			/*
			 * Failed to open the NBI Filesystem API configuration file.
			 * Assuming we are in the case (B): let's try to open the FileM configuration file.
			 */
			else if ((nbi_root_fd = ::open(HCF_FILEM_CONFIGURATION_FILE_PATH, O_RDONLY)) > 0) {
				/*
				 * Configuration file successfully opened: this means that we are in case (B).
				 * Read the NBI root path from the opened configuration file.
				 */
				char buffer[4 * 1024];
				size_t bytes_read = 0;
				if ((bytes_read = ::read(nbi_root_fd, buffer, TSL_ARRAY_SIZE(buffer))) > 0) {
					buffer[bytes_read - 1] = 0;
					::snprintf(_output_folder_path, TSL_ARRAY_SIZE(_output_folder_path), "%s/%s", buffer, HCF_OUTPUT_FOLDER_NAME);
				}
				::close(nbi_root_fd);
			}
			else {
				// Configuration file not found: use the default FileM path (if it exists)
				struct stat info;
				HCF_CC_TRACE(GetOutputFolder, "FileM configuration file cannot be opened: using the default one (if exists)", HCF_FILEM_INTERNAL_ROOT_DEFAULT_PATH);
				if (!stat(HCF_FILEM_INTERNAL_ROOT_DEFAULT_PATH, &info)) {
					// The FileM default path exists: set the filem_path variable
					::snprintf(_output_folder_path, TSL_ARRAY_SIZE(_output_folder_path), "%s/%s", HCF_FILEM_INTERNAL_ROOT_DEFAULT_PATH, HCF_OUTPUT_FOLDER_NAME);
					HCF_CC_TRACE(GetOutputFolder, "The FileM default path exists, the output folder path is:", _output_folder_path);
				}
			}
		}
	TSL_CRITICAL_SECTION_END;
	return _output_folder_path;
}

int __TSL_CLASS_NAME__::link_nbi_to_output_folder () {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(LinkNBIToOutputFolder, "Received a request to link the HCF internal output folder to the NBI output folder!", get_internal_output_folder(), get_output_folder());
	if (::symlink(get_internal_output_folder(), get_output_folder())) {
		const int errno_save = errno;
		if (errno_save != EEXIST) {
			HCF_CC_TRACE(LinkNBIToOutputFolderErr, "Call 'symlink' failed.", errno_save);
			HCF_CC_LOG(LEVEL_ERROR, "Call 'symlink' failed for folder '%s' and link '%s', errno == %d",
					get_internal_output_folder(), get_output_folder(), errno_save);
			return -1;
		}
	}
	return 0;
}

void __TSL_CLASS_NAME__::clear_loaded_configuration (configuration_type_t conf_to_delete_mask) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(ClearLoadedConfiguration, "Received a request to erase the loaded configuration!", conf_to_delete_mask);

	// Clear all the configuration info according to the provided flags
	TSL_CRITICAL_SECTION_BEGIN(sync_t, _configuration_sync);
		// Delete the loaded configuration about the ManagedElementId value
		if (conf_to_delete_mask & CONFIGURATION_TYPE_MEID) {
			*_managed_element_id = 0;
		}

		// Delete the loaded configuration about the predefined HCF folders
		if (conf_to_delete_mask & CONFIGURATION_TYPE_FOLDERS) {
			*_internal_output_folder_path = 0;
			*_output_folder_path = 0;
		}

		// Delete the loaded configuration about the nodes belonging to the cluster
		if (conf_to_delete_mask & CONFIGURATION_TYPE_NODES) {
			_controllers_map.clear();
			_payloads_map.clear();
		}
	TSL_CRITICAL_SECTION_END;
}

int __TSL_CLASS_NAME__::load_nodes_from_imm () {
	HCF_CC_TRACE_FUNCTION;

	// Initialize the OM Handler object
	TSL_NS_HCF_IMM::om_handler om_handler;
	HCF_CC_TRACE(LoadNodesFromImm, "Initializing the internal OM Handler object!", "");
	if (om_handler.Init() != TSL_NS_HCF_IMM::OPERATION_SUCCESS) {
		HCF_CC_TRACE(LoadNodesFromImmErr, "Call 'om_handler.Init' failed.", om_handler.getInternalLastError(), om_handler.getInternalLastErrorText());
		HCF_CC_LOG(LEVEL_ERROR, "Call 'om_handler.Init()' failed, error_code == %d, error_message == '%s'",
				om_handler.getInternalLastError(), om_handler.getInternalLastErrorText());
		return TSL_NS_HCF_IMM::ERR_OM_HANDLER_INIT_FAILED;
	}

	// Retrieve all the instances of the AmfNode MOC
	std::vector<std::string> nodes_dns;
	int imm_error_code = TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_IMM_TRY_AGAIN;
	for (int retries_count = HCF_IMM_OPERATIONS_MAX_NUMBER_OF_RETRIES; (retries_count > 0) && (imm_error_code == TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_IMM_TRY_AGAIN); --retries_count) {
		HCF_CC_TRACE(LoadNodesFromImm, "Trying to retrieve all the instances of the following class:", HCF_AMF_NODE_CLASS_NAME);
		if (om_handler.getClassInstances(HCF_AMF_NODE_CLASS_NAME, nodes_dns) != TSL_NS_HCF_IMM::OPERATION_SUCCESS) {
			imm_error_code = om_handler.getInternalLastError();
			HCF_CC_TRACE(LoadNodesFromImmErr, "Call 'om_handler.getClassInstances' failed.", om_handler.getInternalLastError(), om_handler.getInternalLastErrorText());
			HCF_CC_LOG(LEVEL_ERROR, "Call 'om_handler.getClassInstances()' failed (RETRY == %d) for class '%s', "
					"error_code == %d, error_message == '%s'", (HCF_IMM_OPERATIONS_MAX_NUMBER_OF_RETRIES - retries_count + 1),
					HCF_AMF_NODE_CLASS_NAME, om_handler.getInternalLastError(), om_handler.getInternalLastErrorText());

			// Wait for a while if the error is "TRY AGAIN": this could solve the IMM problem!!!
			if (imm_error_code == TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_IMM_TRY_AGAIN)	::sleep(1);
		}
		else imm_error_code = TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_NO_ERR;
	}
	om_handler.Finalize();

	if (imm_error_code != TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_NO_ERR) {
		HCF_CC_TRACE(LoadNodesFromImmErr, "Failed to retrieve the instances of the SaAMFNode class!", imm_error_code, "");
		return TSL_NS_HCF_IMM::ERR_OM_HANDLER_GET_INSTANCES_FAILED;
	}

	// For each retrieved value, check if it's a SC node or a PL one and fill-in accordingly the class members
	for (size_t i = 0; i < nodes_dns.size(); ++i) {
		HCF_CC_TRACE(LoadNodesFromImm, "Parsing the following node DN:", nodes_dns[i].c_str());

		// FIRST: extract from the DN the node ID
		size_t equal_pos = nodes_dns[i].find_first_of('=');
		size_t comma_pos = nodes_dns[i].find_first_of(',');
		if ((equal_pos == std::string::npos) || (comma_pos == std::string::npos)) {
			HCF_CC_TRACE(LoadNodesFromImmErr, "The node DN is not correctly formed, skipping it!!", -1, "");
			HCF_CC_LOG(LEVEL_WARNING, "The node DN (%s) is not correctly formed, skipping it...", nodes_dns[i].c_str());
			continue;
		}
		std::string node_id = nodes_dns[i].substr(equal_pos + 1, comma_pos - equal_pos - 1);
		HCF_CC_TRACE(LoadNodesFromImm, "Retrieving the hostname for the following node identifier:", node_id.c_str());

		/*
		 * SECOND: retrieve from the system configuration the hostname associated to the retrieved node.
		 * To do this it's required to extract from the node id the node progressive number (that is
		 * after the '-' character) and access to the file system to retrieve the hostname associated to it.
		 */
		char node_config_file[PATH_MAX + 1];
		::snprintf(node_config_file, TSL_ARRAY_SIZE(node_config_file), "%s/%s/%s", HCF_CLUSTER_NODES_CONFIG_FILE_PATH,
				node_id.substr(node_id.find_first_of('-') + 1).c_str(), HCF_CLUSTER_NODES_HOSTNAME_FILE_NAME);

		int config_file_fd = -1;
		HCF_CC_TRACE(LoadNodesFromImm, "Trying to open the following configuration file:", node_config_file);
		if ((config_file_fd = ::open(node_config_file, O_RDONLY)) < 0) {
			const int errno_save = errno;
			HCF_CC_TRACE(LoadNodesFromImmErr, "Call 'open' failed.", errno_save, "");
			HCF_CC_LOG(LEVEL_ERROR, "Failed to open the file '%s' for the node '%s', errno == %d", node_config_file, node_id.c_str(), errno_save);
			continue;
		}

		// After opening the file, read from it the node hostname value
		char buffer[1024];
		ssize_t bytes_read = -1;
		HCF_CC_TRACE(LoadNodesFromImm, "Trying to read from the following configuration file:", node_config_file);
		if ((bytes_read = ::read(config_file_fd, buffer, TSL_ARRAY_SIZE(buffer))) < 0) {
			const int errno_save = errno;
			HCF_CC_TRACE(LoadNodesFromImmErr, "Call 'read' failed.", errno_save, "");
			HCF_CC_LOG(LEVEL_ERROR, "Failed to read the file '%s' for the node '%s', errno == %d", node_config_file, node_id.c_str(), errno_save);
			::close(config_file_fd);
			continue;
		}
		::close(config_file_fd);

		// Set the termination character at the first not printable character or at the end of the string
		size_t j = 0;
		for (; (j < TSL_ARRAY_SIZE(buffer)) && (::isprint(buffer[j])); ++j) ;
		buffer[j] = 0;

		// THIRD: After getting the hostname value, check the node type and fill the correct structure
		if (node_id.find(HCF_SYSTEM_CONTROLLER_NODE_PREFIX) != std::string::npos)
			_controllers_map[node_id] = buffer;
		else if (node_id.find(HCF_PAYLOAD_NODE_PREFIX) != std::string::npos)
			_payloads_map[node_id] = buffer;
		else {
			HCF_CC_TRACE(LoadNodesFromImmErr, "The retrieved node identifier is unknown!", -1, "");
			HCF_CC_LOG(LEVEL_WARNING, "The node having ID == '%s' is UNKNOWN!", node_id.c_str());
		}
	}

	HCF_CC_LOG(LEVEL_NOTICE, "The cluster in built of %zu nodes: %zu controllers and %zu payloads",
			(_controllers_map.size() + _payloads_map.size()), _controllers_map.size(), _payloads_map.size());
	return ERR_NO_ERROR;
}

void __TSL_CLASS_NAME__::set_managed_element_id (const char * meid) {
	HCF_CC_TRACE_FUNCTION;
	TSL_CRITICAL_SECTION_BEGIN(sync_t, _configuration_sync);
		::strncpy(_managed_element_id, meid, TSL_ARRAY_SIZE(_managed_element_id));
	TSL_CRITICAL_SECTION_END;
}

HCF_NS_HCF_SRV_END
