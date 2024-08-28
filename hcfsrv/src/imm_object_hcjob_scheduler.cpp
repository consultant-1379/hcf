#include "hcf/core/logger.h"
#include "hcf/core/utils.h"
#include "hcf/imm/imm_error_constants.h"
#include "hcf/imm/imm_config_class.h"

#include "job_executor.h"
#include "imm_object_hcjob.h"
#include "imm_object_hcjob_scheduler.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_SRV_IMM_OBJECT_HCJOB_SCHEDULER__

// Extern objects
extern TSL_NS_HCF_IMM::imm_config_class<HCF_NS_HCF_SRV::imm_object_hcjob> hcjob_class_handler;
extern HCF_NS_HCF_SRV::job_executor hcjob_executor;
extern HCF_NS_HCF_SRV::clock_alarm_notifier alarm_notifier;

HCF_NS_HCF_SRV_BEGIN

// Static fields initialization
const char * const __TSL_CLASS_NAME__::HCF_HCJOB_SCHEDULER_CLASS_NAME                      = "HCMHcJobScheduler";
const char * const __TSL_CLASS_NAME__::HCF_HCJOB_SCHEDULER_IMPLEMENTER_NAME                = "HcJobScheduler_Implementer";
const char * const __TSL_CLASS_NAME__::HCF_HCJOB_SCHEDULER_SCHEDULER_ID_ATTR_NAME          = "hcJobSchedulerId";
const char * const __TSL_CLASS_NAME__::HCF_HCJOB_SCHEDULER_ADMINISTRATIVE_STATE_ATTR_NAME  = "administrativeState";
const char * const __TSL_CLASS_NAME__::HCF_HCJOB_SCHEDULER_NEXT_SCHEDULED_TIME_ATTR_NAME   = "nextScheduledTime";
const char * const __TSL_CLASS_NAME__::HCF_HCJOB_SCHEDULER_DEFAULT_RDN_VALUE               = "hcJobSchedulerId=1";

int __TSL_CLASS_NAME__::set_object_attributes (std::vector<std::pair<std::string, std::vector<void *> > > & attributes, TSL_NS_HCF_IMM::IMM_API_ObjectImplementer_CallbackInfo * callback_info) {
	HCF_CC_TRACE_FUNCTION;
	// Save the old values before modifying them, in order to rollback in case of errors
	_adm_state_old           = _adm_state;
	_next_scheduled_time_old = _next_scheduled_time;

	// For each of the provided attributes, understand the attribute name and set the class field accordingly
	for (size_t i = 0; i < attributes.size(); ++i) {

		// Extract from the given attributes vector the attribute name and its value
		std::string attr_name = attributes[i].first;
		std::vector<void *> attr_values = attributes[i].second;
		HCF_CC_TRACE(SetObjectAttributes, "Received a request to set/change the value of the following attribute:", attr_name.c_str());

		if (!attr_name.compare(HCF_HCJOB_SCHEDULER_SCHEDULER_ID_ATTR_NAME)) {
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
		else if (!attr_name.compare(HCF_HCJOB_SCHEDULER_ADMINISTRATIVE_STATE_ATTR_NAME)) {
			if (attr_values.size() > 0) {
				_adm_state = *(reinterpret_cast<administrative_state_t *>(attr_values[0]));
				HCF_CC_TRACE(SetObjectAttributes, "The value of the scheduler administrative state is:", (_adm_state == ADMINISTRATIVE_STATE_LOCKED) ? "LOCKED" : "UNLOCKED");
			}
			else {
				// No value provided, using the default value
				_adm_state = ADMINISTRATIVE_STATE_LOCKED;
				HCF_CC_TRACE(SetObjectAttributes, "No value provided for the administrative state, using the default one:", "LOCKED");
			}
		}
		else if (!attr_name.compare(HCF_HCJOB_SCHEDULER_NEXT_SCHEDULED_TIME_ATTR_NAME)) {
			if (attr_values.size() > 0) {
				_next_scheduled_time = reinterpret_cast<char *>(attr_values[0]);
				HCF_CC_TRACE(SetObjectAttributes, "The value of the next scheduled time attribute is:", _next_scheduled_time.c_str());
			}
			else {
				_next_scheduled_time.clear();
				HCF_CC_TRACE(SetObjectAttributes, "No value for the next scheduled time was provided, resetting it!", "");
			}
		}
	}

	const bool augmented_operation_to_be_executed = ((callback_info) && (
			((_adm_state == ADMINISTRATIVE_STATE_UNLOCKED) && (_adm_state_old == ADMINISTRATIVE_STATE_LOCKED)) ||
			((_adm_state == ADMINISTRATIVE_STATE_LOCKED) && (_adm_state_old == ADMINISTRATIVE_STATE_UNLOCKED))));

	if (augmented_operation_to_be_executed) {
		HCF_CC_TRACE(SetObjectAttributes, "The administrative value was changed, something must be done on the nextScehduledTime attribute!", "");
		/*
		 * When the HcJobScheduler passes from the LOCKED to the UNLOCKED state, the nextScheduledTime
		 * attribute value must be chosen. In all the other cases, the value must be cleared (in that case
		 * the string default constructor is used).
		 */
		std::string next_time;
		if ((_adm_state == ADMINISTRATIVE_STATE_UNLOCKED) && (_adm_state_old == ADMINISTRATIVE_STATE_LOCKED)) {
			HCF_CC_TRACE(SetObjectAttributes, "The scheduler passed from the LOCKED to the UNLOCKED state, choosing a new value for the nextScheduledTime attribute.", "");
			if (const int call_result = choose_next_scheduled_time(next_time)) {
				HCF_CC_TRACE(SetObjectAttributesErr, "Call 'choose_next_scheduled_time' failed.", call_result);
				HCF_CC_LOG(LEVEL_ERROR, "Call 'choose_next_scheduled_time' failed, call_result == %d", call_result);
				set_error_message(HCF_GENERIC_ERROR_ERROR_MSG);
				return call_result;
			}
		}
		HCF_CC_TRACE(SetObjectAttributes, "Setting the value of the nextScheduledTime attribute to the following value:", next_time.c_str());

		// Set the time value previously retrieved (or the default one)
		if (const int call_result = set_next_scheduled_time(next_time, callback_info)) {
			HCF_CC_TRACE(SetObjectAttributesErr, "Call 'set_next_scheduled_time' failed.", call_result);
			HCF_CC_LOG(LEVEL_ERROR, "Call 'set_next_scheduled_time' failed, call_result == %d", call_result);
			set_error_message(HCF_GENERIC_ERROR_ERROR_MSG);
			return call_result;
		}
	}

	HCF_CC_LOG(LEVEL_NOTICE, "Object attributes correctly modified: RDN == '%s', _adm_state == %d, "
			"_next_scheduled_time == '%s'", _obj_rdn.c_str(), _adm_state, _next_scheduled_time.c_str());
	return ERR_NO_ERROR;
}

int __TSL_CLASS_NAME__::set_object_attributes_and_dn (std::vector<std::pair<std::string, std::vector<void *> > > & attributes, const char * parent_dn, TSL_NS_HCF_IMM::IMM_API_ObjectImplementer_CallbackInfo * callback_info) {
	HCF_CC_TRACE_FUNCTION;
	// FIRST: Calculate the ID of the father (HcJob) object
	_job = imm_object_hcjob::get_obj_id_from_dn(parent_dn);
	HCF_CC_TRACE(SetObjectAttributes, "The father HcJob object has the following ID:", _job.c_str());

	// SECOND: Call the same method in the base class
	if (const int call_result = TSL_NS_HCF_IMM::imm_object::set_object_attributes_and_dn(attributes, parent_dn, callback_info)) {
		HCF_CC_TRACE(SetObjectAttributesErr, "Call 'imm_object::set_object_attributes_and_dn' failed.", call_result);
		HCF_CC_LOG(LEVEL_ERROR, "Call 'set_object_attributes_and_dn' failed, return_code == %d", call_result);
		return call_result;
	}

	// THIRD: Assign the correct value for the ID field (that is equal to the HcJob ID
	_obj_id = _job;
	HCF_CC_TRACE(SetObjectAttributes, "The ID of this object is:", _obj_id.c_str());
	return ERR_NO_ERROR;
}

int __TSL_CLASS_NAME__::finalize_object (TSL_NS_HCF_IMM::IMM_API_ObjectImplementer_CallbackInfo * TSL_UNUSED_PAR(callback_info)) {
	HCF_CC_TRACE_FUNCTION;

	TSL_CRITICAL_SECTION_BEGIN(sync_t, _scheduling_policies_mutex);
		if (_object_finalized) {
			HCF_CC_TRACE(FinalizeObject, "Received a request to finalize this object: the object has been already finalized, nothing to do!", _obj_id.c_str(), "");
			return ERR_NO_ERROR;
		}

		std::map<std::string, job_scheduling_policy *>::iterator item;

		HCF_CC_TRACE(FinalizeObject, "Received a request to finalize this object: erasing all its scheduling policies.", _obj_id.c_str(), "");
		HCF_CC_LOG(LEVEL_NOTICE, "Finalizing this object: erasing %zd scheduling policies!", _scheduling_policies.size());
		_scheduling_policies_old = _scheduling_policies;

		for (item = _scheduling_policies.begin(); item != _scheduling_policies.end(); ++item) {
			HCF_CC_TRACE(FinalizeObject, "Removing the scheduling policy with the following ID:", _obj_id.c_str(), item->first.c_str());
			if (const int call_result = item->second->remove_timeplan(item->first, &alarm_notifier)) {
				HCF_CC_TRACE(FinalizeObjectErr, "Call 'item->second->remove_timeplan' failed", call_result);
				HCF_CC_LOG(LEVEL_ERROR, "Call 'policy_ptr->remove_timeplan' failed with call_result == %d", call_result);
			}
		}

		HCF_CC_TRACE(FinalizeObject, "All the scheduling policies have been removed, this object is now finalized!", _obj_id.c_str(), "");
		_scheduling_policies.clear();
		_object_finalized = true;
	TSL_CRITICAL_SECTION_END;

	return ERR_NO_ERROR;
}

int __TSL_CLASS_NAME__::abort_object_modification (TSL_NS_HCF_IMM::IMM_API_ObjectImplementer_CallbackInfo * TSL_UNUSED_PAR(callback_info)) {
	HCF_CC_TRACE_FUNCTION;
	_object_finalized = false;

	// The callback has been aborted, restore the original values
	_adm_state           = _adm_state_old;
	_next_scheduled_time = _next_scheduled_time_old;

	/*
	 * Check if the abort callback is coming from a failure during object creation/modification or from a removal failure.
	 * In the second case, the scheduling policies configuration must be restored, communicating to the alarm handler
	 * the new state. In that case in the _scheduling_policies_old map has been correctly saved the status before the
	 * object removal: so only if there is at least one scheduling policy restore it.
	 */
	if (_scheduling_policies_old.size() <= 0) {// Not an abort-after-delete or no scheduling policy defined: nothing to do!
		HCF_CC_TRACE(AbortObjectModification, "Received a request to abort all the modifications to this "
				"object: the request is not coming from a delete or no policies are defined, nothing to do!", _obj_id.c_str(), "");
		return ERR_NO_ERROR;
	}

	std::map<std::string, job_scheduling_policy *>::iterator item;

	TSL_CRITICAL_SECTION_BEGIN(sync_t, _scheduling_policies_mutex);
		_scheduling_policies = _scheduling_policies_old;
		HCF_CC_TRACE(AbortObjectModification, "Received a request to abort all the modifications to this object: restoring all the scheduling policies", _obj_id.c_str(), "");
		HCF_CC_LOG(LEVEL_NOTICE, "Restoring this object: adding %zd scheduling policies!", _scheduling_policies.size());

		for (item = _scheduling_policies.begin(); item != _scheduling_policies.end(); ++item) {
			HCF_CC_TRACE(AbortObjectModification, "Restoring the data for the following scheduling policy:", _obj_id.c_str(), item->first.c_str());
			if (const int call_result = item->second->add_timeplan(item->first, &alarm_notifier, this)) {
				HCF_CC_TRACE(AbortObjectModificationErr, "Call 'item->second->add_timeplan' failed.", call_result);
				HCF_CC_LOG(LEVEL_ERROR, "Call 'policy_ptr->add_timeplan' failed with call_result == %d", call_result);
			}
		}
	TSL_CRITICAL_SECTION_END;

	return ERR_NO_ERROR;
}

int __TSL_CLASS_NAME__::handle_alarm (bool event_in_the_past) {
	HCF_CC_TRACE_FUNCTION;
	// Check the scheduler status: in case it is LOCKED, no operation must be done
	if (is_locked()) {
		HCF_CC_TRACE(HandleAlarm, "A job should be triggered, but the scheduler state is LOCKED!", _obj_id.c_str());
		HCF_CC_LOG(LEVEL_NOTICE, "A job should be triggered, but the scheduler state is LOCKED!");
		return 0;
	}

	// Retrieve the HcJob instance in order to schedule the job execution
	if (!event_in_the_past) {
		HCF_CC_TRACE(HandleAlarm, "Received a request to execute the HcJob!", _obj_id.c_str());
		HCF_CC_LOG(LEVEL_NOTICE, "Received a request to execute the HcJob '%s'!", _job.c_str());
		imm_object_hcjob * job;
		if (!(job = hcjob_class_handler.get_instance(_job))) {
			HCF_CC_TRACE(HandleAlarmErr, "Call 'hcjob_class_handler.get_instance' failed.", ERR_NULL_INSTANCE_RETRIEVED);
			HCF_CC_LOG(LEVEL_ERROR, "Call 'hcjob_class_handler.get_instance' failed with ID == '%s'!", _job.c_str());
			return 0;
		}

		HCF_CC_TRACE(HandleAlarm, "Retrieving the ID of the jobToTrigger associated to the job to be executed (if exists)!", _obj_id.c_str());
		std::string job_to_trigger_id;
		if (!job->get_job_to_trigger_dn().empty()) {
			job_to_trigger_id	= imm_object_hcjob::get_obj_id_from_dn(job->get_job_to_trigger_dn().c_str());
			HCF_CC_TRACE(HandleAlarm, "The jobToTrigger associated to the scheduled job has the following ID:", job_to_trigger_id.c_str());
		}

		// Order the current job execution
		HCF_CC_TRACE(HandleAlarm, "Ordering the automatic execution of the health check job!", _obj_id.c_str());
		if (const int call_result = hcjob_executor.execute_job(job->get_id(), job->get_rules_categories(), "", "", job_to_trigger_id, true)) {
			HCF_CC_TRACE(HandleAlarmErr, "Call 'hcjob_executor.execute_job' failed.", call_result);
			HCF_CC_LOG(LEVEL_ERROR, "Call 'hcjob_executor.execute_job', call_result == %d", call_result);
			return 0;
		}
	}
	else {
		HCF_CC_TRACE(HandleAlarm, "Received a request to execute the HcJob, but the event is in the past: nothing to do!", _obj_id.c_str());
		HCF_CC_LOG(LEVEL_WARNING, "The HcJob having ID == '%s' should be triggered, but the event is in the past, "
				"next_scheduled_time == '%s'", _job.c_str(), _next_scheduled_time.c_str());
	}

	// Find the next time that the scheduler must be invoked
	HCF_CC_TRACE(HandleAlarm, "Choosing the new value for the nextScheduledTime attribute!", _obj_id.c_str());
	std::string next_time;
	if (const int call_result = choose_next_scheduled_time(next_time)) {
		HCF_CC_TRACE(HandleAlarmErr, "Call 'choose_next_scheduled_time' failed.", call_result);
		HCF_CC_LOG(LEVEL_ERROR, "Call 'choose_next_scheduled_time', call_result == %d", call_result);
	}

	// Set the value previously retrieved
	if (next_time.empty()) {
		HCF_CC_TRACE(HandleAlarm, "No new value was found for the nextScheduledTime attribute, clearing it!", _obj_id.c_str());
		clear_next_scheduled_time();
	}
	else {
		HCF_CC_TRACE(HandleAlarm, "Setting the value found for the nextScheduledTime attribute!", _obj_id.c_str());
		set_next_scheduled_time(next_time);
	}

	return 0;
}

std::string __TSL_CLASS_NAME__::get_obj_id_from_dn (const char * obj_dn) {
	HCF_CC_TRACE_FUNCTION;
	/*
   * In HcJobScheduler class, the object ID must be calculated in a different way.
   * If the object has the following DN: "hcJobSchedulerId=<ID>,hcJobId=<JOB_ID>,HCMhealthCheckMId=1",
   * then the object ID will be <JOB_ID> (the same of the parent HcJob MOC instance).
   */
  std::string dn(obj_dn);
  HCF_CC_TRACE(GetObjectIdFromDn, "Received a request to calculate the HcJobScheduler ID starting from the DN: extracting the first part!", obj_dn);

  size_t comma_pos = dn.find_first_of(",");
  size_t equal_pos = dn.find_first_of("=");
  if (equal_pos == std::string::npos) {
    HCF_CC_TRACE(GetObjectIdFromDn, "The HcJobScheduler DN is not correctly formed!", obj_dn);
		HCF_CC_LOG(LEVEL_WARNING, "The HcJobScheduler DN (%s) is not correctly formed!", obj_dn);
  	return "";
  }

  HCF_CC_TRACE(GetObjectIdFromDn, "Received a request to calculate the HcJobScheduler ID starting from the DN: extracting the second part!", obj_dn);
  comma_pos = dn.find_first_of(",", (comma_pos + 1));
  equal_pos = dn.find_first_of("=", (equal_pos + 1));
  if (equal_pos == std::string::npos) {
    HCF_CC_TRACE(GetObjectIdFromDn, "The HcJobScheduler DN is not correctly formed!", obj_dn);
		HCF_CC_LOG(LEVEL_WARNING, "The HcJobScheduler DN (%s) is not correctly formed!", obj_dn);
  	return "";
  }
  return dn.substr((equal_pos + 1), (comma_pos - equal_pos - 1));
}

int __TSL_CLASS_NAME__::add_scheduling_policy (const std::string & policy_id, job_scheduling_policy * policy_ptr, TSL_NS_HCF_IMM::IMM_API_ObjectImplementer_CallbackInfo * callback_info) {
	HCF_CC_TRACE_FUNCTION;
	int call_result = 0;

	TSL_CRITICAL_SECTION_BEGIN(sync_t, _scheduling_policies_mutex);
		// Tell to the created object that a new timeplan must be added to the notifier
		HCF_CC_TRACE(AddSchedulingPolicy, "Adding the scheduling policy to the internal data structures!", policy_id.c_str(), _obj_id.c_str());
		if ((call_result = policy_ptr->add_timeplan(policy_id, &alarm_notifier, this))) {
			HCF_CC_TRACE(AddSchedulingPolicyErr, "Call 'policy_ptr->add_timeplan' failed.", call_result);
			HCF_CC_LOG(LEVEL_ERROR, "Call 'policy_ptr->add_timeplan' failed with call_result == %d", call_result);
			return call_result;
		}

		// Add the scheduling policy to the internal data structure
		HCF_CC_TRACE(AddSchedulingPolicy, "The scheduling policy was correctly added to the internal data structures!", policy_id.c_str(), _obj_id.c_str());
		HCF_CC_LOG(LEVEL_NOTICE, "Scheduling policy with ID == '%s' correctly added!", policy_id.c_str());
		_scheduling_policies[policy_id] = policy_ptr;

		if (is_locked()) {
			HCF_CC_TRACE(AddSchedulingPolicy, "The HcJobScheduler object is locked, nothing to do!", policy_id.c_str(), _obj_id.c_str());
			return ERR_NO_ERROR;
		}

		// The scheduling policies group is changed: find the next time that the scheduler will be invoked
		HCF_CC_TRACE(AddSchedulingPolicy, "The HcJobScheduler object is unlocked: choosing the new value for the nextScheduledTime attribute!", policy_id.c_str(), _obj_id.c_str());
		std::string next_time;
		if ((call_result = choose_next_scheduled_time(next_time))) {
			HCF_CC_TRACE(AddSchedulingPolicyErr, "Call 'choose_next_scheduled_time' failed.", call_result);
			HCF_CC_LOG(LEVEL_ERROR, "Call 'choose_next_scheduled_time', call_result == %d", call_result);
		}

		// Set the value previously retrieved
		HCF_CC_TRACE(AddSchedulingPolicy, "Setting the value found for the nextScheduledTime attribute!", policy_id.c_str(), _obj_id.c_str());
		if ((call_result = set_next_scheduled_time(next_time, callback_info))) {
			HCF_CC_TRACE(AddSchedulingPolicyErr, "Call 'set_next_scheduled_time' failed.", call_result);
			HCF_CC_LOG(LEVEL_ERROR, "Call 'set_next_scheduled_time' failed, call_result == %d", call_result);
			return call_result;
		}
	TSL_CRITICAL_SECTION_END;

	return ERR_NO_ERROR;
}

int __TSL_CLASS_NAME__::modify_scheduling_policy (const std::string & policy_id, job_scheduling_policy * policy_ptr, TSL_NS_HCF_IMM::IMM_API_ObjectImplementer_CallbackInfo * callback_info) {
	HCF_CC_TRACE_FUNCTION;
	int call_result = 0;

	TSL_CRITICAL_SECTION_BEGIN(sync_t, _scheduling_policies_mutex);
		// Find the scheduling policy to be modified into the internal data structure
		HCF_CC_TRACE(ModifySchedulingPolicy, "Looking for the provided scheduling policy into internal data structures!", policy_id.c_str(), _obj_id.c_str());
		if (_scheduling_policies.find(policy_id) == _scheduling_policies.end()) {
			HCF_CC_TRACE(ModifySchedulingPolicyErr, "Call '_scheduling_policies.find' failed.", ERR_NULL_INSTANCE_RETRIEVED);
			HCF_CC_LOG(LEVEL_ERROR, "Call '_scheduling_policies.find' failed with key == '%s'!", policy_id.c_str());
			return ERR_NULL_INSTANCE_RETRIEVED;
		}

		// Tell to the retrieved object that its timeplan must be modified
		HCF_CC_TRACE(ModifySchedulingPolicy, "Modifying the scheduling policy from the internal data structures!", policy_id.c_str(), _obj_id.c_str());
		if ((call_result = policy_ptr->modify_timeplan(policy_id, &alarm_notifier, this))) {
			HCF_CC_TRACE(ModifySchedulingPolicyErr, "Call 'policy_ptr->modify_timeplan' failed.", call_result);
			HCF_CC_LOG(LEVEL_ERROR, "Call 'policy_ptr->modify_timeplan' failed with call_result == %d", call_result);
			return call_result;
		}
		HCF_CC_TRACE(ModifySchedulingPolicy, "The scheduling policy was correctly modified into the internal data structures!", policy_id.c_str(), _obj_id.c_str());
		HCF_CC_LOG(LEVEL_NOTICE, "Scheduling policy with ID == '%s' correctly modified!", policy_id.c_str());

		if (is_locked()) {
			HCF_CC_TRACE(ModifySchedulingPolicy, "The HcJobScheduler object is locked, nothing to do!", policy_id.c_str(), _obj_id.c_str());
			return ERR_NO_ERROR;
		}

		// The scheduling policies group is changed: find the next time that the scheduler will be invoked
		HCF_CC_TRACE(ModifySchedulingPolicy, "The HcJobScheduler object is unlocked: choosing the new value for the nextScheduledTime attribute!", policy_id.c_str(), _obj_id.c_str());
		std::string next_time;
		if ((call_result = choose_next_scheduled_time(next_time))) {
			HCF_CC_TRACE(ModifySchedulingPolicyErr, "Call 'choose_next_scheduled_time' failed.", call_result);
			HCF_CC_LOG(LEVEL_ERROR, "Call 'choose_next_scheduled_time', call_result == %d", call_result);
		}

		// Set the value previously retrieved
		HCF_CC_TRACE(ModifySchedulingPolicy, "Setting the value found for the nextScheduledTime attribute!", policy_id.c_str(), _obj_id.c_str());
		if ((call_result = set_next_scheduled_time(next_time, callback_info))) {
			HCF_CC_TRACE(ModifySchedulingPolicyErr, "Call 'set_next_scheduled_time' failed.", call_result);
			HCF_CC_LOG(LEVEL_ERROR, "Call 'set_next_scheduled_time' failed, call_result == %d", call_result);
			return call_result;
		}
	TSL_CRITICAL_SECTION_END;

	return ERR_NO_ERROR;
}

int __TSL_CLASS_NAME__::remove_scheduling_policy (const std::string & policy_id, TSL_NS_HCF_IMM::IMM_API_ObjectImplementer_CallbackInfo * callback_info) {
	HCF_CC_TRACE_FUNCTION;
	std::map<std::string, job_scheduling_policy *>::iterator item;
	int call_result = 0;

	TSL_CRITICAL_SECTION_BEGIN(sync_t, _scheduling_policies_mutex);
		// Find the scheduling policy to be deleted into the internal data structure
		HCF_CC_TRACE(RemoveSchedulingPolicy, "Looking for the provided scheduling policy into internal data structures!", policy_id.c_str(), _obj_id.c_str());
		if ((item = _scheduling_policies.find(policy_id)) == _scheduling_policies.end()) {
			HCF_CC_TRACE(RemoveSchedulingPolicyErr, "Call '_scheduling_policies.find' failed.", ERR_NULL_INSTANCE_RETRIEVED);
			HCF_CC_LOG(LEVEL_ERROR, "Call '_scheduling_policies.find' failed with key == '%s'!", policy_id.c_str());
			return ERR_NULL_INSTANCE_RETRIEVED;
		}

		// Tell to the retrieved object that its timeplan must be removed from the notifier
		HCF_CC_TRACE(RemoveSchedulingPolicy, "Removing the scheduling policy from the internal data structures!", policy_id.c_str(), _obj_id.c_str());
		if ((call_result = item->second->remove_timeplan(policy_id, &alarm_notifier))) {
			HCF_CC_TRACE(RemoveSchedulingPolicyErr, "Call 'policy_ptr->remove_timeplan' failed.", call_result);
			HCF_CC_LOG(LEVEL_ERROR, "Call 'policy_ptr->remove_timeplan' failed with call_result == %d", call_result);
			return call_result;
		}

		// Remove the scheduling policy from the internal data structure
		HCF_CC_TRACE(RemoveSchedulingPolicy, "The scheduling policy was correctly removed from the internal data structures!", policy_id.c_str(), _obj_id.c_str());
		HCF_CC_LOG(LEVEL_NOTICE, "Scheduling policy with ID == '%s' correctly removed!", policy_id.c_str());
		_scheduling_policies.erase(policy_id);

		if (is_locked()) {
			HCF_CC_TRACE(RemoveSchedulingPolicy, "The HcJobScheduler object is locked, nothing to do!", policy_id.c_str(), _obj_id.c_str());
			return ERR_NO_ERROR;
		}

		// The scheduling policies group is changed: find the next time that the scheduler will be invoked
		HCF_CC_TRACE(RemoveSchedulingPolicy, "The HcJobScheduler object is unlocked: choosing the new value for the nextScheduledTime attribute!", policy_id.c_str(), _obj_id.c_str());
		std::string next_time;
		if ((call_result = choose_next_scheduled_time(next_time))) {
			HCF_CC_TRACE(RemoveSchedulingPolicyErr, "Call 'choose_next_scheduled_time' failed.", call_result);
			HCF_CC_LOG(LEVEL_ERROR, "Call 'choose_next_scheduled_time', call_result == %d", call_result);
		}

		// Set the value previously retrieved
		HCF_CC_TRACE(RemoveSchedulingPolicy, "Setting the value found for the nextScheduledTime attribute!", policy_id.c_str(), _obj_id.c_str());
		if ((call_result = set_next_scheduled_time(next_time, callback_info))) {
			HCF_CC_TRACE(RemoveSchedulingPolicyErr, "Call 'set_next_scheduled_time' failed.", call_result);
			HCF_CC_LOG(LEVEL_ERROR, "Call 'set_next_scheduled_time' failed, call_result == %d", call_result);
			return call_result;
		}
	TSL_CRITICAL_SECTION_END;

	return ERR_NO_ERROR;
}

int __TSL_CLASS_NAME__::choose_next_scheduled_time (std::string & next_scheduled_time) {
	HCF_CC_TRACE_FUNCTION;
	int call_result = 0;

	// Choose the next time that the scheduler will be invoked. By default, it is assumed to be DATE_MAX (infinite)
	date_time next_time = date_time::DATE_MAX;

	// Loop over all the defined scheduling policies and get its next time, updating (if needed) the minimum value
	HCF_CC_TRACE(ChooseNextScheduledTime, "Looping over all the defined scheduling policies in order to get the new value for the nextScheduledTime attribute!", _obj_id.c_str());
	TSL_CRITICAL_SECTION_BEGIN(sync_t, _scheduling_policies_mutex);
		for (std::map<std::string, job_scheduling_policy *>::iterator item = _scheduling_policies.begin();
				item != _scheduling_policies.end(); ++item) {
			::tm tm;
			HCF_CC_TRACE(ChooseNextScheduledTime, "Retrieving the next time for the following scheduling policy:", item->first.c_str());
			if ((call_result = item->second->get_next_scheduled_time(tm))) {
				// ERROR: Failed to get the next scheduled time for this policy, skip it!
				HCF_CC_TRACE(ChooseNextScheduledTimeErr, "Call 'item->second->get_next_scheduled_time' failed: no next time available!", call_result);
				HCF_CC_LOG(LEVEL_NOTICE, "No next time available for the scheduling policy having ID == '%s'", item->first.c_str());
			}
			else if (tm < next_time) {
				// A closer date has been found: assign it to the minimum value
				next_time = tm;
				HCF_CC_TRACE(ChooseNextScheduledTime, "A new minimum has been found, setting it!", TSL_NS_HCF_CORE::utils::calculate_time_string(&tm).c_str());
			}
		}
	TSL_CRITICAL_SECTION_END;

	// Check if a valid time value has been found. If no, return immediately.
	if (next_time == date_time::DATE_MAX) {
		HCF_CC_TRACE(ChooseNextScheduledTime, "No next time available to schedule the following HcJob:", _job.c_str());
		HCF_CC_LOG(LEVEL_NOTICE, "No next time available to schedule the HcJob == '%s'", _job.c_str());
		next_scheduled_time.clear();
	}
	else {
		// A value has been found: convert it into the ISO 8601 format
		HCF_CC_TRACE(ChooseNextScheduledTime, "Converting into ISO 8601 format the retrieved time:", "");
		if ((call_result = job_scheduling_policy::convert_datetime_into_iso8601_format(next_time, next_scheduled_time))) {
			HCF_CC_TRACE(ChooseNextScheduledTimeErr, "Call 'job_scheduling_policy::convert_datetime_into_iso8601_format' failed: no next time available!", call_result);
			HCF_CC_LOG(LEVEL_ERROR, "Call 'job_scheduling_policy::convert_datetime_into_iso8601_format' failed, call_result == %d", call_result);
			return call_result;
		}
		HCF_CC_TRACE(ChooseNextScheduledTime, "The scheduled will be invoked at the following time:", next_scheduled_time.c_str());
		HCF_CC_LOG(LEVEL_NOTICE, "The scheduler will be invoked on '%s'!", next_scheduled_time.c_str());
	}

	return ERR_NO_ERROR;
}

int __TSL_CLASS_NAME__::set_next_scheduled_time (const std::string & value, TSL_NS_HCF_IMM::IMM_API_ObjectImplementer_CallbackInfo * callback_info) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(SetNextScheduledTime, "Received a request to set the nextScheduledTime to the following value:", value.c_str());

	if (!callback_info) {
		HCF_CC_TRACE(SetNextScheduledTimeErr, "NULL pointer provided for the callback_info argument.", ERR_NULL_INSTANCE_RETRIEVED);
		HCF_CC_LOG(LEVEL_WARNING, "NULL pointer provided for callback_info argument!");
		return ERR_NO_ERROR;
	}

	HCF_CC_TRACE(SetNextScheduledTime, "An augmented CCB operation must be executed: checking if the initialization phase is needed!", value.c_str());
	bool apply_to_be_executed = false;
	if ((!callback_info->initialized) && (callback_info->object_implementer->getCcbAugmentationInitialize(callback_info->oi_handle, callback_info->ccb_id, &(callback_info->ccb_handle), &(callback_info->admin_owner_handler)) != TSL_NS_HCF_IMM::OPERATION_SUCCESS)) {
		HCF_CC_TRACE(SetNextScheduledTimeErr, "Call 'getCcbAugmentationInitialize' failed.", callback_info->object_implementer->getInternalLastError());
		HCF_CC_LOG(LEVEL_ERROR, "Call 'getCcbAugmentationInitialize' failed, error_code == %d, error_message == '%s'",
				callback_info->object_implementer->getInternalLastError(), callback_info->object_implementer->getInternalLastErrorText());
		return ERR_AUGMENTED_CCB_INIT_FAILED;
	}
	else if (!callback_info->initialized)	callback_info->initialized = apply_to_be_executed = true;

	// Modify the nextScheduledTIme attribute value of HcJobScheduler class
	HCF_CC_TRACE(SetNextScheduledTime, "Preparing the data structures to set the nextScheduledTime attribute for the following object:", _obj_id.c_str());
	TSL_NS_HCF_IMM::IMM_API_ImmParameter parameter;
	parameter.attrName = const_cast<char *>(imm_object_hcjob_scheduler::HCF_HCJOB_SCHEDULER_NEXT_SCHEDULED_TIME_ATTR_NAME);
	parameter.attrType = TSL_NS_HCF_IMM::ATTR_STRINGT;
	parameter.attrValuesNum = (value.length() > 0) ? 1 : 0;
	void * tmp_time[1];

	if (parameter.attrValuesNum > 0) {
		tmp_time[0] = const_cast<char *>(value.c_str());
		parameter.attrValues = tmp_time;
	}

	HCF_CC_TRACE(SetNextScheduledTime, "Modifying the nextScheduledTime attribute value for the following object:", _obj_id.c_str());
	if (callback_info->object_implementer->modifyObjectAugmentCcb(callback_info->ccb_handle, callback_info->admin_owner_handler, _obj_dn.c_str(), &parameter) != TSL_NS_HCF_IMM::OPERATION_SUCCESS) {
		const int error_code = callback_info->object_implementer->getInternalLastError();
		if (error_code != TSL_NS_HCF_IMM::IMM_API_ERROR_CODE_IMM_NOT_EXIST) {
			HCF_CC_TRACE(SetNextScheduledTimeErr, "Call 'modifyObjectAugmentCcb' failed.", error_code);
			HCF_CC_LOG(LEVEL_ERROR, "Call 'modifyObjectAugmentCcb' failed for object having DN == '%s', error_code == %d, error_message == '%s'",
					_obj_dn.c_str(), callback_info->object_implementer->getInternalLastError(), callback_info->object_implementer->getInternalLastErrorText());
			return ERR_AUGMENTED_CCB_MODIFY_FAILED;
		}

		HCF_CC_TRACE(SetNextScheduledTimeErr, "Call 'modifyObjectAugmentCcb' returned IMM_API_ERROR_CODE_IMM_NOT_EXIST: Probably it has been deleted!", error_code);
		HCF_CC_LOG(LEVEL_WARNING, "Call 'modifyObjectAugmentCcb' returned IMM_API_ERROR_CODE_IMM_NOT_EXIST: Probably it has been deleted!");
	}

	// Apply the modification operation
	HCF_CC_TRACE(SetNextScheduledTime, "An augmented CCB operation has been executed: finalizing handles.", "");
	if ((apply_to_be_executed) && (callback_info->object_implementer->applyAugmentCcb(callback_info->ccb_handle) != TSL_NS_HCF_IMM::OPERATION_SUCCESS)) {
		HCF_CC_TRACE(SetNextScheduledTimeErr, "Call 'applyAugmentCcb' failed.", callback_info->object_implementer->getInternalLastError());
		HCF_CC_LOG(LEVEL_ERROR, "Call 'applyAugmentCcb' failed, error_code == %d, error_message == '%s'",
				callback_info->object_implementer->getInternalLastError(), callback_info->object_implementer->getInternalLastErrorText());
		return ERR_AUGMENTED_CCB_APPLY_FAILED;
	}

	return ERR_NO_ERROR;
}

HCF_NS_HCF_SRV_END
