#ifndef __INCLUDE_GUARD_CLASS_CORE_EVENT_REACTOR_IMPL__
#define __INCLUDE_GUARD_CLASS_CORE_EVENT_REACTOR_IMPL__ event_reactor_impl

/*
 *	@file event_reactor_impl.h
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

#include <stdint.h>
#include <poll.h>

#include "macros.h"
#include "error_constants.h"
#include "error_info_provider.h"
#include "event_handler.h"
#include "thread_mutex.h"
#include "thread_condition.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_CORE_EVENT_REACTOR_IMPL__

TSL_NS_HCF_CORE_BEGIN

class __TSL_CLASS_NAME__ : public error_info_provider {
	//=========//
	// Friends //
	//=========//
	friend class event_reactor;

	//===========//
	// Constants //
	//===========//
protected:
	enum {
		ERR_INT_NO_ERROR	=	0
		, ERR_INT_MAP_ITEM_NOT_FOUND
		, ERR_INT_EVENT_HANDLER_NULL_POINTER
	};

	enum event_reactor_state_t {
		TSL_EVENT_REACTOR_STATE_NULL											=	0x0000

		, TSL_EVENT_REACTOR_STATE_CLOSED		=	0x0001
		/* NOT USED FOR NOW */ , TSL_EVENT_REACTOR_STATE_OPENING	=	0x0002
		/* NOT USED FOR NOW */ , TSL_EVENT_REACTOR_STATE_OPENED		=	0x0004
		/* NOT USED FOR NOW */ , TSL_EVENT_REACTOR_STATE_CLOSING	=	0x0008

		, TSL_EVENT_REACTOR_STATE_NOT_LOOPING							=	0x0010
		/* NOT USED FOR NOW */ , TSL_EVENT_REACTOR_STATE_STARTING	=	0x0020
		, TSL_EVENT_REACTOR_STATE_LOOPING									=	0x0040
		, TSL_EVENT_REACTOR_STATE_STOPPING								=	0x0080

		, TSL_EVENT_REACTOR_STATE_UPDATING_EVENT_MONITOR	=	0x1000
		, TSL_EVENT_REACTOR_STATE_LEADERSHIP_OK						=	0x2000
	};

	enum {
		TSL_HANDLER_ACTION_NONE												=	0x0000
		, TSL_HANDLER_ACTION_TABLE_ADD								=	0x0001
		, TSL_HANDLER_ACTION_TABLE_CHANGE							=	0x0002

		, TSL_HANDLER_ACTION_TABLE_ADD_CHANGE_GROUP		=	TSL_HANDLER_ACTION_TABLE_ADD | TSL_HANDLER_ACTION_TABLE_CHANGE

		, TSL_HANDLER_ACTION_HANDLE_INPUT_ONGOING			=	0x0010
		, TSL_HANDLER_ACTION_HANDLE_OUTPUT_ONGOING		=	0x0020
		, TSL_HANDLER_ACTION_HANDLE_EXCEPTION_ONGOING	=	0x0040

		, TSL_HANDLER_ACTION_HANDLE_ONGOING_GROUP			=	TSL_HANDLER_ACTION_HANDLE_INPUT_ONGOING | TSL_HANDLER_ACTION_HANDLE_OUTPUT_ONGOING | TSL_HANDLER_ACTION_HANDLE_EXCEPTION_ONGOING
	};

	//==============//
	// Constructors //
	//==============//
public:
	inline TSL_CTOR ()
	TSL_CTOR_INIT_LIST(error_info_provider(), _state(TSL_EVENT_REACTOR_STATE_CLOSED), _access_sync(),
			_event_loop_mutex(_access_sync), _event_loop_condition(), _wakeup_handler(), _threads_looping_counter(0)) {
		_wakeup_handler.fd = -1;
		_wakeup_handler.events = POLLIN | POLLPRI;
		_wakeup_handler.revents = 0;
	}

private:
	TSL_CTOR (const __TSL_CLASS_NAME__ &);

	//============//
	// Destructor //
	//============//
public:
	inline virtual TSL_DTOR () {}

	//=======================//
	// Protected sub-classes //
	//=======================//
protected:
	template <typename event_info_t>
	class fd_handler_info {
	public:
		inline fd_handler_info ()
		: _event_handler(0), _event_info(), _events_mask(TSL_EVENT_NULL_MASK), _action_state(TSL_HANDLER_ACTION_NONE) {}

		event_handler * _event_handler;
		event_info_t _event_info;
		uint32_t _events_mask;
		uint32_t _action_state;
	};

	//==========//
	// Typedefs //
	//==========//
protected:
	typedef thread_mutex_recursive sync_t;

	//===========//
	// Functions //
	//===========//
public:
	inline uint32_t state () const { return _state; }

	virtual int open () = 0;
	virtual int close () = 0;

	virtual int run_event_loop () = 0;
	virtual int end_event_loop () = 0;

	inline int register_handler (event_handler * handler, uint32_t events_mask) {
		if (!handler) TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_NULL_ARGUMENT);
		return register_handler_C_1(handler->get_handle(), handler, events_mask);
	}

	inline int register_handler (int io_handle, event_handler * handler, uint32_t events_mask) {
		return register_handler_C_1_2(io_handle, handler, events_mask);
	}

	inline int remove_handler (event_handler * handler, uint32_t events_mask = 0) {
		if (!handler) TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_NULL_ARGUMENT);
		return remove_handler(handler->get_handle(), events_mask);
	}

	virtual int remove_handler (int io_handle, uint32_t events_mask = 0) = 0;

protected:
	inline int register_handler_C_1 (int io_handle, event_handler * handler, uint32_t events_mask) {
		if (io_handle < 0) TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_INVALID_HANDLE);
		return register_handler_(io_handle, handler, events_mask);
	}

	inline int register_handler_C_1_2 (int io_handle, event_handler * handler, uint32_t events_mask) {
		if (io_handle < 0) TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_INVALID_HANDLE);
		if (!handler) TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_NULL_ARGUMENT);
		return register_handler_(io_handle, handler, events_mask);
	}

	virtual int register_handler_ (int io_handle, event_handler * handler, uint32_t events_mask) = 0;

	inline void signal_to_wakeup () {
		const uint64_t wakeup_handle_inc = 1;
		const int tmp = ::write(_wakeup_handler.fd, &wakeup_handle_inc, sizeof(wakeup_handle_inc)); (void)tmp;
	}

	int is_wakeup_handler_signalled ();

	virtual int get_internal_handle () = 0;

	//===========//
	// Operators //
	//===========//
private:
	__TSL_CLASS_NAME__ & operator= (const __TSL_CLASS_NAME__ &);

	//========//
	// Fields //
	//========//
protected:
	uint32_t _state;
	sync_t _access_sync;

	sync_t & _event_loop_mutex;
	thread_condition _event_loop_condition;

	pollfd _wakeup_handler;

	int _threads_looping_counter;
};

TSL_NS_HCF_CORE_END

#endif /* __INCLUDE_GUARD_CLASS_CORE_EVENT_REACTOR_IMPL__ */
