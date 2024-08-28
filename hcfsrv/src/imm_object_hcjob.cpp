#include <string.h>
#include <set>

#include "hcf/core/logger.h"
#include "hcf/imm/om_handler.h"
#include "hcf/imm/imm_error_constants.h"
#include "hcf/imm/imm_config_class.h"

#include "srv_utils.h"
#include "configurator.h"
#include "job_executor.h"
#include "imm_object_healthcheckm.h"
#include "imm_object_hcrule.h"
#include "imm_object_hcjob_scheduler.h"

#include "hcf/trace/com_ericsson_common_hcf_srv_hcjob.h"
#include "hcf/trace/tracing_macros.h"

#include "imm_object_hcjob.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_SRV_IMM_OBJECT_HCJOB__

// Extern objects
extern TSL_NS_HCF_IMM::imm_config_class<HCF_NS_HCF_SRV::imm_object_hcrule> hcrule_class_handler;
extern TSL_NS_HCF_IMM::imm_config_class<HCF_NS_HCF_SRV::imm_object_hcjob_scheduler> hcjob_scheduler_class_handler;
extern TSL_NS_HCF_IMM::imm_config_class<HCF_NS_HCF_SRV::imm_object_hcjob> hcjob_class_handler;
extern HCF_NS_HCF_SRV::job_executor hcjob_executor;

HCF_NS_HCF_SRV_BEGIN

// Static fields initialization
const char * const __TSL_CLASS_NAME__::HCF_HCJOB_CLASS_NAME                                = "HCMHcJob";
const char * const __TSL_CLASS_NAME__::HCF_HCJOB_IMPLEMENTER_NAME                          = "HcJob_Implementer";
const char * const __TSL_CLASS_NAME__::HCF_HCJOB_JOB_ID_ATTR_NAME                          = "hcJobId";
const char * const __TSL_CLASS_NAME__::HCF_HCJOB_FAILED_RULES_ATTR_NAME                    = "failedRules";
const char * const __TSL_CLASS_NAME__::HCF_HCJOB_INPUT_PARAMETERS_ATTR_NAME                = "inputParameters";
const char * const __TSL_CLASS_NAME__::HCF_HCJOB_JOB_TO_TRIGGER_ATTR_NAME                  = "jobToTrigger";
const char * const __TSL_CLASS_NAME__::HCF_HCJOB_LAST_REPORT_FILE_NAME_ATTR_NAME           = "lastReportFileName";
const char * const __TSL_CLASS_NAME__::HCF_HCJOB_LAST_RUN_TIME_ATTR_NAME                   = "lastRunTime";
const char * const __TSL_CLASS_NAME__::HCF_HCJOB_LOCAL_FILE_STORE_PATH_ATTR_NAME           = "localFileStorePath";
const char * const __TSL_CLASS_NAME__::HCF_HCJOB_PROGRESS_REPORT_ATTR_NAME                 = "progressReport";
const char * const __TSL_CLASS_NAME__::HCF_HCJOB_RULES_CATEGORIES_ATTR_NAME                = "rulesCategories";
const char * const __TSL_CLASS_NAME__::HCF_HCJOB_STATUS_ATTR_NAME                          = "status";
const uint64_t __TSL_CLASS_NAME__::HCF_HCJOB_EXECUTE_ACTION_ID                             = 0;
const char * const __TSL_CLASS_NAME__::HCF_HCJOB_EXPORT_URI_ARGUMENT_NAME                  = "exportUri";
const char * const __TSL_CLASS_NAME__::HCF_HCJOB_EXPORT_PASSWORD_ARGUMENT_NAME             = "exportPassword";

const char * const __TSL_CLASS_NAME__::HCF_PROGRESS_REPORT_CLASS_NAME                      = "HCMAsyncActionProgress";
const char * const __TSL_CLASS_NAME__::HCF_PROGRESS_REPORT_DEFAULT_RDN_VALUE               = "id=1";
const char * const __TSL_CLASS_NAME__::HCF_PROGRESS_REPORT_LAST_UPDATE_TIME_ATTR_NAME      = "timeOfLastStatusUpdate";
const char * const __TSL_CLASS_NAME__::HCF_PROGRESS_REPORT_ACTION_STARTED_TIME_ATTR_NAME   = "timeActionStarted";
const char * const __TSL_CLASS_NAME__::HCF_PROGRESS_REPORT_ACTION_COMPLETED_TIME_ATTR_NAME = "timeActionCompleted";
const char * const __TSL_CLASS_NAME__::HCF_PROGRESS_REPORT_STATE_ATTR_NAME                 = "state";
const char * const __TSL_CLASS_NAME__::HCF_PROGRESS_REPORT_RESULT_INFO_ATTR_NAME           = "resultInfo";
const char * const __TSL_CLASS_NAME__::HCF_PROGRESS_REPORT_RESULT_ATTR_NAME                = "result";
const char * const __TSL_CLASS_NAME__::HCF_PROGRESS_REPORT_PROGRESS_PRECENTAGE_ATTR_NAME   = "progressPercentage";
const char * const __TSL_CLASS_NAME__::HCF_PROGRESS_REPORT_PROGRESS_INFO_ATTR_NAME         = "progressInfo";
const char * const __TSL_CLASS_NAME__::HCF_PROGRESS_REPORT_ID_ATTR_NAME                    = "id";
const char * const __TSL_CLASS_NAME__::HCF_PROGRESS_REPORT_ADDITIONAL_INFO_ATTR_NAME       = "additionalInfo";
const char * const __TSL_CLASS_NAME__::HCF_PROGRESS_REPORT_ACTION_NAME_ATTR_NAME           = "actionName";
const char * const __TSL_CLASS_NAME__::HCF_PROGRESS_REPORT_ACTION_ID_ATTR_NAME             = "actionId";
const char * const __TSL_CLASS_NAME__::HCF_PROGRESS_REPORT_ACTION_NAME_DEFAULT_VALUE       = "EXECUTE";
const char * const __TSL_CLASS_NAME__::HCF_PROGRESS_REPORT_PROGRESS_INFO_INIT_VALUE        = "Job Execution ongoing";
const char * const __TSL_CLASS_NAME__::HCF_PROGRESS_REPORT_PROGRESS_INFO_FINAL_VALUE       = "Job Execution completed";

const char * const __TSL_CLASS_NAME__::HCF_FAILED_RULE_CLASS_NAME                          = "HCMFailedHcRule";
const char * const __TSL_CLASS_NAME__::HCF_FAILED_RULE_ID_ATTR_NAME                        = "id";
const char * const __TSL_CLASS_NAME__::HCF_FAILED_RULE_HCRULE_ATTR_NAME                    = "hcRule";
const char * const __TSL_CLASS_NAME__::HCF_FAILED_RULE_SEVERITY_ATTR_NAME                  = "severity";
const char * const __TSL_CLASS_NAME__::HCF_FAILED_RULE_REASON_ATTR_NAME                    = "reason";
const char * const __TSL_CLASS_NAME__::HCF_FAILED_RULE_ID_FORMAT                           = "rule_%lu";

std::string __TSL_CLASS_NAME__::_ongoing_job_dn;
TSL_NS_HCF_CORE::thread_mutex_recursive __TSL_CLASS_NAME__::_ongoing_job_mutex;

int __TSL_CLASS_NAME__::set_object_attributes (std::vector<std::pair<std::string, std::vector<void *> > > & attributes, TSL_NS_HCF_IMM::IMM_API_ObjectImplementer_CallbackInfo * callback_info) {
	HCF_CC_TRACE_FUNCTION;
	// Save the old values before modifying them, in order to rollback in case of errors
	_failed_rules_dns_old      = _failed_rules_dns;
	_job_to_trigger_dn_old     = _job_to_trigger_dn;
	_last_report_file_name_old = _last_report_file_name;
	_last_run_time_old         = _last_run_time;
	_local_file_store_path_old = _local_file_store_path;
	_progress_report_dn_old    = _progress_report_dn;
	_rules_categories_old      = _rules_categories;
	_status_old                = _status;

	bool augmentation_operation_to_be_executed = false;

	// For each of the provided attributes, understand the attribute name and set the class field accordingly
	for (size_t i = 0; i < attributes.size(); ++i) {

		// Extract from the given attributes vector the attribute name and its value
		std::string attr_name = attributes[i].first;
		std::vector<void *> attr_values = attributes[i].second;
		HCF_CC_TRACE(SetObjectAttributes, "Received a request to set/change the value of the following attribute:", attr_name.c_str());

		if (!attr_name.compare(HCF_HCJOB_JOB_ID_ATTR_NAME)) {
			if (attr_values.size() > 0) {
				std::string obj_rdn = reinterpret_cast<char *>(attr_values[0]);
				std::string obj_id = obj_rdn.substr(obj_rdn.find('=') + 1);

				// Check if the provided RDN is valid or not
				if (!is_job_name_valid(obj_id)) {
					HCF_CC_TRACE(SetObjectAttributesErr, "Invalid value provided for the RDN attribute.", ERR_BAD_VALUE_PROVIDED);
					HCF_CC_LOG(LEVEL_ERROR, "Invalid value (%s) provided for attribute '%s'!", obj_rdn.c_str(), attr_name.c_str());
					set_error_message(HCF_INVALID_JOB_RDN_ERROR_MSG);
					return ERR_BAD_VALUE_PROVIDED;
				}

				// The HcJob RDN is valid, set it.
				_obj_rdn = obj_rdn;
				HCF_CC_TRACE(SetObjectAttributes, "The value of the object RDN is:", _obj_rdn.c_str());
			}
			else {
				// ERROR: the ID field cannot be empty!
				HCF_CC_TRACE(SetObjectAttributesErr, "No value provided for the mandatory attribute.", ERR_IMM_MANDATORY_VALUE_MISSING);
				HCF_CC_LOG(LEVEL_ERROR, "No value provided for the mandatory attribute '%s'!", attr_name.c_str());
				set_error_message(HCF_MANDATORY_VALUE_MISSING_ERROR_MSG);
				return ERR_IMM_MANDATORY_VALUE_MISSING;
			}
		}
		else if (!attr_name.compare(HCF_HCJOB_FAILED_RULES_ATTR_NAME)) {
			for (size_t j = 0; j < attr_values.size(); ++j) {
				std::string failed_rule_dn = reinterpret_cast<char *>(attr_values[j]);
				_failed_rules_dns.insert(failed_rule_dn);
				HCF_CC_TRACE(SetObjectAttributes, "Added the following failedRule to the internal data structures:", failed_rule_dn.c_str());
			}
		}
		else if (!attr_name.compare(HCF_HCJOB_INPUT_PARAMETERS_ATTR_NAME)) {}	// Nothing to do
		else if (!attr_name.compare(HCF_HCJOB_JOB_TO_TRIGGER_ATTR_NAME)) {
			_job_to_trigger_dn = (attr_values.size() > 0) ? reinterpret_cast<char *>(attr_values[0]) : "";
			HCF_CC_TRACE(SetObjectAttributes, "The value of the jobToTrigger attribute is:", _job_to_trigger_dn.c_str());
		}
		else if (!attr_name.compare(HCF_HCJOB_LAST_REPORT_FILE_NAME_ATTR_NAME)) {
			if (attr_values.size() > 0) {
				_last_report_file_name = reinterpret_cast<char *>(attr_values[0]);
				HCF_CC_TRACE(SetObjectAttributes, "The value of the lastReportFileName attribute is:", _last_report_file_name.c_str());
			}
			else {} // No value provided, using the default value
		}
		else if (!attr_name.compare(HCF_HCJOB_LAST_RUN_TIME_ATTR_NAME)) {
			if (attr_values.size() > 0) {
				_last_run_time = reinterpret_cast<char *>(attr_values[0]);
				HCF_CC_TRACE(SetObjectAttributes, "The value of the lastRunTime attribute is:", _last_run_time.c_str());
			}
			else {} // No value provided, using the default value
		}
		else if (!attr_name.compare(HCF_HCJOB_LOCAL_FILE_STORE_PATH_ATTR_NAME)) {
			if (attr_values.size() > 0) {
				_local_file_store_path = reinterpret_cast<char *>(attr_values[0]);
				HCF_CC_TRACE(SetObjectAttributes, "The provided value of the localFileStorePath attribute is:", _local_file_store_path.c_str());
			}
			else {
				_local_file_store_path = std::string(configurator::get_output_folder());
				augmentation_operation_to_be_executed = true;
				HCF_CC_TRACE(SetObjectAttributes, "No value provided for the localFileStorePath attribute, used the default one:", _local_file_store_path.c_str());
			}
		}
		else if (!attr_name.compare(HCF_HCJOB_PROGRESS_REPORT_ATTR_NAME)) {
			if (attr_values.size() > 0) {
				// Progress report structure already created, take its DN
				_progress_report_dn = reinterpret_cast<char *>(attr_values[0]);
				HCF_CC_TRACE(SetObjectAttributes, "The provided value of the progressReport attribute is:", _progress_report_dn.c_str());
			}
			else	augmentation_operation_to_be_executed = true;
		}
		else if (!attr_name.compare(HCF_HCJOB_RULES_CATEGORIES_ATTR_NAME)) {
			std::set<category_t> new_categories;
			const size_t num_of_categories = attr_values.size();

			if (num_of_categories == 0) {
				// At least one category must be provided!
				HCF_CC_TRACE(SetObjectAttributesErr, "No value provided for the mandatory attribute.", ERR_IMM_MANDATORY_VALUE_MISSING);
				HCF_CC_LOG(LEVEL_ERROR, "No value provided for the mandatory attribute '%s'!", attr_name.c_str());
				set_error_message(HCF_MANDATORY_VALUE_MISSING_ERROR_MSG);
				return ERR_IMM_MANDATORY_VALUE_MISSING;
			}

			for (size_t j = 0; j < num_of_categories; ++j) {
				category_t category = *(reinterpret_cast<category_t *>(attr_values[j]));

				// Check if the given category has a meaningful value
				if ((category < CATEGORY_BACKOFFICE) || (category > CATEGORY_OTHER)) {
					HCF_CC_TRACE(SetObjectAttributesErr, "Invalid category provided!", ERR_UNDEFINED_CATEGORY);
					HCF_CC_LOG(LEVEL_ERROR, "The provided category (%d) doesn't exist!", category);
					set_error_message(HCF_UNDEFINED_CATEGORY_ERROR_MSG);
					return ERR_UNDEFINED_CATEGORY;
				}

				new_categories.insert(category);
				HCF_CC_TRACE(SetObjectAttributes, "The provided value of the category attribute is", utils::map_category_value_to_category_string(category));
			}

			// Erase the old content of categories set and add the provided values
			_rules_categories.clear();
			_rules_categories = new_categories;
		}
		else if (!attr_name.compare(HCF_HCJOB_STATUS_ATTR_NAME)) {
			if (attr_values.size() > 0) {
				_status = *(reinterpret_cast<health_status_t *>(attr_values[0]));
				HCF_CC_TRACE(SetObjectAttributes, "The provided value of the status attribute is:", utils::map_health_status_to_string(_status));
			}
			else {
				// No value provided, using the default value
				_status = HEALTH_STATUS_NOT_AVAILABLE;
				HCF_CC_TRACE(SetObjectAttributes, "No value provided for the status attribute, using the default value:", utils::map_health_status_to_string(_status));
			}
		}
	}
	std::string job_id = _obj_rdn.substr(_obj_rdn.find('=') + 1);

	/*
	 * Extract the HcJob ID from its RDN in order to validate the jobToTrigger attribute value.
	 * All those checks must be done only if the object has been created and not if it is read
	 * at HCF startup. This can be understand by checking the value of callback_info pointer.
	 */
	if ((callback_info) && (_job_to_trigger_dn != _job_to_trigger_dn_old) && !(_job_to_trigger_dn.empty())) {
		std::string job_to_trigger_id = get_obj_id_from_dn(_job_to_trigger_dn.c_str());

		// FIRST: Check that the provided value is an existing HcJob instance
		HCF_CC_TRACE(SetObjectAttributes, "Validating the value of the following jobToTrigger: checking if the job exists.", job_to_trigger_id.c_str());
		if (!hcjob_class_handler.get_instance(job_to_trigger_id)) {
			HCF_CC_TRACE(SetObjectAttributesErr, "Invalid value provided for jobToTrigger attribute: it is not existing.", ERR_BAD_VALUE_PROVIDED);
			HCF_CC_LOG(LEVEL_ERROR, "The provided value for jobToTrigger attribute (%s) is not existing!", _job_to_trigger_dn.c_str());
			set_error_message(HCF_NOT_EXISTING_JOBTOTRIGGER_ERROR_MSG);
			return ERR_BAD_VALUE_PROVIDED;
		}

		// SECOND: Avoid useless loops: the jobToTrigger MUST be different from the current job!
		HCF_CC_TRACE(SetObjectAttributes, "Validating the value of the following jobToTrigger: checking if it's different from the current job!", job_to_trigger_id.c_str());
		if (!job_id.compare(job_to_trigger_id)) {
			HCF_CC_TRACE(SetObjectAttributesErr, "Invalid value provided for jobToTrigger attribute: it's equal to current job.", ERR_BAD_VALUE_PROVIDED);
			HCF_CC_LOG(LEVEL_ERROR, "The provided value for jobToTrigger attribute (%s) is equal to the current job!", _job_to_trigger_dn.c_str());
			set_error_message(HCF_INVALID_JOBTOTRIGGER_VALUE_ERROR_MSG);
			return ERR_BAD_VALUE_PROVIDED;
		}
	}

	// Check if the child HcJobScheduler instance has been already created: its ID is the same of the HcJob object
	imm_object_hcjob_scheduler * scheduler = hcjob_scheduler_class_handler.get_instance(job_id);
	augmentation_operation_to_be_executed = (scheduler) ? augmentation_operation_to_be_executed : true;

	if (augmentation_operation_to_be_executed) {
		/*
		 * Progress report structure not existing, it must be created:
		 * check if a valid callback_info structure has been provided.
		 * During the augmented operation execution change also the localFileStorePath attribute value.
		 */
		if (callback_info) {
			HCF_CC_TRACE(SetObjectAttributes, "Executing all the other operations to create the children objects!", "");
			if (const int call_result = execute_augmented_operations(callback_info)) {
				HCF_CC_TRACE(SetObjectAttributesErr, "Call 'execute_augmented_operations' failed.", call_result);
				HCF_CC_LOG(LEVEL_ERROR, "Call 'execute_augmented_operations' failed with error code == %d", call_result);
				set_error_message(HCF_CHILDREN_OBJECTS_CREATION_FAILED_ERROR_MSG);
				return call_result;
			}
		}
		else {
			HCF_CC_TRACE(SetObjectAttributesErr, "NULL instance provided for callback_info structure.", ERR_NULL_INSTANCE_RETRIEVED);
			HCF_CC_LOG(LEVEL_ERROR, "NULL instance provided for callback_info structure!");
			set_error_message(HCF_GENERIC_ERROR_ERROR_MSG);
			return ERR_NULL_INSTANCE_RETRIEVED;
		}
	}

	HCF_CC_LOG(LEVEL_NOTICE, "Object attributes correctly modified: RDN == '%s', _failed_rules_dns(count) == %lu, "
			"_job_to_trigger_dn == '%s', _last_report_file_name == '%s', _last_run_time == '%s', _local_file_store_path == '%s', "
			"_progress_report_dn == '%s', _rules_categories(count) == %lu, _status == %d", _obj_rdn.c_str(), _failed_rules_dns.size(),
			_job_to_trigger_dn.c_str(), _last_report_file_name.c_str(), _last_run_time.c_str(), _local_file_store_path.c_str(),
			_progress_report_dn.c_str(), _rules_categories.size(), _status);
	return ERR_NO_ERROR;
}

int __TSL_CLASS_NAME__::finalize_object (TSL_NS_HCF_IMM::IMM_API_ObjectImplementer_CallbackInfo * TSL_UNUSED_PAR(callback_info)) {
	HCF_CC_TRACE_FUNCTION;

	// A HcJob instance can be deleted only if it is not running right now
	TSL_CRITICAL_SECTION_BEGIN(sync_t, _ongoing_job_mutex);
		HCF_CC_TRACE(FinalizeObject, "Received a request to finalize this object: checking if the job is currently running!", _obj_id.c_str());
		if (_obj_dn == _ongoing_job_dn) {
			HCF_CC_TRACE(FinalizeObjectErr, "It is not possible to finalize this object, since the job is currently running!", ERR_JOB_EXECUTION_ONGOING);
			HCF_CC_LOG(LEVEL_ERROR, "It is not possible to delete the job having ID == '%s', since it is currently running!", _obj_id.c_str());
			set_error_message(HCF_JOB_EXECUTION_ONGOING_ERROR_MSG);
			return ERR_JOB_EXECUTION_ONGOING;
		}
	TSL_CRITICAL_SECTION_END;

	// A HcJob instance can be deleted only if it is not linked to any other HcJob instance as jobToTrigger
	HCF_CC_TRACE(FinalizeObject, "Received a request to finalize this object: checking if the job is linked to some other job!", _obj_id.c_str());
	std::list<imm_object_hcjob *> jobs_list = hcjob_class_handler.get_all_instances();
	for (std::list<imm_object_hcjob *>::const_iterator job = jobs_list.begin(); job != jobs_list.end(); ++job) {
		if ((*job)->get_job_to_trigger_dn() == _obj_dn) {
			HCF_CC_TRACE(FinalizeObjectErr, "It is not possible to finalize this object, since the job is linked to another job!", ERR_JOB_EXECUTION_ONGOING);
			HCF_CC_LOG(LEVEL_ERROR, "It is not possible to delete the job having ID == '%s', since it is linked "
					"as jobToTrigger to the job having ID == '%s'", _obj_id.c_str(), (*job)->get_id().c_str());
			set_error_message(HCF_JOB_LINKED_TO_ANOTHER_JOB_ERROR_MSG);
			return ERR_JOB_LINKED_TO_ANOTHER_JOB;
		}
	}

	// Signal to the HcJobScheduler object that is must be finalized.
	HCF_CC_TRACE(FinalizeObject, "Received a request to finalize this object: signaling the info to the child HcJobScheduler object!", _obj_id.c_str());
	imm_object_hcjob_scheduler * scheduler = 0;
	if (!(scheduler = hcjob_scheduler_class_handler.get_instance(_obj_id))) {
		HCF_CC_TRACE(FinalizeObjectErr, "NULL instance retrieved for HCJobScheduler instance!", ERR_NULL_INSTANCE_RETRIEVED);
		HCF_CC_LOG(LEVEL_WARNING, "No HcJobScheduler instance found with ID == '%s'!", _obj_id.c_str());
		return ERR_NO_ERROR;
	}
	scheduler->finalize_object();
	return ERR_NO_ERROR;
}

int __TSL_CLASS_NAME__::abort_object_modification (TSL_NS_HCF_IMM::IMM_API_ObjectImplementer_CallbackInfo * TSL_UNUSED_PAR(callback_info)) {
	HCF_CC_TRACE_FUNCTION;

	// The callback has been aborted, restore the original values
	_failed_rules_dns      = _failed_rules_dns_old;
	_job_to_trigger_dn     = _job_to_trigger_dn_old;
	_last_report_file_name = _last_report_file_name_old;
	_last_run_time         = _last_run_time_old;
	_local_file_store_path = _local_file_store_path_old;
	_progress_report_dn    = _progress_report_dn_old;
	_rules_categories      = _rules_categories_old;
	_status                = _status_old;

	return ERR_NO_ERROR;
}

int __TSL_CLASS_NAME__::execute_admin_operation (uint64_t operation_id, TSL_NS_HCF_IMM::IMM_API_AdminOperationParamType ** arguments) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(ExecuteAdminOperation, "Received a request to execute an administrative operation on the object!", _obj_id.c_str(), operation_id);

	// FIRST: check if the operation ID received is the expected one
	if (operation_id != HCF_HCJOB_EXECUTE_ACTION_ID) {
		HCF_CC_TRACE(ExecuteAdminOperationErr, "Unsupported administrative operation identifier received!", ERR_OPERATION_NOT_IMPLEMENTED);
		HCF_CC_LOG(LEVEL_ERROR, "Unsupported administrative operation id received (%lu)!", operation_id);
		set_error_message(HCF_OPERATION_NOT_IMPLEMENTED_ERROR_MSG);
		return ERR_OPERATION_NOT_IMPLEMENTED;
	}

	// SECOND: check if another job is already running
	HCF_CC_TRACE(ExecuteAdminOperation, "Received a request to executed this job: checking if there is some job running!", _obj_id.c_str(), operation_id);
	if (is_job_execution_ongoing()) {
		HCF_CC_TRACE(ExecuteAdminOperationErr, "Another job is already running!", ERR_ANOTHER_JOB_ONGOING);
		HCF_CC_LOG(LEVEL_ERROR, "Another job is already running!");
		set_error_message(HCF_JOB_ALREADY_RUNNING_ERROR_MSG);
		return ERR_ANOTHER_JOB_ONGOING;
	}

	// THIRD: parse the input parameters (if any)
	std::string export_uri;
	std::string export_password;
	HCF_CC_TRACE(ExecuteAdminOperation, "Received a request to executed this job: extracting the input parameters!", _obj_id.c_str(), operation_id);

	for (int i = 0; arguments[i]; ++i) {
		const char * attr_name = arguments[i]->attrName;
		void * attr_value = arguments[i]->attrValues;

		if (!(::strcmp(attr_name, HCF_HCJOB_EXPORT_URI_ARGUMENT_NAME))) {
			export_uri = reinterpret_cast<char *>(attr_value);
		}
		else if (!(::strcmp(attr_name, HCF_HCJOB_EXPORT_PASSWORD_ARGUMENT_NAME))) {
			export_password = reinterpret_cast<char *>(attr_value);
		}
		else {
			// Undefined parameter received, log error and return an error message
			HCF_CC_TRACE(ExecuteAdminOperationErr, "Undefined argument received!", ERR_NULL_INSTANCE_RETRIEVED);
			HCF_CC_LOG(LEVEL_ERROR, "Undefined argument received: Name == '%s'", attr_name);
			set_error_message(HCF_GENERIC_ERROR_ERROR_MSG);
			return ERR_NULL_INSTANCE_RETRIEVED;
		}
	}

	// FOURTH: call the execute method with the provided arguments
	HCF_CC_TRACE(ExecuteAdminOperation, "Received a request to executed this job: executing the job!", _obj_id.c_str(), operation_id);
	return execute(export_uri, export_password);
}

int __TSL_CLASS_NAME__::clear_failed_rules () {
	HCF_CC_TRACE_FUNCTION;
	const size_t num_of_failed_rules = _failed_rules_dns.size();
	HCF_CC_LOG(LEVEL_NOTICE, "Removing %lu HcFailedRule objects...", num_of_failed_rules);

	HCF_CC_TRACE(ClearFailedRules, "Received a request to delete all the instance of failedRules!", "");
	if (const int call_result = initialize_om_handler()) {
		HCF_CC_TRACE(ClearFailedRulesErr, "Call 'initialize_om_handler' failed.", call_result);
		HCF_CC_LOG(LEVEL_ERROR, "Call 'initialize_om_handler' failed, return_code == %d!", call_result);
		return call_result;
	}

	// Delete each created failed rule object
	TSL_CRITICAL_SECTION_BEGIN(sync_t, _om_handler_mutex);
		for (std::set<std::string>::const_iterator it = _failed_rules_dns.begin(); it != _failed_rules_dns.end(); ++it) {
			HCF_CC_TRACE(ClearFailedRules, "Deleting the following failedRule:", it->c_str());

			int imm_error_code = TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_IMM_TRY_AGAIN;
			for (int retries_count = HCF_IMM_OPERATIONS_MAX_NUMBER_OF_RETRIES; (retries_count > 0) && (imm_error_code == TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_IMM_TRY_AGAIN); --retries_count) {
				if (_om_handler.deleteObject(it->c_str()) != TSL_NS_HCF_IMM::OPERATION_SUCCESS) {
					imm_error_code = _om_handler.getInternalLastError();
					HCF_CC_TRACE(ClearFailedRulesErr, "Call '_om_handler.deleteObject' failed.", imm_error_code);
					HCF_CC_LOG(LEVEL_ERROR, "Call '_om_handler.deleteObject' failed (RETRY == %d) for the object having DN == '%s', "
							"return_code == %d, error_message == '%s'!", (HCF_IMM_OPERATIONS_MAX_NUMBER_OF_RETRIES - retries_count + 1),
							it->c_str(), _om_handler.getInternalLastError(), _om_handler.getInternalLastErrorText());

					// Wait for a while if the error is "TRY AGAIN": this could solve the IMM problem!!!
					if (imm_error_code == TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_IMM_TRY_AGAIN)	::sleep(1);
				}
				else imm_error_code = TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_NO_ERR;
			}

			if (imm_error_code != TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_NO_ERR) {
				HCF_CC_TRACE(ClearFailedRulesErr, "Failed to delete a failed rule instance after many tries!", ERR_FAILED_RULE_DELETE_FAILED);
				return ERR_FAILED_RULE_DELETE_FAILED;
			}
		}
	TSL_CRITICAL_SECTION_END;

	// Modify the attribute in the HcJob
	HCF_CC_TRACE(ClearFailedRules, "Resetting the value of the failedRules attribute!", "");
	if (const int call_result = modify_object_attribute(HCF_HCJOB_FAILED_RULES_ATTR_NAME, TSL_NS_HCF_IMM::ATTR_NAMET, 0, 0)) {
		HCF_CC_TRACE(ClearFailedRulesErr, "Call 'modify_object_attribute' failed.", call_result);
		HCF_CC_LOG(LEVEL_ERROR, "Call 'modify_object_attribute' failed with return_code == %d", call_result);
		return ERR_FAILED_RULE_MODIFY_FAILED;
	}

	_failed_rules_dns.clear();
	return ERR_NO_ERROR;
}

int __TSL_CLASS_NAME__::set_failed_rule (const std::string & rule_id, const std::string & reason) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(SetFailedRule, "Received a request to add a failed rule object: retrieving the HcRule instance!", _obj_id.c_str(), rule_id.c_str(), reason.c_str());

	// Retrieve the HcRule instance with the given ID
	imm_object_hcrule * hcrule_ptr = hcrule_class_handler.get_instance(rule_id);
	if (!hcrule_ptr) {
		HCF_CC_TRACE(SetFailedRuleErr, "NULL instance retrieved for the HcRule object!", ERR_NULL_INSTANCE_RETRIEVED);
		HCF_CC_LOG(LEVEL_ERROR, "Call 'hcrule_class_handler.get_instance' failed with ID == '%s'!", rule_id.c_str());
		return ERR_NULL_INSTANCE_RETRIEVED;
	}
	HCF_CC_TRACE(SetFailedRule, "Received a request to add a failed rule object: initializing the internal objects!", _obj_id.c_str(), rule_id.c_str(), reason.c_str());

	if (const int call_result = initialize_om_handler()) {
		HCF_CC_TRACE(SetFailedRuleErr, "Call 'initialize_om_handler' failed.", call_result);
		HCF_CC_LOG(LEVEL_ERROR, "Call 'initialize_om_handler' failed, return_code == %d!", call_result);
		return call_result;
	}
	HCF_CC_TRACE(SetFailedRule, "Received a request to add a failed rule object: preparing the data structures!", _obj_id.c_str(), rule_id.c_str(), reason.c_str());

	// Create the FailedHcRule object
	std::vector<TSL_NS_HCF_IMM::IMM_API_ValuesDefinitionTypes> attr_list;
	TSL_NS_HCF_IMM::IMM_API_ValuesDefinitionTypes id_attr;
	id_attr.attrName = const_cast<char *>(HCF_FAILED_RULE_ID_ATTR_NAME);
	id_attr.attrType = TSL_NS_HCF_IMM::ATTR_STRINGT;
	id_attr.attrValuesNum = 1;
	char failed_rule_rdn[2048];
	::snprintf(failed_rule_rdn, TSL_ARRAY_SIZE(failed_rule_rdn), HCF_FAILED_RULE_ID_FORMAT, _failed_rules_dns.size());
	void * tmp_0[1] = { failed_rule_rdn };
	id_attr.attrValues = tmp_0;
	attr_list.push_back(id_attr);

	TSL_NS_HCF_IMM::IMM_API_ValuesDefinitionTypes rule_attr;
	rule_attr.attrName = const_cast<char *>(HCF_FAILED_RULE_HCRULE_ATTR_NAME);
	rule_attr.attrType = TSL_NS_HCF_IMM::ATTR_STRINGT;
	rule_attr.attrValuesNum = 1;
	void * tmp_1[1] = { const_cast<char *>(hcrule_ptr->get_rdn().c_str()) };
	rule_attr.attrValues = tmp_1;
	attr_list.push_back(rule_attr);

	TSL_NS_HCF_IMM::IMM_API_ValuesDefinitionTypes reason_attr;
	reason_attr.attrName = const_cast<char *>(HCF_FAILED_RULE_REASON_ATTR_NAME);
	reason_attr.attrType = TSL_NS_HCF_IMM::ATTR_STRINGT;
	reason_attr.attrValuesNum = 1;
	void * tmp_2[1] = { const_cast<char *>(reason.c_str()) };
	reason_attr.attrValues = tmp_2;
	attr_list.push_back(reason_attr);

	TSL_NS_HCF_IMM::IMM_API_ValuesDefinitionTypes severity_attr;
	severity_attr.attrName = const_cast<char *>(HCF_FAILED_RULE_SEVERITY_ATTR_NAME);
	severity_attr.attrType = TSL_NS_HCF_IMM::ATTR_INT32T;
	severity_attr.attrValuesNum = 1;
	severity_t severity = (hcrule_ptr->is_critical()) ? SEVERITY_CRITICAL : SEVERITY_WARNING;
	void * tmp_3[1] = { &severity };
	severity_attr.attrValues = tmp_3;
	attr_list.push_back(severity_attr);

	TSL_CRITICAL_SECTION_BEGIN(sync_t, _om_handler_mutex);
		// Create the object with the given parameters
		int imm_error_code = TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_IMM_TRY_AGAIN;
		for (int retries_count = HCF_IMM_OPERATIONS_MAX_NUMBER_OF_RETRIES; (retries_count > 0) && (imm_error_code == TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_IMM_TRY_AGAIN); --retries_count) {
			HCF_CC_TRACE(SetFailedRule, "Creating the failed rule object.", _obj_id.c_str(), rule_id.c_str(), reason.c_str());

			if (_om_handler.createObject(HCF_FAILED_RULE_CLASS_NAME, _obj_dn.c_str(), attr_list) != TSL_NS_HCF_IMM::OPERATION_SUCCESS) {
				imm_error_code = _om_handler.getInternalLastError();
				HCF_CC_TRACE(SetFailedRuleErr, "Call '_om_handler.createObject' failed.", imm_error_code);
				HCF_CC_LOG(LEVEL_ERROR, "Call '_om_handler.createObject' failed (RETRY == %d) for the class %s, "
						"under the object having dn == '%s', return_code == %d, error_message == '%s'!",
						(HCF_IMM_OPERATIONS_MAX_NUMBER_OF_RETRIES - retries_count + 1), HCF_FAILED_RULE_CLASS_NAME,
						_obj_dn.c_str(), _om_handler.getInternalLastError(), _om_handler.getInternalLastErrorText());

				// Wait for a while if the error is "TRY AGAIN": this could solve the IMM problem!!!
				if (imm_error_code == TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_IMM_TRY_AGAIN)	::sleep(1);
			}
			else imm_error_code = TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_NO_ERR;
		}

		if (imm_error_code != TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_NO_ERR) {
			HCF_CC_TRACE(SetFailedRuleErr, "Failed to create the failed rule object after many tries.", imm_error_code);
			return ERR_FAILED_RULE_CREATE_FAILED;
		}
	TSL_CRITICAL_SECTION_END;

	// Calculate the DN of the created object and add it to the internal structure
	std::string new_failed_rule = std::string(failed_rule_rdn) + std::string(",") + _obj_dn.c_str();
	_failed_rules_dns.insert(new_failed_rule);

	// Prepare the attribute to modify under HcJob class
	int i = 0;
	const size_t num_of_attributes = _failed_rules_dns.size();
	void * new_values[num_of_attributes];

	HCF_CC_TRACE(SetFailedRule, "Failed rule object correctly created: preparing the data structures to update the current job object!", _obj_id.c_str(), rule_id.c_str(), reason.c_str());
	for (std::set<std::string>::iterator it = _failed_rules_dns.begin(); it != _failed_rules_dns.end(); ++it)
		new_values[i++] = const_cast<char *>(it->c_str());

	// Modify the attribute value in the HcJob class
	if (const int call_result = modify_object_attribute(HCF_HCJOB_FAILED_RULES_ATTR_NAME, TSL_NS_HCF_IMM::ATTR_NAMET, num_of_attributes, new_values)) {
		HCF_CC_TRACE(SetFailedRuleErr, "Call 'modify_object_attribute' failed.", call_result);
		HCF_CC_LOG(LEVEL_ERROR, "Call 'modify_object_attribute' failed for attribute %s, return_code == %d, error_message == '%s'!",
				HCF_HCJOB_FAILED_RULES_ATTR_NAME, _om_handler.getInternalLastError(), _om_handler.getInternalLastErrorText());

		// In case of failure, remove the object previously created (also from internal data structures)
		HCF_CC_TRACE(SetFailedRuleErr, "Failed to update the job object, removing also the failed rule object previously created.", call_result);
		TSL_CRITICAL_SECTION_BEGIN(sync_t, _om_handler_mutex);
			_om_handler.deleteObject(new_failed_rule.c_str());
		TSL_CRITICAL_SECTION_END;

		_failed_rules_dns.erase(new_failed_rule);
		return ERR_FAILED_RULE_MODIFY_FAILED;
	}
	return ERR_NO_ERROR;
}

int __TSL_CLASS_NAME__::execute (std::string export_uri, std::string export_password, bool is_scheduled_execution) {
	HCF_CC_TRACE_FUNCTION;

	std::string job_to_trigger_id = (!_job_to_trigger_dn.empty()) ? get_obj_id_from_dn(_job_to_trigger_dn.c_str()) : "";
	HCF_CC_TRACE(Execute, "Received a request to execute the current HcJob!", _obj_id.c_str(),
			job_to_trigger_id.c_str(), export_uri.c_str(), export_password.c_str(), is_scheduled_execution);
	HCF_CC_LOG(LEVEL_NOTICE, "Executing the HcJob '%s': exportUri == '%s', exportPassword == '%s', jobToTrigger == '%s'",
			_obj_id.c_str(), export_uri.c_str(), export_password.c_str(), job_to_trigger_id.c_str());

	// Notify the job executor that a new job must be executed
	return hcjob_executor.execute_job(_obj_id, _rules_categories, export_uri, export_password, job_to_trigger_id, is_scheduled_execution);
}

int __TSL_CLASS_NAME__::execute_augmented_operations (TSL_NS_HCF_IMM::IMM_API_ObjectImplementer_CallbackInfo * callback_info) {
	HCF_CC_TRACE_FUNCTION;
	TSL_NS_HCF_IMM::object_implementer_interface * object_implementer = callback_info->object_implementer;

	// Initialize the Augmented CCB to create the progress report structure
	HCF_CC_TRACE(ExecuteAugmentedOperations, "An augmented CCB operation must be executed: initializing handles.", "");
	if (object_implementer->getCcbAugmentationInitialize(callback_info->oi_handle, callback_info->ccb_id, &(callback_info->ccb_handle), &(callback_info->admin_owner_handler)) != TSL_NS_HCF_IMM::OPERATION_SUCCESS) {
		HCF_CC_TRACE(ExecuteAugmentedOperationsErr, "Call 'getCcbAugmentationInitialize' failed.", object_implementer->getInternalLastError(), object_implementer->getInternalLastErrorText());
		HCF_CC_LOG(LEVEL_ERROR, "Call 'getCcbAugmentationInitialize' failed, error_code == %d, error_message == '%s'",
				object_implementer->getInternalLastError(), object_implementer->getInternalLastErrorText());
		return ERR_AUGMENTED_CCB_INIT_FAILED;
	}

	// Create the DN of HcJob instance under which the progressReport structure must be created
	char hcjob_dn[2048];
	::snprintf(hcjob_dn, TSL_ARRAY_SIZE(hcjob_dn), "%s,%s=%s", _obj_rdn.c_str(),
			imm_object_healthcheckm::HCF_HEALTHCHECKM_HEALTH_CHECK_M_ID_ATTR_NAME, imm_object_healthcheckm::HCF_HEALTHCHECKM_ID_DEFAULT_VALUE);

	// Create the progressReport structure for HcJob class instance and attach it
	HCF_CC_TRACE(ExecuteAugmentedOperations, "Create the progressReport structure and attach it to the following HcJob:", hcjob_dn);
	if (const int call_result = create_progress_report_and_attach_to_parent(hcjob_dn, object_implementer, callback_info->ccb_handle, callback_info->admin_owner_handler, _progress_report_dn)) {
		HCF_CC_TRACE(ExecuteAugmentedOperationsErr, "Call 'create_progress_report_and_attach_to_parent' failed.", call_result, "");
		HCF_CC_LOG(LEVEL_ERROR, "Call 'create_progress_report_and_attach_to_parent' for object having "
				"DN == '%s' failed with error code == %d", hcjob_dn, call_result);
		return call_result;
	}
	HCF_CC_TRACE(ExecuteAugmentedOperations, "Preparing the needed data structures to set the localFileStorePath attribute to the following value:", _local_file_store_path.c_str());

	// Modify the localFileStorePath attribute value of HcJob class
	TSL_NS_HCF_IMM::IMM_API_ImmParameter parameter;
	parameter.attrName = const_cast<char *>(HCF_HCJOB_LOCAL_FILE_STORE_PATH_ATTR_NAME);
	parameter.attrType = TSL_NS_HCF_IMM::ATTR_STRINGT;
	parameter.attrValuesNum = 1;
	void * tmp_path[1] = { const_cast<char *>(_local_file_store_path.c_str()) };
	parameter.attrValues = tmp_path;

	HCF_CC_TRACE(ExecuteAugmentedOperations, "Modifying the localFileStorePath attribute for the following object:", hcjob_dn);
	if (object_implementer->modifyObjectAugmentCcb(callback_info->ccb_handle, callback_info->admin_owner_handler, hcjob_dn, &parameter) != TSL_NS_HCF_IMM::OPERATION_SUCCESS) {
		HCF_CC_TRACE(ExecuteAugmentedOperationsErr, "Call 'modifyObjectAugmentCcb' failed.", object_implementer->getInternalLastError(), object_implementer->getInternalLastErrorText());
		HCF_CC_LOG(LEVEL_ERROR, "Call 'modifyObjectAugmentCcb' failed for object having DN == '%s', error_code == %d, error_message == '%s'",
				hcjob_dn, object_implementer->getInternalLastError(), object_implementer->getInternalLastErrorText());
		return ERR_AUGMENTED_CCB_MODIFY_FAILED;
	}
	HCF_CC_TRACE(ExecuteAugmentedOperations, "Preparing the needed data structures to create the HcJobScheduler object under the following job:", _obj_id.c_str());

	// Fill the HcJobScheduler MOC attributes
	std::vector<TSL_NS_HCF_IMM::IMM_API_ValuesDefinitionTypes> attr_list;
	TSL_NS_HCF_IMM::IMM_API_ValuesDefinitionTypes scheduler_rdn_attr;
	scheduler_rdn_attr.attrName = const_cast<char *>(imm_object_hcjob_scheduler::HCF_HCJOB_SCHEDULER_SCHEDULER_ID_ATTR_NAME);
	scheduler_rdn_attr.attrType = TSL_NS_HCF_IMM::ATTR_STRINGT;
	scheduler_rdn_attr.attrValuesNum = 1;
	void * tmp_sched_rdn[1] = { const_cast<char *>(imm_object_hcjob_scheduler::HCF_HCJOB_SCHEDULER_DEFAULT_RDN_VALUE) };
	scheduler_rdn_attr.attrValues = tmp_sched_rdn;
	attr_list.push_back(scheduler_rdn_attr);

	// Create the HcJobScheduler MOC instance
	HCF_CC_TRACE(ExecuteAugmentedOperations, "Creating the HcJobScheduler instance under the following object:", hcjob_dn);
	const int call_result = object_implementer->createObjectAugmentCcb(callback_info->ccb_handle, callback_info->admin_owner_handler, imm_object_hcjob_scheduler::HCF_HCJOB_SCHEDULER_CLASS_NAME, hcjob_dn, attr_list);
	int error_code = object_implementer->getInternalLastError();

	if ((call_result != TSL_NS_HCF_IMM::OPERATION_SUCCESS) && (error_code != TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_IMM_EXIST)) {
		HCF_CC_TRACE(ExecuteAugmentedOperationsErr, "Call 'createObjectAugmentCcb' failed.", object_implementer->getInternalLastError(), object_implementer->getInternalLastErrorText());
		HCF_CC_LOG(LEVEL_ERROR, "Call 'createObjectAugmentCcb' failed for object having DN == '%s' under the object having DN == '%s',"
				" error_code == %d, error_message == '%s'", imm_object_hcjob_scheduler::HCF_HCJOB_SCHEDULER_DEFAULT_RDN_VALUE,
				hcjob_dn, object_implementer->getInternalLastError(), object_implementer->getInternalLastErrorText());
		return ERR_AUGMENTED_CCB_CREATE_FAILED;
	}

	// Apply the modification operations
	HCF_CC_TRACE(ExecuteAugmentedOperations, "Applying all the CCB augmented operations previously executed!", "");
	if (object_implementer->applyAugmentCcb(callback_info->ccb_handle) != TSL_NS_HCF_IMM::OPERATION_SUCCESS) {
		HCF_CC_TRACE(ExecuteAugmentedOperationsErr, "Call 'applyAugmentCcb' failed.", object_implementer->getInternalLastError(), object_implementer->getInternalLastErrorText());
		HCF_CC_LOG(LEVEL_ERROR, "Call 'applyAugmentCcb' failed, error_code == %d, error_message == '%s'",
				object_implementer->getInternalLastError(), object_implementer->getInternalLastErrorText());
		return ERR_AUGMENTED_CCB_APPLY_FAILED;
	}
	return ERR_NO_ERROR;
}

int __TSL_CLASS_NAME__::create_progress_report_and_attach_to_parent (const char * parent_dn, TSL_NS_HCF_IMM::object_implementer_interface * object_implementer, TSL_NS_HCF_IMM::IMM_API_CcbHandle ccb_handle, TSL_NS_HCF_IMM::IMM_API_AdminOwnerHandle admin_owner_handler, std::string & progress_report_dn) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(CreateProgressReportAndAttachToParent, "Preparing the needed data structures to create the progressReport structure.", parent_dn, progress_report_dn.c_str());

	// Fill the attributes of the object to be created
	std::vector<TSL_NS_HCF_IMM::IMM_API_ValuesDefinitionTypes> attr_list;
	TSL_NS_HCF_IMM::IMM_API_ValuesDefinitionTypes id_attr;
	id_attr.attrName = const_cast<char *>(HCF_PROGRESS_REPORT_ID_ATTR_NAME);
	id_attr.attrType = TSL_NS_HCF_IMM::ATTR_STRINGT;
	id_attr.attrValuesNum = 1;
	void * tmp[1] = { const_cast<char *>(HCF_PROGRESS_REPORT_DEFAULT_RDN_VALUE) };
	id_attr.attrValues = tmp;
	attr_list.push_back(id_attr);

	TSL_NS_HCF_IMM::IMM_API_ValuesDefinitionTypes act_name_attr;
	act_name_attr.attrName = const_cast<char *>(HCF_PROGRESS_REPORT_ACTION_NAME_ATTR_NAME);
	act_name_attr.attrType = TSL_NS_HCF_IMM::ATTR_STRINGT;
	act_name_attr.attrValuesNum = 1;
	void * tmp_2[1] = { const_cast<char *>("") };
	act_name_attr.attrValues = tmp_2;
	attr_list.push_back(act_name_attr);

	TSL_NS_HCF_IMM::IMM_API_ValuesDefinitionTypes progr_info_attr;
	progr_info_attr.attrName = const_cast<char *>(HCF_PROGRESS_REPORT_PROGRESS_INFO_ATTR_NAME);
	progr_info_attr.attrType = TSL_NS_HCF_IMM::ATTR_STRINGT;
	progr_info_attr.attrValuesNum = 1;
	void * tmp_3[1] = { const_cast<char *>("") };
	progr_info_attr.attrValues = tmp_3;
	attr_list.push_back(progr_info_attr);

	TSL_NS_HCF_IMM::IMM_API_ValuesDefinitionTypes progr_per_attr;
	progr_per_attr.attrName = const_cast<char *>(HCF_PROGRESS_REPORT_PROGRESS_PRECENTAGE_ATTR_NAME);
	progr_per_attr.attrType = TSL_NS_HCF_IMM::ATTR_UINT32T;
	progr_per_attr.attrValuesNum = 1;
	uint32_t progress = 0;
	void * tmp_4[1] = { &progress };
	progr_per_attr.attrValues = tmp_4;
	attr_list.push_back(progr_per_attr);

	TSL_NS_HCF_IMM::IMM_API_ValuesDefinitionTypes result_attr;
	result_attr.attrName = const_cast<char *>(HCF_PROGRESS_REPORT_RESULT_ATTR_NAME);
	result_attr.attrType = TSL_NS_HCF_IMM::ATTR_INT32T;
	result_attr.attrValuesNum = 1;
	int result = RESULT_NOT_AVAILABLE;
	void * tmp_5[1] = { &result };
	result_attr.attrValues = tmp_5;
	attr_list.push_back(result_attr);

	TSL_NS_HCF_IMM::IMM_API_ValuesDefinitionTypes result_info_attr;
	result_info_attr.attrName = const_cast<char *>(HCF_PROGRESS_REPORT_RESULT_INFO_ATTR_NAME);
	result_info_attr.attrType = TSL_NS_HCF_IMM::ATTR_STRINGT;
	result_info_attr.attrValuesNum = 1;
	void * tmp_6[1] = { const_cast<char *>("") };
	result_info_attr.attrValues = tmp_6;
	attr_list.push_back(result_info_attr);

	// Create the progress report structure
	HCF_CC_TRACE(CreateProgressReportAndAttachToParent, "Creating the progressReport structure.", parent_dn, progress_report_dn.c_str());
	TSL_NS_HCF_IMM::IMM_API_ReturnTypes call_result = object_implementer->createObjectAugmentCcb(ccb_handle, admin_owner_handler, HCF_PROGRESS_REPORT_CLASS_NAME, parent_dn, attr_list);
	int error_code = object_implementer->getInternalLastError();

	if ((call_result != TSL_NS_HCF_IMM::OPERATION_SUCCESS) && (error_code != TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_IMM_EXIST)) {
		HCF_CC_TRACE(CreateProgressReportAndAttachToParentErr, "Call 'createObjectAugmentCcb' failed.", object_implementer->getInternalLastError(), object_implementer->getInternalLastErrorText());
		HCF_CC_LOG(LEVEL_ERROR, "Call 'createObjectAugmentCcb' failed for object having DN == '%s' under the object having DN == '%s',"
				" error_code == %d, error_message == '%s'", HCF_PROGRESS_REPORT_DEFAULT_RDN_VALUE, parent_dn,
				object_implementer->getInternalLastError(), object_implementer->getInternalLastErrorText());
		return ERR_AUGMENTED_CCB_CREATE_FAILED;
	}

	// Set the DN of the object previously created
	progress_report_dn = HCF_PROGRESS_REPORT_DEFAULT_RDN_VALUE + std::string(",") + std::string(parent_dn);
	HCF_CC_TRACE(CreateProgressReportAndAttachToParent, "Preparing the needed data structures to update the HcJob object.", parent_dn, progress_report_dn.c_str());

	// Modify the progressReport attribute of HcJob class, adding the DN of the object previously created
	TSL_NS_HCF_IMM::IMM_API_ImmParameter parameter;
	parameter.attrName = const_cast<char *>(HCF_HCJOB_PROGRESS_REPORT_ATTR_NAME);
	parameter.attrType = TSL_NS_HCF_IMM::ATTR_NAMET;
	parameter.attrValuesNum = 1;
	void * tmp_7[1] = { const_cast<char *>(progress_report_dn.c_str()) };
	parameter.attrValues = tmp_7;

	HCF_CC_TRACE(CreateProgressReportAndAttachToParent, "Updating the HcJob object.", parent_dn, progress_report_dn.c_str());
	if (object_implementer->modifyObjectAugmentCcb(ccb_handle, admin_owner_handler, parent_dn, &parameter) != TSL_NS_HCF_IMM::OPERATION_SUCCESS) {
		HCF_CC_TRACE(CreateProgressReportAndAttachToParentErr, "Call 'modifyObjectAugmentCcb' failed.", object_implementer->getInternalLastError(), object_implementer->getInternalLastErrorText());
		HCF_CC_LOG(LEVEL_ERROR, "Call 'modifyObjectAugmentCcb' failed for object having DN == '%s', error_code == %d, error_message == '%s'",
				parent_dn, object_implementer->getInternalLastError(), object_implementer->getInternalLastErrorText());
		return ERR_AUGMENTED_CCB_MODIFY_FAILED;
	}
	return ERR_NO_ERROR;
}

int __TSL_CLASS_NAME__::init_progress_report () {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(InitProgressReport, "Received a request to initialize the progressRerpot structure: initializing the internal objects!", _progress_report_dn.c_str(), "");

	if (const int call_result = initialize_om_handler()) {
		HCF_CC_TRACE(InitProgressReportErr, "Call 'initialize_om_handler' failed.", call_result, "");
		HCF_CC_LOG(LEVEL_ERROR, "Call 'initialize_om_handler' failed, return_code == %d!", call_result);
		return call_result;
	}

	TSL_CRITICAL_SECTION_BEGIN(sync_t, _om_handler_mutex);
		HCF_CC_TRACE(InitProgressReport, "Preparing the needed data structures to update the following attribute:", _progress_report_dn.c_str(), HCF_PROGRESS_REPORT_ACTION_NAME_DEFAULT_VALUE);
		TSL_NS_HCF_IMM::IMM_API_ImmParameter parameter;
		parameter.attrName = const_cast<char *>(HCF_PROGRESS_REPORT_ACTION_NAME_ATTR_NAME);
		parameter.attrType = TSL_NS_HCF_IMM::ATTR_STRINGT;
		parameter.attrValuesNum = 1;
		void * tmp_0[1] = { const_cast<char *>(HCF_PROGRESS_REPORT_ACTION_NAME_DEFAULT_VALUE) };
		parameter.attrValues = tmp_0;

		int imm_error_code = TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_IMM_TRY_AGAIN;
		for (int retries_count = HCF_IMM_OPERATIONS_MAX_NUMBER_OF_RETRIES; (retries_count > 0) && (imm_error_code == TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_IMM_TRY_AGAIN); --retries_count) {
			HCF_CC_TRACE(InitProgressReport, "Modifying the following object attribute:", _progress_report_dn.c_str(), HCF_PROGRESS_REPORT_ACTION_NAME_DEFAULT_VALUE);

			if (_om_handler.modifyAttribute(_progress_report_dn.c_str(), &parameter) != TSL_NS_HCF_IMM::OPERATION_SUCCESS) {
				imm_error_code = _om_handler.getInternalLastError();
				HCF_CC_TRACE(InitProgressReportErr, "Call '_om_handler.modifyAttribute' failed.", _om_handler.getInternalLastError(), _om_handler.getInternalLastErrorText());
				HCF_CC_LOG(LEVEL_ERROR, "Call '_om_handler.modifyAttribute' failed (RETRY == %d) for the attribute %s, "
						"of the object having dn == '%s', return_code == %d, error_message == '%s'!",
						(HCF_IMM_OPERATIONS_MAX_NUMBER_OF_RETRIES - retries_count + 1), parameter.attrName,
						_progress_report_dn.c_str(), _om_handler.getInternalLastError(), _om_handler.getInternalLastErrorText());

				// Wait for a while if the error is "TRY AGAIN": this could solve the IMM problem!!!
				if (imm_error_code == TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_IMM_TRY_AGAIN)	::sleep(1);
			}
			else imm_error_code = TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_NO_ERR;
		}

		if (imm_error_code != TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_NO_ERR) {
			HCF_CC_TRACE(InitProgressReportErr, "Failed to update the progressReport attribute after many tries.", imm_error_code, "");
			return ERR_PROGRESS_REPORT_INIT_FAILED;
		}

		HCF_CC_TRACE(InitProgressReport, "Preparing the needed data structures to update the following attribute:", _progress_report_dn.c_str(), HCF_PROGRESS_REPORT_PROGRESS_INFO_ATTR_NAME);
		parameter.attrName = const_cast<char *>(HCF_PROGRESS_REPORT_PROGRESS_INFO_ATTR_NAME);
		parameter.attrType = TSL_NS_HCF_IMM::ATTR_STRINGT;
		parameter.attrValuesNum = 1;
		void * tmp_1[1] = { const_cast<char *>(HCF_PROGRESS_REPORT_PROGRESS_INFO_INIT_VALUE) };
		parameter.attrValues = tmp_1;

		imm_error_code = TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_IMM_TRY_AGAIN;
		for (int retries_count = HCF_IMM_OPERATIONS_MAX_NUMBER_OF_RETRIES; (retries_count > 0) && (imm_error_code == TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_IMM_TRY_AGAIN); --retries_count) {
			HCF_CC_TRACE(InitProgressReport, "Modifying the following object attribute:", _progress_report_dn.c_str(), HCF_PROGRESS_REPORT_PROGRESS_INFO_ATTR_NAME);

			if (_om_handler.modifyAttribute(_progress_report_dn.c_str(), &parameter) != TSL_NS_HCF_IMM::OPERATION_SUCCESS) {
				imm_error_code = _om_handler.getInternalLastError();
				HCF_CC_TRACE(InitProgressReportErr, "Call '_om_handler.modifyAttribute' failed.", _om_handler.getInternalLastError(), _om_handler.getInternalLastErrorText());
				HCF_CC_LOG(LEVEL_ERROR, "Call '_om_handler.modifyAttribute' failed (RETRY == %d) for the attribute %s, "
						"of the object having dn == '%s', return_code == %d, error_message == '%s'!",
						(HCF_IMM_OPERATIONS_MAX_NUMBER_OF_RETRIES - retries_count + 1), parameter.attrName,
						_progress_report_dn.c_str(), _om_handler.getInternalLastError(), _om_handler.getInternalLastErrorText());

				// Wait for a while if the error is "TRY AGAIN": this could solve the IMM problem!!!
				if (imm_error_code == TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_IMM_TRY_AGAIN)	::sleep(1);
			}
			else imm_error_code = TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_NO_ERR;
		}

		if (imm_error_code != TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_NO_ERR) {
			HCF_CC_TRACE(InitProgressReportErr, "Failed to update the progressReport attribute after many tries.", imm_error_code, "");
			return ERR_PROGRESS_REPORT_INIT_FAILED;
		}

		HCF_CC_TRACE(InitProgressReport, "Preparing the needed data structures to update the following attribute:", _progress_report_dn.c_str(), HCF_PROGRESS_REPORT_PROGRESS_PRECENTAGE_ATTR_NAME);
		parameter.attrName = const_cast<char *>(HCF_PROGRESS_REPORT_PROGRESS_PRECENTAGE_ATTR_NAME);
		parameter.attrType = TSL_NS_HCF_IMM::ATTR_UINT32T;
		parameter.attrValuesNum = 1;
		int attr_2_val = 0;
		void * tmp_2[1] = { &attr_2_val };
		parameter.attrValues = tmp_2;

		imm_error_code = TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_IMM_TRY_AGAIN;
		for (int retries_count = HCF_IMM_OPERATIONS_MAX_NUMBER_OF_RETRIES; (retries_count > 0) && (imm_error_code == TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_IMM_TRY_AGAIN); --retries_count) {
			HCF_CC_TRACE(InitProgressReport, "Modifying the following object attribute:", _progress_report_dn.c_str(), HCF_PROGRESS_REPORT_PROGRESS_PRECENTAGE_ATTR_NAME);

			if (_om_handler.modifyAttribute(_progress_report_dn.c_str(), &parameter) != TSL_NS_HCF_IMM::OPERATION_SUCCESS) {
				imm_error_code = _om_handler.getInternalLastError();
				HCF_CC_TRACE(InitProgressReportErr, "Call '_om_handler.modifyAttribute' failed.", _om_handler.getInternalLastError(), _om_handler.getInternalLastErrorText());
				HCF_CC_LOG(LEVEL_ERROR, "Call '_om_handler.modifyAttribute' failed (RETRY == %d) for the attribute %s, "
						"of the object having dn == '%s', return_code == %d, error_message == '%s'!",
						(HCF_IMM_OPERATIONS_MAX_NUMBER_OF_RETRIES - retries_count + 1), parameter.attrName,
						_progress_report_dn.c_str(), _om_handler.getInternalLastError(), _om_handler.getInternalLastErrorText());

				// Wait for a while if the error is "TRY AGAIN": this could solve the IMM problem!!!
				if (imm_error_code == TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_IMM_TRY_AGAIN)	::sleep(1);
			}
			else imm_error_code = TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_NO_ERR;
		}

		if (imm_error_code != TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_NO_ERR) {
			HCF_CC_TRACE(InitProgressReportErr, "Failed to update the progressReport attribute after many tries.", imm_error_code, "");
			return ERR_PROGRESS_REPORT_INIT_FAILED;
		}

		HCF_CC_TRACE(InitProgressReport, "Preparing the needed data structures to update the following attribute:", _progress_report_dn.c_str(), HCF_PROGRESS_REPORT_RESULT_ATTR_NAME);
		parameter.attrName = const_cast<char *>(HCF_PROGRESS_REPORT_RESULT_ATTR_NAME);
		parameter.attrType = TSL_NS_HCF_IMM::ATTR_INT32T;
		parameter.attrValuesNum = 1;
		int attr_3_val = RESULT_NOT_AVAILABLE;
		void * tmp_3[1] = { &attr_3_val };
		parameter.attrValues = tmp_3;

		imm_error_code = TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_IMM_TRY_AGAIN;
		for (int retries_count = HCF_IMM_OPERATIONS_MAX_NUMBER_OF_RETRIES; (retries_count > 0) && (imm_error_code == TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_IMM_TRY_AGAIN); --retries_count) {
			HCF_CC_TRACE(InitProgressReport, "Modifying the following object attribute:", _progress_report_dn.c_str(), HCF_PROGRESS_REPORT_RESULT_ATTR_NAME);

			if (_om_handler.modifyAttribute(_progress_report_dn.c_str(), &parameter) != TSL_NS_HCF_IMM::OPERATION_SUCCESS) {
				imm_error_code = _om_handler.getInternalLastError();
				HCF_CC_TRACE(InitProgressReportErr, "Call '_om_handler.modifyAttribute' failed.", _om_handler.getInternalLastError(), _om_handler.getInternalLastErrorText());
				HCF_CC_LOG(LEVEL_ERROR, "Call '_om_handler.modifyAttribute' failed (RETRY == %d) for the attribute %s, "
						"of the object having dn == '%s', return_code == %d, error_message == '%s'!",
						(HCF_IMM_OPERATIONS_MAX_NUMBER_OF_RETRIES - retries_count + 1), parameter.attrName,
						_progress_report_dn.c_str(), _om_handler.getInternalLastError(), _om_handler.getInternalLastErrorText());

				// Wait for a while if the error is "TRY AGAIN": this could solve the IMM problem!!!
				if (imm_error_code == TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_IMM_TRY_AGAIN)	::sleep(1);
			}
			else imm_error_code = TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_NO_ERR;
		}

		if (imm_error_code != TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_NO_ERR) {
			HCF_CC_TRACE(InitProgressReportErr, "Failed to update the progressReport attribute after many tries.", imm_error_code, "");
			return ERR_PROGRESS_REPORT_INIT_FAILED;
		}

		HCF_CC_TRACE(InitProgressReport, "Preparing the needed data structures to update the following attribute:", _progress_report_dn.c_str(), HCF_PROGRESS_REPORT_RESULT_INFO_ATTR_NAME);
		parameter.attrName = const_cast<char *>(HCF_PROGRESS_REPORT_RESULT_INFO_ATTR_NAME);
		parameter.attrType = TSL_NS_HCF_IMM::ATTR_STRINGT;
		parameter.attrValuesNum = 1;
		void * tmp_4[1] = { const_cast<char *>("") };
		parameter.attrValues = tmp_4;

		imm_error_code = TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_IMM_TRY_AGAIN;
		for (int retries_count = HCF_IMM_OPERATIONS_MAX_NUMBER_OF_RETRIES; (retries_count > 0) && (imm_error_code == TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_IMM_TRY_AGAIN); --retries_count) {
			HCF_CC_TRACE(InitProgressReport, "Modifying the following object attribute:", _progress_report_dn.c_str(), HCF_PROGRESS_REPORT_RESULT_INFO_ATTR_NAME);

			if (_om_handler.modifyAttribute(_progress_report_dn.c_str(), &parameter) != TSL_NS_HCF_IMM::OPERATION_SUCCESS) {
				imm_error_code = _om_handler.getInternalLastError();
				HCF_CC_TRACE(InitProgressReportErr, "Call '_om_handler.modifyAttribute' failed.", _om_handler.getInternalLastError(), _om_handler.getInternalLastErrorText());
				HCF_CC_LOG(LEVEL_ERROR, "Call '_om_handler.modifyAttribute' failed (RETRY == %d) for the attribute %s, "
						"of the object having dn == '%s', return_code == %d, error_message == '%s'!",
						(HCF_IMM_OPERATIONS_MAX_NUMBER_OF_RETRIES - retries_count + 1), parameter.attrName,
						_progress_report_dn.c_str(), _om_handler.getInternalLastError(), _om_handler.getInternalLastErrorText());

				// Wait for a while if the error is "TRY AGAIN": this could solve the IMM problem!!!
				if (imm_error_code == TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_IMM_TRY_AGAIN)	::sleep(1);
			}
			else imm_error_code = TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_NO_ERR;
		}

		if (imm_error_code != TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_NO_ERR) {
			HCF_CC_TRACE(InitProgressReportErr, "Failed to update the progressReport attribute after many tries.", imm_error_code, "");
			return ERR_PROGRESS_REPORT_INIT_FAILED;
		}

		HCF_CC_TRACE(InitProgressReport, "Preparing the needed data structures to update the following attribute:", _progress_report_dn.c_str(), HCF_PROGRESS_REPORT_STATE_ATTR_NAME);
		parameter.attrName = const_cast<char *>(HCF_PROGRESS_REPORT_STATE_ATTR_NAME);
		parameter.attrType = TSL_NS_HCF_IMM::ATTR_INT32T;
		parameter.attrValuesNum = 1;
		int attr_5_val = STATE_RUNNING;
		void * tmp_5[1] = { &attr_5_val };
		parameter.attrValues = tmp_5;

		imm_error_code = TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_IMM_TRY_AGAIN;
		for (int retries_count = HCF_IMM_OPERATIONS_MAX_NUMBER_OF_RETRIES; (retries_count > 0) && (imm_error_code == TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_IMM_TRY_AGAIN); --retries_count) {
			HCF_CC_TRACE(InitProgressReport, "Modifying the following object attribute:", _progress_report_dn.c_str(), HCF_PROGRESS_REPORT_STATE_ATTR_NAME);

			if (_om_handler.modifyAttribute(_progress_report_dn.c_str(), &parameter) != TSL_NS_HCF_IMM::OPERATION_SUCCESS) {
				imm_error_code = _om_handler.getInternalLastError();
				HCF_CC_TRACE(InitProgressReportErr, "Call '_om_handler.modifyAttribute' failed.", _om_handler.getInternalLastError(), _om_handler.getInternalLastErrorText());
				HCF_CC_LOG(LEVEL_ERROR, "Call '_om_handler.modifyAttribute' failed (RETRY == %d) for the attribute %s, "
						"of the object having dn == '%s', return_code == %d, error_message == '%s'!",
						(HCF_IMM_OPERATIONS_MAX_NUMBER_OF_RETRIES - retries_count + 1), parameter.attrName,
						_progress_report_dn.c_str(), _om_handler.getInternalLastError(), _om_handler.getInternalLastErrorText());

				// Wait for a while if the error is "TRY AGAIN": this could solve the IMM problem!!!
				if (imm_error_code == TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_IMM_TRY_AGAIN)	::sleep(1);
			}
			else imm_error_code = TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_NO_ERR;
		}

		if (imm_error_code != TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_NO_ERR) {
			HCF_CC_TRACE(InitProgressReportErr, "Failed to update the progressReport attribute after many tries.", imm_error_code, "");
			return ERR_PROGRESS_REPORT_INIT_FAILED;
		}

		HCF_CC_TRACE(InitProgressReport, "Preparing the needed data structures to update the following attribute:", _progress_report_dn.c_str(), HCF_PROGRESS_REPORT_ACTION_STARTED_TIME_ATTR_NAME);
		char now_timestamp[1024];
		utils::get_current_timestamp(now_timestamp, TSL_ARRAY_SIZE(now_timestamp));

		parameter.attrName = const_cast<char *>(HCF_PROGRESS_REPORT_ACTION_STARTED_TIME_ATTR_NAME);
		parameter.attrType = TSL_NS_HCF_IMM::ATTR_STRINGT;
		parameter.attrValuesNum = 1;
		void * tmp_6[1] = { now_timestamp };
		parameter.attrValues = tmp_6;

		imm_error_code = TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_IMM_TRY_AGAIN;
		for (int retries_count = HCF_IMM_OPERATIONS_MAX_NUMBER_OF_RETRIES; (retries_count > 0) && (imm_error_code == TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_IMM_TRY_AGAIN); --retries_count) {
			HCF_CC_TRACE(InitProgressReport, "Modifying the following object attribute:", _progress_report_dn.c_str(), HCF_PROGRESS_REPORT_ACTION_STARTED_TIME_ATTR_NAME);

			if (_om_handler.modifyAttribute(_progress_report_dn.c_str(), &parameter) != TSL_NS_HCF_IMM::OPERATION_SUCCESS) {
				imm_error_code = _om_handler.getInternalLastError();
				HCF_CC_TRACE(InitProgressReportErr, "Call '_om_handler.modifyAttribute' failed.", _om_handler.getInternalLastError(), _om_handler.getInternalLastErrorText());
				HCF_CC_LOG(LEVEL_ERROR, "Call '_om_handler.modifyAttribute' failed (RETRY == %d) for the attribute %s, "
						"of the object having dn == '%s', return_code == %d, error_message == '%s'!",
						(HCF_IMM_OPERATIONS_MAX_NUMBER_OF_RETRIES - retries_count + 1), parameter.attrName,
						_progress_report_dn.c_str(), _om_handler.getInternalLastError(), _om_handler.getInternalLastErrorText());

				// Wait for a while if the error is "TRY AGAIN": this could solve the IMM problem!!!
				if (imm_error_code == TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_IMM_TRY_AGAIN)	::sleep(1);
			}
			else imm_error_code = TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_NO_ERR;
		}

		if (imm_error_code != TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_NO_ERR) {
			HCF_CC_TRACE(InitProgressReportErr, "Failed to update the progressReport attribute after many tries.", imm_error_code, "");
			return ERR_PROGRESS_REPORT_INIT_FAILED;
		}

		HCF_CC_TRACE(InitProgressReport, "Preparing the needed data structures to update the following attribute:", _progress_report_dn.c_str(), HCF_PROGRESS_REPORT_LAST_UPDATE_TIME_ATTR_NAME);
		parameter.attrName = const_cast<char *>(HCF_PROGRESS_REPORT_LAST_UPDATE_TIME_ATTR_NAME);
		parameter.attrType = TSL_NS_HCF_IMM::ATTR_STRINGT;
		parameter.attrValuesNum = 1;
		void * tmp_7[1] = { now_timestamp };
		parameter.attrValues = tmp_7;

		// Modify the current object with the provided info
		imm_error_code = TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_IMM_TRY_AGAIN;
		for (int retries_count = HCF_IMM_OPERATIONS_MAX_NUMBER_OF_RETRIES; (retries_count > 0) && (imm_error_code == TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_IMM_TRY_AGAIN); --retries_count) {
			HCF_CC_TRACE(InitProgressReport, "Modifying the following object attribute:", _progress_report_dn.c_str(), HCF_PROGRESS_REPORT_LAST_UPDATE_TIME_ATTR_NAME);

			if (_om_handler.modifyAttribute(_progress_report_dn.c_str(), &parameter) != TSL_NS_HCF_IMM::OPERATION_SUCCESS) {
				imm_error_code = _om_handler.getInternalLastError();
				HCF_CC_TRACE(InitProgressReportErr, "Call '_om_handler.modifyAttribute' failed.", _om_handler.getInternalLastError(), _om_handler.getInternalLastErrorText());
				HCF_CC_LOG(LEVEL_ERROR, "Call '_om_handler.modifyAttribute' failed (RETRY == %d) for the attribute %s, "
						"of the object having dn == '%s', return_code == %d, error_message == '%s'!",
						(HCF_IMM_OPERATIONS_MAX_NUMBER_OF_RETRIES - retries_count + 1), parameter.attrName,
						_progress_report_dn.c_str(), _om_handler.getInternalLastError(), _om_handler.getInternalLastErrorText());

				// Wait for a while if the error is "TRY AGAIN": this could solve the IMM problem!!!
				if (imm_error_code == TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_IMM_TRY_AGAIN)	::sleep(1);
			}
			else imm_error_code = TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_NO_ERR;
		}

		if (imm_error_code != TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_NO_ERR) {
			HCF_CC_TRACE(InitProgressReportErr, "Failed to update the progressReport attribute after many tries.", imm_error_code, "");
			return ERR_PROGRESS_REPORT_INIT_FAILED;
		}

		HCF_CC_TRACE(InitProgressReport, "Preparing the needed data structures to update the following attribute:", _progress_report_dn.c_str(), HCF_PROGRESS_REPORT_ACTION_COMPLETED_TIME_ATTR_NAME);
		parameter.attrName = const_cast<char *>(HCF_PROGRESS_REPORT_ACTION_COMPLETED_TIME_ATTR_NAME);
		parameter.attrType = TSL_NS_HCF_IMM::ATTR_STRINGT;
		parameter.attrValuesNum = 0;

		// Modify the current object with the provided info
		imm_error_code = TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_IMM_TRY_AGAIN;
		for (int retries_count = HCF_IMM_OPERATIONS_MAX_NUMBER_OF_RETRIES; (retries_count > 0) && (imm_error_code == TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_IMM_TRY_AGAIN); --retries_count) {
			HCF_CC_TRACE(InitProgressReport, "Modifying the following object attribute:", _progress_report_dn.c_str(), HCF_PROGRESS_REPORT_ACTION_COMPLETED_TIME_ATTR_NAME);

			if (_om_handler.modifyAttribute(_progress_report_dn.c_str(), &parameter) != TSL_NS_HCF_IMM::OPERATION_SUCCESS) {
				imm_error_code = _om_handler.getInternalLastError();
				HCF_CC_TRACE(InitProgressReportErr, "Call '_om_handler.modifyAttribute' failed.", _om_handler.getInternalLastError(), _om_handler.getInternalLastErrorText());
				HCF_CC_LOG(LEVEL_ERROR, "Call '_om_handler.modifyAttribute' failed (RETRY == %d) for the attribute %s, "
						"of the object having dn == '%s', return_code == %d, error_message == '%s'!",
						(HCF_IMM_OPERATIONS_MAX_NUMBER_OF_RETRIES - retries_count + 1), parameter.attrName,
						_progress_report_dn.c_str(), _om_handler.getInternalLastError(), _om_handler.getInternalLastErrorText());

				// Wait for a while if the error is "TRY AGAIN": this could solve the IMM problem!!!
				if (imm_error_code == TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_IMM_TRY_AGAIN)	::sleep(1);
			}
			else imm_error_code = TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_NO_ERR;
		}

		if (imm_error_code != TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_NO_ERR) {
			HCF_CC_TRACE(InitProgressReportErr, "Failed to update the progressReport attribute after many tries.", imm_error_code, "");
			return ERR_PROGRESS_REPORT_INIT_FAILED;
		}
	TSL_CRITICAL_SECTION_END;

	return ERR_NO_ERROR;
}

int __TSL_CLASS_NAME__::update_progress_report (uint32_t percentage) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(UpdateProgressReport, "Received a request to update the progressRerpot structure: initializing the internal objects!", _progress_report_dn.c_str(), percentage, "");

	if (const int call_result = initialize_om_handler()) {
		HCF_CC_TRACE(UpdateProgressReportErr, "Call 'initialize_om_handler' failed.", call_result, "");
		HCF_CC_LOG(LEVEL_ERROR, "Call 'initialize_om_handler' failed, return_code == %d!", call_result);
		return call_result;
	}

	TSL_CRITICAL_SECTION_BEGIN(sync_t, _om_handler_mutex);
		HCF_CC_TRACE(UpdateProgressReport, "Preparing the needed data structures to update the following attribute:", _progress_report_dn.c_str(), percentage, HCF_PROGRESS_REPORT_LAST_UPDATE_TIME_ATTR_NAME);
		TSL_NS_HCF_IMM::IMM_API_ImmParameter parameter;
		char now_timestamp[1024];
		utils::get_current_timestamp(now_timestamp, TSL_ARRAY_SIZE(now_timestamp));
		parameter.attrName = const_cast<char *>(HCF_PROGRESS_REPORT_LAST_UPDATE_TIME_ATTR_NAME);
		parameter.attrType = TSL_NS_HCF_IMM::ATTR_STRINGT;
		parameter.attrValuesNum = 1;
		void * tmp_0[1] = { now_timestamp };
		parameter.attrValues = tmp_0;

		int imm_error_code = TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_IMM_TRY_AGAIN;
		for (int retries_count = HCF_IMM_OPERATIONS_MAX_NUMBER_OF_RETRIES; (retries_count > 0) && (imm_error_code == TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_IMM_TRY_AGAIN); --retries_count) {
			HCF_CC_TRACE(UpdateProgressReport, "Modifying the following object attribute:", _progress_report_dn.c_str(), percentage, HCF_PROGRESS_REPORT_LAST_UPDATE_TIME_ATTR_NAME);

			if (_om_handler.modifyAttribute(_progress_report_dn.c_str(), &parameter) != TSL_NS_HCF_IMM::OPERATION_SUCCESS) {
				imm_error_code = _om_handler.getInternalLastError();
				HCF_CC_TRACE(UpdateProgressReportErr, "Call '_om_handler.modifyAttribute' failed.", _om_handler.getInternalLastError(), _om_handler.getInternalLastErrorText());
				HCF_CC_LOG(LEVEL_ERROR, "Call '_om_handler.modifyAttribute' failed (RETRY == %d) for the attribute %s, "
						"of the object having dn == '%s', return_code == %d, error_message == '%s'!",
						(HCF_IMM_OPERATIONS_MAX_NUMBER_OF_RETRIES - retries_count + 1), HCF_PROGRESS_REPORT_LAST_UPDATE_TIME_ATTR_NAME,
						_progress_report_dn.c_str(), _om_handler.getInternalLastError(), _om_handler.getInternalLastErrorText());

				// Wait for a while if the error is "TRY AGAIN": this could solve the IMM problem!!!
				if (imm_error_code == TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_IMM_TRY_AGAIN)	::sleep(1);
			}
			else imm_error_code = TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_NO_ERR;
		}

		if (imm_error_code != TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_NO_ERR) {
			HCF_CC_TRACE(UpdateProgressReportErr, "Failed to update the progressReport attribute after many tries.", imm_error_code, "");
			return ERR_PROGRESS_REPORT_MODIFY_FAILED;
		}

		HCF_CC_TRACE(UpdateProgressReport, "Preparing the needed data structures to update the following attribute:", _progress_report_dn.c_str(), percentage, HCF_PROGRESS_REPORT_PROGRESS_PRECENTAGE_ATTR_NAME);
		parameter.attrName = const_cast<char *>(HCF_PROGRESS_REPORT_PROGRESS_PRECENTAGE_ATTR_NAME);
		parameter.attrType = TSL_NS_HCF_IMM::ATTR_UINT32T;
		parameter.attrValuesNum = 1;
		void * tmp_1[1] = { &percentage };
		parameter.attrValues = tmp_1;

		imm_error_code = TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_IMM_TRY_AGAIN;
		for (int retries_count = HCF_IMM_OPERATIONS_MAX_NUMBER_OF_RETRIES; (retries_count > 0) && (imm_error_code == TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_IMM_TRY_AGAIN); --retries_count) {
			HCF_CC_TRACE(UpdateProgressReport, "Modifying the following object attribute:", _progress_report_dn.c_str(), percentage, HCF_PROGRESS_REPORT_PROGRESS_PRECENTAGE_ATTR_NAME);

			if (_om_handler.modifyAttribute(_progress_report_dn.c_str(), &parameter) != TSL_NS_HCF_IMM::OPERATION_SUCCESS) {
				imm_error_code = _om_handler.getInternalLastError();
				HCF_CC_TRACE(UpdateProgressReportErr, "Call '_om_handler.modifyAttribute' failed.", _om_handler.getInternalLastError(), _om_handler.getInternalLastErrorText());
				HCF_CC_LOG(LEVEL_ERROR, "Call '_om_handler.modifyAttribute' failed (RETRY == %d) for the attribute %s, "
						"of the object having dn == '%s', return_code == %d, error_message == '%s'!",
						(HCF_IMM_OPERATIONS_MAX_NUMBER_OF_RETRIES - retries_count + 1), HCF_PROGRESS_REPORT_PROGRESS_PRECENTAGE_ATTR_NAME,
						_progress_report_dn.c_str(), _om_handler.getInternalLastError(), _om_handler.getInternalLastErrorText());

				// Wait for a while if the error is "TRY AGAIN": this could solve the IMM problem!!!
				if (imm_error_code == TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_IMM_TRY_AGAIN)	::sleep(1);
			}
			else imm_error_code = TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_NO_ERR;
		}

		if (imm_error_code != TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_NO_ERR) {
			HCF_CC_TRACE(UpdateProgressReportErr, "Failed to update the progressReport attribute after many tries.", imm_error_code, "");
			return ERR_PROGRESS_REPORT_MODIFY_FAILED;
		}
	TSL_CRITICAL_SECTION_END;

	return ERR_NO_ERROR;
}

int __TSL_CLASS_NAME__::finalize_progress_report (bool is_success, const std::string & msg) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(FinalizeProgressReport, "Received a request to finalize the progressRerpot structure: initializing the internal objects!", _progress_report_dn.c_str(), is_success, msg.c_str(), "");

	if (const int call_result = initialize_om_handler()) {
		HCF_CC_TRACE(FinalizeProgressReportErr, "Call 'initialize_om_handler' failed.", call_result, "");
		HCF_CC_LOG(LEVEL_ERROR, "Call 'initialize_om_handler' failed, return_code == %d!", call_result);
		return call_result;
	}

	TSL_CRITICAL_SECTION_BEGIN(sync_t, _om_handler_mutex);
		HCF_CC_TRACE(FinalizeProgressReport, "Preparing the needed data structures to update the following attribute:", _progress_report_dn.c_str(), is_success, msg.c_str(), HCF_PROGRESS_REPORT_ACTION_COMPLETED_TIME_ATTR_NAME);
		TSL_NS_HCF_IMM::IMM_API_ImmParameter parameter;
		char now_timestamp[1024];
		utils::get_current_timestamp(now_timestamp, TSL_ARRAY_SIZE(now_timestamp));
		parameter.attrName = const_cast<char *>(HCF_PROGRESS_REPORT_ACTION_COMPLETED_TIME_ATTR_NAME);
		parameter.attrType = TSL_NS_HCF_IMM::ATTR_STRINGT;
		parameter.attrValuesNum = 1;
		void * tmp_0[1] = { now_timestamp };
		parameter.attrValues = tmp_0;

		int imm_error_code = TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_IMM_TRY_AGAIN;
		for (int retries_count = HCF_IMM_OPERATIONS_MAX_NUMBER_OF_RETRIES; (retries_count > 0) && (imm_error_code == TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_IMM_TRY_AGAIN); --retries_count) {
			HCF_CC_TRACE(FinalizeProgressReport, "Modifying the following object attribute:", _progress_report_dn.c_str(), is_success, msg.c_str(), HCF_PROGRESS_REPORT_ACTION_COMPLETED_TIME_ATTR_NAME);

			if (_om_handler.modifyAttribute(_progress_report_dn.c_str(), &parameter) != TSL_NS_HCF_IMM::OPERATION_SUCCESS) {
				imm_error_code = _om_handler.getInternalLastError();
				HCF_CC_TRACE(FinalizeProgressReportErr, "Call '_om_handler.modifyAttribute' failed.", _om_handler.getInternalLastError(), _om_handler.getInternalLastErrorText());
				HCF_CC_LOG(LEVEL_ERROR, "Call '_om_handler.modifyAttribute' failed (RETRY == %d) for the attribute %s, "
						"of the object having dn == '%s', return_code == %d, error_message == '%s'!",
						(HCF_IMM_OPERATIONS_MAX_NUMBER_OF_RETRIES - retries_count + 1), HCF_PROGRESS_REPORT_ACTION_COMPLETED_TIME_ATTR_NAME,
						_progress_report_dn.c_str(), _om_handler.getInternalLastError(), _om_handler.getInternalLastErrorText());

				// Wait for a while if the error is "TRY AGAIN": this could solve the IMM problem!!!
				if (imm_error_code == TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_IMM_TRY_AGAIN)	::sleep(1);
			}
			else imm_error_code = TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_NO_ERR;
		}

		if (imm_error_code != TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_NO_ERR) {
			HCF_CC_TRACE(FinalizeProgressReportErr, "Failed to update the progressReport attribute after many tries.", imm_error_code, "");
			return ERR_PROGRESS_REPORT_FINALIZE_FAILED;
		}

		HCF_CC_TRACE(FinalizeProgressReport, "Preparing the needed data structures to update the following attribute:", _progress_report_dn.c_str(), is_success, msg.c_str(), HCF_PROGRESS_REPORT_LAST_UPDATE_TIME_ATTR_NAME);
		parameter.attrName = const_cast<char *>(HCF_PROGRESS_REPORT_LAST_UPDATE_TIME_ATTR_NAME);
		parameter.attrType = TSL_NS_HCF_IMM::ATTR_STRINGT;
		parameter.attrValuesNum = 1;
		void * tmp_1[1] = { now_timestamp };
		parameter.attrValues = tmp_1;

		imm_error_code = TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_IMM_TRY_AGAIN;
		for (int retries_count = HCF_IMM_OPERATIONS_MAX_NUMBER_OF_RETRIES; (retries_count > 0) && (imm_error_code == TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_IMM_TRY_AGAIN); --retries_count) {
			HCF_CC_TRACE(FinalizeProgressReport, "Modifying the following object attribute:", _progress_report_dn.c_str(), is_success, msg.c_str(), HCF_PROGRESS_REPORT_LAST_UPDATE_TIME_ATTR_NAME);

			if (_om_handler.modifyAttribute(_progress_report_dn.c_str(), &parameter) != TSL_NS_HCF_IMM::OPERATION_SUCCESS) {
				imm_error_code = _om_handler.getInternalLastError();
				HCF_CC_TRACE(FinalizeProgressReportErr, "Call '_om_handler.modifyAttribute' failed.", _om_handler.getInternalLastError(), _om_handler.getInternalLastErrorText());
				HCF_CC_LOG(LEVEL_ERROR, "Call '_om_handler.modifyAttribute' failed (RETRY == %d) for the attribute %s, "
						"of the object having dn == '%s', return_code == %d, error_message == '%s'!",
						(HCF_IMM_OPERATIONS_MAX_NUMBER_OF_RETRIES - retries_count + 1), HCF_PROGRESS_REPORT_PROGRESS_PRECENTAGE_ATTR_NAME,
						_progress_report_dn.c_str(), _om_handler.getInternalLastError(), _om_handler.getInternalLastErrorText());

				// Wait for a while if the error is "TRY AGAIN": this could solve the IMM problem!!!
				if (imm_error_code == TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_IMM_TRY_AGAIN)	::sleep(1);
			}
			else imm_error_code = TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_NO_ERR;
		}

		if (imm_error_code != TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_NO_ERR) {
			HCF_CC_TRACE(FinalizeProgressReportErr, "Failed to update the progressReport attribute after many tries.", imm_error_code, "");
			return ERR_PROGRESS_REPORT_FINALIZE_FAILED;
		}

		HCF_CC_TRACE(FinalizeProgressReport, "Preparing the needed data structures to update the following attribute:", _progress_report_dn.c_str(), is_success, msg.c_str(), HCF_PROGRESS_REPORT_PROGRESS_PRECENTAGE_ATTR_NAME);
		parameter.attrName = const_cast<char *>(HCF_PROGRESS_REPORT_PROGRESS_PRECENTAGE_ATTR_NAME);
		parameter.attrType = TSL_NS_HCF_IMM::ATTR_UINT32T;
		parameter.attrValuesNum = 1;
		uint32_t percentage = 100;
		void * tmp_2[1] = { &percentage };
		parameter.attrValues = tmp_2;

		imm_error_code = TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_IMM_TRY_AGAIN;
		for (int retries_count = HCF_IMM_OPERATIONS_MAX_NUMBER_OF_RETRIES; (retries_count > 0) && (imm_error_code == TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_IMM_TRY_AGAIN); --retries_count) {
			HCF_CC_TRACE(FinalizeProgressReport, "Modifying the following object attribute:", _progress_report_dn.c_str(), is_success, msg.c_str(), HCF_PROGRESS_REPORT_PROGRESS_PRECENTAGE_ATTR_NAME);

			if (_om_handler.modifyAttribute(_progress_report_dn.c_str(), &parameter) != TSL_NS_HCF_IMM::OPERATION_SUCCESS) {
				imm_error_code = _om_handler.getInternalLastError();
				HCF_CC_TRACE(FinalizeProgressReportErr, "Call '_om_handler.modifyAttribute' failed.", _om_handler.getInternalLastError(), _om_handler.getInternalLastErrorText());
				HCF_CC_LOG(LEVEL_ERROR, "Call '_om_handler.modifyAttribute' failed (RETRY == %d) for the attribute %s, "
						"of the object having dn == '%s', return_code == %d, error_message == '%s'!",
						(HCF_IMM_OPERATIONS_MAX_NUMBER_OF_RETRIES - retries_count + 1), HCF_PROGRESS_REPORT_PROGRESS_PRECENTAGE_ATTR_NAME,
						_progress_report_dn.c_str(), _om_handler.getInternalLastError(), _om_handler.getInternalLastErrorText());

				// Wait for a while if the error is "TRY AGAIN": this could solve the IMM problem!!!
				if (imm_error_code == TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_IMM_TRY_AGAIN)	::sleep(1);
			}
			else imm_error_code = TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_NO_ERR;
		}

		if (imm_error_code != TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_NO_ERR) {
			HCF_CC_TRACE(FinalizeProgressReportErr, "Failed to update the progressReport attribute after many tries.", imm_error_code, "");
			return ERR_PROGRESS_REPORT_FINALIZE_FAILED;
		}

		HCF_CC_TRACE(FinalizeProgressReport, "Preparing the needed data structures to update the following attribute:", _progress_report_dn.c_str(), is_success, msg.c_str(), HCF_PROGRESS_REPORT_RESULT_ATTR_NAME);
		parameter.attrName = const_cast<char *>(HCF_PROGRESS_REPORT_RESULT_ATTR_NAME);
		parameter.attrType = TSL_NS_HCF_IMM::ATTR_INT32T;
		parameter.attrValuesNum = 1;
		int result = (is_success) ? RESULT_SUCCESS : RESULT_FAILURE;
		void * tmp_3[1] = { &result };
		parameter.attrValues = tmp_3;

		imm_error_code = TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_IMM_TRY_AGAIN;
		for (int retries_count = HCF_IMM_OPERATIONS_MAX_NUMBER_OF_RETRIES; (retries_count > 0) && (imm_error_code == TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_IMM_TRY_AGAIN); --retries_count) {
			HCF_CC_TRACE(FinalizeProgressReport, "Modifying the following object attribute:", _progress_report_dn.c_str(), is_success, msg.c_str(), HCF_PROGRESS_REPORT_RESULT_ATTR_NAME);
			if (_om_handler.modifyAttribute(_progress_report_dn.c_str(), &parameter) != TSL_NS_HCF_IMM::OPERATION_SUCCESS) {
				imm_error_code = _om_handler.getInternalLastError();
				HCF_CC_TRACE(FinalizeProgressReportErr, "Call '_om_handler.modifyAttribute' failed.", _om_handler.getInternalLastError(), _om_handler.getInternalLastErrorText());
				HCF_CC_LOG(LEVEL_ERROR, "Call '_om_handler.modifyAttribute' failed (RETRY == %d) for the attribute %s, "
						"of the object having dn == '%s', return_code == %d, error_message == '%s'!",
						(HCF_IMM_OPERATIONS_MAX_NUMBER_OF_RETRIES - retries_count + 1), HCF_PROGRESS_REPORT_RESULT_ATTR_NAME,
						_progress_report_dn.c_str(), _om_handler.getInternalLastError(), _om_handler.getInternalLastErrorText());

				// Wait for a while if the error is "TRY AGAIN": this could solve the IMM problem!!!
				if (imm_error_code == TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_IMM_TRY_AGAIN)	::sleep(1);
			}
			else imm_error_code = TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_NO_ERR;
		}

		if (imm_error_code != TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_NO_ERR) {
			HCF_CC_TRACE(FinalizeProgressReportErr, "Failed to update the progressReport attribute after many tries.", imm_error_code, "");
			return ERR_PROGRESS_REPORT_FINALIZE_FAILED;
		}

		HCF_CC_TRACE(FinalizeProgressReport, "Preparing the needed data structures to update the following attribute:", _progress_report_dn.c_str(), is_success, msg.c_str(), HCF_PROGRESS_REPORT_STATE_ATTR_NAME);
		parameter.attrName = const_cast<char *>(HCF_PROGRESS_REPORT_STATE_ATTR_NAME);
		parameter.attrType = TSL_NS_HCF_IMM::ATTR_INT32T;
		parameter.attrValuesNum = 1;
		int state = (is_success) ? STATE_FINISHED : STATE_CANCELLED;
		void * tmp_4[1] = { &state };
		parameter.attrValues = tmp_4;

		imm_error_code = TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_IMM_TRY_AGAIN;
		for (int retries_count = HCF_IMM_OPERATIONS_MAX_NUMBER_OF_RETRIES; (retries_count > 0) && (imm_error_code == TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_IMM_TRY_AGAIN); --retries_count) {
			HCF_CC_TRACE(FinalizeProgressReport, "Modifying the following object attribute:", _progress_report_dn.c_str(), is_success, msg.c_str(), HCF_PROGRESS_REPORT_STATE_ATTR_NAME);

			if (_om_handler.modifyAttribute(_progress_report_dn.c_str(), &parameter) != TSL_NS_HCF_IMM::OPERATION_SUCCESS) {
				imm_error_code = _om_handler.getInternalLastError();
				HCF_CC_TRACE(FinalizeProgressReportErr, "Call '_om_handler.modifyAttribute' failed.", _om_handler.getInternalLastError(), _om_handler.getInternalLastErrorText());
				HCF_CC_LOG(LEVEL_ERROR, "Call '_om_handler.modifyAttribute' failed (RETRY == %d) for the attribute %s, "
						"of the object having dn == '%s', return_code == %d, error_message == '%s'!",
						(HCF_IMM_OPERATIONS_MAX_NUMBER_OF_RETRIES - retries_count + 1), HCF_PROGRESS_REPORT_STATE_ATTR_NAME,
						_progress_report_dn.c_str(), _om_handler.getInternalLastError(), _om_handler.getInternalLastErrorText());

				// Wait for a while if the error is "TRY AGAIN": this could solve the IMM problem!!!
				if (imm_error_code == TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_IMM_TRY_AGAIN)	::sleep(1);
			}
			else imm_error_code = TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_NO_ERR;
		}

		if (imm_error_code != TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_NO_ERR) {
			HCF_CC_TRACE(FinalizeProgressReportErr, "Failed to update the progressReport attribute after many tries.", imm_error_code, "");
			return ERR_PROGRESS_REPORT_FINALIZE_FAILED;
		}

		HCF_CC_TRACE(FinalizeProgressReport, "Preparing the needed data structures to update the following attribute:", _progress_report_dn.c_str(), is_success, msg.c_str(), HCF_PROGRESS_REPORT_RESULT_INFO_ATTR_NAME);
		parameter.attrName = const_cast<char *>(HCF_PROGRESS_REPORT_RESULT_INFO_ATTR_NAME);
		parameter.attrType = TSL_NS_HCF_IMM::ATTR_STRINGT;
		parameter.attrValuesNum = 1;
		void * tmp_5[1] = { const_cast<char *>(msg.c_str()) };
		parameter.attrValues = tmp_5;

		imm_error_code = TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_IMM_TRY_AGAIN;
		for (int retries_count = HCF_IMM_OPERATIONS_MAX_NUMBER_OF_RETRIES; (retries_count > 0) && (imm_error_code == TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_IMM_TRY_AGAIN); --retries_count) {
			HCF_CC_TRACE(FinalizeProgressReport, "Modifying the following object attribute:", _progress_report_dn.c_str(), is_success, msg.c_str(), HCF_PROGRESS_REPORT_RESULT_INFO_ATTR_NAME);

			if (_om_handler.modifyAttribute(_progress_report_dn.c_str(), &parameter) != TSL_NS_HCF_IMM::OPERATION_SUCCESS) {
				imm_error_code = _om_handler.getInternalLastError();
				HCF_CC_TRACE(FinalizeProgressReportErr, "Call '_om_handler.modifyAttribute' failed.", _om_handler.getInternalLastError(), _om_handler.getInternalLastErrorText());
				HCF_CC_LOG(LEVEL_ERROR, "Call '_om_handler.modifyAttribute' failed (RETRY == %d) for the attribute %s, "
						"of the object having dn == '%s', return_code == %d, error_message == '%s'!",
						(HCF_IMM_OPERATIONS_MAX_NUMBER_OF_RETRIES - retries_count + 1), HCF_PROGRESS_REPORT_RESULT_INFO_ATTR_NAME,
						_progress_report_dn.c_str(), _om_handler.getInternalLastError(), _om_handler.getInternalLastErrorText());

				// Wait for a while if the error is "TRY AGAIN": this could solve the IMM problem!!!
				if (imm_error_code == TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_IMM_TRY_AGAIN)	::sleep(1);
			}
			else imm_error_code = TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_NO_ERR;
		}

		if (imm_error_code != TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_NO_ERR) {
			HCF_CC_TRACE(FinalizeProgressReportErr, "Failed to update the progressReport attribute after many tries.", imm_error_code, "");
			return ERR_PROGRESS_REPORT_FINALIZE_FAILED;
		}

		HCF_CC_TRACE(FinalizeProgressReport, "Preparing the needed data structures to update the following attribute:", _progress_report_dn.c_str(), is_success, msg.c_str(), HCF_PROGRESS_REPORT_PROGRESS_INFO_ATTR_NAME);
		parameter.attrName = const_cast<char *>(HCF_PROGRESS_REPORT_PROGRESS_INFO_ATTR_NAME);
		parameter.attrType = TSL_NS_HCF_IMM::ATTR_STRINGT;
		parameter.attrValuesNum = 1;
		void * tmp_6[1] = { const_cast<char *>(HCF_PROGRESS_REPORT_PROGRESS_INFO_FINAL_VALUE) };
		parameter.attrValues = tmp_6;

		imm_error_code = TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_IMM_TRY_AGAIN;
		for (int retries_count = HCF_IMM_OPERATIONS_MAX_NUMBER_OF_RETRIES; (retries_count > 0) && (imm_error_code == TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_IMM_TRY_AGAIN); --retries_count) {
			HCF_CC_TRACE(FinalizeProgressReport, "Modifying the following object attribute:", _progress_report_dn.c_str(), is_success, msg.c_str(), HCF_PROGRESS_REPORT_PROGRESS_INFO_ATTR_NAME);

			if (_om_handler.modifyAttribute(_progress_report_dn.c_str(), &parameter) != TSL_NS_HCF_IMM::OPERATION_SUCCESS) {
				imm_error_code = _om_handler.getInternalLastError();
				HCF_CC_TRACE(FinalizeProgressReportErr, "Call '_om_handler.modifyAttribute' failed.", _om_handler.getInternalLastError(), _om_handler.getInternalLastErrorText());
				HCF_CC_LOG(LEVEL_ERROR, "Call '_om_handler.modifyAttribute' failed (RETRY == %d) for the attribute %s, "
						"of the object having dn == '%s', return_code == %d, error_message == '%s'!",
						(HCF_IMM_OPERATIONS_MAX_NUMBER_OF_RETRIES - retries_count + 1), HCF_PROGRESS_REPORT_RESULT_INFO_ATTR_NAME,
						_progress_report_dn.c_str(), _om_handler.getInternalLastError(), _om_handler.getInternalLastErrorText());

				// Wait for a while if the error is "TRY AGAIN": this could solve the IMM problem!!!
				if (imm_error_code == TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_IMM_TRY_AGAIN)	::sleep(1);
			}
			else imm_error_code = TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_NO_ERR;
		}

		if (imm_error_code != TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_NO_ERR) {
			HCF_CC_TRACE(FinalizeProgressReportErr, "Failed to update the progressReport attribute after many tries.", imm_error_code, "");
			return ERR_PROGRESS_REPORT_FINALIZE_FAILED;
		}
	TSL_CRITICAL_SECTION_END;

	return ERR_NO_ERROR;
}

HCF_NS_HCF_SRV_END
