#ifndef __INCLUDE_GUARD_CLASS_SRV_DATE_TIME__
#define __INCLUDE_GUARD_CLASS_SRV_DATE_TIME__ date_time

/**
 *  @file date_time.h
 *	@brief
 *	@author xcasale (Alessio Cascone)
 *	@date 2015-04-08
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
 *	| 2015-001 | 2015-04-08 | xcasale      | Creation and first revision.        |
 *	+==========+============+==============+=====================================+
 */

#include <time.h>

#include "hcf/core/alarm_timeplan.h"

#include "srv_macros.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_SRV_DATE_TIME__

HCF_NS_HCF_SRV_BEGIN

class __TSL_CLASS_NAME__ {
	//==============//
	// Constructors //
	//==============//
public:
	inline TSL_CTOR  ()
		TSL_CTOR_INIT_LIST(_year(1970), _month(1), _day(1), _hour(0), _min(0), _sec(0)) {}

	inline TSL_CTOR (unsigned y, unsigned mo, unsigned d, unsigned h, unsigned mi, unsigned s)
		TSL_CTOR_INIT_LIST(_year(y), _month(mo), _day(d), _hour(h), _min(mi), _sec(s)) {}

	inline TSL_CTOR (const __TSL_CLASS_NAME__ & rhs)
		TSL_CTOR_INIT_LIST(_year(rhs._year), _month(rhs._month), _day(rhs._day),
				_hour(rhs._hour), _min(rhs._min), _sec(rhs._sec)) {}

	inline TSL_CTOR (const ::tm & t)
		TSL_CTOR_INIT_LIST(_year(t.tm_year + 1900), _month(t.tm_mon + 1),
				_day(t.tm_mday), _hour(t.tm_hour), _min(t.tm_min), _sec(t.tm_sec)) {}

	//============//
	// Destructor //
	//============//
public:
	virtual inline TSL_DTOR () {}

	//===========//
	// Functions //
	//===========//
public:
	static inline bool is_valid (const __TSL_CLASS_NAME__ & d) { return !((check_date(d._year, d._month, d._day)) || (check_time(d._hour, d._min, d._sec))); }
	static inline int check_date (unsigned y, unsigned m, unsigned d) { return TSL_NS_HCF_CORE::alarm_timeplan::check_date(y, m, d); }
	static inline int check_time (unsigned h, unsigned m, unsigned s) { return TSL_NS_HCF_CORE::alarm_timeplan::check_time(h, m, s); }
	static inline int is_leap_year (unsigned y) { return TSL_NS_HCF_CORE::alarm_timeplan::is_leap_year(y); }
	static inline __TSL_CLASS_NAME__ now () { const time_t t = ::time(0); return __TSL_CLASS_NAME__(*(::localtime(&t))); }

	//============//
	// Predicates //
	//============//
public:
	inline bool less_than (const __TSL_CLASS_NAME__ & rhs) const {
		if (_year  != rhs._year)	return (_year  < rhs._year);
		if (_month != rhs._month)	return (_month < rhs._month);
		if (_day   != rhs._day)		return (_day   < rhs._day);
		if (_hour  != rhs._hour)	return (_hour  < rhs._hour);
		if (_min   != rhs._min)		return (_min   < rhs._min);
		return (_sec < rhs._sec);
	}
	inline bool equal_to (const __TSL_CLASS_NAME__ & rhs) const {
		return ((_year == rhs._year) && (_month == rhs._month) && (_day == rhs._day) &&
				(_hour == rhs._hour) && (_min == rhs._min) && (_sec == rhs._sec));
	}

	//===========//
	// Operators //
	//===========//
public:
	inline __TSL_CLASS_NAME__ & operator= (const __TSL_CLASS_NAME__ & rhs) {
		(&rhs != this) && (_year = rhs._year, _month = rhs._month, _day = rhs._day,
				_hour = rhs._hour, _min = rhs._min, _sec = rhs._sec);
		return *this;
	}
	inline __TSL_CLASS_NAME__ & operator= (const ::tm & t) {
 		_year = t.tm_year + 1900; _month = t.tm_mon + 1; _day = t.tm_mday;
 		_hour = t.tm_hour; _min = t.tm_min; _sec = t.tm_sec;
		return *this;
	}

	//================//
	// Setter methods //
	//================//
public:
	int set_date (unsigned y, unsigned m, unsigned d);
	int set_time (unsigned h, unsigned m, unsigned s);
	int set_date_time (unsigned y, unsigned mo, unsigned d, unsigned h, unsigned mi, unsigned s);

	//================//
	// Getter methods //
	//================//
public:
	inline unsigned year () const { return _year; }
	inline unsigned month () const { return _month; }
	inline unsigned day () const { return _day; }
	inline unsigned hour () const { return _hour; }
	inline unsigned min () const { return _min; }
	inline unsigned sec () const { return _sec; }

	//========//
	// Fields //
	//========//
public:
	static __TSL_CLASS_NAME__ DATE_MAX;

private:
	unsigned _year  : 16;
	unsigned _month : 4;
	unsigned _day   : 5;
	unsigned _hour  : 5;
	unsigned _min   : 6;
	unsigned _sec   : 6;
};


//===========================//
// date_time class operators //
//===========================//
inline bool operator< (const __TSL_CLASS_NAME__ & lhs, const __TSL_CLASS_NAME__ & rhs) { return lhs.less_than(rhs); }
inline bool operator> (const __TSL_CLASS_NAME__ & lhs, const __TSL_CLASS_NAME__ & rhs) { return rhs.less_than(lhs); }
inline bool operator== (const __TSL_CLASS_NAME__ & lhs, const __TSL_CLASS_NAME__ & rhs) { return lhs.equal_to(rhs); }
inline bool operator!= (const __TSL_CLASS_NAME__ & lhs, const __TSL_CLASS_NAME__ & rhs) { return !(lhs == rhs); }
inline bool operator<= (const __TSL_CLASS_NAME__ & lhs, const __TSL_CLASS_NAME__ & rhs) { return !(rhs < lhs); }
inline bool operator>= (const __TSL_CLASS_NAME__ & lhs, const __TSL_CLASS_NAME__ & rhs) { return !(lhs < rhs); }

HCF_NS_HCF_SRV_END

#endif	// __INCLUDE_GUARD_CLASS_SRV_DATE_TIME__
