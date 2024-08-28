#ifndef __INCLUDE_GUARD_CLASS_CORE_EPOLL_EVENT_REACTOR__
#define __INCLUDE_GUARD_CLASS_CORE_EPOLL_EVENT_REACTOR__ epoll_event_reactor

/*
 *	@file epoll_event_reactor.h
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

#include <map>

#include "epoll_event_monitor.h"
#include "event_reactor_impl.h"

#include "hcf/trace/com_ericsson_common_hcf_core_epolleventreactor.h"
#include "hcf/trace/tracing_macros.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_CORE_EPOLL_EVENT_REACTOR__

TSL_NS_HCF_CORE_BEGIN

HCF_CC_FUNCTION_TRACER_CLASS_DEFINE

class __TSL_CLASS_NAME__ : public event_reactor_impl {
	//===========//
	// Constants //
	//===========//
private:
	enum {
		TSL_EPOLL_EVENT_READ_GROUP_MASK					=	EPOLLIN | EPOLLPRI | EPOLLRDNORM | EPOLLRDBAND
		, TSL_EPOLL_EVENT_WRITE_GROUP_MASK			=	EPOLLOUT | EPOLLWRNORM | EPOLLWRBAND
		, TSL_EPOLL_EVENT_EXCEPTION_GROUP_MASK	=	EPOLLERR | EPOLLHUP | EPOLLRDHUP
	};

	//==============//
	// Constructors //
	//==============//
public:
	inline TSL_CTOR () TSL_CTOR_INIT_LIST(event_reactor_impl(), _event_monitor(), _fd_handler_map()) {}

private:
	TSL_CTOR (const __TSL_CLASS_NAME__ &);

	//============//
	// Destructor //
	//============//
public:
	inline virtual TSL_DTOR () { close(); }

	//==========//
	// Typedefs //
	//==========//
private:
	typedef fd_handler_info<epoll_event_info> epoll_fd_handler_info_t;
	typedef std::map<int, epoll_fd_handler_info_t> fd_handler_map_t;

	//===========//
	// Functions //
	//===========//
public:
	virtual int open ();
	virtual int close ();

	virtual int run_event_loop ();
	virtual int end_event_loop ();

	virtual int remove_handler (int io_handle, uint32_t events_mask = 0);

protected:
	inline virtual int get_internal_handle () { return _event_monitor.get_internal_handle(); }

private:
	inline uint32_t map_to_epoll_events_mask (uint32_t events_mask) const {
		HCF_CC_TRACE_FUNCTION;
		return (((events_mask & TSL_EVENT_READ_MASK) ? static_cast<uint32_t>(TSL_EPOLL_EVENT_READ_GROUP_MASK) : 0) |
						((events_mask & TSL_EVENT_WRITE_MASK) ? static_cast<uint32_t>(TSL_EPOLL_EVENT_WRITE_GROUP_MASK) : 0) |
						((events_mask & TSL_EVENT_EXCEPTION_MASK) ? static_cast<uint32_t>(TSL_EPOLL_EVENT_EXCEPTION_GROUP_MASK) : 0) |
						EPOLLONESHOT
					 );
	}

	int do_event_loop ();

	int take_ownership_wait_call ();

	int handle_event (epoll_event_info & event_info);

	int event_handler_handle_input (epoll_fd_handler_info_t & fd_handler_info_ref);

	int event_handler_handle_output (epoll_fd_handler_info_t & fd_handler_info_ref);

	int event_handler_handle_exception (epoll_fd_handler_info_t & fd_handler_info_ref, bool error_flag, bool hungup_flag, bool read_hungup_flag);

	virtual int register_handler_ (int io_handle, event_handler * handler, uint32_t events_mask);

	inline int remove_handler_ (int io_handle, uint32_t events_mask) {
		fd_handler_map_t::iterator map_item_it = _fd_handler_map.find(io_handle);

		if (map_item_it == _fd_handler_map.end()) {
			// To be safely, try to remove the I/O event from the epoll interface internal repository
			_event_monitor.remove(io_handle);

			TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_NOT_FOUND);
		}

		return remove_handler_(map_item_it, events_mask);
	}

	int remove_handler_ (fd_handler_map_t::iterator & map_item_it, uint32_t events_mask);

	inline int init_event_monitor () {
		HCF_CC_TRACE_FUNCTION;
		if (_event_monitor.initialized()) return ERR_ALREADY_INITIALIZED;
		return ((_event_monitor.init() < 0) ? ERR_ON_INIT : 0);
	}

	int update_event_monitor ();

	//===========//
	// Operators //
	//===========//
private:
	__TSL_CLASS_NAME__ & operator= (const __TSL_CLASS_NAME__ &);

	//========//
	// Fields //
	//========//
private:
	epoll_event_monitor _event_monitor;

	fd_handler_map_t _fd_handler_map;
};

TSL_NS_HCF_CORE_END

#endif /* __INCLUDE_GUARD_CLASS_CORE_EPOLL_EVENT_REACTOR__ */
