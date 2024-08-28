#ifndef __INCLUDE_GUARD_CLASS_SRV_CLASS_CONFIGURATOR__
#define __INCLUDE_GUARD_CLASS_SRV_CLASS_CONFIGURATOR__ configurator

/*
 *	@file configurator.h
 *	@brief
 *	@author xcasale (Alessio Cascone)
 *	@date 2015-02-09
 *
 *	COPYRIGHT Ericsson AB, 2015
 *	All rights reserved.
 *
 *	The information in this document is the property of Ericsson.
 *	Except as specifically authorized in writing by Ericsson, the receiver of
 *	this document shall keep the information contained herein confidential and
 *	shall protect the same in whole or in part from disclosure and dissemination
 *	to third parties. Disclosure and disseminations to the receivers employees
 *	shall only be made on a strict need to know basis.
 *
 *	REVISION INFO
 *	+==========+============+==============+=====================================+
 *	| REV      | DATE       | AUTHOR       | DESCRIPTION                         |
 *	+==========+============+==============+=====================================+
 *	| 2015-001 | 2015-02-09 | xcasale      | Creation and first revision.        |
 *	+==========+============+==============+=====================================+
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <linux/limits.h>
#include <netdb.h>

#include <string>
#include <set>
#include <map>

#include "hcf/core/thread_mutex.h"

#include "srv_macros.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_SRV_CLASS_CONFIGURATOR__

HCF_NS_HCF_SRV_BEGIN

class __TSL_CLASS_NAME__ {
	//=========//
	// Friends //
	//=========//
	friend class configuration_handler;

	//==========//
	// Typedefs //
	//==========//
private:
	typedef TSL_NS_HCF_CORE::thread_mutex_recursive sync_t;

	typedef enum {
		CONFIGURATION_TYPE_UNKNOWN   = 0x00000000
		, CONFIGURATION_TYPE_MEID    = 0x00000001
		, CONFIGURATION_TYPE_FOLDERS = 0x00000002
		, CONFIGURATION_TYPE_NODES   = 0x00000004
		, CONFIGURATION_TYPE_ALL     = 0xFFFFFFFF
	} configuration_type_t;

	//==============//
	// Constructors //
	//==============//
private:
	TSL_CTOR ();
	TSL_CTOR (const __TSL_CLASS_NAME__ & rhs);

	//============//
	// Destructor //
	//============//
public:
	inline virtual TSL_DTOR () {}

	//===========//
	// Functions //
	//===========//
public:
	/**
	 * This method retrieves the ID of the Managed Element.
	 *
	 * @param[in] meid_buffer - The buffer to be filled with the Managed Element ID.
	 * @param[in/out] buffer_size - The maximum number of element that meid_buffer can contain.
	 *                              It's updated with the Managed Element ID size on exit.
	 * @return 0 on success, -1 on failure.
	 */
	static int get_managed_element_id (char * meid_buffer, size_t & buffer_size);

	/**
	 * This method retrieves the set of the System Control nodes defined into the cluster.
	 * It returns the set of the symbolic names associated to each of them.
	 *
	 * @param[out] sc_set - The set of the SC nodes defined into the cluster.
	 * @return 0 on success, -1 on failure.
	 */
	static int get_system_controllers_set (std::set<std::string> & sc_set);

	/**
	 * This method retrieves the set of the Payload nodes defined into the cluster.
	 * It returns the set of the symbolic names associated to each of them.
	 *
	 * @param[out] pl_set - The set of the PL nodes defined into the cluster.
	 * @return 0 on success, -1 on failure.
	 */
	static int get_payloads_set (std::set<std::string> & pl_set);

	/**
	 * This method retrieves the complete set of the nodes defined into the cluster.
	 * It returns the set of the symbolic names associated to each of them.
	 *
	 * @param[out] nodes_set - The set of the nodes defined into the cluster.
	 * @return 0 on success, -1 on failure.
	 */
	static int get_nodes_set (std::set<std::string> & nodes_set);

	/**
	 * This method returns the hostname associated to the given node.
	 *
	 * @param[in]  node - The node for which the hostname must be retrieved.
	 * @return A pointer to the hostname string if existing, NULL otherwise.
	 */
	static const char * get_node_hostname (const std::string & node);

	/**
	 * This method retrieves the name of the current host.
	 *
	 * @param[out]  hostname - The hostname string to be filled.
	 * @param[in/out] length - The hostname string length.
	 * @return 0 on success, -1 on failure.
	 */
	static int get_current_hostname (char * hostname, size_t & length);

	/**
	 * This method retrieves the address for the given hostname.
	 *
	 * @param[in]  hostname - The hostname for which the address must be searched.
	 * @param[out] host_addr - The retrieved address.
	 * @return 0 on success, -1 on failure.
	 */
	static int get_addr_by_hostname (const char * hostname, struct sockaddr * host_addr);

	/**
	 * This method retrieves and returns the path of the HCF internal output folder.
	 *
	 * @return a pointer to a string containing the HCF internal output folder path.
	 */
	static const char * get_internal_output_folder ();

	/**
	 * This method creates (if not yet done) the HCF output folder under /cluster file system.
	 *
	 * @return 0 on success, -1 on failure.
	 */
	static int create_output_folder ();

	/**
	 * This method retrieves HCF output folder available in the FileM NBI folders.
	 *
	 * @return a pointer to a string containing the HCF output NBI folder path.
	 */
	static const char * get_output_folder ();

	/**
	 * This method create a symbolic link into the FileM NBI folder to the HCF output folder under /cluster file system.
	 *
	 * @return 0 on success, -1 on failure.
	 */
	static int link_nbi_to_output_folder ();

	/**
	 * This method deletes all the configuration information previously loaded.
	 *
	 * @param[in] conf_to_delete_mask - A bitwise OR mask providing information about the configuration to be deleted.
	 */
	static void clear_loaded_configuration (configuration_type_t conf_to_delete_mask = CONFIGURATION_TYPE_ALL);

private:
	/**
	 * This method retrieves from IMM the complete information about the nodes configured in the cluster.
	 */
	static int load_nodes_from_imm ();

	/**
	 * This sets the ID of the Managed Element.
	 *
	 * @param[in] meid - The Managed Element ID value.
	 */
	static void set_managed_element_id (const char * meid);

	//===========//
	// Operators //
	//===========//
private:
	__TSL_CLASS_NAME__ & operator=(const __TSL_CLASS_NAME__ & rhs);

	//========//
	// Fields //
	//========//
public:
	static const char * const HCF_SYSTEM_CONTROLLER_NODE_PREFIX;
	static const char * const HCF_PAYLOAD_NODE_PREFIX;
	static const char * const HCF_ALL_NODES_TARGET_VALUE;
	static const char * const HCF_ALL_CONTROLLER_NODES_TARGET_VALUE;
	static const char * const HCF_ALL_PAYLOAD_NODES_TARGET_VALUE;

private:
	static const char * const HCF_AMF_NODE_CLASS_NAME;
	static const char * const HCF_CLUSTER_NODES_CONFIG_FILE_PATH;
	static const char * const HCF_CLUSTER_NODES_HOSTNAME_FILE_NAME;
	static const int HCF_IMM_OPERATIONS_MAX_NUMBER_OF_RETRIES = 3;

	static TSL_NS_HCF_CORE::thread_mutex_recursive _configuration_sync;
	static char _managed_element_id[2048];
	static char _internal_output_folder_path[PATH_MAX + 1];
	static char _output_folder_path[PATH_MAX + 1];

	// Map storing the Cluster configuration: the key is the AMF node name, while the value is the node hostname
	static std::map<std::string, std::string> _controllers_map;
	static std::map<std::string, std::string> _payloads_map;
};

HCF_NS_HCF_SRV_END

#endif /* __INCLUDE_GUARD_CLASS_SRV_CLASS_CONFIGURATOR__ */
