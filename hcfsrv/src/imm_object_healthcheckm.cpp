#include <string.h>

#include "hcf/core/logger.h"

#include "constants.h"
#include "srv_utils.h"
#include "imm_object_healthcheckm.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_SRV_IMM_OBJECT_HEALTHCHECKM__

HCF_NS_HCF_SRV_BEGIN

// Static fields initialization
const char * const __TSL_CLASS_NAME__::HCF_HEALTHCHECKM_CLASS_NAME                         = "HCMHealthCheckM";
const char * const __TSL_CLASS_NAME__::HCF_HEALTHCHECKM_IMPLEMENTER_NAME                   = "HealthCheckM_Implementer";
const char * const __TSL_CLASS_NAME__::HCF_HEALTHCHECKM_HEALTH_CHECK_M_ID_ATTR_NAME        = "HCMhealthCheckMId";
const char * const __TSL_CLASS_NAME__::HCF_HEALTHCHECKM_LAST_EXECUTED_JOB_ATTR_NAME        = "lastExecutedJob";
const char * const __TSL_CLASS_NAME__::HCF_HEALTHCHECKM_STATUS_ATTR_NAME                   = "lastStatus";
const char * const __TSL_CLASS_NAME__::HCF_HEALTHCHECKM_LAST_UPDATE_TIME_ATTR_NAME         = "lastUpdateTime";
const char * const __TSL_CLASS_NAME__::HCF_HEALTHCHECKM_MAX_NO_OF_REPORT_FILES_ATTR_NAME   = "maxNoOfReportFiles";
const char * const __TSL_CLASS_NAME__::HCF_HEALTHCHECKM_ID_DEFAULT_VALUE                   = "1";
const int __TSL_CLASS_NAME__::HCF_HEALTHCHECKM_MAX_NO_OF_REPORT_FILES_DEFAULT_VALUE        = 10;

int __TSL_CLASS_NAME__::set_object_attributes (std::vector<std::pair<std::string, std::vector<void *> > > & attributes, TSL_NS_HCF_IMM::IMM_API_ObjectImplementer_CallbackInfo * TSL_UNUSED_PAR(callback_info)) {
	HCF_CC_TRACE_FUNCTION;
	// Save the old values before modifying them, in order to rollback in case of errors
	_last_executed_job_old      = _last_executed_job;
	_last_status_old            = _last_status;
	_last_update_time_old       = _last_update_time;
	_max_no_of_report_files_old = _max_no_of_report_files;

	// For each of the provided attributes, understand the attribute name and set the class field accordingly
	for (size_t i = 0; i < attributes.size(); ++i) {

		// Extract from the given attributes vector the attribute name and its value
		std::string attr_name = attributes[i].first;
		std::vector<void *> attr_values = attributes[i].second;
		HCF_CC_TRACE(SetObjectAttributes, "Received a request to set/change the value of the following attribute:", attr_name.c_str());

		if (!attr_name.compare(HCF_HEALTHCHECKM_HEALTH_CHECK_M_ID_ATTR_NAME)) {
			if (attr_values.size() > 0) {
				_obj_rdn = reinterpret_cast<char *>(attr_values[0]);
				HCF_CC_TRACE(SetObjectAttributes, "The value of the object RDN is:", _obj_rdn.c_str());
			}
			else {	// ERROR: the ID field cannot be empty!
				HCF_CC_TRACE(SetObjectAttributesErr, "No value provided for the mandatory attribute.", ERR_IMM_MANDATORY_VALUE_MISSING);
				HCF_CC_LOG(LEVEL_ERROR, "No value provided for the mandatory attribute '%s'!", attr_name.c_str());
				set_error_message(HCF_MANDATORY_VALUE_MISSING_ERROR_MSG);
				return ERR_IMM_MANDATORY_VALUE_MISSING;
			}
		}
		else if (!attr_name.compare(HCF_HEALTHCHECKM_LAST_EXECUTED_JOB_ATTR_NAME)) {
			if (attr_values.size() > 0) {
				_last_executed_job = reinterpret_cast<char *>(attr_values[0]);
				HCF_CC_TRACE(SetObjectAttributes, "The value of the lastExecutedJob attribute is:", _last_executed_job.c_str());
			}
			else {} // No value provided, using the default value
		}
		else if (!attr_name.compare(HCF_HEALTHCHECKM_STATUS_ATTR_NAME)) {
			if (attr_values.size() > 0) {
				_last_status = *(reinterpret_cast<health_status_t *>(attr_values[0]));
				HCF_CC_TRACE(SetObjectAttributes, "The provided value of the lastStatus attribute is:", utils::map_health_status_to_string(_last_status));
			}
			else {
				// No value provided, using the default value
				_last_status = HEALTH_STATUS_NOT_AVAILABLE;
				HCF_CC_TRACE(SetObjectAttributes, "No value provided for the lastStatus attribute, used the default one:", utils::map_health_status_to_string(_last_status));
			}
		}
		else if (!attr_name.compare(HCF_HEALTHCHECKM_LAST_UPDATE_TIME_ATTR_NAME)) {
			if (attr_values.size() > 0) {
				_last_update_time = reinterpret_cast<char *>(attr_values[0]);
				HCF_CC_TRACE(SetObjectAttributes, "The value of the lastUpdateTime attribute is:", _last_update_time.c_str());
			}
			else {} // No value provided, using the default value
		}
		else if (!attr_name.compare(HCF_HEALTHCHECKM_MAX_NO_OF_REPORT_FILES_ATTR_NAME)) {
			if (attr_values.size() > 0) {
				_max_no_of_report_files = *(reinterpret_cast<int *>(attr_values[0]));
				HCF_CC_TRACE(SetObjectAttributes, "The provided value of the maxNoOfReportFiles attribute is:", std::to_string(_max_no_of_report_files).c_str());
			}
			else {
				_max_no_of_report_files = HCF_HEALTHCHECKM_MAX_NO_OF_REPORT_FILES_DEFAULT_VALUE;
				HCF_CC_TRACE(SetObjectAttributes, "No value provided for the maxNoOfReportFiles attribute, used the default one:", std::to_string(_max_no_of_report_files).c_str());
			}
		}
	}
	HCF_CC_LOG(LEVEL_NOTICE, "Object attributes correctly modified: RDN == '%s', _last_executed_job == '%s', "
			"_last_status == '%d', _last_update_time == '%s', _max_no_of_report_files == '%d'",	_obj_rdn.c_str(),
			_last_executed_job.c_str(), _last_status, _last_update_time.c_str(), _max_no_of_report_files);
	return ERR_NO_ERROR;
}

int __TSL_CLASS_NAME__::abort_object_modification (TSL_NS_HCF_IMM::IMM_API_ObjectImplementer_CallbackInfo * TSL_UNUSED_PAR(callback_info)) {
	HCF_CC_TRACE_FUNCTION;
	// The callback has been aborted, restore the original values
	_last_executed_job      = _last_executed_job_old;
	_last_status            = _last_status_old;
	_last_update_time       = _last_update_time_old;
	_max_no_of_report_files = _max_no_of_report_files_old;

	return ERR_NO_ERROR;
}

HCF_NS_HCF_SRV_END
