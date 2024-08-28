#ifndef __INCLUDE_GUARD_CLASS_CORE_EVENT_REACTOR__
#define __INCLUDE_GUARD_CLASS_CORE_EVENT_REACTOR__ event_reactor

/*
 *	@file event_reactor.h
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
#include "event_reactor_impl.h"
#include "epoll_event_reactor.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_CORE_EVENT_REACTOR__

TSL_NS_HCF_CORE_BEGIN

class __TSL_CLASS_NAME__ : public error_info_provider {
	//=========//
	// Friends //
	//=========//
	friend class event_processor;

	//==============//
	// Constructors //
	//==============//
public:
	inline TSL_CTOR (event_reactor_impl * implementation = 0, bool delete_implementation = false)
	TSL_CTOR_INIT_LIST(error_info_provider(), _eri(implementation), _delete_eri(delete_implementation)) {
		if (!_eri) _delete_eri = (_eri = new (std::nothrow) epoll_event_reactor);
	}

	inline TSL_CTOR (event_reactor_impl & implementation)
	TSL_CTOR_INIT_LIST(error_info_provider(), _eri(&implementation), _delete_eri(false)) {
	}

private:
	TSL_CTOR (const __TSL_CLASS_NAME__ &);

	//============//
	// Destructor //
	//============//
public:
	inline TSL_DTOR () { if (_delete_eri) delete _eri; }

	//===========//
	// Functions //
	//===========//
public:
	inline uint32_t state () {
		if (!_eri) TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_IMPLEMENTATION_NOT_FOUND);
		return _eri->state();
	}

	inline int open () {
		if (!_eri) TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_IMPLEMENTATION_NOT_FOUND);
		return _eri->open();
	}

	inline int close () {
		if (!_eri) TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_IMPLEMENTATION_NOT_FOUND);
		return _eri->close();
	}

	inline int run_event_loop () {
		if (!_eri) TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_IMPLEMENTATION_NOT_FOUND);
		return _eri->run_event_loop();
	}

	inline int end_event_loop () {
		if (!_eri) TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_IMPLEMENTATION_NOT_FOUND);
		return _eri->end_event_loop();
	}

	inline int register_handler (event_handler * handler, uint32_t events_mask) {
		if (!_eri) TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_IMPLEMENTATION_NOT_FOUND);
		return _eri->register_handler(handler, events_mask);
	}

	inline int register_handler (int io_handle, event_handler * handler, uint32_t events_mask) {
		if (!_eri) TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_IMPLEMENTATION_NOT_FOUND);
		return _eri->register_handler(io_handle, handler, events_mask);
	}

	inline int remove_handler (event_handler * handler, uint32_t events_mask = 0) {
		if (!_eri) TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_IMPLEMENTATION_NOT_FOUND);
		return _eri->remove_handler(handler, events_mask);
	}

	inline int remove_handler (int io_handle, uint32_t events_mask = 0) {
		if (!_eri) TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_IMPLEMENTATION_NOT_FOUND);
		return _eri->remove_handler(io_handle, events_mask);
	}

	inline int error () const {
		return (_eri ? _eri->error() : error_info_provider::error());
	}

	inline const char * error_text () const {
		return (_eri ? _eri->error_text() : error_info_provider::error_text());
	}

	inline int sys_error () const {
		return (_eri ? _eri->sys_error() : error_info_provider::sys_error());
	}

private:
	inline int get_internal_handle () {
		if (!_eri) TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_IMPLEMENTATION_NOT_FOUND);
		return _eri->get_internal_handle();
	}

	//===========//
	// Operators //
	//===========//
private:
	__TSL_CLASS_NAME__ & operator= (const __TSL_CLASS_NAME__ &);

	//========//
	// Fields //
	//========//
private:
	event_reactor_impl * _eri;
	bool _delete_eri;
};

TSL_NS_HCF_CORE_END

#endif /* __INCLUDE_GUARD_CLASS_CORE_EVENT_REACTOR__ */
