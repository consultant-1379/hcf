#ifndef __INCLUDE_GUARD_CLASS_SRV_CLOCK_ALARM_NOTIFIER__
#define __INCLUDE_GUARD_CLASS_SRV_CLOCK_ALARM_NOTIFIER__ clock_alarm_notifier

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

#include <time.h>

#include <string>
#include <map>
#include <vector>

#include "hcf/core/error_constants.h"
#include "hcf/core/error_info_provider.h"
#include "hcf/core/event_handler.h"
#include "hcf/core/oneshot_alarm_timeplan.h"
#include "hcf/core/clock_alarm_handler.h"
#include "hcf/core/thread_mutex.h"

#include "srv_macros.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_SRV_CLOCK_ALARM_NOTIFIER__

HCF_NS_HCF_SRV_BEGIN

//===========//
// Constants //
//===========//
enum {
	TSL_CLOCK_POLICY_NULL_MASK		=	0x0000

	, TSL_CLOCK_POLICY_REALTIME		=	0x0000
	, TSL_CLOCK_POLICY_MONOTONIC	=	0x0001
};

HCF_NS_HCF_SRV_END

HCF_NS_HCF_SRV_BEGIN

class __TSL_CLASS_NAME__ : public TSL_NS_HCF_CORE::event_handler, public TSL_NS_HCF_CORE::error_info_provider {
	//==============//
	// Constructors //
	//==============//
public:
	inline TSL_CTOR ()
		TSL_CTOR_INIT_LIST(TSL_NS_HCF_CORE::event_handler(), TSL_NS_HCF_CORE::error_info_provider(), _timer_fd(-1),
			_timeplan_handler_map(), _sync(), _atp_alarmed(), _handle_input_ongoing(false), _armed_timeval())
	{}

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
	int open (unsigned flags = TSL_CLOCK_POLICY_REALTIME);

//	int start ();

//	int stop ();

	int close ();

	template <typename timeplan_t>
	inline int add_timeplan (const std::string & id, const timeplan_t & atp, TSL_NS_HCF_CORE::clock_alarm_handler * handler) {
		if (timeplan_t * tp = new (std::nothrow) timeplan_t(atp)) {
			const int call_result = add_timeplan_(id, tp, handler);
			if (call_result) delete tp;
			return call_result;
		}
		TSL_SET_ERROR_INFO_AND_RETURN(-1, TSL_NS_HCF_CORE::ERR_NO_RESOURCES_AVAILABLE, "No memory available");
	}

	template <typename timeplan_t>
	inline int change_timeplan (const std::string & id, const timeplan_t & atp, TSL_NS_HCF_CORE::clock_alarm_handler * handler) {
		int call_result = remove_timeplan(id);
		if (call_result) return call_result;
		if ((call_result = add_timeplan(id, atp, handler))) return call_result;
		return TSL_NS_HCF_CORE::ERR_NO_ERROR;
	}

	int remove_timeplan (const std::string & id);

	//=========================//
	// Event_handler interface //
	//===================================================================//
	inline virtual int get_handle () { return _timer_fd; }

	virtual int handle_input ();

	inline virtual int handle_output () { return 0; }

	inline virtual int handle_exception (
			bool TSL_UNUSED_PAR(error_flag),
			bool TSL_UNUSED_PAR(hungup_flag),
			bool TSL_UNUSED_PAR(read_hungup_flag)) { return -1; }

	inline virtual int handle_close () { close(); return -1; }
	//===================================================================//
	// Event_handler interface //
	//=========================//

private:
	int add_timeplan_ (
			const std::string & id,
			TSL_NS_HCF_CORE::alarm_timeplan * atp,
			TSL_NS_HCF_CORE::clock_alarm_handler * handler);

	int activate_timer_for_next_alarms (const ::timeval & tv_from);

	//===========//
	// Operators //
	//===========//
private:
	__TSL_CLASS_NAME__ & operator= (const __TSL_CLASS_NAME__ &);

	//=============//
	// Sub-classes //
	//=============//
private:
	class atp_handler_pair {
	public:
		inline atp_handler_pair () : _atp_ptr(0), _cah_ptr(0) {}
		inline atp_handler_pair (TSL_NS_HCF_CORE::alarm_timeplan * atp_ptr, TSL_NS_HCF_CORE::clock_alarm_handler * cah_ptr)
		: _atp_ptr(atp_ptr), _cah_ptr(cah_ptr) {}
		inline atp_handler_pair (TSL_NS_HCF_CORE::alarm_timeplan & atp, TSL_NS_HCF_CORE::clock_alarm_handler & cah)
		: _atp_ptr(&atp), _cah_ptr(&cah) {}
		inline atp_handler_pair (const atp_handler_pair & rhs)
		: _atp_ptr(rhs._atp_ptr), _cah_ptr(rhs._cah_ptr) {}

		inline ~atp_handler_pair () {}

		inline atp_handler_pair & operator= (const atp_handler_pair & rhs) {
			if (this != &rhs) { _atp_ptr = rhs._atp_ptr; _cah_ptr = rhs._cah_ptr; }
			return *this;
		}

		TSL_NS_HCF_CORE::alarm_timeplan * _atp_ptr;
		TSL_NS_HCF_CORE::clock_alarm_handler * _cah_ptr;
	};

	//========//
	// Fields //
	//========//
private:
	int _timer_fd;

	typedef std::map<std::string, atp_handler_pair> _timeplan_handler_map_t;
	_timeplan_handler_map_t _timeplan_handler_map;

	typedef TSL_NS_HCF_CORE::thread_mutex_recursive _sync_t;
	_sync_t _sync;

	std::vector<std::string> _atp_alarmed;

	bool _handle_input_ongoing;

	::timeval _armed_timeval;
};

HCF_NS_HCF_SRV_END

#endif /* __INCLUDE_GUARD_CLASS_SRV_CLOCK_ALARM_NOTIFIER__ */
