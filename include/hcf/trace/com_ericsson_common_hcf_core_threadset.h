/*
 *	@file com_ericsson_common_hcf_core_threadset.h
 *	@brief
 *	@author xcasale (Alessio Cascone)
 *	@date 2015-08-13
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
 *	| 2015-001 | 2015-08-13 | xcasale      | Creation and first revision.        |
 *	+==========+============+==============+=====================================+
 *
 */

#undef TRACEPOINT_PROVIDER
#define TRACEPOINT_PROVIDER com_ericsson_common_hcf_core_threadset

#undef TRACEPOINT_INCLUDE_FILE
#define TRACEPOINT_INCLUDE_FILE ./hcf/trace/com_ericsson_common_hcf_core_threadset.h

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(_COM_ERICSSON_COMMON_HCF_CORE_THREADSET_H) || defined(TRACEPOINT_HEADER_MULTI_READ)
#define _COM_ERICSSON_COMMON_HCF_CORE_THREADSET_H

#include <lttng/tracepoint.h>

TRACEPOINT_EVENT(com_ericsson_common_hcf_core_threadset, FunctionTracing,
		TP_ARGS(const char *, msg, const char *, function),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(FUNC, function)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_core_threadset, FunctionTracing, TRACE_INFO)

TRACEPOINT_EVENT(com_ericsson_common_hcf_core_threadset, Activate,
		TP_ARGS(const char *, msg, int, flags, int, n_threads),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer_hex(int, FLAGS, flags) ctf_integer(int, NUM_THREADS, n_threads)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_core_threadset, Activate, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_core_threadset, ActivateErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_core_threadset, ActivateErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_core_threadset, Join,
		TP_ARGS(const char *, msg, int, n_threads),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, NUM_THREADS, n_threads)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_core_threadset, Join, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_core_threadset, JoinErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_core_threadset, JoinErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_core_threadset, JoinThread,
		TP_ARGS(const char *, msg, unsigned long, id),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(unsigned long, THREAD_ID, id)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_core_threadset, JoinThread, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_core_threadset, JoinThreadErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_core_threadset, JoinThreadErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_core_threadset, ThreadDetachState,
		TP_ARGS(const char *, msg, unsigned long, id, int, state),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(unsigned long, THREAD_ID, id) ctf_integer(int, STATE, state)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_core_threadset, ThreadDetachState, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_core_threadset, ThreadDetachStateErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_core_threadset, ThreadDetachStateErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_core_threadset, StartRoutine,
		TP_ARGS(const char *, msg, unsigned long, id),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(unsigned long, THREAD_ID, id)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_core_threadset, StartRoutine, TRACE_NOTICE)

#endif /* _COM_ERICSSON_COMMON_HCF_CORE_THREADSET_H */

/* This part must be outside protection */
#include <lttng/tracepoint-event.h>

#ifdef __cplusplus
}
#endif
