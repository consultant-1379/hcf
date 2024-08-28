#include "hcf/core/logger.h"
#include "hcf/core/guard.h"
#include "hcf/core/utils.h"
#include "hcf/imm/imm_config_class.h"

#include "imm_object_hcjob_scheduler.h"
#include "imm_object_hcsingle_event.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_SRV_IMM_OBJECT_HCSINGLE_EVENT__

// Extern objects
extern TSL_NS_HCF_IMM::imm_config_class<HCF_NS_HCF_SRV::imm_object_hcjob_scheduler> hcjob_scheduler_class_handler;

HCF_NS_HCF_SRV_BEGIN

// Static fields initialization
const char * const __TSL_CLASS_NAME__::HCF_HCSINGLE_EVENT_CLASS_NAME               = "HCMHcSingleEvent";
const char * const __TSL_CLASS_NAME__::HCF_HCSINGLE_EVENT_IMPLEMENTER_NAME         = "HcSingleEvent_Implementer";
const char * const __TSL_CLASS_NAME__::HCF_HCSINGLE_EVENT_ID_ATTR_NAME             = "hcSingleEventId";
const char * const __TSL_CLASS_NAME__::HCF_HCSINGLE_EVENT_SCHEDULED_TIME_ATTR_NAME = "scheduledTime";
const char * const __TSL_CLASS_NAME__::HCF_HCSINGLE_EVENT_ID_SEPARATOR_VALUE       = "##single##";

int __TSL_CLASS_NAME__::set_object_attributes (std::vector<std::pair<std::string, std::vector<void *> > > & attributes, TSL_NS_HCF_IMM::IMM_API_ObjectImplementer_CallbackInfo * callback_info) {
	HCF_CC_TRACE_FUNCTION;
	// Save the old values before modifying them, in order to rollback in case of errors
	TSL_CRITICAL_SECTION_BEGIN(sync_t, _mutex);
		_scheduled_time_old = _scheduled_time;
		_timeplan_old       = _timeplan;
	TSL_CRITICAL_SECTION_END;

	int call_result = 0;

	// For each of the provided attributes, understand the attribute name and set the class field accordingly
	for (size_t i = 0; i < attributes.size(); ++i) {

		// Extract from the given attributes vector the attribute name and its value
		std::string attr_name = attributes[i].first;
		std::vector<void *> attr_values = attributes[i].second;
		HCF_CC_TRACE(SetObjectAttributes, "Received a request to set/change the value of the following attribute:", attr_name.c_str());

		if (!attr_name.compare(HCF_HCSINGLE_EVENT_ID_ATTR_NAME)) {
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
		else if (!attr_name.compare(HCF_HCSINGLE_EVENT_SCHEDULED_TIME_ATTR_NAME)) {
			if (attr_values.size() > 0) {
				std::string tmp_time = reinterpret_cast<char *>(attr_values[0]);
				HCF_CC_TRACE(SetObjectAttributes, "The new value of the scheduled time attribute is:", tmp_time.c_str());

				/*
				 * Check that the provided time is valid (>= now).
				 * This check must be done only if the object is created/modified via model.
				 * The object is created/modified via model iff callback_info is not NULL.
				 */
				date_time date;
				if ((call_result = convert_iso8601_format_into_datetime(tmp_time, date))) {
					HCF_CC_TRACE(SetObjectAttributesErr, "Call 'convert_iso8601_format_into_datetime' failed.", call_result);
					HCF_CC_LOG(LEVEL_ERROR, "Failed to convert the string '%s' into a date_time object, "
							"call_result == %d", tmp_time.c_str(), call_result);
					set_error_message(HCF_INVALID_TIME_VALUE_ERROR_MSG);
					return call_result;
				}

				HCF_CC_TRACE(SetObjectAttributes, "The date value is valid, checking if it is not in the past!", tmp_time.c_str());
				if ((date < date_time::now()) && (callback_info)) {
					// ERROR: the date is in the past!
					HCF_CC_TRACE(SetObjectAttributesErr, "The provided date value is in the past.", ERR_BAD_VALUE_PROVIDED);
					HCF_CC_LOG(LEVEL_ERROR, "The provided time value (%s) is in the past!", tmp_time.c_str());
					set_error_message(HCF_PAST_TIME_VALUE_ERROR_MSG);
					return ERR_BAD_VALUE_PROVIDED;
				}

				_scheduled_time = tmp_time;
			}
			else { // ERROR: the ID field cannot be empty!
				HCF_CC_TRACE(SetObjectAttributesErr, "No value provided for the mandatory attribute.", ERR_IMM_MANDATORY_VALUE_MISSING);
				HCF_CC_LOG(LEVEL_ERROR, "No value provided for the mandatory attribute '%s'!", attr_name.c_str());
				set_error_message(HCF_MANDATORY_VALUE_MISSING_ERROR_MSG);
				return ERR_IMM_MANDATORY_VALUE_MISSING;
			}
		}
	}

	// Check if the stored time plan must be updated
	if (_scheduled_time != _scheduled_time_old) {
		HCF_CC_TRACE(SetObjectAttributes, "The internal timeplan object must be updated: converting scheduled time.", _scheduled_time.c_str());
		// Convert the new scheduledTime string value into a date_time object
		date_time date;
		if ((call_result = convert_iso8601_format_into_datetime(_scheduled_time, date))) {
			HCF_CC_TRACE(SetObjectAttributesErr, "Call 'convert_iso8601_format_into_datetime' failed.", call_result);
			HCF_CC_LOG(LEVEL_ERROR, "Failed to convert the string '%s' into a date_time object, "
					"call_result == %d", _scheduled_time.c_str(), call_result);
			set_error_message(HCF_GENERIC_ERROR_ERROR_MSG);
			return call_result;
		}
		HCF_CC_TRACE(SetObjectAttributes, "The internal timeplan object must be updated: setting scheduled time.", _scheduled_time.c_str());

		// Store the parsed information into the proper class member
		TSL_CRITICAL_SECTION_BEGIN(sync_t, _mutex);
			if ((call_result = _timeplan.set_datetime(date.year(), date.month(), date.day(), date.hour(), date.min(), date.sec()))) {
				HCF_CC_TRACE(SetObjectAttributesErr, "Call '_timeplan.set_datetime' failed.", call_result);
				HCF_CC_LOG(LEVEL_ERROR, "Call '_timeplan.set_datetime' failed with call_result == %d, "
						"year == %u, month == %u, day == %u, hour == %u, min == %u, sec == %u", call_result,
						date.year(), date.month(), date.day(), date.hour(), date.min(), date.sec());
				set_error_message(HCF_GENERIC_ERROR_ERROR_MSG);
				return call_result;
			}
		TSL_CRITICAL_SECTION_END;
	}
	HCF_CC_TRACE(SetObjectAttributes, "Retrieving the HcJobScheduler instance associated to the object!", _scheduler.c_str());

	// Notify the parent HcJobScheduler object that a new scheduling policy has been added
	imm_object_hcjob_scheduler * scheduler_ptr = hcjob_scheduler_class_handler.get_instance(_scheduler);
	if (!scheduler_ptr) {
		HCF_CC_TRACE(SetObjectAttributesErr, "Call 'hcjob_scheduler_class_handler.get_instance' failed.", ERR_NULL_INSTANCE_RETRIEVED);
		HCF_CC_LOG(LEVEL_ERROR, "Call 'hcjob_scheduler_class_handler.get_instance' failed with ID == '%s'!", _scheduler.c_str());
		set_error_message(HCF_GENERIC_ERROR_ERROR_MSG);
		return ERR_NULL_INSTANCE_RETRIEVED;
	}

	// Extract the time plan ID if not yet done
	if (_timeplan_id.empty()) {
		HCF_CC_TRACE(SetObjectAttributes, "The ID of the timeplan was not yet calculated!", "");
		size_t equal_pos = _obj_rdn.find_first_of("=");
		if (equal_pos == std::string::npos) {
			HCF_CC_TRACE(SetObjectAttributesErr, "The HcSingleEvent RDN is not correctly formed.", ERR_NULL_INSTANCE_RETRIEVED);
			HCF_CC_LOG(LEVEL_ERROR, "The HcSingleEvent RDN (%s) is not correctly formed!", _obj_rdn.c_str());
			set_error_message(HCF_GENERIC_ERROR_ERROR_MSG);
			return ERR_NULL_INSTANCE_RETRIEVED;
		}
		_timeplan_id = _obj_rdn.substr(equal_pos + 1) + HCF_HCSINGLE_EVENT_ID_SEPARATOR_VALUE + scheduler_ptr->get_id();
		HCF_CC_TRACE(SetObjectAttributes, "The ID of the timeplan is", _timeplan_id.c_str());
	}

  // Add (or modify) the current scheduling policy ID to the HcJobScheduler instance
	if (_scheduled_time_old.empty()) {
		// The HcSingleEvent object has been created just now, the scheduling policy must be added
		HCF_CC_TRACE(SetObjectAttributes, "This object was created just now, adding the new scheduling policy, with the following ID:", _timeplan_id.c_str());
		if ((call_result = scheduler_ptr->add_scheduling_policy(_timeplan_id, this, callback_info))) {
			HCF_CC_TRACE(SetObjectAttributesErr, "Call 'scheduler_ptr->add_scheduling_policy' failed.", call_result);
	  	HCF_CC_LOG(LEVEL_ERROR, "Call 'scheduler_ptr->add_scheduling_policy' failed for ID == '%s', call_result == %d", _timeplan_id.c_str(), call_result);
			set_error_message(HCF_GENERIC_ERROR_ERROR_MSG);
	  	return call_result;
		}
	}
	else {
		// The HcSingleEvent object has been modified, the scheduling policy must be modified
		HCF_CC_TRACE(SetObjectAttributes, "This object was updated now, modifying the new scheduling policy, with the following ID:", _timeplan_id.c_str());
		if ((call_result = scheduler_ptr->modify_scheduling_policy(_timeplan_id, this, callback_info))) {
			HCF_CC_TRACE(SetObjectAttributesErr, "Call 'scheduler_ptr->modify_scheduling_policy' failed.", call_result);
	  	HCF_CC_LOG(LEVEL_ERROR, "Call 'scheduler_ptr->modify_scheduling_policy' failed for ID == '%s', call_result == %d", _timeplan_id.c_str(), call_result);
			set_error_message(HCF_GENERIC_ERROR_ERROR_MSG);
	  	return call_result;
		}
	}

	HCF_CC_LOG(LEVEL_NOTICE, "Object attributes correctly modified: RDN == '%s', _scheduled_time == '%s'", _obj_rdn.c_str(), _scheduled_time.c_str());
	return ERR_NO_ERROR;
}

int __TSL_CLASS_NAME__::set_object_attributes_and_dn (std::vector<std::pair<std::string, std::vector<void *> > > & attributes, const char * parent_dn, TSL_NS_HCF_IMM::IMM_API_ObjectImplementer_CallbackInfo * callback_info) {
	HCF_CC_TRACE_FUNCTION;
	// FIRST: Calculate the ID of the father (HcJobScheduler) object
	_scheduler = imm_object_hcjob_scheduler::get_obj_id_from_dn(parent_dn);
	HCF_CC_TRACE(SetObjectAttributes, "The father HcJobScheduler object has the following ID:", _scheduler.c_str());

	// SECOND: Call the same method in the base class
	HCF_CC_TRACE(SetObjectAttributes, "Setting the object attributes and its DN.", "");
	if (const int call_result = TSL_NS_HCF_IMM::imm_object::set_object_attributes_and_dn(attributes, parent_dn, callback_info)) {
		HCF_CC_TRACE(SetObjectAttributesErr, "Call 'imm_object::set_object_attributes_and_dn' failed.", call_result);
		HCF_CC_LOG(LEVEL_ERROR, "Call 'set_object_attributes_and_dn' failed, return_code == %d", call_result);
		return call_result;
	}

	// THIRD: Assign the correct value for the ID field
	_obj_id = get_obj_id_from_dn(_obj_dn.c_str());
	HCF_CC_TRACE(SetObjectAttributes, "The ID of this object is:", _obj_id.c_str());
	return ERR_NO_ERROR;
}

int __TSL_CLASS_NAME__::finalize_object (TSL_NS_HCF_IMM::IMM_API_ObjectImplementer_CallbackInfo * callback_info) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(FinalizeObject, "Received a request to finalize this object: retrieving its HcJobScheduler instance!", _obj_id.c_str(), _scheduler.c_str());

	// When this instance of HcSingleEvent class is deleted, the parent HcJobScheduler instance must be notified
	imm_object_hcjob_scheduler * scheduler_ptr = hcjob_scheduler_class_handler.get_instance(_scheduler);
	if (!scheduler_ptr) {
		HCF_CC_TRACE(FinalizeObjectErr, "Call 'hcjob_scheduler_class_handler.get_instance' failed.", ERR_NULL_INSTANCE_RETRIEVED);
		HCF_CC_LOG(LEVEL_ERROR, "Call 'hcjob_scheduler_class_handler.get_instance' failed with ID == '%s', "
				"probably the scheduler has been already deleted, nothing to do!", _scheduler.c_str());
		return ERR_NO_ERROR;
	}

	/*
	 * Check if also the scheduler object is involved in the delete callback.
	 * In this case, the policy should have been already removed
	 */
	HCF_CC_TRACE(FinalizeObject, "Checking if the scheduling policy was associated to the scheduler object!", _obj_id.c_str(), _timeplan_id.c_str());
	if (!scheduler_ptr->scheduling_policy_present(_timeplan_id)) {
		HCF_CC_TRACE(FinalizeObject, "The scheduling policy was already removed, nothing to do!", _obj_id.c_str(), _timeplan_id.c_str());
		HCF_CC_LOG(LEVEL_NOTICE, "The scheduling policy with ID == '%s' has been already removed: nothing to do!", _timeplan_id.c_str());
		return ERR_NO_ERROR;
	}

	HCF_CC_TRACE(FinalizeObject, "Removing the scheduling policy from the internal data structures!", _obj_id.c_str(), _timeplan_id.c_str());
	// Remove the current scheduling policy ID from the HcJobScheduler instance
	if (const int call_result = scheduler_ptr->remove_scheduling_policy(_timeplan_id, callback_info)) {
		HCF_CC_TRACE(FinalizeObjectErr, "Call 'scheduler_ptr->remove_scheduling_policy' failed.", call_result);
  	HCF_CC_LOG(LEVEL_ERROR, "Call 'scheduler_ptr->remove_scheduling_policy' failed for ID == '%s', call_result == %d", _timeplan_id.c_str(), call_result);
	}

	return ERR_NO_ERROR;
}

int __TSL_CLASS_NAME__::abort_object_modification (TSL_NS_HCF_IMM::IMM_API_ObjectImplementer_CallbackInfo * callback_info) {
	HCF_CC_TRACE_FUNCTION;
	// Restore the original values
	TSL_CRITICAL_SECTION_BEGIN(sync_t, _mutex);
		_scheduled_time = _scheduled_time_old;
		_timeplan       = _timeplan_old;
	TSL_CRITICAL_SECTION_END;

	// The callback has been aborted, restore the initial situation
	HCF_CC_TRACE(AbortObjectModification, "Received a request to abort all the modifications to this object: retrieving its HcJobScheduler instance!", _obj_id.c_str(), _scheduler.c_str());
	imm_object_hcjob_scheduler * scheduler_ptr = hcjob_scheduler_class_handler.get_instance(_scheduler);
	if (!scheduler_ptr) {
		HCF_CC_TRACE(AbortObjectModificationErr, "Call 'hcjob_scheduler_class_handler.get_instance' failed.", ERR_NULL_INSTANCE_RETRIEVED);
		HCF_CC_LOG(LEVEL_ERROR, "Call 'hcjob_scheduler_class_handler.get_instance' failed with ID == '%s'", _scheduler.c_str());
		return ERR_NULL_INSTANCE_RETRIEVED;
	}

	// Modify the scheduling policy, restoring the original one
	HCF_CC_TRACE(AbortObjectModification, "Modifying the scheduling policy into the internal data structures!", _obj_id.c_str(), _timeplan_id.c_str());
	if (const int call_result = scheduler_ptr->modify_scheduling_policy(_timeplan_id, this, callback_info)) {
		HCF_CC_TRACE(AbortObjectModificationErr, "Call 'scheduler_ptr->modify_scheduling_policy' failed.", call_result);
  	HCF_CC_LOG(LEVEL_ERROR, "Call 'scheduler_ptr->modify_scheduling_policy' failed for ID == '%s', call_result == %d", _timeplan_id.c_str(), call_result);
  	return call_result;
	}

	return ERR_NO_ERROR;
}

int __TSL_CLASS_NAME__::add_timeplan (const std::string & id, clock_alarm_notifier * notifier, TSL_NS_HCF_CORE::clock_alarm_handler * handler) {
	HCF_CC_TRACE_FUNCTION;
	TSL_CRITICAL_SECTION_BEGIN(sync_t, _mutex);
		HCF_CC_TRACE(AddTimeplan, "Adding the timeplan to the Clock Alarm Notifier object!", id.c_str(), _obj_id.c_str());
		if (const int call_result = notifier->add_timeplan(id, _timeplan, handler)) {
			HCF_CC_TRACE(AddTimeplanErr, "Call 'notifier->add_timeplan' failed.", notifier->error());
			HCF_CC_LOG(LEVEL_ERROR, "Call 'notifier->add_timeplan' failed, call_result == %d, error == %d, error_message == '%s'",
					call_result, notifier->error(), notifier->error_text());
			return notifier->error();
		}
	TSL_CRITICAL_SECTION_END;
	return ERR_NO_ERROR;
}

int __TSL_CLASS_NAME__::modify_timeplan (const std::string & id, clock_alarm_notifier * notifier, TSL_NS_HCF_CORE::clock_alarm_handler * handler) {
	HCF_CC_TRACE_FUNCTION;
	TSL_CRITICAL_SECTION_BEGIN(sync_t, _mutex);
		HCF_CC_TRACE(ModifyTimeplan, "Modifying the timeplan into the Clock Alarm Notifier object!", id.c_str(), _obj_id.c_str());
		if (const int call_result = notifier->change_timeplan(id, _timeplan, handler)) {
			HCF_CC_TRACE(ModifyTimeplanErr, "Call 'notifier->change_timeplan' failed.", notifier->error());
			HCF_CC_LOG(LEVEL_ERROR, "Call 'notifier->change_timeplan' failed, call_result == %d, error == %d, error_message == '%s'",
					call_result, notifier->error(), notifier->error_text());
			return notifier->error();
		}
	TSL_CRITICAL_SECTION_END;
	return ERR_NO_ERROR;
}

int __TSL_CLASS_NAME__::remove_timeplan (const std::string & id, clock_alarm_notifier * notifier) {
	HCF_CC_TRACE_FUNCTION;
	TSL_CRITICAL_SECTION_BEGIN(sync_t, _mutex);
		HCF_CC_TRACE(RemoveTimeplan, "Removing the timeplan from the Clock Alarm Notifier object!", id.c_str(), _obj_id.c_str());
		if (const int call_result = notifier->remove_timeplan(id)) {
			HCF_CC_TRACE(RemoveTimeplanErr, "Call 'notifier->remove_timeplan' failed.", notifier->error());
			HCF_CC_LOG(LEVEL_ERROR, "Call 'notifier->remove_timeplan' failed, call_result == %d, error == %d, error_message == '%s'",
					call_result, notifier->error(), notifier->error_text());
			return notifier->error();
		}
	TSL_CRITICAL_SECTION_END;
	return ERR_NO_ERROR;
}

int __TSL_CLASS_NAME__::get_next_scheduled_time (::tm & tm) {
	HCF_CC_TRACE_FUNCTION;
	TSL_CRITICAL_SECTION_BEGIN(sync_t, _mutex);
		HCF_CC_TRACE(GetNextScheduledTime, "Received a request to retrieve the next scheduled time for this timeplan:", _timeplan_id.c_str());
		if (const int call_result = _timeplan.get_next_datetime(tm)) {
			HCF_CC_TRACE(GetNextScheduledTimeErr, "Call '_timeplan.get_next_datetime' failed.", call_result);
			HCF_CC_LOG(LEVEL_ERROR, "Call '_timeplan.get_next_datetime' failed, call_result == %d", call_result);
			return call_result;
		}
	TSL_CRITICAL_SECTION_END;
	HCF_CC_TRACE(GetNextScheduledTime, "The next scheduled time for this timeplan is:", TSL_NS_HCF_CORE::utils::calculate_time_string(&tm).c_str());
	return ERR_NO_ERROR;
}

std::string __TSL_CLASS_NAME__::get_obj_id_from_dn (const char * obj_dn) {
	HCF_CC_TRACE_FUNCTION;
	/*
   * In HcSingleEvent class, the object ID must be calculated in a different way.
   * If the object has the following DN:
   * "hcSingleEventId=<ID>,hcJobSchedulerId=<SCHED_ID>,hcJobId=<JOB_ID>,HCMhealthCheckMId=1",
   * then the object ID will be "<ID>##single##<JOB_ID>".
   */
  std::string dn(obj_dn);
  HCF_CC_TRACE(GetObjectIdFromDn, "Received a request to calculate the HcSingleEvent ID starting from the DN: extracting the first part!", obj_dn);

  size_t comma_pos = dn.find_first_of(",");
  size_t equal_pos = dn.find_first_of("=");
  if (equal_pos == std::string::npos) {
    HCF_CC_TRACE(GetObjectIdFromDn, "The HcSingleEvent DN is not correctly formed!", obj_dn);
  	HCF_CC_LOG(LEVEL_WARNING, "The HcSingleEvent DN (%s) is not correctly formed!", obj_dn);
  	return "";
  }
  std::string single_event_id = dn.substr((equal_pos + 1), (comma_pos - equal_pos - 1));

  // Skip <SCHED_ID> (it's always equal to 1)
  HCF_CC_TRACE(GetObjectIdFromDn, "Received a request to calculate the HcSingleEvent ID starting from the DN: extracting the second part!", obj_dn);
  comma_pos = dn.find_first_of(",", (comma_pos + 1));
  equal_pos = dn.find_first_of("=", (equal_pos + 1));
  if (equal_pos == std::string::npos) {
    HCF_CC_TRACE(GetObjectIdFromDn, "The HcSingleEvent DN is not correctly formed!", obj_dn);
  	HCF_CC_LOG(LEVEL_WARNING, "The HcSingleEvent DN (%s) is not correctly formed!", obj_dn);
  	return "";
  }
  HCF_CC_TRACE(GetObjectIdFromDn, "Received a request to calculate the HcSingleEvent ID starting from the DN: extracting the third part!", obj_dn);

  comma_pos = dn.find_first_of(",", (comma_pos + 1));
  equal_pos = dn.find_first_of("=", (equal_pos + 1));
  if (equal_pos == std::string::npos) {
    HCF_CC_TRACE(GetObjectIdFromDn, "The HcSingleEvent DN is not correctly formed!", obj_dn);
  	HCF_CC_LOG(LEVEL_WARNING, "The HcSingleEvent DN (%s) is not correctly formed!", obj_dn);
  	return "";
  }
  std::string job_id = dn.substr((equal_pos + 1), (comma_pos - equal_pos - 1));
  return single_event_id + HCF_HCSINGLE_EVENT_ID_SEPARATOR_VALUE + job_id;
}

HCF_NS_HCF_SRV_END
