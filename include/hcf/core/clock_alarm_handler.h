#ifndef __INCLUDE_GUARD_CLASS_CORE_CLOCK_ALARM_HANDLER__
#define __INCLUDE_GUARD_CLASS_CORE_CLOCK_ALARM_HANDLER__ clock_alarm_handler

/*
 *	@file clock_alarm_notifier.h
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

#include "macros.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_CORE_CLOCK_ALARM_HANDLER__

TSL_NS_HCF_CORE_BEGIN

class __TSL_CLASS_NAME__ {
	//==============//
	// Constructors //
	//==============//
public:
	inline TSL_CTOR () {}

	inline TSL_CTOR (const __TSL_CLASS_NAME__ & /*rhs*/) {}

	//============//
	// Destructor //
	//============//
public:
	inline virtual TSL_DTOR () {}

	//===========//
	// Functions //
	//===========//
public:
	virtual int handle_alarm (bool event_in_the_past) = 0;

	//===========//
	// Operators //
	//===========//
public:
	inline __TSL_CLASS_NAME__ & operator= (const __TSL_CLASS_NAME__ & /*rhs*/) {
		return *this;
	}

	//========//
	// Fields //
	//========//
private:
};

TSL_NS_HCF_CORE_END

#endif /* __INCLUDE_GUARD_CLASS_CORE_CLOCK_ALARM_HANDLER__ */
