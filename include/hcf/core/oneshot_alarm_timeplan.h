#ifndef __INCLUDE_GUARD_CLASS_CORE_ONESHOT_ALARM_TIMEPLAN__
#define __INCLUDE_GUARD_CLASS_CORE_ONESHOT_ALARM_TIMEPLAN__ oneshot_alarm_timeplan

/*
 *	@file oneshot_alarm_timeplan.h
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

#include "alarm_timeplan.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_CORE_ONESHOT_ALARM_TIMEPLAN__

TSL_NS_HCF_CORE_BEGIN

class __TSL_CLASS_NAME__ : public alarm_timeplan {
	//==============//
	// Constructors //
	//==============//
public:
	inline TSL_CTOR ()
	TSL_CTOR_INIT_LIST(alarm_timeplan(), _tm()) {
		_tm.tm_sec = 0; _tm.tm_min = 0; _tm.tm_hour = 0; _tm.tm_mday = 1; _tm.tm_mon = 0; _tm.tm_year = 0; _tm.tm_isdst = -1;
	}

	inline TSL_CTOR (const __TSL_CLASS_NAME__ & rhs)
	TSL_CTOR_INIT_LIST(alarm_timeplan(), _tm(rhs._tm)) {}

	//============//
	// Destructor //
	//============//
public:
	inline virtual TSL_DTOR () {}

	//===========//
	// Functions //
	//===========//
public:
	inline unsigned sec () const { return _tm.tm_sec; }
	inline unsigned min () const { return _tm.tm_min; }
	inline unsigned hour () const { return _tm.tm_hour; }
	inline unsigned day () const { return _tm.tm_mday; }
	inline unsigned mon () const { return _tm.tm_mon + 1; }
	inline unsigned year () const { return _tm.tm_year + 1900; }

	inline int set_time (unsigned h, unsigned m, unsigned s) {
		if (const int call_result = check_time(h, m, s)) return call_result;
		_tm.tm_hour = h; _tm.tm_min = m; _tm.tm_sec = s;
		return ERR_NO_ERROR;
	}

	inline int set_date (unsigned y, unsigned m, unsigned d) {
		if (const int call_result = check_date(y, m, d)) return call_result;
		_tm.tm_year = y - 1900; _tm.tm_mon = m - 1; _tm.tm_mday = d;
		return ERR_NO_ERROR;
	}

	inline int set_datetime (unsigned y, unsigned mo, unsigned d, unsigned h, unsigned mi, unsigned s) {
		int call_result = check_date(y, mo, d);
		if (call_result) return call_result;
		if ((call_result = check_time(h, mi, s))) return call_result;
		_tm.tm_year = y - 1900; _tm.tm_mon = mo - 1; _tm.tm_mday = d; _tm.tm_hour = h; _tm.tm_min = mi; _tm.tm_sec = s;
		return ERR_NO_ERROR;
	}

	using alarm_timeplan::get_next_timer_point;
	virtual int get_next_timer_point (::timeval * relative_value, const ::timeval * tv_from);

	//===========//
	// Operators //
	//===========//
public:
	inline __TSL_CLASS_NAME__ & operator= (const __TSL_CLASS_NAME__ & rhs) {
		if (this != &rhs) {
			alarm_timeplan::operator=(rhs);
			_tm = rhs._tm;
		}
		return *this;
	}

	//========//
	// Fields //
	//========//
private:
	::tm _tm;
};

TSL_NS_HCF_CORE_END

#endif /* __INCLUDE_GUARD_CLASS_CORE_ONESHOT_ALARM_TIMEPLAN__ */
