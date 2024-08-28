#ifndef __INCLUDE_GUARD_CLASS_SRV_ROOT_WORKER_THREAD__
#define __INCLUDE_GUARD_CLASS_SRV_ROOT_WORKER_THREAD__ root_worker_thread

/*
 *	@file root_worker_thread.h
 *	@brief
 *	@author xcasale (Alessio Cascone)
 *	@date 2015-01-07
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
 *	| 2015-001 | 2015-01-07 | xcasale      | Creation and first revision.        |
 *	+==========+============+==============+=====================================+
 *	| 2015-002 | 2015-03-06 | xcasale      | Event handling adaptation.          |
 *	+==========+============+==============+=====================================+
 */

#include "hcf/core/thread.h"
#include "hcf/core/thread_condition.h"
#include "hcf/core/thread_mutex.h"

#include "srv_macros.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_SRV_ROOT_WORKER_THREAD__

HCF_NS_HCF_SRV_BEGIN

class __TSL_CLASS_NAME__ : public TSL_NS_HCF_CORE::thread {
	//==========//
	// Typedefs //
	//==========//
private:
	typedef TSL_NS_HCF_CORE::thread_mutex_recursive sync_t;

	//==============//
	// Constructors //
	//==============//
public:
	inline TSL_CTOR () TSL_CTOR_INIT_LIST(TSL_NS_HCF_CORE::thread(), _stop_condition_mutex(), _stop_condition(), _stop_signaled(false)) {}

private:
	TSL_CTOR (const __TSL_CLASS_NAME__ & rhs);

	//============//
	// Destructor //
	//============//
public:
	virtual inline TSL_DTOR () {}

	//==================//
	// Thread Interface //
	//==================//
public:
	/**
	 * The thread main function.
	 *
	 * @param[in] arg - The argument to be passed to the Root Worker Thread.
	 * @return 0 on success, a value different than 0 otherwise.
	 */
	virtual int main (void * arg);

	//===========//
	// Functions //
	//===========//
public:
	/**
	 * This method signals the stop to the current thread.
	 */
	inline void stop () {
		// Signal the stop condition and set to true the boolean flag
		_stop_signaled = true;
		_stop_condition.signal();
	}

	/**
	 * This method checks if the HCF server received a request to stop.
	 *
	 * @return 1 if the stop has been signaled, 0 otherwise.
	 */
	static inline int is_hcf_server_stopping () { return __sync_fetch_and_or(&_hcf_server_stopping, 0); }

	//===========//
	// Operators //
	//===========//
private:
	__TSL_CLASS_NAME__ & operator=(const __TSL_CLASS_NAME__ & rhs);

	//========//
	// Fields //
	//========//
private:
	static int _hcf_server_stopping;

	TSL_NS_HCF_CORE::thread_mutex_recursive _stop_condition_mutex;
	TSL_NS_HCF_CORE::thread_condition _stop_condition;
	bool _stop_signaled;
};

HCF_NS_HCF_SRV_END

#endif /* __INCLUDE_GUARD_CLASS_SRV_ROOT_WORKER_THREAD__ */
