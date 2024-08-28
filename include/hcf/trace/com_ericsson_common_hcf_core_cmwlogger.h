/*
 *	@file com_ericsson_common_hcf_core_cmwlogger.h
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
#define TRACEPOINT_PROVIDER com_ericsson_common_hcf_core_cmwlogger

#undef TRACEPOINT_INCLUDE_FILE
#define TRACEPOINT_INCLUDE_FILE ./hcf/trace/com_ericsson_common_hcf_core_cmwlogger.h

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(_COM_ERICSSON_COMMON_HCF_CORE_CMWLOGGER_H) || defined(TRACEPOINT_HEADER_MULTI_READ)
#define _COM_ERICSSON_COMMON_HCF_CORE_CMWLOGGER_H

#include <lttng/tracepoint.h>

TRACEPOINT_EVENT(com_ericsson_common_hcf_core_cmwlogger, FunctionTracing,
		TP_ARGS(const char *, msg, const char *, function),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(FUNC, function)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_core_cmwlogger, FunctionTracing, TRACE_DEBUG)	// WARNING (*)

TRACEPOINT_EVENT(com_ericsson_common_hcf_core_cmwlogger, Initialize,
		TP_ARGS(const char *, msg, const char *, app, const char *, str),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(APPLICATION, app) ctf_string(STREAM, str)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_core_cmwlogger, Initialize, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_core_cmwlogger, InitializeErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_core_cmwlogger, InitializeErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_core_cmwlogger, Finalize,
		TP_ARGS(const char *, msg, const char *, app),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(APPLICATION, app)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_core_cmwlogger, Finalize, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_core_cmwlogger, FinalizeErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_core_cmwlogger, FinalizeErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_core_cmwlogger, SendLogBuffer,
		TP_ARGS(const char *, msg, int, l, const char *, buf),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, LEVEL, l) ctf_string(BUFFER, buf)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_core_cmwlogger, SendLogBuffer, TRACE_DEBUG)	// WARNING (*)

TRACEPOINT_EVENT(com_ericsson_common_hcf_core_cmwlogger, SendLogBufferErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_core_cmwlogger, SendLogBufferErr, TRACE_ERR)

// (*)
// ! ! ! ! ! ! ! ! ! ! WARNING ! ! ! ! ! ! ! ! ! ! !
// XCASALE: Use for this event the lowest trace level in order to avoid to catch
//          the event if default tracing session settings (level == 12) are used!
// ! ! ! ! ! ! ! ! ! ! WARNING ! ! ! ! ! ! ! ! ! ! !
#endif /* _COM_ERICSSON_COMMON_HCF_CORE_CMWLOGGER_H */

/* This part must be outside protection */
#include <lttng/tracepoint-event.h>

#ifdef __cplusplus
}
#endif
