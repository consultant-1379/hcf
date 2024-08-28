#ifndef __INCLUDE_GUARD_CLASS_CORE_THREAD_MUTEX_ATTR__
#define __INCLUDE_GUARD_CLASS_CORE_THREAD_MUTEX_ATTR__ thread_mutex_attr

/*
 *	@file thread_mutex_attr.h
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

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_CORE_THREAD_MUTEX_ATTR__

//#if defined(TSL_USE_NAMESPACE_FOR_CONSTANTS)
TSL_NS_HCF_CORE_BEGIN
//#endif

//===========//
// Constants //
//===========//
//enum TSL_PREFIX_ENUM_NAME_IF_NO_NAMESPACE(ThreadMutex_Types) {
enum ThreadMutex_Types {
		TSL_THREAD_MUTEX_FAST =
#if defined(__USE_GNU)
			PTHREAD_MUTEX_FAST_NP
#elif defined(__USE_UNIX98) || defined(__USE_XOPEN2K8)
			PTHREAD_MUTEX_NORMAL
#else
			PTHREAD_MUTEX_TIMED_NP
#endif

	, TSL_THREAD_MUTEX_RECURSIVE =
#if defined(__USE_UNIX98) || defined(__USE_XOPEN2K8)
			PTHREAD_MUTEX_RECURSIVE
#else
			PTHREAD_MUTEX_RECURSIVE_NP
#endif

	, TSL_THREAD_MUTEX_ERRORCHECK =
#if defined(__USE_UNIX98) || defined(__USE_XOPEN2K8)
			PTHREAD_MUTEX_ERRORCHECK
#else
			PTHREAD_MUTEX_ERRORCHECK_NP
#endif

	, TSL_THREAD_MUTEX_ADAPTIVE = PTHREAD_MUTEX_ADAPTIVE_NP
};

enum ThreadMutex_PShared_Flags {
		TSL_THREAD_MUTEX_PROCESS_PRIVATE = PTHREAD_PROCESS_PRIVATE
	, TSL_THREAD_MUTEX_PROCESS_SHARED = PTHREAD_PROCESS_SHARED
};

enum ThreadMutex_Protocols {
		TSL_THREAD_MUTEX_PROTOCOL_PRIO_NONE = PTHREAD_PRIO_NONE
	, TSL_THREAD_MUTEX_PROTOCOL_PRIO_INHERIT = PTHREAD_PRIO_INHERIT
	, TSL_THREAD_MUTEX_PROTOCOL_PRIO_PROTECT = PTHREAD_PRIO_PROTECT
};

//#if defined(TSL_USE_NAMESPACE_FOR_CONSTANTS)
TSL_NS_HCF_CORE_END
//#endif

#if defined (TSL_THREAD_MUTEX_METHOD_FACADE_DEFAULT)
#	undef TSL_THREAD_MUTEX_METHOD_FACADE_DEFAULT
#endif
#define TSL_THREAD_MUTEX_METHOD_FACADE_DEFAULT(mn, syscall) \
		int mn () { \
			const int errno_code = syscall(&_attr); \
			return (errno_code ? (errno = errno_code, -1) : 0); \
		}

#if defined (TSL_THREAD_MUTEX_METHOD_FACADE_GETTER)
#	undef TSL_THREAD_MUTEX_METHOD_FACADE_GETTER
#endif
#define TSL_THREAD_MUTEX_METHOD_FACADE_GETTER(mn, syscall) \
		int mn () const { \
			int tmp = 0; \
			const int errno_code = syscall(&_attr, &tmp); \
			return (errno_code ? (errno = errno_code, -1) : tmp); \
		}

#if defined (TSL_THREAD_MUTEX_METHOD_FACADE_SETTER)
#	undef TSL_THREAD_MUTEX_METHOD_FACADE_SETTER
#endif
#define TSL_THREAD_MUTEX_METHOD_FACADE_SETTER(mn, syscall, type_val) \
		int mn (type_val val) { \
			const int errno_code = syscall(&_attr, val); \
			return (errno_code ? (errno = errno_code, -1) : 0); \
		}

#if defined (TSL_THREAD_MUTEX_METHOD_FACADE_ACCESSORS)
#	undef TSL_THREAD_MUTEX_METHOD_FACADE_ACCESSORS
#endif
#define TSL_THREAD_MUTEX_METHOD_FACADE_ACCESSORS(mn, get_syscall, set_syscall, type_val) \
		TSL_THREAD_MUTEX_METHOD_FACADE_GETTER(mn, get_syscall); \
		TSL_THREAD_MUTEX_METHOD_FACADE_SETTER(mn, set_syscall, type_val)

#if defined (TSL_THREAD_MUTEX_METHOD_FACADE_ACCESSORS_INLINE)
#	undef TSL_THREAD_MUTEX_METHOD_FACADE_ACCESSORS_INLINE
#endif
#define TSL_THREAD_MUTEX_METHOD_FACADE_ACCESSORS_INLINE(mn, get_syscall, set_syscall, type_val) \
		inline TSL_THREAD_MUTEX_METHOD_FACADE_GETTER(mn, get_syscall); \
		inline TSL_THREAD_MUTEX_METHOD_FACADE_SETTER(mn, set_syscall, type_val)

TSL_NS_HCF_CORE_BEGIN

class __TSL_CLASS_NAME__ {
	//=========//
	// Friends //
	//=========//
friend class thread_mutex_base;

	//==============//
	// Constructors //
	//==============//
public:
	inline TSL_CTOR () TSL_CTOR_INIT_LIST(_attr()) { init(); }

	inline TSL_CTOR (ThreadMutex_Types m_type) TSL_CTOR_INIT_LIST(_attr()) { init(m_type); }

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
	inline TSL_THREAD_MUTEX_METHOD_FACADE_DEFAULT(init, ::pthread_mutexattr_init);

	int init (ThreadMutex_Types m_type) {
		return (((errno = ::pthread_mutexattr_init(&_attr))) ? -1 : mutex_type(m_type));
	}

	inline TSL_THREAD_MUTEX_METHOD_FACADE_DEFAULT(destroy, ::pthread_mutexattr_destroy);

	TSL_THREAD_MUTEX_METHOD_FACADE_ACCESSORS_INLINE(
			pshared_flag,
			::pthread_mutexattr_getpshared,
			::pthread_mutexattr_setpshared,
			ThreadMutex_PShared_Flags);

	TSL_THREAD_MUTEX_METHOD_FACADE_ACCESSORS_INLINE(
			mutex_type,
			::pthread_mutexattr_gettype,
			::pthread_mutexattr_settype,
			ThreadMutex_Types);

	TSL_THREAD_MUTEX_METHOD_FACADE_ACCESSORS_INLINE(
			protocol,
			::pthread_mutexattr_getprotocol,
			::pthread_mutexattr_setprotocol,
			ThreadMutex_Protocols);

	TSL_THREAD_MUTEX_METHOD_FACADE_ACCESSORS_INLINE(
			priority_ceiling,
			::pthread_mutexattr_getprioceiling,
			::pthread_mutexattr_setprioceiling,
			int);

#if defined(__USE_XOPEN2K)
	TSL_THREAD_MUTEX_METHOD_FACADE_ACCESSORS_INLINE(
			robustness,
			::pthread_mutexattr_getrobust,
			::pthread_mutexattr_setrobust,
			int);
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
	::pthread_mutexattr_t _attr;
};

TSL_NS_HCF_CORE_END

#endif /* __INCLUDE_GUARD_CLASS_CORE_THREAD_MUTEX_ATTR__ */
