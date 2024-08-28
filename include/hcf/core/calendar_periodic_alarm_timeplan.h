#ifndef __INCLUDE_GUARD_CLASS_CORE_CALENDAR_PERIODIC_ALARM_TIMEPLAN__
#define __INCLUDE_GUARD_CLASS_CORE_CALENDAR_PERIODIC_ALARM_TIMEPLAN__ calendar_periodic_alarm_timeplan

/*
 *	@file calendar_periodic_alarm_timeplan.h
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
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_CORE_CALENDAR_PERIODIC_ALARM_TIMEPLAN__

TSL_NS_HCF_CORE_BEGIN

class __TSL_CLASS_NAME__ : public alarm_timeplan {
	//==============//
	// Constructors //
	//==============//
public:
	inline TSL_CTOR ()
	TSL_CTOR_INIT_LIST(alarm_timeplan(), _start_tm(), _end_tm(),
			_t_time_hour(0), _t_time_min(0), _t_time_sec(0), _t_month(0), _t_dayofmonth(0), _t_dayofweek(0),
			_t_dayofweek_occurrence(0)) {
		_start_tm.tm_sec = 0; _start_tm.tm_min = 0; _start_tm.tm_hour = 0;
		_start_tm.tm_mday = 1; _start_tm.tm_mon = 0; _start_tm.tm_year = 0; _start_tm.tm_isdst = -1;

		_end_tm.tm_sec = 0; _end_tm.tm_min = 0; _end_tm.tm_hour = 0;
		_end_tm.tm_mday = 1; _end_tm.tm_mon = 0; _end_tm.tm_year = 0; _end_tm.tm_isdst = -1;
	}

	inline TSL_CTOR (const __TSL_CLASS_NAME__ & rhs)
	TSL_CTOR_INIT_LIST(alarm_timeplan(rhs), _start_tm(rhs._start_tm), _end_tm(rhs._end_tm),
			_t_time_hour(rhs._t_time_hour), _t_time_min(rhs._t_time_min), _t_time_sec(rhs._t_time_sec),
			_t_month(rhs._t_month), _t_dayofmonth(rhs._t_dayofmonth), _t_dayofweek(rhs._t_dayofweek),
			_t_dayofweek_occurrence(rhs._t_dayofweek_occurrence))
		{}

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

	inline int t_time (unsigned h, unsigned m, unsigned s) {
		if (const int call_result = check_time(h, m, s)) return call_result;
		_t_time_hour = h; _t_time_min = m; _t_time_sec = s;
		return ERR_NO_ERROR;
	}
	inline unsigned t_time_hour () const { return _t_time_hour; }
	inline unsigned t_time_min () const { return _t_time_min; }
	inline unsigned t_time_sec () const { return _t_time_sec; }

	inline uint32_t t_month () const { return _t_month; }
	inline uint32_t t_dayofmonth () const { return _t_dayofmonth; }
	inline uint32_t t_dayofweek () const { return _t_dayofweek; }
	inline uint32_t t_dayofweek_occurrence () const { return _t_dayofweek_occurrence; }

	inline void t_month (uint32_t new_value) { if (new_value <= 12) _t_month = new_value; }
	inline void t_dayofmonth (uint32_t new_value) { if (new_value <= 31) _t_dayofmonth = new_value; }
	inline void t_dayofweek(uint32_t new_value) { if (new_value <= 7) _t_dayofweek = new_value; }
	inline void t_dayofweek_occurrence(uint32_t new_value) { if (new_value <= 5) _t_dayofweek_occurrence = new_value; }

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

			_t_time_hour = rhs._t_time_hour;
			_t_time_min = rhs._t_time_min;
			_t_time_sec = rhs._t_time_sec;

			_t_month = rhs._t_month;
			_t_dayofmonth = rhs._t_dayofmonth;
			_t_dayofweek = rhs._t_dayofweek;
			_t_dayofweek_occurrence = rhs._t_dayofweek_occurrence;
		}
		return *this;
	}

	//========//
	// Fields //
	//========//
private:
	::tm _start_tm;
	::tm _end_tm;

	unsigned _t_time_hour : 5;
	unsigned _t_time_min : 6;
	unsigned _t_time_sec : 6;

	uint32_t _t_month; // [0, 12]: 0 == ALL
	uint32_t _t_dayofmonth; // [0, 31]: 0 == ALL
	uint32_t _t_dayofweek;	// 0 == ALL, 1 == MONDAY, 2 == TUESDAY, 3 == WEDNESDAY, 4 == THURSDAY,
													// 5 == FRIDAY, 6 == SATURDAY, 7 == SUNDAY
	uint32_t _t_dayofweek_occurrence; // 0 == ALL, 1 == FIRST, 2 == SECOND, 3 == THIRD, 4 == FOURTH, 5 == LAST
};

TSL_NS_HCF_CORE_END
#endif /* __INCLUDE_GUARD_CLASS_CORE_CALENDAR_PERIODIC_ALARM_TIMEPLAN__ */
