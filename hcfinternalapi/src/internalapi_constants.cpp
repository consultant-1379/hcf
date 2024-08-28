#include <fcntl.h>

#include "hcf/core/guard.h"

#include "hcf/trace/com_ericsson_common_hcf_internal_internalapiconstants.h"
#include "hcf/trace/tracing_macros.h"

#include "internalapi_constants.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_INTERNALAPI_CONSTANTS__

TSL_NS_HCF_INTERNALAPI_BEGIN

// Static fields initialization
const char * const __TSL_CLASS_NAME__::HCF_CLUSTER_FOLDER_NAME          = "hcf-apr9010574";
const char * const __TSL_CLASS_NAME__::HCF_PSO_NOBACKUP_PATH_CONF_FILE  = "/usr/share/pso/storage-paths/no-backup";
const char * const __TSL_CLASS_NAME__::HCF_PSO_NOBACKUP_PATH_CONF_FILE2 = "/usr/share/ericsson/cba/pso/storage-paths/no-backup";
const char * const __TSL_CLASS_NAME__::HCF_PSO_CONFIG_PATH_CONF_FILE    = "/usr/share/pso/storage-paths/config";
const char * const __TSL_CLASS_NAME__::HCF_PSO_CONFIG_PATH_CONF_FILE2   = "/usr/share/ericsson/cba/pso/storage-paths/config";
const char * const __TSL_CLASS_NAME__::HCF_RSF_INSTALLED_FOLDER_NAME    = "installed";
const char * const __TSL_CLASS_NAME__::HCF_RSF_INSTALLING_FOLDER_NAME   = "installing";
const char * const __TSL_CLASS_NAME__::HCF_RSF_DELETING_FOLDER_NAME     = "deleting";
const char * const __TSL_CLASS_NAME__::HCF_JOB_EXPORTING_FOLDER_NAME    = "exporting";
const char * const __TSL_CLASS_NAME__::HCF_RSF_SCHEMA_FILE_NAME         = "hcf_rsf_schema.xsd";
const char * const __TSL_CLASS_NAME__::HCF_CMD_SERVER_ADDRESS_FILE_NAME = ".srv_addr";

TSL_NS_HCF_CORE::thread_mutex_recursive __TSL_CLASS_NAME__::_sync;
char __TSL_CLASS_NAME__::_hcf_cluster_nobackup_path[PATH_MAX + 1]        = {0};
char __TSL_CLASS_NAME__::_hcf_cluster_config_backup_path[PATH_MAX + 1]   = {0};
char __TSL_CLASS_NAME__::_hcf_rsf_installed_path[PATH_MAX + 1]           = {0};
char __TSL_CLASS_NAME__::_hcf_rsf_installing_path[PATH_MAX + 1]          = {0};
char __TSL_CLASS_NAME__::_hcf_rsf_deleting_path[PATH_MAX + 1]            = {0};
char __TSL_CLASS_NAME__::_hcf_job_exporting_path[PATH_MAX + 1]           = {0};
char __TSL_CLASS_NAME__::_hcf_rsf_schema_file_path[PATH_MAX + 1]         = {0};
char __TSL_CLASS_NAME__::_hcf_cmd_server_address_file_path[PATH_MAX + 1] = {0};

HCF_CC_FUNCTION_TRACER_CLASS_DEFINE

const char * __TSL_CLASS_NAME__::get_hcf_cluster_nobackup_path () {
	HCF_CC_TRACE_FUNCTION;
	TSL_CRITICAL_SECTION_BEGIN(sync_t, _sync);
		if (!(*_hcf_cluster_nobackup_path)) {
			// The desired path wasn't yet calculated: retrieve it!
			HCF_CC_TRACE(GetClusterNoBackupPath, "The HCF no-backup path under /cluster filesystem wasn't"
					" yet calculated, opening the following PSO configuration file: ", HCF_PSO_NOBACKUP_PATH_CONF_FILE);

			int conf_file_fd = -1;
			if ((conf_file_fd = ::open(HCF_PSO_NOBACKUP_PATH_CONF_FILE, O_RDONLY)) < 0) {
				HCF_CC_TRACE(GetClusterNoBackupPath, "The first configuration file was not "
						"present: using the following PSO configuration file: ", HCF_PSO_NOBACKUP_PATH_CONF_FILE2);

				// The first configuration file was not present, let's try with the second one!
				if ((conf_file_fd = ::open(HCF_PSO_NOBACKUP_PATH_CONF_FILE2, O_RDONLY)) < 0) {
					const int errno_save = errno;
					HCF_CC_TRACE(GetClusterNoBackupPathErr, "Call 'open' failed. ", errno_save);
				}
			}

			char buffer[PATH_MAX + 1] = {0};
			if (conf_file_fd != -1) {
				// A valid configuration file has been opened: read its data
				HCF_CC_TRACE(GetClusterNoBackupPath, "A valid configuration file has been found: reading the path stored in it!", "");
				ssize_t bytes_read = -1;
				if ((bytes_read = ::read(conf_file_fd, buffer, TSL_ARRAY_SIZE(buffer))) > 0)	buffer[bytes_read - 1] = 0;
				else {
					const int errno_save = errno;
					HCF_CC_TRACE(GetClusterNoBackupPathErr, "Call 'read' failed. ", errno_save);
				}

				HCF_CC_TRACE(GetClusterNoBackupPath, "The value read from the PSO configuration file is:", buffer);
				::close(conf_file_fd);
			}

			// All the needed data is available, build the requested path
			::snprintf(_hcf_cluster_nobackup_path, TSL_ARRAY_SIZE(_hcf_cluster_nobackup_path), "%s/%s", buffer, HCF_CLUSTER_FOLDER_NAME);
		}
	TSL_CRITICAL_SECTION_END;
	return _hcf_cluster_nobackup_path;
}

const char * __TSL_CLASS_NAME__::get_hcf_cluster_backup_path () {
	HCF_CC_TRACE_FUNCTION;
	TSL_CRITICAL_SECTION_BEGIN(sync_t, _sync);
		if (!(*_hcf_cluster_config_backup_path)) {
			// The desired path wasn't yet calculated: retrieve it!
			HCF_CC_TRACE(GetClusterBackupPath, "The HCF backup path under /cluster filesystem wasn't"
					" yet calculated, opening the following PSO configuration file: ", HCF_PSO_CONFIG_PATH_CONF_FILE);

			int conf_file_fd = -1;
			if ((conf_file_fd = ::open(HCF_PSO_CONFIG_PATH_CONF_FILE, O_RDONLY)) < 0) {
				HCF_CC_TRACE(GetClusterBackupPath, "The first configuration file was not "
						"present: using the following PSO configuration file: ", HCF_PSO_CONFIG_PATH_CONF_FILE2);

				// The first configuration file was not present, let's try with the second one!
				if ((conf_file_fd = ::open(HCF_PSO_CONFIG_PATH_CONF_FILE2, O_RDONLY)) < 0) {
					const int errno_save = errno;
					HCF_CC_TRACE(GetClusterBackupPathErr, "Call 'open' failed. ", errno_save);
				}
			}

			char buffer[PATH_MAX + 1] = {0};
			if (conf_file_fd != -1) {
				// A valid configuration file has been opened: read its data
				HCF_CC_TRACE(GetClusterBackupPath, "A valid configuration file has been found: reading the path stored in it!", "");
				ssize_t bytes_read = -1;
				if ((bytes_read = ::read(conf_file_fd, buffer, TSL_ARRAY_SIZE(buffer))) > 0)	buffer[bytes_read - 1] = 0;
				else {
					const int errno_save = errno;
					HCF_CC_TRACE(GetClusterBackupPathErr, "Call 'read' failed. ", errno_save);
				}

				HCF_CC_TRACE(GetClusterBackupPath, "The value read from the PSO configuration file is:", buffer);
				::close(conf_file_fd);
			}

			// All the needed data is available, build the requested path
			::snprintf(_hcf_cluster_config_backup_path, TSL_ARRAY_SIZE(_hcf_cluster_config_backup_path), "%s/%s", buffer, HCF_CLUSTER_FOLDER_NAME);
		}
	TSL_CRITICAL_SECTION_END;
	return _hcf_cluster_config_backup_path;
}

const char * __TSL_CLASS_NAME__::get_rsf_installed_path () {
	HCF_CC_TRACE_FUNCTION;
	TSL_CRITICAL_SECTION_BEGIN(sync_t, _sync);
		if (!(*_hcf_rsf_installed_path)) {	// The requested path wasn't yet calculated, build it!
			::snprintf(_hcf_rsf_installed_path, TSL_ARRAY_SIZE(_hcf_rsf_installed_path), "%s/%s", get_hcf_cluster_backup_path(), HCF_RSF_INSTALLED_FOLDER_NAME);
		}
	TSL_CRITICAL_SECTION_END;
	return _hcf_rsf_installed_path;
}

const char * __TSL_CLASS_NAME__::get_rsf_installing_path () {
	HCF_CC_TRACE_FUNCTION;
	TSL_CRITICAL_SECTION_BEGIN(sync_t, _sync);
		if (!(*_hcf_rsf_installing_path)) {	// The requested path wasn't yet calculated, build it!
			::snprintf(_hcf_rsf_installing_path, TSL_ARRAY_SIZE(_hcf_rsf_installing_path), "%s/%s", get_hcf_cluster_backup_path(), HCF_RSF_INSTALLING_FOLDER_NAME);
		}
	TSL_CRITICAL_SECTION_END;
	return _hcf_rsf_installing_path;
}

const char * __TSL_CLASS_NAME__::get_rsf_deleting_path () {
	HCF_CC_TRACE_FUNCTION;
	TSL_CRITICAL_SECTION_BEGIN(sync_t, _sync);
		if (!(*_hcf_rsf_deleting_path)) {	// The requested path wasn't yet calculated, build it!
			::snprintf(_hcf_rsf_deleting_path, TSL_ARRAY_SIZE(_hcf_rsf_deleting_path), "%s/%s", get_hcf_cluster_backup_path(), HCF_RSF_DELETING_FOLDER_NAME);
		}
	TSL_CRITICAL_SECTION_END;
	return _hcf_rsf_deleting_path;
}

const char * __TSL_CLASS_NAME__::get_job_exporting_path () {
	HCF_CC_TRACE_FUNCTION;
	TSL_CRITICAL_SECTION_BEGIN(sync_t, _sync);
		if (!(*_hcf_job_exporting_path)) {	// The requested path wasn't yet calculated, build it!
			::snprintf(_hcf_job_exporting_path, TSL_ARRAY_SIZE(_hcf_job_exporting_path), "%s/%s", get_hcf_cluster_nobackup_path(), HCF_JOB_EXPORTING_FOLDER_NAME);
		}
	TSL_CRITICAL_SECTION_END;
	return _hcf_job_exporting_path;
}

const char * __TSL_CLASS_NAME__::get_rsf_schema_file_path () {
	HCF_CC_TRACE_FUNCTION;
	TSL_CRITICAL_SECTION_BEGIN(sync_t, _sync);
		if (!(*_hcf_rsf_schema_file_path)) {	// The requested path wasn't yet calculated, build it!
			::snprintf(_hcf_rsf_schema_file_path, TSL_ARRAY_SIZE(_hcf_rsf_schema_file_path), "%s/%s", get_hcf_cluster_nobackup_path(), HCF_RSF_SCHEMA_FILE_NAME);
		}
	TSL_CRITICAL_SECTION_END;
	return _hcf_rsf_schema_file_path;
}

const char * __TSL_CLASS_NAME__::get_cmd_server_address_file_path () {
	HCF_CC_TRACE_FUNCTION;
	TSL_CRITICAL_SECTION_BEGIN(sync_t, _sync);
		if (!(*_hcf_cmd_server_address_file_path)) {	// The requested path wasn't yet calculated, build it!
			::snprintf(_hcf_cmd_server_address_file_path, TSL_ARRAY_SIZE(_hcf_cmd_server_address_file_path), "%s/%s", get_hcf_cluster_nobackup_path(), HCF_CMD_SERVER_ADDRESS_FILE_NAME);
		}
	TSL_CRITICAL_SECTION_END;
	return _hcf_cmd_server_address_file_path;
}

void __TSL_CLASS_NAME__::clear_loaded_configuration () {
	HCF_CC_TRACE_FUNCTION;
	TSL_CRITICAL_SECTION_BEGIN(sync_t, _sync);
		*_hcf_cluster_nobackup_path = 0;
		*_hcf_cluster_config_backup_path = 0;
		*_hcf_rsf_installed_path = 0;
		*_hcf_rsf_installing_path = 0;
		*_hcf_rsf_deleting_path = 0;
		*_hcf_job_exporting_path = 0;
		*_hcf_rsf_schema_file_path = 0;
		*_hcf_cmd_server_address_file_path = 0;
	TSL_CRITICAL_SECTION_END;
}

TSL_NS_HCF_INTERNALAPI_END
