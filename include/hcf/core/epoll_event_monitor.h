#ifndef __INCLUDE_GUARD_CLASS_CORE_EPOLL_EVENT_MONITOR__
#define __INCLUDE_GUARD_CLASS_CORE_EPOLL_EVENT_MONITOR__ epoll_event_monitor

/*
 *	@file epoll_event_monitor.h
 *	@brief
 *	@author xnicmut (Nicola Muto)
 *	@date 2014-11-18
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
 *	| 2014-001 | 2014-11-18 | xnicmut      | Creation and first revision.        |
 *	+==========+============+==============+=====================================+
 */

#include <unistd.h>
#include <sys/epoll.h>

#include "macros.h"
#include "epoll_event_info.h"

TSL_NS_HCF_CORE_BEGIN

//===========//
// Constants //
//===========//
enum {
	TSL_EPOLL_CLOEXEC = EPOLL_CLOEXEC

#if defined(EPOLL_NONBLOCK)
	,	TSL_EPOLL_NONBLOCK = EPOLL_NONBLOCK
#endif
};

TSL_NS_HCF_CORE_END

namespace {
	template<bool have_the_same_size>
	inline int __wait_m (
			int epoll_fd,
			TSL_NS_HCF_CORE::epoll_event_info * events,
			size_t size,
			int timeout) {
		return ::epoll_wait(epoll_fd, events, size, timeout);
	}

	template<>
	inline int __wait_m<false> (
			int epoll_fd,
			TSL_NS_HCF_CORE::epoll_event_info * events,
			size_t size,
			int timeout) {
		::epoll_event _events [size];
		const int call_result = ::epoll_wait(epoll_fd, _events, size, timeout);
		if (call_result > 0) for (int i = 0; i < call_result; *events++ = _events[i++]) ;
		return call_result;
	}
}

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_CORE_EPOLL_EVENT_MONITOR__

TSL_NS_HCF_CORE_BEGIN

class __TSL_CLASS_NAME__ {
	//=========//
	// Friends //
	//=========//
	friend class epoll_event_reactor;

	//==============//
	// Constructors //
	//==============//
public:
	inline TSL_CTOR ()
	TSL_CTOR_INIT_LIST(_epoll_fd(-1)) {}

private:
	TSL_CTOR (const __TSL_CLASS_NAME__ &);

	//============//
	// Destructor //
	//============//
public:
	inline TSL_DTOR () { close(); }

	//===========//
	// Functions //
	//===========//
public:
	inline int init (int flags = 0) {
		const int epoll_fd_save = _epoll_fd;
		if ((_epoll_fd = ::epoll_create1(flags)) < 0) { // ERROR
			_epoll_fd = epoll_fd_save;
			return -1;
		}
		::close(epoll_fd_save);
		return 0;
	}

	inline bool initialized () const { return (_epoll_fd != -1); }

	inline int close () { return (::close(_epoll_fd) ? -1 : ((_epoll_fd = -1), 0)); }

	inline int add (int fd, const epoll_event_info & ev) { epoll_event_info ev_(ev); return add(fd, &ev_); }
	inline int add (int fd, epoll_event_info & ev) { return add(fd, &ev); }
	inline int add (int fd, epoll_event_info * ev) { return do_control(EPOLL_CTL_ADD, fd, ev); }

	inline int change (int fd, const epoll_event_info & ev) { epoll_event_info ev_(ev); return change(fd, &ev_); }
	inline int change (int fd, epoll_event_info & ev) { return change(fd, &ev); }
	inline int change (int fd, epoll_event_info * ev) { return do_control(EPOLL_CTL_MOD, fd, ev); }

	inline int remove (int fd) { return do_control(EPOLL_CTL_DEL, fd, reinterpret_cast<epoll_event *>(0xB0CACC10 ^ 0xA670CCE)); }

	inline int wait (epoll_event_info & event, int ms_timeout = -1) {
		return wait_single(&event, ms_timeout);
	}
	inline int wait (epoll_event_info * events, size_t size, int ms_timeout = -1) {
		return wait_multiple(events, size, ms_timeout);
	}
	template <size_t size>
	inline int wait (epoll_event_info (& events) [size], int ms_timeout = -1) {
		return wait_multiple(events, size, ms_timeout);
	}

private:
	inline int do_control (int op, int fd, ::epoll_event * epev) { return (::epoll_ctl(_epoll_fd, op, fd, epev) ? -1 : 0); }

	inline int wait_single (epoll_event_info * event, int ms_timeout) {
		return ::epoll_wait(_epoll_fd, event, 1, ms_timeout);
	}

	inline int wait_multiple (epoll_event_info * events, size_t size, int ms_timeout) {
		return __wait_m<sizeof(epoll_event_info) == sizeof(::epoll_event)>(_epoll_fd, events, size, ms_timeout);
	}

	inline int get_internal_handle () { return _epoll_fd; }

	//===========//
	// Operators //
	//===========//
private:
	__TSL_CLASS_NAME__ & operator= (const __TSL_CLASS_NAME__ &);

	//========//
	// Fields //
	//========//
private:
	int _epoll_fd;
};

TSL_NS_HCF_CORE_END

#endif /* __INCLUDE_GUARD_CLASS_CORE_EPOLL_EVENT_MONITOR__ */
