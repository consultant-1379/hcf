#include "hcf/core/logger.h"
#include "hcf/core/utils.h"
#include "hcf/imm/imm_config_class.h"

#include "srv_utils.h"
#include "imm_object_hcjob_scheduler.h"
#include "imm_object_hccalendar_event.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_SRV_IMM_OBJECT_HCCALENDAR_EVENT__

// Extern objects
extern TSL_NS_HCF_IMM::imm_config_class<HCF_NS_HCF_SRV::imm_object_hcjob_scheduler> hcjob_scheduler_class_handler;

HCF_NS_HCF_SRV_BEGIN

// Static fields initialization
const char * const           __TSL_CLASS_NAME__::HCF_HCCALENDAR_EVENT_CLASS_NAME                       = "HCMHcCalendarBasedPeriodicEvent";
const char * const           __TSL_CLASS_NAME__::HCF_HCCALENDAR_EVENT_IMPLEMENTER_NAME                 = "HcCalendarBasedPeriodicEvent_Implementer";
const char * const           __TSL_CLASS_NAME__::HCF_HCCALENDAR_EVENT_ID_ATTR_NAME                     = "hcCalendarBasedPeriodicEventId";
const char * const           __TSL_CLASS_NAME__::HCF_HCCALENDAR_EVENT_DAY_OF_MONTH_ATTR_NAME           = "dayOfMonth";
const char * const           __TSL_CLASS_NAME__::HCF_HCCALENDAR_EVENT_DAY_OF_WEEK_ATTR_NAME            = "dayOfWeek";
const char * const           __TSL_CLASS_NAME__::HCF_HCCALENDAR_EVENT_DAY_OF_WEEK_OCCURR_ATTR_NAME     = "dayOfWeekOccurrence";
const char * const           __TSL_CLASS_NAME__::HCF_HCCALENDAR_EVENT_MONTH_ATTR_NAME                  = "month";
const char * const           __TSL_CLASS_NAME__::HCF_HCCALENDAR_EVENT_START_TIME_ATTR_NAME             = "startTime";
const char * const           __TSL_CLASS_NAME__::HCF_HCCALENDAR_EVENT_STOP_TIME_ATTR_NAME              = "stopTime";
const char * const           __TSL_CLASS_NAME__::HCF_HCCALENDAR_EVENT_TIME_ATTR_NAME                   = "time";
const char * const           __TSL_CLASS_NAME__::HCF_HCCALENDAR_EVENT_ID_SEPARATOR_VALUE               = "##calendar##";
const int32_t                __TSL_CLASS_NAME__::HCF_HCCALENDAR_EVENT_DAY_OF_MONTH_DEFAULT_VALUE       = 0;
const dayofweek_t 	         __TSL_CLASS_NAME__::HCF_HCCALENDAR_EVENT_DAY_OF_WEEK_DEFAULT_VALUE        = ALL_DAYS;
const dayofweek_occurrence_t __TSL_CLASS_NAME__::HCF_HCCALENDAR_EVENT_DAY_OF_WEEK_OCCURR_DEFAULT_VALUE = ALL_OCCURRENCES;
const int32_t                __TSL_CLASS_NAME__::HCF_HCCALENDAR_EVENT_MONTH_DEFAULT_VALUE              = 0;
const char * const           __TSL_CLASS_NAME__::HCF_HCCALENDAR_EVENT_STOP_TIME_DEFAULT_VALUE          = "2099-12-31T23:59:59";

int __TSL_CLASS_NAME__::set_object_attributes (std::vector<std::pair<std::string, std::vector<void *> > > & attributes, TSL_NS_HCF_IMM::IMM_API_ObjectImplementer_CallbackInfo * callback_info) {
	HCF_CC_TRACE_FUNCTION;
	// Save the old values before modifying them, in order to rollback in case of errors
	TSL_CRITICAL_SECTION_BEGIN(sync_t, _mutex);
		_dayofmonth_old           = _dayofmonth;
		_dayofweek_old            = _dayofweek;
		_dayofweek_occurrence_old = _dayofweek_occurrence;
		_month_old                = _month;
		_start_time_old           = _start_time;
		_stop_time_old            = _stop_time;
		_time_old                 = _time;
		_timeplan_old             = _timeplan;
	TSL_CRITICAL_SECTION_END;

	int call_result = 0;
	bool augmentation_operation_to_be_executed = false;

	// For each of the provided attributes, understand the attribute name and set the class field accordingly
	for (size_t i = 0; i < attributes.size(); ++i) {

		// Extract from the given attributes vector the attribute name and its value
		std::string attr_name = attributes[i].first;
		std::vector<void *> attr_values = attributes[i].second;
		HCF_CC_TRACE(SetObjectAttributes, "Received a request to set/change the value of the following attribute:", attr_name.c_str());

		if (!attr_name.compare(HCF_HCCALENDAR_EVENT_ID_ATTR_NAME)) {
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
		else if (!attr_name.compare(HCF_HCCALENDAR_EVENT_DAY_OF_MONTH_ATTR_NAME)) {
			if (attr_values.size() > 0) {
				int32_t tmp_value = *(reinterpret_cast<int32_t *>(attr_values[0]));
				HCF_CC_TRACE(SetObjectAttributes, "The new value of the dayofmonth attribute is:", std::to_string(tmp_value).c_str());

				// Check if a valid value has been provided for the dayOfMonth attribute
				if (!is_valid_dayofmonth(tmp_value)) {
					// Invalid value provided for the attribute: this shouldn't occur (COM should do the check)!
					HCF_CC_TRACE(SetObjectAttributesErr, "Invalid value provided for the attribute.", ERR_IMM_MANDATORY_VALUE_MISSING);
					HCF_CC_LOG(LEVEL_ERROR, "Invalid value (%d) provided for the attribute '%s'!", tmp_value, attr_name.c_str());
					set_error_message(HCF_INVALID_VALUE_PROVIDED_ERROR_MSG);
					return ERR_IMM_MANDATORY_VALUE_MISSING;
				}
				_dayofmonth = tmp_value;
			}
			else {
				// No value provided, using the default value
				_dayofmonth = HCF_HCCALENDAR_EVENT_DAY_OF_MONTH_DEFAULT_VALUE;
				HCF_CC_TRACE(SetObjectAttributes, "No value for the dayofmonth attribute was provided, using the default:", std::to_string(_dayofmonth).c_str());
			}
		}
		else if (!attr_name.compare(HCF_HCCALENDAR_EVENT_DAY_OF_WEEK_ATTR_NAME)) {
			if (attr_values.size() > 0) {
				dayofweek_t tmp_value = *(reinterpret_cast<dayofweek_t *>(attr_values[0]));
				HCF_CC_TRACE(SetObjectAttributes, "The new value of the dayofweek attribute is:", std::to_string(tmp_value).c_str());

				// Check if a valid value has been provided for the dayOfWeek attribute
				if (!is_valid_dayofweek(tmp_value)) {
					// Invalid value provided for the attribute: this shouldn't occur (COM should do the check)!
					HCF_CC_TRACE(SetObjectAttributesErr, "Invalid value provided for the attribute.", ERR_IMM_MANDATORY_VALUE_MISSING);
					HCF_CC_LOG(LEVEL_ERROR, "Invalid value (%d) provided for the attribute '%s'!", tmp_value, attr_name.c_str());
					set_error_message(HCF_INVALID_VALUE_PROVIDED_ERROR_MSG);
					return ERR_IMM_MANDATORY_VALUE_MISSING;
				}
				_dayofweek = tmp_value;
			}
			else {
				// No value provided, using the default value
				_dayofweek = HCF_HCCALENDAR_EVENT_DAY_OF_WEEK_DEFAULT_VALUE;
				HCF_CC_TRACE(SetObjectAttributes, "No value for the dayofweek attribute was provided, using the default:", std::to_string(_dayofweek).c_str());
			}
		}
		else if (!attr_name.compare(HCF_HCCALENDAR_EVENT_DAY_OF_WEEK_OCCURR_ATTR_NAME)) {
			if (attr_values.size() > 0) {
				dayofweek_occurrence_t tmp_value = *(reinterpret_cast<dayofweek_occurrence_t *>(attr_values[0]));
				HCF_CC_TRACE(SetObjectAttributes, "The new value of the dayofweekoccurrence attribute is:", std::to_string(tmp_value).c_str());

				// Check if a valid value has been provided for the dayOfWeekOccurrence attribute
				if (!is_valid_dayofweek_occurrence(tmp_value)) {
					// Invalid value provided for the attribute: this shouldn't occur (COM should do the check)!
					HCF_CC_TRACE(SetObjectAttributesErr, "Invalid value provided for the attribute.", ERR_IMM_MANDATORY_VALUE_MISSING);
					HCF_CC_LOG(LEVEL_ERROR, "Invalid value (%d) provided for the attribute '%s'!", tmp_value, attr_name.c_str());
					set_error_message(HCF_INVALID_VALUE_PROVIDED_ERROR_MSG);
					return ERR_IMM_MANDATORY_VALUE_MISSING;
				}
				_dayofweek_occurrence = tmp_value;
			}
			else {
				// No value provided, using the default value
				_dayofweek_occurrence = HCF_HCCALENDAR_EVENT_DAY_OF_WEEK_OCCURR_DEFAULT_VALUE;
				HCF_CC_TRACE(SetObjectAttributes, "No value for the dayofweekoccurrence attribute was provided, using the default:", std::to_string(_dayofweek_occurrence).c_str());
			}
		}
		else if (!attr_name.compare(HCF_HCCALENDAR_EVENT_MONTH_ATTR_NAME)) {
			if (attr_values.size() > 0) {
				int32_t tmp_value = *(reinterpret_cast<int32_t *>(attr_values[0]));
				HCF_CC_TRACE(SetObjectAttributes, "The new value of the month attribute is:", std::to_string(tmp_value).c_str());

				// Check if a valid value has been provided for the month attribute
				if (!is_valid_month(tmp_value)) {
					// Invalid value provided for the attribute: this shouldn't occur (COM should do the check)!
					HCF_CC_TRACE(SetObjectAttributesErr, "Invalid value provided for the attribute.", ERR_IMM_MANDATORY_VALUE_MISSING);
					HCF_CC_LOG(LEVEL_ERROR, "Invalid value (%d) provided for the attribute '%s'!", tmp_value, attr_name.c_str());
					set_error_message(HCF_INVALID_VALUE_PROVIDED_ERROR_MSG);
					return ERR_IMM_MANDATORY_VALUE_MISSING;
				}
				_month = tmp_value;
			}
			else {
				// No value provided, using the default value
				_month = HCF_HCCALENDAR_EVENT_MONTH_DEFAULT_VALUE;
				HCF_CC_TRACE(SetObjectAttributes, "No value for the month attribute was provided, using the default:", std::to_string(_month).c_str());
			}
		}
		else if (!attr_name.compare(HCF_HCCALENDAR_EVENT_START_TIME_ATTR_NAME)) {
			if (attr_values.size() > 0)	{
				std::string tmp_time = reinterpret_cast<char *>(attr_values[0]);
				HCF_CC_TRACE(SetObjectAttributes, "The new value of the start time attribute is:", tmp_time.c_str());

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

				_start_time = tmp_time;
			}
			else {	// No value provided, using the current timestamp
				HCF_CC_TRACE(SetObjectAttributes, "No value for the start time attribute was provided, extracting the current time", "");
				std::string now_timestamp;
				if ((call_result = convert_datetime_into_iso8601_format(date_time::now(), now_timestamp))) {
					HCF_CC_TRACE(SetObjectAttributesErr, "Call 'convert_iso8601_format_into_datetime' failed.", call_result);
					HCF_CC_LOG(LEVEL_ERROR, "Failed to convert the current timestamp into a string, call_result == %d", call_result);
					set_error_message(HCF_GENERIC_ERROR_ERROR_MSG);
					return call_result;
				}

				_start_time = now_timestamp;
				HCF_CC_TRACE(SetObjectAttributes, "No value for the start time attribute was provided, using the current time:", _start_time.c_str());
				augmentation_operation_to_be_executed = true;
			}
		}
		else if (!attr_name.compare(HCF_HCCALENDAR_EVENT_STOP_TIME_ATTR_NAME)) {
			if (attr_values.size() > 0)	{
				std::string tmp_time = reinterpret_cast<char *>(attr_values[0]);
				HCF_CC_TRACE(SetObjectAttributes, "The new value of the stop time attribute is:", tmp_time.c_str());

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

				_stop_time = tmp_time;
			}
			else {	// No value provided, using the default value
				_stop_time = HCF_HCCALENDAR_EVENT_STOP_TIME_DEFAULT_VALUE;
				HCF_CC_TRACE(SetObjectAttributes, "No value for the stop time attribute was provided, using the default value:", _stop_time.c_str());
			}
		}
		else if (!attr_name.compare(HCF_HCCALENDAR_EVENT_TIME_ATTR_NAME)) {
			if (attr_values.size() > 0) {
				std::string tmp_value = reinterpret_cast<char *>(attr_values[0]);
				HCF_CC_TRACE(SetObjectAttributes, "The new value of the time attribute is:", tmp_value.c_str());

				// Check if a valid value has been provided for the time attribute
				date_time date;
				if ((call_result = convert_time_string_into_datetime_format(tmp_value, date))) {
					// Invalid value provided for the attribute: this shouldn't occur (COM should do the check)!
					HCF_CC_TRACE(SetObjectAttributesErr, "Call 'convert_time_string_into_datetime_format' failed.", call_result);
					HCF_CC_LOG(LEVEL_ERROR, "Invalid value (%s) provided for the attribute '%s', call_result == %d",
							tmp_value.c_str(), attr_name.c_str(), call_result);
					set_error_message(HCF_INVALID_VALUE_PROVIDED_ERROR_MSG);
					return ERR_IMM_MANDATORY_VALUE_MISSING;
				}
				_time = tmp_value;
			}
			else { // ERROR: mandatory attributes cannot be empty!
				HCF_CC_TRACE(SetObjectAttributesErr, "No value provided for the mandatory attribute.", ERR_IMM_MANDATORY_VALUE_MISSING);
				HCF_CC_LOG(LEVEL_ERROR, "No value provided for the mandatory attribute '%s'!", attr_name.c_str());
				set_error_message(HCF_MANDATORY_VALUE_MISSING_ERROR_MSG);
				return ERR_IMM_MANDATORY_VALUE_MISSING;
			}
		}
	}

	/*
	 * Check if the interval [startTime, stopTime] is a valid interval.
	 * It is considered valid iff startTime <= stopTime.
	 * No checks on conversion result are needed, since the parsing was already done.
	 */
	date_time start, stop;
	convert_iso8601_format_into_datetime(_start_time, start);
	convert_iso8601_format_into_datetime(_stop_time, stop);

	if (start > stop) {
		HCF_CC_TRACE(SetObjectAttributesErr, "The value of the start time is bigger than the stop time.", ERR_BAD_VALUE_PROVIDED);
		HCF_CC_LOG(LEVEL_ERROR, "Invalid time interval provided: start is bigger than stop "
				"(_start_time == '%s', _stop_time == '%s')!", _start_time.c_str(), _stop_time.c_str());
		set_error_message(HCF_INVALID_TIME_INTERVAL_ERROR_MSG);
		return ERR_BAD_VALUE_PROVIDED;
	}

	/*
	 * Check if the values of the dayOfWeekOccurrence attribute is consistent.
	 * The value is considered consistent iff the value of dayOfWeek attribute is not ALL.
	 */
	if ((_dayofweek_occurrence != ALL_OCCURRENCES) && (_dayofweek == ALL_DAYS)) {
		HCF_CC_TRACE(SetObjectAttributesErr, "Incoherent value provided for dayOfWeekOccurrence than dayOfWeek.", ERR_BAD_VALUE_PROVIDED);
		HCF_CC_LOG(LEVEL_ERROR, "Incoherent value provided for dayOfWeekOccurrence "
				"(%d) than dayOfWeek (%d)", _dayofweek_occurrence, _dayofweek);
		set_error_message(HCF_INVALID_DAYOFWEEK_OCCURRENCE_ERROR_MSG);
		return ERR_BAD_VALUE_PROVIDED;
	}

	// Check if the stored time plan must be updated: it must be changed iff at least one class member has been changed
	if ((_dayofmonth != _dayofmonth_old) || (_dayofweek != _dayofweek_old) || (_dayofweek_occurrence != _dayofweek_occurrence_old) ||
			(_month != _month_old) || (_start_time != _start_time_old) || (_stop_time != _stop_time_old) || (_time != _time_old)) {
		date_time date;
		HCF_CC_TRACE(SetObjectAttributes, "The internal timeplan object must be updated: converting stop time.", _stop_time.c_str());

		// Convert the new stopTime string value into a date_time object
		if ((call_result = convert_iso8601_format_into_datetime(_stop_time, date))) {
			HCF_CC_TRACE(SetObjectAttributesErr, "Call 'convert_iso8601_format_into_datetime' failed.", call_result);
			HCF_CC_LOG(LEVEL_ERROR, "Failed to convert the string '%s' into a date_time object, "
					"call_result == %d", _stop_time.c_str(), call_result);
			set_error_message(HCF_GENERIC_ERROR_ERROR_MSG);
			return call_result;
		}
		HCF_CC_TRACE(SetObjectAttributes, "The internal timeplan object must be updated: setting stop time.", _stop_time.c_str());

		// Store the parsed info as start time for the periodic timeplan object
		TSL_CRITICAL_SECTION_BEGIN(sync_t, _mutex);
			if ((call_result = _timeplan.set_datetime_end(date.year(), date.month(), date.day(), date.hour(), date.min(), date.sec()))) {
				HCF_CC_TRACE(SetObjectAttributesErr, "Call '_timeplan.set_datetime_end' failed.", call_result);
				HCF_CC_LOG(LEVEL_ERROR, "Call '_timeplan.set_datetime_end' failed with call_result == %d, "
						"year == %u, month == %u, day == %u, hour == %u, min == %u, sec == %u", call_result,
						date.year(), date.month(), date.day(), date.hour(), date.min(), date.sec());
				set_error_message(HCF_GENERIC_ERROR_ERROR_MSG);
				return call_result;
			}
		TSL_CRITICAL_SECTION_END;

		// Convert the new startTime string value into a date_time object
		HCF_CC_TRACE(SetObjectAttributes, "The internal timeplan object must be updated: converting start time.", _start_time.c_str());
		if ((call_result = convert_iso8601_format_into_datetime(_start_time, date))) {
			HCF_CC_TRACE(SetObjectAttributesErr, "Call 'convert_iso8601_format_into_datetime' failed.", call_result);
			HCF_CC_LOG(LEVEL_ERROR, "Failed to convert the string '%s' into a date_time object, "
					"call_result == %d", _start_time.c_str(), call_result);
			set_error_message(HCF_GENERIC_ERROR_ERROR_MSG);
			return call_result;
		}
		HCF_CC_TRACE(SetObjectAttributes, "The internal timeplan object must be updated: setting start time.", _start_time.c_str());

		// Store the parsed info as start time for the periodic timeplan object
		TSL_CRITICAL_SECTION_BEGIN(sync_t, _mutex);
			if ((call_result = _timeplan.set_datetime_start(date.year(), date.month(), date.day(), date.hour(), date.min(), date.sec()))) {
				HCF_CC_TRACE(SetObjectAttributesErr, "Call '_timeplan.set_datetime_start' failed.", call_result);
				HCF_CC_LOG(LEVEL_ERROR, "Call '_timeplan.set_datetime_start' failed with call_result == %d, "
						"year == %u, month == %u, day == %u, hour == %u, min == %u, sec == %u", call_result,
						date.year(), date.month(), date.day(), date.hour(), date.min(), date.sec());
				set_error_message(HCF_GENERIC_ERROR_ERROR_MSG);
				return call_result;
			}
		TSL_CRITICAL_SECTION_END;

		// Convert the new time string value into a date_time object
		HCF_CC_TRACE(SetObjectAttributes, "The internal timeplan object must be updated: converting time.", _time.c_str());
		if ((call_result = convert_time_string_into_datetime_format(_time, date))) {
			HCF_CC_TRACE(SetObjectAttributesErr, "Call 'convert_iso8601_format_into_datetime' failed.", call_result);
			HCF_CC_LOG(LEVEL_ERROR, "Failed to convert the string '%s' into a date_time object, call_result == %d", _time.c_str(), call_result);
			set_error_message(HCF_GENERIC_ERROR_ERROR_MSG);
			return call_result;
		}
		HCF_CC_TRACE(SetObjectAttributes, "The internal timeplan object must be updated: setting time.", _time.c_str());

		TSL_CRITICAL_SECTION_BEGIN(sync_t, _mutex);
			if ((call_result = _timeplan.t_time(date.hour(), date.min(), date.sec()))) {
				HCF_CC_TRACE(SetObjectAttributesErr, "Call '_timeplan.t_time' failed.", call_result);
				HCF_CC_LOG(LEVEL_ERROR, "Call '_timeplan.t_time' failed for triple (%u, %u, %u), call_result == %d",
						date.hour(), date.min(), date.sec(), call_result);
				set_error_message(HCF_GENERIC_ERROR_ERROR_MSG);
				return call_result;
			}
			HCF_CC_TRACE(SetObjectAttributes, "The internal timeplan object must be updated: setting all the other info.", "");

			// Set all the remaining info (that doesn't need any parsing operation)
			_timeplan.t_month(_month);
			_timeplan.t_dayofmonth(_dayofmonth);
			_timeplan.t_dayofweek(_dayofweek);
			_timeplan.t_dayofweek_occurrence(_dayofweek_occurrence);
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
			HCF_CC_TRACE(SetObjectAttributesErr, "The HcCalendarBasedPeriodicEvent RDN is not correctly formed.", ERR_NULL_INSTANCE_RETRIEVED);
			HCF_CC_LOG(LEVEL_ERROR, "The HcCalendarBasedPeriodicEvent RDN (%s) is not correctly formed!", _obj_rdn.c_str());
			set_error_message(HCF_GENERIC_ERROR_ERROR_MSG);
			return ERR_NULL_INSTANCE_RETRIEVED;
		}
		_timeplan_id = _obj_rdn.substr(equal_pos + 1) + HCF_HCCALENDAR_EVENT_ID_SEPARATOR_VALUE + scheduler_ptr->get_id();
		HCF_CC_TRACE(SetObjectAttributes, "The ID of the timeplan is", _timeplan_id.c_str());
	}

	// Initialize the Augmented CCB to add the scheduling policy
	if (callback_info) {
		HCF_CC_TRACE(SetObjectAttributes, "An augmented CCB operation must be executed: initializing handles.", "");
		if (callback_info->object_implementer->getCcbAugmentationInitialize(callback_info->oi_handle, callback_info->ccb_id, &(callback_info->ccb_handle), &(callback_info->admin_owner_handler)) != TSL_NS_HCF_IMM::OPERATION_SUCCESS) {
			HCF_CC_TRACE(SetObjectAttributesErr, "Call 'getCcbAugmentationInitialize' failed.", callback_info->object_implementer->getInternalLastError());
			HCF_CC_LOG(LEVEL_ERROR, "Call 'getCcbAugmentationInitialize' failed, error_code == %d, error_message == '%s'",
					callback_info->object_implementer->getInternalLastError(), callback_info->object_implementer->getInternalLastErrorText());
			set_error_message(HCF_GENERIC_ERROR_ERROR_MSG);
			return ERR_AUGMENTED_CCB_INIT_FAILED;
		}
		callback_info->initialized = true;
	}

  /*
   * Add (or modify) the current scheduling policy ID to the HcJobScheduler instance.
   * The current scheduling policy must be added only if the _time_old variable is empty.
   * This because the attribute time is the only mandatory attribute of HcCalendarBasedPeriodicEvent MOC,
   * so it must always have a previous value, except when it is just created.
   */
	if (_time_old.empty()) {
		// The HcCalendarBasedPeriodicEvent object has been created just now, the scheduling policy must be added
		HCF_CC_TRACE(SetObjectAttributes, "This object was created just now, adding the new scheduling policy, with the following ID:", _timeplan_id.c_str());
		if ((call_result = scheduler_ptr->add_scheduling_policy(_timeplan_id, this, callback_info))) {
			HCF_CC_TRACE(SetObjectAttributesErr, "Call 'scheduler_ptr->add_scheduling_policy' failed.", call_result);
	  	HCF_CC_LOG(LEVEL_ERROR, "Call 'scheduler_ptr->add_scheduling_policy' failed for ID == '%s', call_result == %d", _timeplan_id.c_str(), call_result);
			set_error_message(HCF_GENERIC_ERROR_ERROR_MSG);
	  	return call_result;
		}
	}
	else {
		// The HcCalendarBasedPeriodicEvent object has been modified, the scheduling policy must be modified
		HCF_CC_TRACE(SetObjectAttributes, "This object was updated now, modifying the new scheduling policy, with the following ID:", _timeplan_id.c_str());
		if ((call_result = scheduler_ptr->modify_scheduling_policy(_timeplan_id, this, callback_info))) {
			HCF_CC_TRACE(SetObjectAttributesErr, "Call 'scheduler_ptr->modify_scheduling_policy' failed.", call_result);
	  	HCF_CC_LOG(LEVEL_ERROR, "Call 'scheduler_ptr->modify_scheduling_policy' failed for ID == '%s', call_result == %d", _timeplan_id.c_str(), call_result);
			set_error_message(HCF_GENERIC_ERROR_ERROR_MSG);
	  	return call_result;
		}
	}

	if (augmentation_operation_to_be_executed) {
		/*
		 * The value for startTime attribute has not been provided.
		 * So, update its value with the current timestamp.
		 */
		if (callback_info) {
			HCF_CC_TRACE(SetObjectAttributes, "Executing the augmented operation as requested!", "");
			if ((call_result = execute_augmented_operations(callback_info))) {
				HCF_CC_TRACE(SetObjectAttributesErr, "Call 'execute_augmented_operations' failed.", call_result);
				HCF_CC_LOG(LEVEL_ERROR, "Call 'execute_augmented_operations' failed with error code == %d", call_result);
				set_error_message(HCF_GENERIC_ERROR_ERROR_MSG);
				return call_result;
			}
		}
		else {
			HCF_CC_TRACE(SetObjectAttributesErr, "NULL instance provided for callback_info structure!", ERR_NULL_INSTANCE_RETRIEVED);
			HCF_CC_LOG(LEVEL_ERROR, "NULL instance provided for callback_info structure!");
			set_error_message(HCF_GENERIC_ERROR_ERROR_MSG);
			return ERR_NULL_INSTANCE_RETRIEVED;
		}
	}

	// Apply the Augmented CCB operation
	if (callback_info) {
		HCF_CC_TRACE(SetObjectAttributes, "An augmented CCB operation has been executed: finalizing handles.", "");
		if (callback_info->object_implementer->applyAugmentCcb(callback_info->ccb_handle) != TSL_NS_HCF_IMM::OPERATION_SUCCESS) {
			HCF_CC_TRACE(SetObjectAttributesErr, "Call 'applyAugmentCcb' failed.", callback_info->object_implementer->getInternalLastError());
			HCF_CC_LOG(LEVEL_ERROR, "Call 'applyAugmentCcb' failed, error_code == %d, error_message == '%s'",
					callback_info->object_implementer->getInternalLastError(), callback_info->object_implementer->getInternalLastErrorText());
			set_error_message(HCF_GENERIC_ERROR_ERROR_MSG);
			return ERR_AUGMENTED_CCB_APPLY_FAILED;
		}
	}

	HCF_CC_LOG(LEVEL_NOTICE, "Object attributes correctly modified: RDN == '%s', _dayofmonth == %d, _dayofweek == %d,"
			"_dayofweek_occurrence == %d, _month == %d, _start_time == '%s', _stop_time == '%s', _time == '%s'", _obj_rdn.c_str(),
			_dayofmonth, _dayofweek, _dayofweek_occurrence, _month, _start_time.c_str(), _stop_time.c_str(), _time.c_str());
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

	// When this instance of HcCalendarBasedPeriodicEvent class is deleted, the parent HcJobScheduler instance must be notified
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
	// The callback has been aborted, restore the original values
	TSL_CRITICAL_SECTION_BEGIN(sync_t, _mutex);
		_dayofmonth           = _dayofmonth_old;
		_dayofweek            = _dayofweek_old;
		_dayofweek_occurrence = _dayofweek_occurrence_old;
		_month                = _month_old;
		_start_time           = _start_time_old;
		_stop_time            = _stop_time_old;
		_time                 = _time_old;
		_timeplan             = _timeplan_old;
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
   * In HcCalendarBasedPeriodicEvent class, the object ID must be calculated in a different way.
   * If the object has the following DN:
   * "hcCalendarBasedPeriodicEventId=<ID>,hcJobSchedulerId=<SCHED_ID>,hcJobId=<JOB_ID>,HCMhealthCheckMId=1",
   * then the object ID will be "<ID>##calendar##<JOB_ID>".
   */
  std::string dn(obj_dn);
  HCF_CC_TRACE(GetObjectIdFromDn, "Received a request to calculate the HcCalendarBasedPeriodicEvent ID starting from the DN: extracting the first part!", obj_dn);

  size_t comma_pos = dn.find_first_of(",");
  size_t equal_pos = dn.find_first_of("=");
  if (equal_pos == std::string::npos) {
    HCF_CC_TRACE(GetObjectIdFromDn, "The HcCalendarBasedPeriodicEvent DN is not correctly formed!", obj_dn);
		HCF_CC_LOG(LEVEL_WARNING, "The HcCalendarBasedPeriodicEvent DN (%s) is not correctly formed!", obj_dn);
  	return "";
  }
  std::string calendar_event_id = dn.substr((equal_pos + 1), (comma_pos - equal_pos - 1));

  // Skip <SCHED_ID> (it's always equal to 1)
  HCF_CC_TRACE(GetObjectIdFromDn, "Received a request to calculate the HcCalendarBasedPeriodicEvent ID starting from the DN: extracting the second part!", obj_dn);
  comma_pos = dn.find_first_of(",", (comma_pos + 1));
  equal_pos = dn.find_first_of("=", (equal_pos + 1));
  if (equal_pos == std::string::npos) {
    HCF_CC_TRACE(GetObjectIdFromDn, "The HcCalendarBasedPeriodicEvent DN is not correctly formed!", obj_dn);
		HCF_CC_LOG(LEVEL_WARNING, "The HcCalendarBasedPeriodicEvent DN (%s) is not correctly formed!", obj_dn);
  	return "";
  }

  HCF_CC_TRACE(GetObjectIdFromDn, "Received a request to calculate the HcCalendarBasedPeriodicEvent ID starting from the DN: extracting the third part!", obj_dn);
  comma_pos = dn.find_first_of(",", (comma_pos + 1));
  equal_pos = dn.find_first_of("=", (equal_pos + 1));
  if (equal_pos == std::string::npos) {
    HCF_CC_TRACE(GetObjectIdFromDn, "The HcCalendarBasedPeriodicEvent DN is not correctly formed!", obj_dn);
		HCF_CC_LOG(LEVEL_WARNING, "The HcCalendarBasedPeriodicEvent DN (%s) is not correctly formed!", obj_dn);
  	return "";
  }
  std::string job_id = dn.substr((equal_pos + 1), (comma_pos - equal_pos - 1));
  return calendar_event_id + HCF_HCCALENDAR_EVENT_ID_SEPARATOR_VALUE + job_id;
}

int __TSL_CLASS_NAME__::execute_augmented_operations (TSL_NS_HCF_IMM::IMM_API_ObjectImplementer_CallbackInfo * callback_info) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(ExecuteAugmentedOperations, "Received a request to set the start time to the following value:", _start_time.c_str());

	// Modify the startTime attribute value of HcCalendarBasedPeriodicEvent class
	TSL_NS_HCF_IMM::IMM_API_ImmParameter parameter;
	parameter.attrName = const_cast<char *>(HCF_HCCALENDAR_EVENT_START_TIME_ATTR_NAME);
	parameter.attrType = TSL_NS_HCF_IMM::ATTR_STRINGT;
	parameter.attrValuesNum = 1;
	void * tmp[1] = { const_cast<char *>(_start_time.c_str()) };
	parameter.attrValues = tmp;

	// Create the DN of HcPeriodicEvent instance under which the modification must be done
	HCF_CC_TRACE(ExecuteAugmentedOperations, "Retrieving the HcJobScheduler instance associated to the object!", _scheduler.c_str());
	imm_object_hcjob_scheduler * scheduler_ptr = hcjob_scheduler_class_handler.get_instance(_scheduler);
	if (!scheduler_ptr) {
		HCF_CC_TRACE(ExecuteAugmentedOperationsErr, "Call 'hcjob_scheduler_class_handler.get_instance' failed.", ERR_NULL_INSTANCE_RETRIEVED);
		HCF_CC_LOG(LEVEL_ERROR, "Call 'hcjob_scheduler_class_handler.get_instance' failed with ID == '%s'!", _scheduler.c_str());
		return ERR_NULL_INSTANCE_RETRIEVED;
	}

	char calendar_event_dn[2048];
	::snprintf(calendar_event_dn, TSL_ARRAY_SIZE(calendar_event_dn), "%s,%s",	_obj_rdn.c_str(), scheduler_ptr->get_dn().c_str());
	HCF_CC_TRACE(ExecuteAugmentedOperations, "Modifying the value of the startTime attribute for the following object:", calendar_event_dn);

	if (callback_info->object_implementer->modifyObjectAugmentCcb(callback_info->ccb_handle, callback_info->admin_owner_handler, calendar_event_dn, &parameter) != TSL_NS_HCF_IMM::OPERATION_SUCCESS) {
		HCF_CC_TRACE(ExecuteAugmentedOperationsErr, "Call 'modifyObjectAugmentCcb' failed.", callback_info->object_implementer->getInternalLastError());
		HCF_CC_LOG(LEVEL_ERROR, "Call 'modifyObjectAugmentCcb' failed for object having DN == '%s', error_code == %d, error_message == '%s'",
				calendar_event_dn, callback_info->object_implementer->getInternalLastError(), callback_info->object_implementer->getInternalLastErrorText());
		return ERR_AUGMENTED_CCB_MODIFY_FAILED;
	}
	return ERR_NO_ERROR;
}

HCF_NS_HCF_SRV_END
