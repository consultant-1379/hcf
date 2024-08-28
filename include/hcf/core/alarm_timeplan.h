#ifndef __INCLUDE_GUARD_CLASS_CORE_ALARM_TIMEPLAN__
#define __INCLUDE_GUARD_CLASS_CORE_ALARM_TIMEPLAN__ alarm_timeplan

/*
 *	@file alarm_timeplan.h
 *	@brief
 *	@author xnicmut (Nicola Muto)
 *	@date 2014-12-04
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
 *	| 2014-001 | 2014-12-04 | xnicmut      | Creation and first revision.        |
 *	+==========+============+==============+=====================================+
 */

#include <time.h>
#include <sys/time.h>

#include "macros.h"
#include "error_constants.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_CORE_ALARM_TIMEPLAN__

TSL_NS_HCF_CORE_BEGIN

class __TSL_CLASS_NAME__ {
	//==============//
	// Constructors //
	//==============//
public:
	inline TSL_CTOR () {}

	inline TSL_CTOR (const __TSL_CLASS_NAME__ & /*rhs*/) {}

	//============//
	// Destructor //
	//============//
public:
	inline virtual TSL_DTOR () {}

	//==================//
	// Static Functions //
	//==================//
public:
	static inline int check_time (unsigned h, unsigned m, unsigned s) {
		return (((h >= 24) || (m >= 60) || (s >= 60)) ? ERR_INVALID_ARGUMENT : ERR_NO_ERROR);
	}

	static inline int check_date (unsigned y, unsigned m, unsigned d) {
		//static const unsigned days_per_month [12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

		return (((m < 1) || (12 < m) || (d < 1) || ((days_per_month[m - 1] + ((m == 2) ? is_leap_year(y) : 0)) < d))
				? ERR_INVALID_ARGUMENT : ERR_NO_ERROR);
	}

	static inline int is_leap_year (unsigned year) {
		return (!(year % 400) || (!(year & 0x03) && (year % 100)) ? 1 : 0);
	}

	//===========//
	// Functions //
	//===========//
public:
	inline int get_next_timer_point (::timeval & relative_value, const ::timeval & tv_from) {
		return get_next_timer_point(&relative_value, &tv_from);
	}

	inline int get_next_timer_point (::timeval * relative_value, const ::timeval & tv_from) {
		return get_next_timer_point(relative_value, &tv_from);
	}

	inline int get_next_timer_point (::timeval & relative_value, const ::timeval * tv_from) {
		return get_next_timer_point(&relative_value, tv_from);
	}

	virtual int get_next_timer_point (::timeval * relative_value, const ::timeval * tv_from) = 0;

	inline int get_next_datetime (::tm & datetime) {
		return get_next_datetime(&datetime);
	}

	inline int get_next_datetime (::tm * datetime) {
		::timeval tv_now;
		if ((::gettimeofday(&tv_now, 0))) return TSL_NS_HCF_CORE::ERR_INTERNAL_ERROR;
		return get_next_datetime(datetime, tv_now);
	}

	inline int get_next_datetime (::tm & datetime, const ::timeval & tv_from) {
		return get_next_datetime(&datetime, &tv_from);
	}

	inline int get_next_datetime (::tm * datetime, const ::timeval & tv_from) {
		return get_next_datetime(datetime, &tv_from);
	}

	inline int get_next_datetime (::tm & datetime, const ::timeval * tv_from) {
		return get_next_datetime(&datetime, tv_from);
	}

	int get_next_datetime (::tm * tm_ptr, const ::timeval * tv_from);

	//===========//
	// Operators //
	//===========//
public:
	inline __TSL_CLASS_NAME__ & operator= (const __TSL_CLASS_NAME__ & /*rhs*/) {
		return *this;
	}

	//========//
	// Fields //
	//========//
protected:
	static const unsigned days_per_month [12];
};

TSL_NS_HCF_CORE_END

#endif /* __INCLUDE_GUARD_CLASS_CORE_ALARM_TIMEPLAN__ */
