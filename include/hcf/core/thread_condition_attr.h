#ifndef __INCLUDE_GUARD_CLASS_CORE_THREAD_CONDITION_ATTR__
#define __INCLUDE_GUARD_CLASS_CORE_THREAD_CONDITION_ATTR__ thread_condition_attr

/*
 *	@file thread_condition_attr.h
 *	@brief
 *	@author xnicmut (Nicola Muto)
 *	@date 2014-11-19
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
 *	| 2014-001 | 2014-11-19 | xnicmut      | Creation and first revision.        |
 *	+==========+============+==============+=====================================+
 */

#include <pthread.h>

#include "macros.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_CORE_THREAD_CONDITION_ATTR__

TSL_NS_HCF_CORE_BEGIN

class __TSL_CLASS_NAME__ {
	//=========//
	// Friends //
	//=========//
friend class thread_condition;

	//==============//
	// Constructors //
	//==============//
public:
	inline TSL_CTOR () TSL_CTOR_INIT_LIST(_attr()) { init(); }

private:
	TSL_CTOR (const __TSL_CLASS_NAME__ &);

	//============//
	// Destructor //
	//============//
public:
	inline TSL_DTOR () { destroy(); }

	//===========//
	// Functions //
	//===========//
public:
	int init () {
		const int errno_code = ::pthread_condattr_init(&_attr);
		return (errno_code ? (errno = errno_code, -1) : 0);
	}

	int destroy () {
		const int errno_code = ::pthread_condattr_destroy(&_attr);
		return (errno_code ? (errno = errno_code, -1) : 0);
	}

	int pshared_flag () const {
		int val = 0;
		const int errno_code = ::pthread_condattr_getpshared(&_attr, &val);
		return (errno_code ? (errno = errno_code, -1) : val);
	}

	int pshared_flag (int val) {
		const int errno_code = ::pthread_condattr_setpshared(&_attr, val);
		return (errno_code ? (errno = errno_code, -1) : 0);
	}

#ifdef __USE_XOPEN2K
	inline int get_clock (::clockid_t & clock_id) const {
		return (::pthread_condattr_getclock(&_attr, &clock_id) ? -1 : 0);
	}

	inline int set_clock (::clockid_t clock_id) {
		return (::pthread_condattr_setclock(&_attr, clock_id) ? -1 : 0);
	}
#endif

	//===========//
	// Operators //
	//===========//
private:
	__TSL_CLASS_NAME__ & operator= (const __TSL_CLASS_NAME__ &);

	//========//
	// Fields //
	//========//
private:
	::pthread_condattr_t _attr;
};

TSL_NS_HCF_CORE_END

#endif /* __INCLUDE_GUARD_CLASS_CORE_THREAD_CONDITION_ATTR__ */
