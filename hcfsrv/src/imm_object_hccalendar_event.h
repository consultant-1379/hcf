#ifndef __INCLUDE_GUARD_CLASS_SRV_IMM_OBJECT_HCCALENDAR_EVENT__
#define __INCLUDE_GUARD_CLASS_SRV_IMM_OBJECT_HCCALENDAR_EVENT__ imm_object_hccalendar_event

/**
 *  @file imm_object_hccalendar_event.h
 *	@brief
 *	@author xcasale (Alessio Cascone)
 *	@date 2015-01-30
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
 *	| 2015-001 | 2015-01-30 | xcasale      | Creation and first revision.        |
 *	+==========+============+==============+=====================================+
 */

#include "hcf/core/calendar_periodic_alarm_timeplan.h"
#include "hcf/imm/imm_object.h"

#include "srv_macros.h"
#include "constants.h"
#include "job_scheduling_policy.h"

#include "hcf/trace/com_ericsson_common_hcf_srv_hccalendarevent.h"
#include "hcf/trace/tracing_macros.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_SRV_IMM_OBJECT_HCCALENDAR_EVENT__

HCF_NS_HCF_SRV_BEGIN

HCF_CC_FUNCTION_TRACER_CLASS_DEFINE

class __TSL_CLASS_NAME__ : public TSL_NS_HCF_IMM::imm_object, public job_scheduling_policy {
	//==============//
	// Constructors //
	//==============//
public:
	inline TSL_CTOR (const char * class_name)
		TSL_CTOR_INIT_LIST(imm_object(class_name), job_scheduling_policy(), _dayofmonth(), _dayofweek(),
				_dayofweek_occurrence(), _month(), _start_time(), _stop_time(), _time(), _scheduler(),
				_timeplan(), _mutex(), _dayofmonth_old(), _dayofweek_old(), _dayofweek_occurrence_old(),
				_month_old(), _start_time_old(), _stop_time_old(), _time_old(), _timeplan_old()) {}

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

	//==============================================================//
	// job_scheduling_policy class Virtual Functions Implementation //
	//==============================================================//
public:
	/**
	 * This method is called each time a new timeplan must be added to the clock notifier.
	 *
	 * @param[in]  id - The scheduling policy id.
	 * @param[in]  notifier - The notifier object to which the new timeplan must be added.
	 * @param[in]  handler - The handler object to be called when an event occurs.
	 * @return 0 on success, a non zero value otherwise.
	 */
	virtual int add_timeplan (const std::string & id, clock_alarm_notifier * notifier, TSL_NS_HCF_CORE::clock_alarm_handler * handler);

	/**
	 * This method modifies an existing scheduling policy for the current scheduler object.
	 *
	 * @param[in]  policy_id - The ID of the scheduling policy.
	 * @param[in]  policy_ptr - The pointer to the scheduling policy object.
	 * @return 0 on success, a not zero value otherwise.
	 */
	virtual int modify_timeplan (const std::string & id, clock_alarm_notifier * notifier, TSL_NS_HCF_CORE::clock_alarm_handler * handler);

	/**
	 * This method is called each time a timeplan must be removed from the clock notifier.
	 *
	 * @param[in]  id - The scheduling policy id.
	 * @param[in]  notifier - The notifier object from which the timeplan must be removed.
	 * @return 0 on success, a non zero value otherwise.
	 */
	virtual int remove_timeplan (const std::string & id, clock_alarm_notifier * notifier);

	/**
	 * This method returns the next scheduling occurrence for the current scheduling policy.
	 *
	 * @return 0 on success, a non zero value otherwise.
	 */
	virtual int get_next_scheduled_time (::tm & tm);

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
	 * This method checks if the provided value is a valid value for the dayOfMonth attribute.
	 * The value is valid if and only if is in the interval [0, 31] where 0 means all the months.
	 *
	 * @param[in]  value - The value to be checked.
	 * @return true/false.
	 */
	static inline bool is_valid_dayofmonth (const int32_t value) { return ((value >= 0) && (value <= 31)); }

	/**
	 * This method checks if the provided value is a valid value for the dayOfWeek attribute.
	 * The value is valid if and only if is in the interval [0, 7] where 0 means all the days, 1 means Monday and so on.
	 *
	 * @param[in]  value - The value to be checked.
	 * @return true/false.
	 */
	static inline bool is_valid_dayofweek (const dayofweek_t value) { return ((value >= 0) && (value <= 7)); }

	/**
	 * This method checks if the provided value is a valid value for the dayOfWeekOccurrence attribute.
	 * The value is valid if and only if is in the interval [0, 5] where 0 means all the occurrences, 1 means the first occurrence and so on.
	 *
	 * @param[in]  value - The value to be checked.
	 * @return true/false.
	 */
	static inline bool is_valid_dayofweek_occurrence (const dayofweek_occurrence_t value) { return ((value >= 0) && (value <= 5)); }

	/**
	 * This method checks if the provided value is a valid value for the month attribute.
	 * The value is valid if and only if is in the interval [0, 12] where 0 means all the months, 1 means January and so on.
	 *
	 * @param[in]  value - The value to be checked.
	 * @return true/false.
	 */
	static inline bool is_valid_month (const int32_t value) { return ((value >= 0) && (value <= 12)); }

private:
	/**
	 * This updates the value of the startTime attribute to the current time.
	 * That operation must be performed together with HcCalendarBasedPeriodicEvent instance creation.
	 *
	 * @param[in]  callback_info - Some useful info about the callback.
	 * @return 0 is returned on success, a negative value on failure.
	 */
	int execute_augmented_operations (TSL_NS_HCF_IMM::IMM_API_ObjectImplementer_CallbackInfo * callback_info);

	//==================//
	// Fields Modifiers //
	//==================//
public:
	inline int32_t get_dayofmonth () const { return _dayofmonth; }

	inline dayofweek_t get_dayofweek () const { return _dayofweek; }

	inline dayofweek_occurrence_t get_dayofweek_occurrence () const { return _dayofweek_occurrence; }

	inline int32_t get_month () const { return _month; }

	inline std::string get_start_time () const { return _start_time; }

	inline std::string get_stop_time () const { return _stop_time; }

	inline std::string get_time () const  { return _time; }

	//===========//
	// Operators //
	//===========//
private:
	__TSL_CLASS_NAME__ & operator=(const __TSL_CLASS_NAME__ & rhs);

	//========//
	// Fields //
	//========//
public:
	static const char * const HCF_HCCALENDAR_EVENT_CLASS_NAME;
	static const char * const HCF_HCCALENDAR_EVENT_IMPLEMENTER_NAME;
	static const char * const HCF_HCCALENDAR_EVENT_ID_ATTR_NAME;
	static const char * const HCF_HCCALENDAR_EVENT_DAY_OF_MONTH_ATTR_NAME;
	static const char * const HCF_HCCALENDAR_EVENT_DAY_OF_WEEK_ATTR_NAME;
	static const char * const HCF_HCCALENDAR_EVENT_DAY_OF_WEEK_OCCURR_ATTR_NAME;
	static const char * const HCF_HCCALENDAR_EVENT_MONTH_ATTR_NAME;
	static const char * const HCF_HCCALENDAR_EVENT_START_TIME_ATTR_NAME;
	static const char * const HCF_HCCALENDAR_EVENT_STOP_TIME_ATTR_NAME;
	static const char * const HCF_HCCALENDAR_EVENT_TIME_ATTR_NAME;
	static const char * const HCF_HCCALENDAR_EVENT_ID_SEPARATOR_VALUE;
	static const int32_t HCF_HCCALENDAR_EVENT_DAY_OF_MONTH_DEFAULT_VALUE;
	static const dayofweek_t HCF_HCCALENDAR_EVENT_DAY_OF_WEEK_DEFAULT_VALUE;
	static const dayofweek_occurrence_t HCF_HCCALENDAR_EVENT_DAY_OF_WEEK_OCCURR_DEFAULT_VALUE;
	static const int32_t HCF_HCCALENDAR_EVENT_MONTH_DEFAULT_VALUE;
	static const char * const HCF_HCCALENDAR_EVENT_STOP_TIME_DEFAULT_VALUE;

private:
	int32_t _dayofmonth;
	dayofweek_t _dayofweek;
	dayofweek_occurrence_t _dayofweek_occurrence;
	int32_t _month;
	std::string _start_time;
	std::string _stop_time;
	std::string _time;
	std::string _scheduler;
	TSL_NS_HCF_CORE::calendar_periodic_alarm_timeplan _timeplan;
	TSL_NS_HCF_CORE::thread_mutex_recursive _mutex;

	// Class members to be used to implement the rollback mechanism for the abort callback
	int32_t _dayofmonth_old;
	dayofweek_t _dayofweek_old;
	dayofweek_occurrence_t _dayofweek_occurrence_old;
	int32_t _month_old;
	std::string _start_time_old;
	std::string _stop_time_old;
	std::string _time_old;
	TSL_NS_HCF_CORE::calendar_periodic_alarm_timeplan _timeplan_old;
};

HCF_NS_HCF_SRV_END

#endif	// __INCLUDE_GUARD_CLASS_SRV_IMM_OBJECT_HCCALENDAR_EVENT__
