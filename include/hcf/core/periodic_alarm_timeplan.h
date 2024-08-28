#ifndef __INCLUDE_GUARD_CLASS_CORE_PERIODIC_ALARM_TIMEPLAN__
#define __INCLUDE_GUARD_CLASS_CORE_PERIODIC_ALARM_TIMEPLAN__ periodic_alarm_timeplan

/*
 *	@file periodic_alarm_timeplan.h
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

#include <stdint.h>

#include "alarm_timeplan.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_CORE_PERIODIC_ALARM_TIMEPLAN__

TSL_NS_HCF_CORE_BEGIN

class __TSL_CLASS_NAME__ : public alarm_timeplan {
	//==============//
	// Constructors //
	//==============//
public:
	inline TSL_CTOR ()
	TSL_CTOR_INIT_LIST(alarm_timeplan(), _start_tm(), _end_tm(),
			_t_months(0), _t_weeks(0), _t_days(0), _t_hours(0), _t_minutes(0)) {
		_start_tm.tm_sec = 0; _start_tm.tm_min = 0; _start_tm.tm_hour = 0;
		_start_tm.tm_mday = 1; _start_tm.tm_mon = 0; _start_tm.tm_year = 0; _start_tm.tm_isdst = -1;

		_end_tm.tm_sec = 0; _end_tm.tm_min = 0; _end_tm.tm_hour = 0;
		_end_tm.tm_mday = 1; _end_tm.tm_mon = 0; _end_tm.tm_year = 0; _end_tm.tm_isdst = -1;
	}

	inline TSL_CTOR (const __TSL_CLASS_NAME__ & rhs)
	TSL_CTOR_INIT_LIST(alarm_timeplan(rhs), _start_tm(rhs._start_tm), _end_tm(rhs._end_tm), _t_months(rhs._t_months),
			_t_weeks(rhs._t_weeks), _t_days(rhs._t_days), _t_hours(rhs._t_hours), _t_minutes(rhs._t_minutes)) {}

	//============//
	// Destructor //
	//============//
public:
	inline virtual TSL_DTOR () {}

	//===========//
	// Functions //
	//===========//
public:
	inline int set_time_start (unsigned h, unsigned m, unsigned s) {
		_start_tm.tm_isdst = -1;
		_end_tm.tm_isdst = -1;
		::tm start_tm = _start_tm;
		if (const int call_result = set_time(start_tm, h, m, s)) return call_result;
		time_t start_time = ::mktime(&start_tm);
		time_t end_time = ::mktime(&_end_tm);
		if (start_time > end_time) return ERR_INVALID_OPERATION;
		_start_tm = start_tm;
		_start_tm.tm_isdst = -1;
		_end_tm.tm_isdst = -1;
		return ERR_NO_ERROR;
	}

	inline int set_date_start (unsigned y, unsigned m, unsigned d) {
		_start_tm.tm_isdst = -1;
		_end_tm.tm_isdst = -1;
		::tm start_tm = _start_tm;
		if (const int call_result = set_date(start_tm, y, m, d)) return call_result;
		time_t start_time = ::mktime(&start_tm);
		time_t end_time = ::mktime(&_end_tm);
		if (start_time > end_time) return ERR_INVALID_OPERATION;
		_start_tm = start_tm;
		_start_tm.tm_isdst = -1;
		_end_tm.tm_isdst = -1;
		return ERR_NO_ERROR;
	}

	inline int set_datetime_start (unsigned y, unsigned mo, unsigned d, unsigned h, unsigned mi, unsigned s) {
		_start_tm.tm_isdst = -1;
		_end_tm.tm_isdst = -1;
		::tm start_tm = _start_tm;
		if (const int call_result = set_datetime(start_tm, y, mo, d, h, mi, s)) return call_result;
		time_t start_time = ::mktime(&start_tm);
		time_t end_time = ::mktime(&_end_tm);
		if (start_time > end_time) return ERR_INVALID_OPERATION;
		_start_tm = start_tm;
		_start_tm.tm_isdst = -1;
		_end_tm.tm_isdst = -1;
		return ERR_NO_ERROR;
	}

	inline int set_time_end (unsigned h, unsigned m, unsigned s) {
		_start_tm.tm_isdst = -1;
		_end_tm.tm_isdst = -1;
		::tm end_tm = _end_tm;
		if (const int call_result = set_time(end_tm, h, m, s)) return call_result;
		time_t start_time = ::mktime(&_start_tm);
		time_t end_time = ::mktime(&end_tm);
		if (start_time > end_time) return ERR_INVALID_OPERATION;
		_end_tm = end_tm;
		_start_tm.tm_isdst = -1;
		_end_tm.tm_isdst = -1;
		return ERR_NO_ERROR;
	}

	inline int set_date_end (unsigned y, unsigned m, unsigned d) {
		_start_tm.tm_isdst = -1;
		_end_tm.tm_isdst = -1;
		::tm end_tm = _end_tm;
		if (const int call_result = set_date(end_tm, y, m, d)) return call_result;
		time_t start_time = ::mktime(&_start_tm);
		time_t end_time = ::mktime(&end_tm);
		if (start_time > end_time) return ERR_INVALID_OPERATION;
		_end_tm = end_tm;
		_start_tm.tm_isdst = -1;
		_end_tm.tm_isdst = -1;
		return ERR_NO_ERROR;
	}

	inline int set_datetime_end (unsigned y, unsigned mo, unsigned d, unsigned h, unsigned mi, unsigned s) {
		_start_tm.tm_isdst = -1;
		_end_tm.tm_isdst = -1;
		::tm end_tm = _end_tm;
		if (const int call_result = set_datetime(end_tm, y, mo, d, h, mi, s)) return call_result;
		time_t start_time = ::mktime(&_start_tm);
		time_t end_time = ::mktime(&end_tm);
		if (start_time > end_time) return ERR_INVALID_OPERATION;
		_end_tm = end_tm;
		_start_tm.tm_isdst = -1;
		_end_tm.tm_isdst = -1;
		return ERR_NO_ERROR;
	}

	inline uint32_t t_months () const { return _t_months; }
	inline uint32_t t_weeks () const { return _t_weeks; }
	inline uint32_t t_days () const { return _t_days; }
	inline uint32_t t_hours () const { return _t_hours; }
	inline uint32_t t_minutes () const { return _t_minutes; }

	inline void t_months (uint32_t new_value) { _t_months = new_value; }
	inline void t_weeks (uint32_t new_value) { _t_weeks = new_value; }
	inline void t_days (uint32_t new_value) { _t_days = new_value; }
	inline void t_hours (uint32_t new_value) { _t_hours = new_value; }
	inline void t_minutes (uint32_t new_value) { _t_minutes = new_value; }

	using alarm_timeplan::get_next_timer_point;
	virtual int get_next_timer_point (::timeval * relative_value, const ::timeval * tv_from);

protected:
	inline int set_time (::tm & tm_, unsigned h, unsigned m, unsigned s) {
		if (const int call_result = check_time(h, m, s)) return call_result;
		tm_.tm_hour = h; tm_.tm_min = m; tm_.tm_sec = s;
		return ERR_NO_ERROR;
	}

	inline int set_date (::tm & tm_, unsigned y, unsigned m, unsigned d) {
		if (const int call_result = check_date(y, m, d)) return call_result;
		tm_.tm_year = y - 1900; tm_.tm_mon = m - 1; tm_.tm_mday = d;
		return ERR_NO_ERROR;
	}

	inline int set_datetime (::tm & tm_, unsigned y, unsigned mo, unsigned d, unsigned h, unsigned mi, unsigned s) {
		int call_result = check_date(y, mo, d);
		if (call_result) return call_result;
		if ((call_result = check_time(h, mi, s))) return call_result;
		tm_.tm_year = y - 1900; tm_.tm_mon = mo - 1; tm_.tm_mday = d; tm_.tm_hour = h; tm_.tm_min = mi; tm_.tm_sec = s;
		return ERR_NO_ERROR;
	}

	//===========//
	// Operators //
	//===========//
public:
	inline __TSL_CLASS_NAME__ & operator= (const __TSL_CLASS_NAME__ & rhs) {
		if (this != &rhs) {
			alarm_timeplan::operator=(rhs);
			_start_tm = rhs._start_tm;
			_end_tm = rhs._end_tm;
			_t_months = rhs._t_months;
			_t_weeks = rhs._t_weeks;
			_t_days = rhs._t_days;
			_t_hours = rhs._t_hours;
			_t_minutes = rhs._t_minutes;
		}
		return *this;
	}

	//========//
	// Fields //
	//========//
private:
	::tm _start_tm;
	::tm _end_tm;

	uint32_t _t_months;
	uint32_t _t_weeks;
	uint32_t _t_days;
	uint32_t _t_hours;
	uint32_t _t_minutes;
};

TSL_NS_HCF_CORE_END

#endif /* __INCLUDE_GUARD_CLASS_CORE_PERIODIC_ALARM_TIMEPLAN__ */
