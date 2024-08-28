#ifndef __INCLUDE_GUARD_CLASS_THREAD_MUTEX_TEST__
#define __INCLUDE_GUARD_CLASS_THREAD_MUTEX_TEST__ thread_mutex_test

/*
 *	@file thread_mutex_test.h
 *	@brief
 *	@author xnicmut (Nicola Muto)
 *	@date 2014-11-11
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
 *	| 2014-001 | 2014-11-11 | xnicmut      | Creation and first revision.        |
 *	+==========+============+==============+=====================================+
 */

#include "hcf/core/thread_mutex.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_THREAD_MUTEX_TEST__

class __TSL_CLASS_NAME__ {
	//==============//
	// Constructors //
	//==============//
public:
	inline TSL_CTOR () {}

private:
	TSL_CTOR (const __TSL_CLASS_NAME__ &);

	//============//
	// Destructor //
	//============//
public:
	inline TSL_DTOR () {}

	//===========//
	// Functions //
	//===========//
public:

	//===========//
	// Operators //
	//===========//
private:
	int operator() ();

	__TSL_CLASS_NAME__ & operator= (const __TSL_CLASS_NAME__ &);

	//========//
	// Fields //
	//========//
private:
};

#endif /* __INCLUDE_GUARD_CLASS_THREAD_MUTEX_TEST__ */
