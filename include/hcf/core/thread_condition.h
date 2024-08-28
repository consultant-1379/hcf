#ifndef __INCLUDE_GUARD_CLASS_CORE_THREAD_CONDITION__
#define __INCLUDE_GUARD_CLASS_CORE_THREAD_CONDITION__ thread_condition

/*
 *	@file thread_condition.h
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

#include <errno.h>
#include <pthread.h>

#include "macros.h"
#include "thread_mutex.h"
#include "thread_condition_attr.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_CORE_THREAD_CONDITION__

TSL_NS_HCF_CORE_BEGIN

const ::pthread_cond_t __TSL_PTHREAD_CONDITION_INITIALIZER = PTHREAD_COND_INITIALIZER;

class __TSL_CLASS_NAME__ {
	//==============//
	// Constructors //
	//==============//
public:
	inline TSL_CTOR ()
	TSL_CTOR_INIT_LIST(_ptrhead_cond(__TSL_PTHREAD_CONDITION_INITIALIZER)) {}

	inline TSL_CTOR (const thread_condition_attr & attr)
	TSL_CTOR_INIT_LIST(_ptrhead_cond()) { init(attr); }

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
	inline int init () { return (((errno = ::pthread_cond_init(&_ptrhead_cond, 0))) ? -1 : 0); }

	inline int init (const thread_condition_attr & attr) {
		return (((errno = ::pthread_cond_init(&_ptrhead_cond, &attr._attr))) ? -1 : 0);
	}

	inline int destroy () { return (((errno = ::pthread_cond_destroy(&_ptrhead_cond))) ? -1 : 0); }

	inline int signal () { return (((errno = ::pthread_cond_signal(&_ptrhead_cond))) ? -1 : 0); }

	inline int broadcast () { return (((errno = ::pthread_cond_broadcast(&_ptrhead_cond))) ? -1 : 0); }

/*
	template <ThreadMutex_Types mtype>
	inline int wait (thread_mutex_base<mtype> & mutex, const timespec * abstime = 0) {
		return
			((errno = (abstime ? (::pthread_cond_timedwait(&_ptrhead_cond, &mutex._pthread_mutex, abstime))
												 : (::pthread_cond_wait(&_ptrhead_cond, &mutex._pthread_mutex))))
			? -1 : 0);
	}
*/

	inline int wait (thread_mutex_base & mutex, const ::timespec * abstime = 0) {
		return
			(((errno = (abstime ? (::pthread_cond_timedwait(&_ptrhead_cond, &mutex._pthread_mutex, abstime))
												 : (::pthread_cond_wait(&_ptrhead_cond, &mutex._pthread_mutex)))))
			? -1 : 0);
	}

	//===========//
	// Operators //
	//===========//
private:
	__TSL_CLASS_NAME__ & operator= (const __TSL_CLASS_NAME__ &);

	//========//
	// Fields //
	//========//
private:
	::pthread_cond_t _ptrhead_cond;
};

TSL_NS_HCF_CORE_END

#endif /* __INCLUDE_GUARD_CLASS_CORE_THREAD_CONDITION__ */
