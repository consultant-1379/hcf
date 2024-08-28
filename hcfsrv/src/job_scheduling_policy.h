#ifndef __INCLUDE_GUARD_CLASS_SRV_JOB_SCHEDULING_POLICY__
#define __INCLUDE_GUARD_CLASS_SRV_JOB_SCHEDULING_POLICY__ job_scheduling_policy

/**
 *  @file job_scheduling_policy.h
 *	@brief
 *	@author xcasale (Alessio Cascone)
 *	@date 2015-04-07
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
 *	| 2015-001 | 2015-04-07 | xcasale      | Creation and first revision.        |
 *	+==========+============+==============+=====================================+
 */

#include <string>

#include "hcf/core/clock_alarm_handler.h"

#include "srv_macros.h"
#include "clock_alarm_notifier.h"
#include "date_time.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_SRV_JOB_SCHEDULING_POLICY__

HCF_NS_HCF_SRV_BEGIN

class __TSL_CLASS_NAME__ {
	//==============//
	// Constructors //
	//==============//
public:
	inline TSL_CTOR ()
		TSL_CTOR_INIT_LIST(_timeplan_id()) {}

private:
	TSL_CTOR (const __TSL_CLASS_NAME__ & rhs);

	//============//
	// Destructor //
	//============//
public:
	virtual inline TSL_DTOR () {}

	//=================================//
	// job_scheduling_policy Interface //
	//=================================//
public:
	/**
	 * This method is called each time a new timeplan must be added to the clock notifier.
	 *
	 * @param[in]  id - The scheduling policy id.
	 * @param[in]  notifier - The notifier object to which the new timeplan must be added.
	 * @param[in]  handler - The handler object to be called when an event occurs.
	 * @return 0 on success, a non zero value otherwise.
	 */
	virtual int add_timeplan (const std::string & id, clock_alarm_notifier * notifier, TSL_NS_HCF_CORE::clock_alarm_handler * handler) = 0;

	/**
	 * This method is called each time a timeplan must be modified.
	 *
	 * @param[in]  id - The scheduling policy id.
	 * @param[in]  notifier - The notifier object for which the timeplan must be changed.
	 * @param[in]  handler - The handler object to be called when an event occurs.
	 * @return 0 on success, a non zero value otherwise.
	 */
	virtual int modify_timeplan (const std::string & id, clock_alarm_notifier * notifier, TSL_NS_HCF_CORE::clock_alarm_handler * handler) = 0;

	/**
	 * This method is called each time a timeplan must be removed from the clock notifier.
	 *
	 * @param[in]  id - The scheduling policy id.
	 * @param[in]  notifier - The notifier object from which the timeplan must be removed.
	 * @return 0 on success, a non zero value otherwise.
	 */
	virtual int remove_timeplan (const std::string & id, clock_alarm_notifier * notifier) = 0;

	/**
	 * This method returns the next scheduling occurrence for the current scheduling policy.
	 *
	 * @return 0 on success, a non zero value otherwise.
	 */
	virtual int get_next_scheduled_time (::tm & tm) = 0;

	//===========//
	// Functions //
	//===========//
public:
	/**
	 * This method converts a date_time object into a date string, following the ISO 8601 standard.
	 *
	 * @param[in]  d - The date_time object to be converted.
	 * @param[out] date - The converted string.
	 * @return 0 on success, a non zero value otherwise.
	 */
	static int convert_datetime_into_iso8601_format (const date_time & d, std::string & date);

	/**
	 * This method converts a date string that follows the ISO 8601 standard, into a date_time object.
	 *
	 * @param[in]  date - The string to be converted.
	 * @param[out] dt - The converted date_time object.
	 * @return 0 on success, a non zero value otherwise.
	 */
	static int convert_iso8601_format_into_datetime (const std::string & date, date_time & dt);

	/**
	 * This method converts a time string (having the format 'HH:MM:SS'), into a date_time object.
	 *
	 * @param[in]  time - The string to be converted.
	 * @param[out] dt - The converted date_time object.
	 * @return 0 on success, a non zero value otherwise.
	 */
	static int convert_time_string_into_datetime_format (const std::string & time, date_time & dt);

	//===========//
	// Operators //
	//===========//
private:
	__TSL_CLASS_NAME__ & operator=(const __TSL_CLASS_NAME__ & rhs);

	//========//
	// Fields //
	//========//
protected:
	std::string _timeplan_id;
};

HCF_NS_HCF_SRV_END

#endif	// __INCLUDE_GUARD_CLASS_SRV_JOB_SCHEDULING_POLICY__
