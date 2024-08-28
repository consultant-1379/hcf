#ifndef __INCLUDE_GUARD_CLASS_SRV_IMM_OBJECT_HCJOB__
#define __INCLUDE_GUARD_CLASS_SRV_IMM_OBJECT_HCJOB__ imm_object_hcjob

/**
 *  @file imm_object_hcjob.h
 *	@brief
 *	@author xcasale (Alessio Cascone)
 *	@date 2014-11-17
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
 *	| 2014-001 | 2014-11-17 | xcasale      | Creation and first revision.        |
 *	+==========+============+==============+=====================================+
 */

#include <set>
#include <string>

#include "hcf/core/guard.h"
#include "hcf/imm/imm_object.h"

#include "constants.h"
#include "srv_macros.h"
#include "app_types.h"

#include "hcf/trace/com_ericsson_common_hcf_srv_hcjob.h"
#include "hcf/trace/tracing_macros.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_SRV_IMM_OBJECT_HCJOB__

HCF_NS_HCF_SRV_BEGIN

HCF_CC_FUNCTION_TRACER_CLASS_DEFINE

class __TSL_CLASS_NAME__ : public TSL_NS_HCF_IMM::imm_object {
	//=========//
	// Friends //
	//=========//
friend class job_executor;

	//==============//
	// Constructors //
	//==============//
public:
	inline TSL_CTOR (const char * class_name)
		TSL_CTOR_INIT_LIST(imm_object(class_name), _failed_rules_dns(), _job_to_trigger_dn(), _last_report_file_name(),
				_last_run_time(), _local_file_store_path(), _progress_report_dn(), _rules_categories(), _status(),
				_failed_rules_dns_old(), _job_to_trigger_dn_old(), _last_report_file_name_old(), _last_run_time_old(),
				_local_file_store_path_old(), _progress_report_dn_old(), _rules_categories_old(), _status_old()) {}

private:
	TSL_CTOR (const __TSL_CLASS_NAME__ & rhs);

	//============//
	// Destructor //
	//============//
public:
	virtual inline TSL_DTOR () {}

	//====================================================//
	// imm_object class Virtual Functions Implementation //
	//===================================================//
public:
	/**
	 * This method sets the attributes values for the current instance of the current class.
	 * The value of each class field is set according to the values present in attributes argument.
	 *
	 * @param[in]  attributes - A vector of pairs representing the new object attributes.
	 *             Each pair has as first item a string indicating the attribute name and second item
	 *             a pointer to the attribute value.
	 * @param[in]  callback_info - Some useful info about the callback.
	 * @return 0 is returned on success, a negative value on failure.
	 */
	virtual int set_object_attributes (std::vector<std::pair<std::string, std::vector<void *> > > & attributes, TSL_NS_HCF_IMM::IMM_API_ObjectImplementer_CallbackInfo * callback_info = 0);

	/**
	 * This method finalizes the current instance of current class.
	 *
	 * @param[in]  callback_info - Some useful info about the callback.
	 * @return 0 is returned on success, a negative value on failure.
	 */
	virtual int finalize_object (TSL_NS_HCF_IMM::IMM_API_ObjectImplementer_CallbackInfo * callback_info = 0);

	/**
	 * This method completes the modification of the current instance of the current class.
	 *
	 * @param[in]  callback_info - Some useful info about the callback.
	 * @return 0 is returned on success, a negative value on failure.
	 */
	inline virtual int complete_object_modification (TSL_NS_HCF_IMM::IMM_API_ObjectImplementer_CallbackInfo * TSL_UNUSED_PAR(callback_info) = 0) {
		HCF_CC_TRACE_FUNCTION;
		return 0;
	}

	/**
	 * This method applies the modification of the current instance of the current class.
	 *
	 * @param[in]  callback_info - Some useful info about the callback.
	 * @return 0 is returned on success, a negative value on failure.
	 */
	inline virtual int apply_object_modification (TSL_NS_HCF_IMM::IMM_API_ObjectImplementer_CallbackInfo * TSL_UNUSED_PAR(callback_info) = 0) {
		HCF_CC_TRACE_FUNCTION;
		return 0;
	}

	/**
	 * This method aborts the modification of the current instance of the current class.
	 *
	 * @param[in]  callback_info - Some useful info about the callback.
	 * @return 0 is returned on success, a negative value on failure.
	 */
	virtual int abort_object_modification (TSL_NS_HCF_IMM::IMM_API_ObjectImplementer_CallbackInfo * callback_info = 0);

	/**
	 * This method updates the value of the runtime attribute with the provided name.
	 *
	 * @param[in]  attr_name - The name of the attribute to be updated.
	 * @param[out] param_to_update - A structure containing all the info about the parameter.
	 * @return 0 is returned on success, a negative value on failure.
	 */
	inline virtual int update_runtime_attribute (const char * TSL_UNUSED_PAR(attr_name), TSL_NS_HCF_IMM::IMM_API_ImmParameter & TSL_UNUSED_PAR(param_to_update)) {
		// No runtime attributes for now!
		HCF_CC_TRACE_FUNCTION;
		return ERR_OPERATION_NOT_IMPLEMENTED;
	}

	/**
	 * This method executes the given administrative operation on the current object.
	 *
	 * @param[in]  operation_id - The administrative operation id.
	 * @param[in]  arguments - The administrative operation optional arguments.
	 * @return 0 is returned on success, a negative value on failure.
	 */
	virtual int execute_admin_operation (uint64_t operation_id, TSL_NS_HCF_IMM::IMM_API_AdminOperationParamType ** arguments);

	//==================//
	// Fields Modifiers //
	//==================//
public:
	inline std::set<std::string> get_failed_rules_dns () const { return _failed_rules_dns; }
	int clear_failed_rules ();
	int set_failed_rule (const std::string & rule_id, const std::string & reason);

	inline std::string get_job_to_trigger_dn () const { return _job_to_trigger_dn; }

	inline std::string get_last_report_file_name () const { return _last_report_file_name; }
	inline void set_last_report_file_name (const std::string & report_name) {
		modify_object_attribute(HCF_HCJOB_LAST_REPORT_FILE_NAME_ATTR_NAME, TSL_NS_HCF_IMM::ATTR_STRINGT, const_cast<char *>(report_name.c_str()));
	}

	inline std::string get_last_run_time () const { return _last_run_time; }
	inline void set_last_run_time (const std::string & last_run_time) {
		modify_object_attribute(HCF_HCJOB_LAST_RUN_TIME_ATTR_NAME, TSL_NS_HCF_IMM::ATTR_STRINGT, const_cast<char *>(last_run_time.c_str()));
	}

	inline std::string get_local_file_store_path () const { return _local_file_store_path; }
	inline void set_local_file_store_path (const std::string & local_file_store_path) {
		modify_object_attribute(HCF_HCJOB_LOCAL_FILE_STORE_PATH_ATTR_NAME, TSL_NS_HCF_IMM::ATTR_STRINGT, const_cast<char *>(local_file_store_path.c_str()));
	}

	inline std::set<category_t> get_rules_categories () const { return _rules_categories; }

	inline health_status_t get_status () const { return _status; }
	inline void set_status (health_status_t status) {
		modify_object_attribute(HCF_HCJOB_STATUS_ATTR_NAME, TSL_NS_HCF_IMM::ATTR_INT32T, &status);
	}

	//===========//
	// Functions //
	//===========//
public:
	/**
	 * This method checks if there is a job execution ongoing.
	 *
	 * return true if there is a job running, false otherwise.
	 */
	inline static bool is_job_execution_ongoing () {
		TSL_CRITICAL_SECTION_BEGIN(sync_t, _ongoing_job_mutex);
			return !(_ongoing_job_dn.empty());
		TSL_CRITICAL_SECTION_END;
	}

	/**
	 * This method returns the DN of the job currently running (if any).
	 * In case there is no job currently running, the empty string is returned.
	 *
	 * @return a string value, empty if no job is running.
	 */
	inline static const std::string get_running_job_dn () {
		TSL_CRITICAL_SECTION_BEGIN(sync_t, _ongoing_job_mutex);
			return _ongoing_job_dn;
		TSL_CRITICAL_SECTION_END;
	}

	/**
	 * This method executes the current instance of HcJob MOC.
	 * It's possible to provide to the method also a couple URI/password to export the job execution result.
	 *
	 * @param[in]  export_uri - The URI of the remote destination.
	 * @param[in]  export_password - The password of the remote destination.
	 * @param[in]  is_scheduled_execution - Flag indicating the type of job execution.
	 */
	int execute (std::string export_uri, std::string export_password, bool is_scheduled_execution = false);

private:
	/**
	 * This method sets the DN of the job that is currently running.
	 *
	 * @param[in]  dn - The DN of the ongoing job. If empty, no job is ongoing.
	 */
	inline static void job_execution_ongoing (std::string dn = "") {
		TSL_CRITICAL_SECTION_BEGIN(sync_t, _ongoing_job_mutex);
			_ongoing_job_dn = dn;
		TSL_CRITICAL_SECTION_END;
	}

	/**
	 * This method checks if the provided string is a valid job name or not.
	 *
	 * @param[in]  name - The string to be checked.
	 * @return true/false.
	 */
	static inline bool is_job_name_valid (const std::string & name) {
		for (size_t i = 0; i < name.size(); ++i)
			if (!(::isalnum(name[i])) && (name[i] != '-') && (name[i] != '_'))	return false;
		return true;
	}

	/**
	 * This method executes some operations to be performed together with HcJob instance creation.
	 * The operations are:
	 * 		1. progressReport structure creation for HcJob MOC.
	 * 		2. HcJob progressReport attribute value update.
	 * 		3. HcJob localFileStorePath attribute value update.
	 * 		4. HcJobScheduler MOC instance creation.
	 * 		5. progressReport structure creation for HcJobScheduler MOC.
	 * 		6. HcJobScheduler progressReport attribute value update.
	 *
	 * @param[in]  callback_info - Some useful info about the callback.
	 * @return 0 is returned on success, a negative value on failure.
	 */
	int execute_augmented_operations (TSL_NS_HCF_IMM::IMM_API_ObjectImplementer_CallbackInfo * callback_info);

	/**
	 * This method creates the progress report structure under the provided
	 * parent object and updates the progressReport attribute in the parent class.
	 *
	 * @param[in]  parent_dn - The DN of the object under which the progressReport must be created.
	 * @param[in]  object_implementer - The object implementer to be used.
	 * @param[in]  ccb_handle - The CCB handle.
	 * @param[in]  admin_owner_handler - The administrative owner handle.
	 * @param[out] progress_report_dn - The DN of the created object.
	 * @return 0 is returned on success, a negative value on failure.
	 */
	int create_progress_report_and_attach_to_parent (const char * parent_dn, TSL_NS_HCF_IMM::object_implementer_interface * object_implementer, TSL_NS_HCF_IMM::IMM_API_CcbHandle ccb_handle, TSL_NS_HCF_IMM::IMM_API_AdminOwnerHandle admin_owner_handler, std::string & progress_report_dn);

	/**
	 * This method initializes the progressReport structure with the default values to be used when a job execution starts.
	 *
	 * @return 0 is returned on success, a negative value on failure.
	 */
	int init_progress_report ();

	/**
	 * This method updates the progressReport structure with the provided percentage.
	 *
	 * @param[in]  percentage - The progress percentage.
	 * @return 0 is returned on success, a negative value on failure.
	 */
	int update_progress_report (uint32_t percentage);

	/**
	 * This method finalizes the progressReport structure with success/failure and with the provided info message.
	 *
	 * @param[in]  is_success - Boolean value indicating if the operation was successful or not.
	 * @param[in]  msg - The info message.
	 * @return 0 is returned on success, a negative value on failure.
	 */
	int finalize_progress_report (bool is_success, const std::string & msg);

	//===========//
	// Operators //
	//===========//
private:
	__TSL_CLASS_NAME__ & operator=(const __TSL_CLASS_NAME__ & rhs);

	//========//
	// Fields //
	//========//
public:
	static const char * const HCF_HCJOB_CLASS_NAME;
	static const char * const HCF_HCJOB_IMPLEMENTER_NAME;
	static const char * const HCF_HCJOB_JOB_ID_ATTR_NAME;
	static const char * const HCF_HCJOB_FAILED_RULES_ATTR_NAME;
	static const char * const HCF_HCJOB_INPUT_PARAMETERS_ATTR_NAME;
	static const char * const HCF_HCJOB_JOB_TO_TRIGGER_ATTR_NAME;
	static const char * const HCF_HCJOB_LAST_REPORT_FILE_NAME_ATTR_NAME;
	static const char * const HCF_HCJOB_LAST_RUN_TIME_ATTR_NAME;
	static const char * const HCF_HCJOB_LOCAL_FILE_STORE_PATH_ATTR_NAME;
	static const char * const HCF_HCJOB_PROGRESS_REPORT_ATTR_NAME;
	static const char * const HCF_HCJOB_RULES_CATEGORIES_ATTR_NAME;
	static const char * const HCF_HCJOB_STATUS_ATTR_NAME;
	static const uint64_t     HCF_HCJOB_EXECUTE_ACTION_ID;
	static const char * const HCF_HCJOB_EXPORT_URI_ARGUMENT_NAME;
	static const char * const HCF_HCJOB_EXPORT_PASSWORD_ARGUMENT_NAME;

	static const char * const HCF_PROGRESS_REPORT_CLASS_NAME;
	static const char * const HCF_PROGRESS_REPORT_DEFAULT_RDN_VALUE;
	static const char * const HCF_PROGRESS_REPORT_LAST_UPDATE_TIME_ATTR_NAME;
	static const char * const HCF_PROGRESS_REPORT_ACTION_STARTED_TIME_ATTR_NAME;
	static const char * const HCF_PROGRESS_REPORT_ACTION_COMPLETED_TIME_ATTR_NAME;
	static const char * const HCF_PROGRESS_REPORT_STATE_ATTR_NAME;
	static const char * const HCF_PROGRESS_REPORT_RESULT_INFO_ATTR_NAME;
	static const char * const HCF_PROGRESS_REPORT_RESULT_ATTR_NAME;
	static const char * const HCF_PROGRESS_REPORT_PROGRESS_PRECENTAGE_ATTR_NAME;
	static const char * const HCF_PROGRESS_REPORT_PROGRESS_INFO_ATTR_NAME;
	static const char * const HCF_PROGRESS_REPORT_ID_ATTR_NAME;
	static const char * const HCF_PROGRESS_REPORT_ADDITIONAL_INFO_ATTR_NAME;
	static const char * const HCF_PROGRESS_REPORT_ACTION_NAME_ATTR_NAME;
	static const char * const HCF_PROGRESS_REPORT_ACTION_ID_ATTR_NAME;
	static const char * const HCF_PROGRESS_REPORT_ACTION_NAME_DEFAULT_VALUE;
	static const char * const HCF_PROGRESS_REPORT_PROGRESS_INFO_INIT_VALUE;
	static const char * const HCF_PROGRESS_REPORT_PROGRESS_INFO_FINAL_VALUE;

	static const char * const HCF_FAILED_RULE_CLASS_NAME;
	static const char * const HCF_FAILED_RULE_ID_ATTR_NAME;
	static const char * const HCF_FAILED_RULE_HCRULE_ATTR_NAME;
	static const char * const HCF_FAILED_RULE_SEVERITY_ATTR_NAME;
	static const char * const HCF_FAILED_RULE_REASON_ATTR_NAME;
	static const char * const HCF_FAILED_RULE_ID_FORMAT;

private:
	static std::string _ongoing_job_dn;
	static TSL_NS_HCF_CORE::thread_mutex_recursive _ongoing_job_mutex;

	std::set<std::string> _failed_rules_dns;
	std::string _job_to_trigger_dn;
	std::string _last_report_file_name;
	std::string _last_run_time;
	std::string _local_file_store_path;
	std::string _progress_report_dn;
	std::set<category_t> _rules_categories;
	health_status_t _status;

	// Class members to be used to implement the rollback mechanism for the abort callback
	std::set<std::string> _failed_rules_dns_old;
	std::string _job_to_trigger_dn_old;
	std::string _last_report_file_name_old;
	std::string _last_run_time_old;
	std::string _local_file_store_path_old;
	std::string _progress_report_dn_old;
	std::set<category_t> _rules_categories_old;
	health_status_t _status_old;
};

HCF_NS_HCF_SRV_END

#endif	// __INCLUDE_GUARD_CLASS_SRV_IMM_OBJECT_HCJOB__
