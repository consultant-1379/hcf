#ifndef __INCLUDE_GUARD_CLASS_CORE_GUARD__
#define __INCLUDE_GUARD_CLASS_CORE_GUARD__ guard

/*
 *	@file guard.h
 *	@brief
 *	@author xnicmut (Nicola Muto)
 *	@date 2014-11-21
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
 *	| 2014-001 | 2014-11-21 | xnicmut      | Creation and first revision.        |
 *	+==========+============+==============+=====================================+
 */

#include "macros.h"

#if !defined (TSL_GUARD_MACROS)
#	define TSL_CRITICAL_SECTION_BEGIN(lock_t,lock) \
		do { TSL_NS_HCF_CORE::guard<lock_t> TSL_CONCAT_2(__execution_sentinel__,__LINE__)(lock)

#	define TSL_CRITICAL_SECTION_LOCKER_BEGIN(lock_t,lock) \
		do { TSL_NS_HCF_CORE::guard<lock_t,TSL_GUARD_STRATEGY_LOCK_UNLOCK> TSL_CONCAT_2(__execution_sentinel__,__LINE__)(lock)

#	define TSL_CRITICAL_SECTION_UNLOCKER_BEGIN(lock_t,lock) \
		do { TSL_NS_HCF_CORE::guard<lock_t,TSL_GUARD_STRATEGY_UNLOCK_LOCK> TSL_CONCAT_2(__execution_sentinel__,__LINE__)(lock)

#	define TSL_CRITICAL_SECTION_END } while (0)

#	define TSL_CRITICAL_SECTION_BREAK break
#endif

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_CORE_GUARD__

TSL_NS_HCF_CORE_BEGIN

enum guard_strategy_t {
	TSL_GUARD_STRATEGY_LOCK_UNLOCK
	, TSL_GUARD_STRATEGY_UNLOCK_LOCK
};

template <typename lock_t, guard_strategy_t guard_strategy = TSL_GUARD_STRATEGY_LOCK_UNLOCK>
class __TSL_CLASS_NAME__ {
	//==============//
	// Constructors //
	//==============//
public:
	inline TSL_CTOR (lock_t * l, bool do_lock = true)
	TSL_CTOR_INIT_LIST(_lock(l), _do_lock(do_lock)) {
		if (guard_strategy == TSL_GUARD_STRATEGY_LOCK_UNLOCK) do_lock ? lock() : try_lock();
		else unlock();
	}

	inline TSL_CTOR (lock_t & l, bool do_lock = true)
	TSL_CTOR_INIT_LIST(_lock(&l), _do_lock(do_lock)) {
		if (guard_strategy == TSL_GUARD_STRATEGY_LOCK_UNLOCK) do_lock ? lock() : try_lock();
		else unlock();
	}

private:
	TSL_CTOR (const __TSL_CLASS_NAME__ &);

	//============//
	// Destructor //
	//============//
public:
	inline TSL_DTOR () {
		if (guard_strategy == TSL_GUARD_STRATEGY_LOCK_UNLOCK) unlock();
		else _do_lock ? lock() : try_lock();
	}

	//===========//
	// Functions //
	//===========//
public:
	inline int lock () { return (_lock ? _lock->lock() : -1); }

	inline int try_lock () { return (_lock ? _lock->try_lock() : -1); }

	inline int unlock () { return (_lock ? _lock->unlock() : -1); }

	//===========//
	// Operators //
	//===========//
private:
	__TSL_CLASS_NAME__ & operator= (const __TSL_CLASS_NAME__ &);

	//========//
	// Fields //
	//========//
private:
	lock_t * _lock;
	bool _do_lock;
};

TSL_NS_HCF_CORE_END

#endif /* __INCLUDE_GUARD_CLASS_CORE_GUARD__ */
