#ifndef __INCLUDE_GUARD_CLASS_SRV_IMM_OBJECT_HCJOB_SCHEDULER__
#define __INCLUDE_GUARD_CLASS_SRV_IMM_OBJECT_HCJOB_SCHEDULER__ imm_object_hcjob_scheduler

/**
 *  @file imm_object_hcjob_scheduler.h
 *	@brief
 *	@author xcasale (Alessio Cascone)
 *	@date 2015-01-29
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
 *	| 2015-001 | 2015-01-29 | xcasale      | Creation and first revision.        |
 *	+==========+============+==============+=====================================+
 */

#include <map>

#include "hcf/imm/imm_object.h"
#include "hcf/core/clock_alarm_handler.h"
#include "hcf/core/guard.h"

#include "srv_macros.h"
#include "constants.h"
#include "app_types.h"
#include "job_scheduling_policy.h"

#include "hcf/trace/com_ericsson_common_hcf_srv_hcjobscheduler.h"
#include "hcf/trace/tracing_macros.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_SRV_IMM_OBJECT_HCJOB_SCHEDULER__

HCF_NS_HCF_SRV_BEGIN

HCF_CC_FUNCTION_TRACER_CLASS_DEFINE

class __TSL_CLASS_NAME__ : public TSL_NS_HCF_IMM::imm_object, public TSL_NS_HCF_CORE::clock_alarm_handler {
	//==============//
	// Constructors //
	//==============//
public:
	inline TSL_CTOR (const char * class_name)
		TSL_CTOR_INIT_LIST(imm_object(class_name), clock_alarm_handler(), _object_finalized(false), _adm_state(ADMINISTRATIVE_STATE_UNDEFINED),
				_next_scheduled_time(), _job(), _scheduling_policies_mutex(), _scheduling_policies(), _adm_state_old(ADMINISTRATIVE_STATE_UNDEFINED),
				_next_scheduled_time_old(), _scheduling_policies_old()) {}

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
	 * This method sets the attributes values for the current instance of the current class.
	 * The value of each class field is set according to the values present in attributes argument.
	 *
	 * @param[in]  attributes - A vector of pairs representing the new object attributes.
	 *             Each pair has as first item a string indicating the attribute name and second item
	 *             is a vector of values.
	 * @param[in]  parent_dn - The dn of the parent object of the current one.
	 * @param[in]  callback_info - Some useful info about the callback.
	 * @return 0 is returned on success, a negative value on failure.
	 */
	virtual int set_object_attributes_and_dn (std::vector<std::pair<std::string, std::vector<void *> > > & attributes, const char * parent_dn, TSL_NS_HCF_IMM::IMM_API_ObjectImplementer_CallbackInfo * callback_info);

	/**
	 * This method finalizes the current instance of current class.
	 *
	 * @param[in]  callback_info - Some useful info about the callback.
	 * @return 0 is returned on success, a negative value on failure.
	 */
	virtual int finalize_object (TSL_NS_HCF_IMM::IMM_API_ObjectImplementer_CallbackInfo * TSL_UNUSED_PAR(callback_info) = 0);

	/**
	 * This method completes the modification of the current instance of the current class.
	 *
	 * @param[in]  callback_info - Some useful info about the callback.
	 * @return 0 is returned on success, a negative value on failure.
	 */
	inline virtual int complete_object_modification (TSL_NS_HCF_IMM::IMM_API_ObjectImplementer_CallbackInfo * TSL_UNUSED_PAR(callback_info) = 0) {
		HCF_CC_TRACE_FUNCTION;
		return ERR_NO_ERROR;
	}

	/**
	 * This method applies the modification of the current instance of the current class.
	 *
	 * @param[in]  callback_info - Some useful info about the callback.
	 * @return 0 is returned on success, a negative value on failure.
	 */
	inline virtual int apply_object_modification (TSL_NS_HCF_IMM::IMM_API_ObjectImplementer_CallbackInfo * TSL_UNUSED_PAR(callback_info) = 0) {
		HCF_CC_TRACE_FUNCTION;
		_scheduling_policies_old.clear();
		return ERR_NO_ERROR;
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
	virtual inline int update_runtime_attribute (const char * TSL_UNUSED_PAR(attr_name), TSL_NS_HCF_IMM::IMM_API_ImmParameter & TSL_UNUSED_PAR(param_to_update)) {
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
	virtual inline int execute_admin_operation (uint64_t TSL_UNUSED_PAR(operation_id), TSL_NS_HCF_IMM::IMM_API_AdminOperationParamType ** TSL_UNUSED_PAR(arguments)) {
		// No administrative operations for now!
		HCF_CC_TRACE_FUNCTION;
		return ERR_OPERATION_NOT_IMPLEMENTED;
	}

	//============================================================//
	// clock_alarm_handler class Virtual Functions Implementation //
	//============================================================//
public:
	/**
	 * This method is invoked when a scheduling event for the current job is available.
	 *
	 * @param[in]  event_in_the_past - A flag indicating if the event is occurred before
	 * 						 the current time. This could occur in case of date change operations.
	 * @return 0 on success, -1 on failure.
	 */
	virtual int handle_alarm (bool event_in_the_past);

	//===========//
	// Functions //
	//===========//
public:
	/**
	 * This method extracts the ID for the current object type from the provided DN.
	 *
	 * @param[in]  obj_dn - The DN from which the ID must be extracted.
	 * @return a string object representing the extracted ID.
	 */
	static std::string get_obj_id_from_dn (const char * obj_dn);

	/**
	 * This method adds a new scheduling policy for the current scheduler object.
	 *
	 * @param[in]  policy_id - The ID of the new scheduling policy.
	 * @param[in]  policy_ptr - The pointer to the scheduling policy object.
	 * @param[in]  callback_info - Some useful info about the callback.
	 * @return 0 on success, a not zero value otherwise.
	 */
	int add_scheduling_policy (const std::string & policy_id, job_scheduling_policy * policy_ptr, TSL_NS_HCF_IMM::IMM_API_ObjectImplementer_CallbackInfo * callback_info);

	/**
	 * This method modifies an existing scheduling policy for the current scheduler object.
	 *
	 * @param[in]  policy_id - The ID of the scheduling policy.
	 * @param[in]  policy_ptr - The pointer to the scheduling policy object.
	 * @param[in]  callback_info - Some useful info about the callback.
	 * @return 0 on success, a not zero value otherwise.
	 */
	int modify_scheduling_policy (const std::string & policy_id, job_scheduling_policy * policy_ptr, TSL_NS_HCF_IMM::IMM_API_ObjectImplementer_CallbackInfo * callback_info);

	/**
	 * This method removes the provided scheduling policy from the current scheduler object.
	 *
	 * @param[in]  policy_id - The ID of the scheduling policy.
	 * @param[in]  callback_info - Some useful info about the callback.
	 * @return 0 on success, a not zero value otherwise.
	 */
	int remove_scheduling_policy (const std::string & policy_id, TSL_NS_HCF_IMM::IMM_API_ObjectImplementer_CallbackInfo * callback_info);

	/**
	 * This method checks if the provided scheduling policy is present into the scheduler configuration.
	 *
	 * @param[in]  policy_id - The ID of the new scheduling policy
	 * @return true/false.
	 */
	inline bool scheduling_policy_present (const std::string & policy_id) {
		TSL_CRITICAL_SECTION_BEGIN(sync_t, _scheduling_policies_mutex);
			return (_scheduling_policies.find(policy_id) != _scheduling_policies.end());
		TSL_CRITICAL_SECTION_END;
	}

private:
	/**
	 * This method retrieves the next value for nextScheduledTime attribute and sets it.
	 *
	 * @param[out] next_scheduled_time - The string indicating the new time that the scheduler will be invoked.
	 * @return 0 on success, a not zero value otherwise.
	 */
	int choose_next_scheduled_time (std::string & next_scheduled_time);

	/**
	 * This method sets the value of the nextScheduledTime attribute of HcJobScheduler MOC.
	 * That operation is performed by using the CCB augmentation feature provided by IMM.
	 *
	 * @param[in]  value - The new value to be assigned.
	 * @param[in]  callback_info - Some useful info about the callback.
	 * @return 0 on success, a not zero value otherwise.
	 */
	int set_next_scheduled_time (const std::string & value, TSL_NS_HCF_IMM::IMM_API_ObjectImplementer_CallbackInfo * callback_info);

	//==================//
	// Fields Modifiers //
	//==================//
public:
	inline bool is_locked () const { return (_adm_state == ADMINISTRATIVE_STATE_LOCKED); }

	inline std::string get_next_scheduled_time () const { return _next_scheduled_time; }
	inline void set_next_scheduled_time (const std::string & next_time) {
		modify_object_attribute(HCF_HCJOB_SCHEDULER_NEXT_SCHEDULED_TIME_ATTR_NAME, TSL_NS_HCF_IMM::ATTR_STRINGT, const_cast<char *>(next_time.c_str()));
	}
	inline void clear_next_scheduled_time () {
		modify_object_attribute(HCF_HCJOB_SCHEDULER_NEXT_SCHEDULED_TIME_ATTR_NAME, TSL_NS_HCF_IMM::ATTR_STRINGT, 0, 0);
	}

	//===========//
	// Operators //
	//===========//
private:
	__TSL_CLASS_NAME__ & operator=(const __TSL_CLASS_NAME__ & rhs);

	//========//
	// Fields //
	//========//
public:
	static const char * const HCF_HCJOB_SCHEDULER_CLASS_NAME;
	static const char * const HCF_HCJOB_SCHEDULER_IMPLEMENTER_NAME;
	static const char * const HCF_HCJOB_SCHEDULER_SCHEDULER_ID_ATTR_NAME;
	static const char * const HCF_HCJOB_SCHEDULER_ADMINISTRATIVE_STATE_ATTR_NAME;
	static const char * const HCF_HCJOB_SCHEDULER_NEXT_SCHEDULED_TIME_ATTR_NAME;
	static const char * const HCF_HCJOB_SCHEDULER_DEFAULT_RDN_VALUE;

private:
	bool _object_finalized;
	administrative_state_t _adm_state;
	std::string _next_scheduled_time;
	std::string _job;
	TSL_NS_HCF_CORE::thread_mutex_recursive _scheduling_policies_mutex;
	std::map<std::string, job_scheduling_policy *> _scheduling_policies;

	// Class members to be used to implement the rollback mechanism for the abort callback
	administrative_state_t _adm_state_old;
	std::string _next_scheduled_time_old;
	std::map<std::string, job_scheduling_policy *> _scheduling_policies_old;
};

HCF_NS_HCF_SRV_END

#endif	// __INCLUDE_GUARD_CLASS_SRV_IMM_OBJECT_HCJOB_SCHEDULER__
