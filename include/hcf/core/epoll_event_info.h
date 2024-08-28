#ifndef __INCLUDE_GUARD_CLASS_CORE_EPOLL_EVENT_INFO__
#define __INCLUDE_GUARD_CLASS_CORE_EPOLL_EVENT_INFO__ epoll_event_info

/*
 *	@file epoll_event_info.h
 *	@brief
 *	@author xnicmut (Nicola Muto)
 *	@date 2014-12-02
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
 *	| 2014-001 | 2014-12-02 | xnicmut      | Creation and first revision.        |
 *	+==========+============+==============+=====================================+
 */

#include <sys/epoll.h>

#include "macros.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_CORE_EPOLL_EVENT_INFO__

//#if defined(TSL_USE_NAMESPACE_FOR_CONSTANTS)
TSL_NS_HCF_CORE_BEGIN
//#endif

//===========//
// Constants //
//===========//
//enum TSL_PREFIX_ENUM_NAME_IF_NO_NAMESPACE(EpollEventTypes) {
enum Epoll_Events {
	TSL_EPOLLIN = EPOLLIN
	,	TSL_EPOLLPRI = EPOLLPRI
	,	TSL_EPOLLOUT = EPOLLOUT
	,	TSL_EPOLLRDNORM = EPOLLRDNORM
	, TSL_EPOLLRDBAND = EPOLLRDBAND
	,	TSL_EPOLLWRNORM = EPOLLWRNORM
	, TSL_EPOLLWRBAND = EPOLLWRBAND
	, TSL_EPOLLMSG = EPOLLMSG
	, TSL_EPOLLERR = EPOLLERR
	, TSL_EPOLLHUP = EPOLLHUP
	, TSL_EPOLLRDHUP = EPOLLRDHUP
	, TSL_EPOLLONESHOT = EPOLLONESHOT
	, TSL_EPOLLET = EPOLLET
};

//#if defined(TSL_USE_NAMESPACE_FOR_CONSTANTS)
TSL_NS_HCF_CORE_END
//#endif

TSL_NS_HCF_CORE_BEGIN

class __TSL_CLASS_NAME__ : public ::epoll_event {
	//==============//
	// Constructors //
	//==============//
public:
	inline TSL_CTOR ()
	TSL_CTOR_INIT_LIST(::epoll_event()) {}

	inline TSL_CTOR (const ::epoll_event & rhs)
	TSL_CTOR_INIT_LIST(::epoll_event(rhs)) {}

	inline TSL_CTOR (uint32_t events_mask, void * data_value)
	TSL_CTOR_INIT_LIST(::epoll_event()) {
		events = events_mask;
		data.ptr = data_value;
	}

	inline TSL_CTOR (uint32_t events_mask, int data_value)
	TSL_CTOR_INIT_LIST(::epoll_event()) {
		events = events_mask;
		data.fd = data_value;
	}

	inline TSL_CTOR (uint32_t events_mask, uint32_t data_value)
	TSL_CTOR_INIT_LIST(::epoll_event()) {
		events = events_mask;
		data.u32 = data_value;
	}

	inline TSL_CTOR (uint32_t events_mask, uint64_t data_value)
	TSL_CTOR_INIT_LIST(::epoll_event()) {
		events = events_mask;
		data.u64 = data_value;
	}

	//============//
	// Destructor //
	//============//
public:
	inline TSL_DTOR () {}
};

TSL_NS_HCF_CORE_END

#endif /* __INCLUDE_GUARD_CLASS_CORE_EPOLL_EVENT_INFO__ */
