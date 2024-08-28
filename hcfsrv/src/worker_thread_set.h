#ifndef __INCLUDE_GUARD_CLASS_SRV_CLASS_WORKER_THREAD_SET__
#define __INCLUDE_GUARD_CLASS_SRV_CLASS_WORKER_THREAD_SET__ worker_thread_set

/*
 *	@file worker_thread_set.h
 *	@brief
 *	@author xcasale (Alessio Cascone)
 *	@date 2015-03-09
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
 *	| 2015-001 | 2015-03-09 | xcasale      | Creation and first revision.        |
 *	+==========+============+==============+=====================================+
 */

#include "hcf/core/thread_set.h"

#include "srv_macros.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_SRV_CLASS_WORKER_THREAD_SET__

HCF_NS_HCF_SRV_BEGIN

class __TSL_CLASS_NAME__ : public TSL_NS_HCF_CORE::thread_set {
	//==============//
	// Constructors //
	//==============//
public:
	inline TSL_CTOR ()
		TSL_CTOR_INIT_LIST(TSL_NS_HCF_CORE::thread_set()) {}

private:
	TSL_CTOR (const __TSL_CLASS_NAME__ & rhs);

	//============//
	// Destructor //
	//============//
public:
	inline virtual TSL_DTOR () {}

	//================================//
	// thread_set interface functions //
	//================================//
public:
	/**
	 * This method represents the main function to be executed by each thread into the thread set.
	 *
	 * @param[in] arg - The argument to be passed to each thread. Currently it is an instance of event_reactor class.
	 * @return 0 on success, -1 on failure.
	 */
	virtual int main (void * arg);

	//===========//
	// Operators //
	//===========//
private:
	__TSL_CLASS_NAME__ & operator=(const __TSL_CLASS_NAME__ & rhs);
};

HCF_NS_HCF_SRV_END

#endif /* __INCLUDE_GUARD_CLASS_SRV_CLASS_WORKER_THREAD_SET__ */
