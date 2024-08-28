/*
 *	@file com_ericsson_common_hcf_core_epolleventreactor.h
 *	@brief
 *	@author xcasale (Alessio Cascone)
 *	@date 2015-08-12
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
 *	| 2015-001 | 2015-08-12 | xcasale      | Creation and first revision.        |
 *	+==========+============+==============+=====================================+
 *
 */

#undef TRACEPOINT_PROVIDER
#define TRACEPOINT_PROVIDER com_ericsson_common_hcf_core_epolleventreactor

#undef TRACEPOINT_INCLUDE_FILE
#define TRACEPOINT_INCLUDE_FILE ./hcf/trace/com_ericsson_common_hcf_core_epolleventreactor.h

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(_COM_ERICSSON_COMMON_HCF_CORE_EPOLLEVENTREACTOR_H) || defined(TRACEPOINT_HEADER_MULTI_READ)
#define _COM_ERICSSON_COMMON_HCF_CORE_EPOLLEVENTREACTOR_H

#include <lttng/tracepoint.h>

TRACEPOINT_EVENT(com_ericsson_common_hcf_core_epolleventreactor, FunctionTracing,
		TP_ARGS(const char *, msg, const char *, function),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(FUNC, function)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_core_epolleventreactor, FunctionTracing, TRACE_INFO)

TRACEPOINT_EVENT(com_ericsson_common_hcf_core_epolleventreactor, Open,
		TP_ARGS(const char *, msg, int, state),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer_hex(int, STATE, state)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_core_epolleventreactor, Open, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_core_epolleventreactor, OpenErr,
		TP_ARGS(const char *, msg, int, err, int, e),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err) ctf_integer(int, ERRNO, e)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_core_epolleventreactor, OpenErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_core_epolleventreactor, Close,
		TP_ARGS(const char *, msg, int, state),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer_hex(int, STATE, state)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_core_epolleventreactor, Close, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_core_epolleventreactor, RunEventLoop,
		TP_ARGS(const char *, msg, int, state),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer_hex(int, STATE, state)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_core_epolleventreactor, RunEventLoop, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_core_epolleventreactor, RunEventLoopErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_core_epolleventreactor, RunEventLoopErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_core_epolleventreactor, EndEventLoop,
		TP_ARGS(const char *, msg, int, state),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer_hex(int, STATE, state)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_core_epolleventreactor, EndEventLoop, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_core_epolleventreactor, DoEventLoop,
		TP_ARGS(const char *, msg, int, state, int, ntreads),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer_hex(int, STATE, state) ctf_integer(int, NUM_THREADS, ntreads)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_core_epolleventreactor, DoEventLoop, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_core_epolleventreactor, TakeOwnershipWait,
		TP_ARGS(const char *, msg, int, state),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer_hex(int, STATE, state)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_core_epolleventreactor, TakeOwnershipWait, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_core_epolleventreactor, HandleEvent,
		TP_ARGS(const char *, msg, int, handle, int, revents, int, mask),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, HANDLE, handle) ctf_integer_hex(int, RECEIVED_EVENTS, revents) ctf_integer_hex(int, EVENTS_MASK, mask)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_core_epolleventreactor, HandleEvent, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_core_epolleventreactor, HandleEventErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_core_epolleventreactor, HandleEventErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_core_epolleventreactor, RegisterHandler,
		TP_ARGS(const char *, msg, int, handle, int, mask),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, HANDLE, handle) ctf_integer_hex(int, EVENTS_MASK, mask)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_core_epolleventreactor, RegisterHandler, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_core_epolleventreactor, RegisterHandlerErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_core_epolleventreactor, RegisterHandlerErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_core_epolleventreactor, RemoveHandler,
		TP_ARGS(const char *, msg, int, handle, int, mask),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, HANDLE, handle) ctf_integer_hex(int, EVENTS_MASK, mask)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_core_epolleventreactor, RemoveHandler, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_core_epolleventreactor, RemoveHandlerErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_core_epolleventreactor, RemoveHandlerErr, TRACE_ERR)

#endif /* _COM_ERICSSON_COMMON_HCF_CORE_EPOLLEVENTREACTOR_H */

/* This part must be outside protection */
#include <lttng/tracepoint-event.h>

#ifdef __cplusplus
}
#endif
