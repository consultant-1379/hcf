#ifndef __INCLUDE_GUARD_CLASS_CORE_THREAD_MUTEX__
#define __INCLUDE_GUARD_CLASS_CORE_THREAD_MUTEX__ thread_mutex

/*
 *	@file thread_mutex.h
 *	@brief
 *	@author xnicmut (Nicola Muto)
 *	@date 2014-11-20
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
 *	| 2014-001 | 2014-11-20 | xnicmut      | Creation and first revision.        |
 *	+==========+============+==============+=====================================+
 */

#include <errno.h>
#include <pthread.h>

#include "macros.h"
#include "thread_mutex_attr.h"

TSL_NS_HCF_CORE_BEGIN

template <TSL_NS_HCF_CORE::ThreadMutex_Types m_type>
struct __thread_mutex_static_initializer {
	static const ::pthread_mutex_t MUTEX_INIT_VALUE;
	enum { MUTEX_TYPE_INIT_VALUE = m_type };
};

template <>
const ::pthread_mutex_t
__thread_mutex_static_initializer<TSL_THREAD_MUTEX_FAST>::MUTEX_INIT_VALUE;

template <>
const ::pthread_mutex_t
__thread_mutex_static_initializer<TSL_THREAD_MUTEX_RECURSIVE>::MUTEX_INIT_VALUE;

template <>
const ::pthread_mutex_t
__thread_mutex_static_initializer<TSL_THREAD_MUTEX_ERRORCHECK>::MUTEX_INIT_VALUE;

template <>
const ::pthread_mutex_t
__thread_mutex_static_initializer<TSL_THREAD_MUTEX_ADAPTIVE>::MUTEX_INIT_VALUE;

TSL_NS_HCF_CORE_END

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ TSL_CONCAT_2(__INCLUDE_GUARD_CLASS_CORE_THREAD_MUTEX__, _base)

TSL_NS_HCF_CORE_BEGIN

class __TSL_CLASS_NAME__ {
	//=========//
	// Friends //
	//=========//
friend class thread_condition;

	//==============//
	// Constructors //
	//==============//
protected:
	inline TSL_CTOR (ThreadMutex_Types m_type = TSL_THREAD_MUTEX_FAST)
	TSL_CTOR_INIT_LIST(_pthread_mutex(), _mutex_type(m_type)) { init(m_type); }

	inline TSL_CTOR (const thread_mutex_attr & attr)
	TSL_CTOR_INIT_LIST(_pthread_mutex(), _mutex_type(attr.mutex_type())) { init(attr); }

	inline TSL_CTOR (const ::pthread_mutex_t & mutex, int mutex_type)
	TSL_CTOR_INIT_LIST(_pthread_mutex(mutex), _mutex_type(mutex_type)) {}

private:
	TSL_CTOR (const __TSL_CLASS_NAME__ &);

	//============//
	// Destructor //
	//============//
public:
	inline virtual TSL_DTOR () { destroy(); }

	//===========//
	// Functions //
	//===========//
public:
	inline int mutex_type () const { return _mutex_type; }

	inline int try_lock () { return (((errno = ::pthread_mutex_trylock(&_pthread_mutex))) ? -1 : 0); }

	inline int lock () { return (((errno = ::pthread_mutex_lock(&_pthread_mutex))) ? -1 : 0); }

#ifdef __USE_XOPEN2K
	inline int lock (const ::timespec * abstime) {
		return (((errno = ::pthread_mutex_timedlock(&_pthread_mutex, abstime))) ? -1 : 0);
	}
#endif

	inline int unlock () { return (((errno = ::pthread_mutex_unlock(&_pthread_mutex))) ? -1 : 0); }

#ifdef __USE_UNIX98
	inline int priority_ceiling () {
		int val = 0;
		const int errno_code = ::pthread_mutex_getprioceiling(&_pthread_mutex, &val);
		return (errno_code ? (errno = errno_code, -1) : val);
	}

	inline int priority_ceiling (int new_value, int & old_value) {
		const int errno_code = ::pthread_mutex_setprioceiling(&_pthread_mutex, new_value, &old_value);
		return (errno_code ? (errno = errno_code, -1) : 0);
	}
#endif

protected:
	inline int destroy () { return (((errno = ::pthread_mutex_destroy(&_pthread_mutex))) ? -1 : 0); }

	inline int init () {
		if ((errno = ::pthread_mutex_init(&_pthread_mutex, 0))) return -1;
		_mutex_type = TSL_THREAD_MUTEX_FAST;
		return 0;
	}

	inline int init (const thread_mutex_attr & attr) {
		if ((errno = ::pthread_mutex_init(&_pthread_mutex, &(attr._attr)))) return -1;
		_mutex_type = attr.mutex_type();
		return 0;
	}

	inline int init (ThreadMutex_Types m_type) { return init(thread_mutex_attr(m_type)); }

	//===========//
	// Operators //
	//===========//
private:
	__TSL_CLASS_NAME__ & operator= (const __TSL_CLASS_NAME__ &);

	//========//
	// Fields //
	//========//
private:
	::pthread_mutex_t _pthread_mutex;
	int _mutex_type;
};

TSL_NS_HCF_CORE_END

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ TSL_CONCAT_2(__INCLUDE_GUARD_CLASS_CORE_THREAD_MUTEX__, _traits)

TSL_NS_HCF_CORE_BEGIN

template <ThreadMutex_Types mtype>
class __TSL_CLASS_NAME__ : public TSL_CONCAT_2(__INCLUDE_GUARD_CLASS_CORE_THREAD_MUTEX__, _base) {
	//==============//
	// Constructors //
	//==============//
public:
	inline TSL_CTOR ()
	TSL_CTOR_INIT_LIST(TSL_CONCAT_2(__INCLUDE_GUARD_CLASS_CORE_THREAD_MUTEX__, _base)
			(__thread_mutex_static_initializer<mtype>::MUTEX_INIT_VALUE,
			 __thread_mutex_static_initializer<mtype>::MUTEX_TYPE_INIT_VALUE)) {}

private:
	TSL_CTOR (const __TSL_CLASS_NAME__ &);

	//============//
	// Destructor //
	//============//
public:
	inline virtual TSL_DTOR () {}

	//===========//
	// Operators //
	//===========//
private:
	__TSL_CLASS_NAME__ & operator= (const __TSL_CLASS_NAME__ &);
};

typedef __TSL_CLASS_NAME__<TSL_THREAD_MUTEX_FAST>				TSL_CONCAT_2(__INCLUDE_GUARD_CLASS_CORE_THREAD_MUTEX__, _fast);
typedef __TSL_CLASS_NAME__<TSL_THREAD_MUTEX_RECURSIVE>	TSL_CONCAT_2(__INCLUDE_GUARD_CLASS_CORE_THREAD_MUTEX__, _recursive);
typedef __TSL_CLASS_NAME__<TSL_THREAD_MUTEX_ERRORCHECK>	TSL_CONCAT_2(__INCLUDE_GUARD_CLASS_CORE_THREAD_MUTEX__, _errorcheck);
typedef __TSL_CLASS_NAME__<TSL_THREAD_MUTEX_ADAPTIVE>		TSL_CONCAT_2(__INCLUDE_GUARD_CLASS_CORE_THREAD_MUTEX__, _adaptive);

TSL_NS_HCF_CORE_END

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_CORE_THREAD_MUTEX__

TSL_NS_HCF_CORE_BEGIN

class __TSL_CLASS_NAME__ : public TSL_CONCAT_2(__INCLUDE_GUARD_CLASS_CORE_THREAD_MUTEX__, _base) {
	//==============//
	// Constructors //
	//==============//
public:
	inline TSL_CTOR (ThreadMutex_Types m_type = TSL_THREAD_MUTEX_FAST)
	TSL_CTOR_INIT_LIST(TSL_CONCAT_2(__INCLUDE_GUARD_CLASS_CORE_THREAD_MUTEX__, _base)(m_type)) {}

	inline TSL_CTOR (const thread_mutex_attr & attr)
	TSL_CTOR_INIT_LIST(TSL_CONCAT_2(__INCLUDE_GUARD_CLASS_CORE_THREAD_MUTEX__, _base)(attr)) {}

private:
	TSL_CTOR (const __TSL_CLASS_NAME__ &);

	//============//
	// Destructor //
	//============//
public:
	inline virtual TSL_DTOR () {}

	//===========//
	// Functions //
	//===========//
public:
	inline int init () {
		return TSL_CONCAT_2(__INCLUDE_GUARD_CLASS_CORE_THREAD_MUTEX__, _base)::init();
	}

	inline int init (const thread_mutex_attr & attr) {
		return TSL_CONCAT_2(__INCLUDE_GUARD_CLASS_CORE_THREAD_MUTEX__, _base)::init(attr);
	}

	inline int init (ThreadMutex_Types m_type) { return init(thread_mutex_attr(m_type)); }

	inline int destroy () {
		return TSL_CONCAT_2(__INCLUDE_GUARD_CLASS_CORE_THREAD_MUTEX__, _base)::destroy();
	}
};

TSL_NS_HCF_CORE_END

#endif /* __INCLUDE_GUARD_CLASS_CORE_THREAD_MUTEX__ */
