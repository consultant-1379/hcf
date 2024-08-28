#ifndef __INCLUDE_GUARD_CLASS_TEST_01_SIGNAL_FD_EVENT_HANDLER__
#define __INCLUDE_GUARD_CLASS_TEST_01_SIGNAL_FD_EVENT_HANDLER__ signal_fd_event_handler

/*
 *	@file signal_fd_event_handler.h
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

#include <unistd.h>
#include <sys/signalfd.h>
#include <signal.h>

#include "hcf/core/event_handler.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_TEST_01_SIGNAL_FD_EVENT_HANDLER__

class __TSL_CLASS_NAME__ : public TSL_NS_HCF_CORE::event_handler {
	//==============//
	// Constructors //
	//==============//
public:
	inline TSL_CTOR ()
	TSL_CTOR_INIT_LIST(event_handler(), _sigfd(-1), _sigset()) {
		::sigemptyset(&_sigset);
	}

private:
	TSL_CTOR (const __TSL_CLASS_NAME__ &);

	//============//
	// Destructor //
	//============//
public:
	inline virtual TSL_DTOR () { close(); }

	//===========//
	// Functions //
	//===========//
public:
	int add (int signum, bool block_sig = true);

	inline int close () { ::close(_sigfd); _sigfd = -1; ::sigemptyset(&_sigset); return 0; }

	//==============================//
	// event_handler implementation //
	//==============================//
public:







	//===========//
	// Operators //
	//===========//
private:
	__TSL_CLASS_NAME__ & operator= (const __TSL_CLASS_NAME__ &);

	//========//
	// Fields //
	//========//
private:
	int _sigfd;
	::sigset_t _sigset;
};

#endif /* __INCLUDE_GUARD_CLASS_TEST_01_SIGNAL_FD_EVENT_HANDLER__ */
