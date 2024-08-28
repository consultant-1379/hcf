/*
 *	@file com_ericsson_common_hcf_srv_clockalarmnotifier.h
 *	@brief
 *	@author xcasale (Alessio Cascone)
 *	@date 2015-10-08
 *
 *	COPYRIGHT Ericsson AB, 2015
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
 *	| 2015-001 | 2015-10-08 | xcasale      | Creation and first revision.        |
 *	+==========+============+==============+=====================================+
 *
 */

#undef TRACEPOINT_PROVIDER
#define TRACEPOINT_PROVIDER com_ericsson_common_hcf_srv_clockalarmnotifier

#undef TRACEPOINT_INCLUDE_FILE
#define TRACEPOINT_INCLUDE_FILE ./hcf/trace/com_ericsson_common_hcf_srv_clockalarmnotifier.h

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(_COM_ERICSSON_COMMON_HCF_SRV_CLOCKALARMNOTIFIER_H) || defined(TRACEPOINT_HEADER_MULTI_READ)
#define _COM_ERICSSON_COMMON_HCF_SRV_CLOCKALARMNOTIFIER_H

#include <lttng/tracepoint.h>

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_clockalarmnotifier, FunctionTracing,
		TP_ARGS(const char *, msg, const char *, function),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(FUNC, function)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_clockalarmnotifier, FunctionTracing, TRACE_INFO)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_clockalarmnotifier, Open,
		TP_ARGS(const char *, msg, unsigned, flag),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(unsigned, FLAGS, flag)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_clockalarmnotifier, Open, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_clockalarmnotifier, OpenErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_clockalarmnotifier, OpenErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_clockalarmnotifier, Close,
		TP_ARGS(const char *, msg),
		TP_FIELDS(ctf_string(MSG, msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_clockalarmnotifier, Close, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_clockalarmnotifier, CloseErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_clockalarmnotifier, CloseErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_clockalarmnotifier, RemoveTimeplan,
		TP_ARGS(const char *, msg, const char *, id),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(TIMEPLAN_ID, id)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_clockalarmnotifier, RemoveTimeplan, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_clockalarmnotifier, RemoveTimeplanErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_clockalarmnotifier, RemoveTimeplanErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_clockalarmnotifier, HandleInput,
		TP_ARGS(const char *, msg, const char *, t),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(TIME, t)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_clockalarmnotifier, HandleInput, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_clockalarmnotifier, HandleInputErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_clockalarmnotifier, HandleInputErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_clockalarmnotifier, AddTimeplan,
		TP_ARGS(const char *, msg, const char *, id),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(TIMEPLAN_ID, id)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_clockalarmnotifier, AddTimeplan, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_clockalarmnotifier, AddTimeplanErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_clockalarmnotifier, AddTimeplanErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_clockalarmnotifier, ActivateTimerForNextAlarms,
		TP_ARGS(const char *, msg, const char *, v),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(VALUE, v)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_clockalarmnotifier, ActivateTimerForNextAlarms, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_clockalarmnotifier, ActivateTimerForNextAlarmsErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_clockalarmnotifier, ActivateTimerForNextAlarmsErr, TRACE_ERR)

#endif /* _COM_ERICSSON_COMMON_HCF_SRV_CLOCKALARMNOTIFIER_H */

/* This part must be outside protection */
#include <lttng/tracepoint-event.h>

#ifdef __cplusplus
}
#endif
