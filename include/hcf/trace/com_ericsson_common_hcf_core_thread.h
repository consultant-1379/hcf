/*
 *	@file com_ericsson_common_hcf_core_thread.h
 *	@brief
 *	@author xcasale (Alessio Cascone)
 *	@date 2015-08-14
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
 *	| 2015-001 | 2015-08-14 | xcasale      | Creation and first revision.        |
 *	+==========+============+==============+=====================================+
 *
 */

#undef TRACEPOINT_PROVIDER
#define TRACEPOINT_PROVIDER com_ericsson_common_hcf_core_thread

#undef TRACEPOINT_INCLUDE_FILE
#define TRACEPOINT_INCLUDE_FILE ./hcf/trace/com_ericsson_common_hcf_core_thread.h

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(_COM_ERICSSON_COMMON_HCF_CORE_THREAD_H) || defined(TRACEPOINT_HEADER_MULTI_READ)
#define _COM_ERICSSON_COMMON_HCF_CORE_THREAD_H

#include <lttng/tracepoint.h>

TRACEPOINT_EVENT(com_ericsson_common_hcf_core_thread, FunctionTracing,
		TP_ARGS(const char *, msg, const char *, function),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(FUNC, function)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_core_thread, FunctionTracing, TRACE_INFO)

TRACEPOINT_EVENT(com_ericsson_common_hcf_core_thread, Activate,
		TP_ARGS(const char *, msg, int, flags),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer_hex(int, FLAGS, flags)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_core_thread, Activate, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_core_thread, ActivateErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_core_thread, ActivateErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_core_thread, Detach,
		TP_ARGS(const char *, msg, unsigned long, id),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(unsigned long, THREAD_ID, id)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_core_thread, Detach, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_core_thread, DetachErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_core_thread, DetachErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_core_thread, Join,
		TP_ARGS(const char *, msg, unsigned long, id, int, retval, const char *, time),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(unsigned long, THREAD_ID, id) ctf_integer(int, RETURN_VALUE, retval) ctf_string(TIME, time)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_core_thread, Join, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_core_thread, JoinErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_core_thread, JoinErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_core_thread, TryJoin,
		TP_ARGS(const char *, msg, unsigned long, id, int, retval),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(unsigned long, THREAD_ID, id) ctf_integer(int, RETURN_VALUE, retval)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_core_thread, TryJoin, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_core_thread, TryJoinErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_core_thread, TryJoinErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_core_thread, StartRoutine,
		TP_ARGS(const char *, msg, unsigned long, id, int, retval),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(unsigned long, THREAD_ID, id) ctf_integer(int, RETURN_VALUE, retval)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_core_thread, StartRoutine, TRACE_NOTICE)

#endif /* _COM_ERICSSON_COMMON_HCF_CORE_THREAD_H */

/* This part must be outside protection */
#include <lttng/tracepoint-event.h>

#ifdef __cplusplus
}
#endif
