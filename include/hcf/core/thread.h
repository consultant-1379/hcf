#ifndef __INCLUDE_GUARD_CLASS_CORE_THREAD__
#define __INCLUDE_GUARD_CLASS_CORE_THREAD__ thread

/*
 *	@file thread.h
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

#include <stdint.h>
#include <sys/types.h>
#include <pthread.h>

#include "macros.h"
#include "error_info_provider.h"

TSL_NS_HCF_CORE_BEGIN

//===========//
// Constants //
//===========//
enum {
	TSL_THREAD_NULL_MASK		=	0x0000
	, TSL_THREAD_DETACHED	=	0x0001
};

TSL_NS_HCF_CORE_END

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_CORE_THREAD__

TSL_NS_HCF_CORE_BEGIN

class __TSL_CLASS_NAME__ : public error_info_provider {
	//==============//
	// Constructors //
	//==============//
public:
	inline TSL_CTOR ()
	TSL_CTOR_INIT_LIST(error_info_provider(), _id(0), _tid(0), _arg(0), _attr(TSL_THREAD_NULL_MASK)) {}

protected:
	inline TSL_CTOR (pthread_t id, pid_t tid, void * arg, uint32_t attr)
	TSL_CTOR_INIT_LIST(error_info_provider(), _id(id), _tid(tid), _arg(arg), _attr(attr)) {}

	inline TSL_CTOR (const __TSL_CLASS_NAME__ & rhs)
	TSL_CTOR_INIT_LIST(error_info_provider(rhs), _id(rhs._id), _tid(rhs._tid), _arg(rhs._arg), _attr(rhs._attr)) {}

	//============//
	// Destructor //
	//============//
public:
	inline virtual TSL_DTOR () {}

	//===========//
	// Functions //
	//===========//
public:
	inline pthread_t id () const { return _id; }

	inline pid_t tid () const { return _tid; }

	inline int activate () { return activate(TSL_THREAD_NULL_MASK, 0); }

	inline int activate (uint32_t flags) { return activate(flags, 0); }

	inline int activate (void * arg) { return activate(TSL_THREAD_NULL_MASK, arg); }

	int activate (uint32_t flags, void * arg);

	int detach ();

	int join (int & ret_val);

	int join (int & ret_val, const ::timespec & abstime);

	int try_join (int & ret_val);

protected:
	virtual int main (void * arg) = 0;

private:
	static void * start_routine (void * arg);

	//===========//
	// Operators //
	//===========//
protected:
	inline __TSL_CLASS_NAME__ & operator= (const __TSL_CLASS_NAME__ & rhs) {
		error_info_provider::operator=(rhs);
		if (this != &rhs) { _id = rhs._id; _tid = rhs._tid; _arg = rhs._arg; _attr = rhs._attr; }
		return *this;
	}

	//========//
	// Fields //
	//========//
private:
	pthread_t _id;
	pid_t _tid;
	void * _arg;
	uint32_t _attr;
};

TSL_NS_HCF_CORE_END

#endif /* __INCLUDE_GUARD_CLASS_CORE_THREAD__ */
