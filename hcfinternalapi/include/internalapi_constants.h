#ifndef __INCLUDE_GUARD_CLASS_INTERNALAPI_CONSTANTS__
#define __INCLUDE_GUARD_CLASS_INTERNALAPI_CONSTANTS__ internalapi_constants

/*
 *	@file internalapi_constants.h
 *	@brief
 *	@author xcasale (Alessio Cascone)
 *	@date 2014-12-29
 *
 *	COPYRIGHT Ericsson AB, 2014
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
 *	| 2014-001 | 2014-12-29 | xcasale      | Creation and first revision.        |
 *	| 2015-001 | 2015-10-27 | xcasale      | Created internalapi constants class.|
 *	+==========+============+==============+=====================================+
 */

#include <linux/limits.h>

#include "hcf/core/thread_mutex.h"

#include "internalapi_macros.h"

TSL_NS_HCF_INTERNALAPI_BEGIN

enum ErrorConstants {
	ERR_NO_ERROR = 0
	, ERR_MEMORY_ALLOCATION_FAILED = 1
	, ERR_UNPACK_PRIMITIVE_FAILED  = 2
	, ERR_NOT_ENOUGH_SPACE         = 3
	, ERR_NO_PRIMITIVE_TO_SEND     = 4
	, ERR_PRIMITIVE_SEND_FAILED    = 5
	, ERR_PRIMITIVE_RECV_FAILED    = 6
};

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_INTERNALAPI_CONSTANTS__

class __TSL_CLASS_NAME__ {
	//==============//
	// Constructors //
	//==============//
private:
	TSL_CTOR ();
	TSL_CTOR (const __TSL_CLASS_NAME__ &);

	//============//
	// Destructor //
	//============//
public:
	inline TSL_DTOR () {}

	//==========//
	// Typedefs //
	//==========//
private:
	typedef TSL_NS_HCF_CORE::thread_mutex_recursive sync_t;

	//===========//
	// Functions //
	//===========//
public:
	/**
	 * This method retrieves the path of the HCF internal folder where the data that doesn't
	 * need to participate to backup/restore operations is stored.
	 *
	 * @return the no-backup path under /cluster file system.
	 */
	static const char * get_hcf_cluster_nobackup_path ();

	/**
	 * This method retrieves the path of the HCF internal folder where the data that need
	 * to participate to backup/restore operations is stored.
	 *
	 * @return the backup path under /cluster file system.
	 */
	static const char * get_hcf_cluster_backup_path ();

	/**
	 * This method retrieves the path of the HCF internal folder where the installed RSFs are stored.
	 *
	 * @return the folder where the RSFs are installed.
	 */
	static const char * get_rsf_installed_path ();

	/**
	 * This method retrieves the path of the HCF internal folder where the RSFs to be installed are stored.
	 *
	 * @return the folder where the RSFs to be installed are stored.
	 */
	static const char * get_rsf_installing_path ();

	/**
	 * This method retrieves the path of the HCF internal folder where the RSFs to be deleted are stored.
	 *
	 * @return the folder where the RSFs to be deleted are stored.
	 */
	static const char * get_rsf_deleting_path ();

	/**
	 * This method retrieves the path of the HCF internal folder where the output files to be sent to a remote destination are stored.
	 *
	 * @return the folder where the output files to be sent to a remote destination are stored.
	 */
	static const char * get_job_exporting_path ();

	/**
	 * This method retrieves the path of the schema file for the HCF RSF.
	 *
	 * @return the path where the RSF schema file is stored.
	 */
	static const char * get_rsf_schema_file_path ();

	/**
	 * This method retrieves the path of file to store the information about the
	 * Command Handler server, used internally by HCF and hcrsfm command.
	 *
	 * @return the path where the internal file is stored.
	 */
	static const char * get_cmd_server_address_file_path ();

	/**
	 * This method deletes all the configuration information previously loaded.
	 */
	static void clear_loaded_configuration ();

	//===========//
	// Operators //
	//===========//
private:
	__TSL_CLASS_NAME__ & operator= (const __TSL_CLASS_NAME__ &);

	//========//
	// Fields //
	//========//
private:
	static const char * const HCF_CLUSTER_FOLDER_NAME;
	static const char * const HCF_PSO_NOBACKUP_PATH_CONF_FILE;
	static const char * const HCF_PSO_NOBACKUP_PATH_CONF_FILE2;
	static const char * const HCF_PSO_CONFIG_PATH_CONF_FILE;
	static const char * const HCF_PSO_CONFIG_PATH_CONF_FILE2;
	static const char * const HCF_RSF_INSTALLED_FOLDER_NAME;
	static const char * const HCF_RSF_INSTALLING_FOLDER_NAME;
	static const char * const HCF_RSF_DELETING_FOLDER_NAME;
	static const char * const HCF_JOB_EXPORTING_FOLDER_NAME;
	static const char * const HCF_RSF_SCHEMA_FILE_NAME;
	static const char * const HCF_CMD_SERVER_ADDRESS_FILE_NAME;

	static TSL_NS_HCF_CORE::thread_mutex_recursive _sync;
	static char _hcf_cluster_nobackup_path[PATH_MAX + 1];
	static char _hcf_cluster_config_backup_path[PATH_MAX + 1];
	static char _hcf_rsf_installed_path[PATH_MAX + 1];
	static char _hcf_rsf_installing_path[PATH_MAX + 1];
	static char _hcf_rsf_deleting_path[PATH_MAX + 1];
	static char _hcf_job_exporting_path[PATH_MAX + 1];
	static char _hcf_rsf_schema_file_path[PATH_MAX + 1];
	static char _hcf_cmd_server_address_file_path[PATH_MAX + 1];
};

TSL_NS_HCF_INTERNALAPI_END

#endif /* __INCLUDE_GUARD_CLASS_INTERNALAPI_CONSTANTS__ */
