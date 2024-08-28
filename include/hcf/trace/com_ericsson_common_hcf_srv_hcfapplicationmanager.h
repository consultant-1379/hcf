/*
 *	@file com_ericsson_common_hcf_srv_hcfapplicationmanager.h
 *	@brief
 *	@author xcasale (Alessio Cascone)
 *	@date 2015-10-20
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
 *	| 2015-001 | 2015-10-20 | xcasale      | Creation and first revision.        |
 *	+==========+============+==============+=====================================+
 *
 */

#undef TRACEPOINT_PROVIDER
#define TRACEPOINT_PROVIDER com_ericsson_common_hcf_srv_hcfapplicationmanager

#undef TRACEPOINT_INCLUDE_FILE
#define TRACEPOINT_INCLUDE_FILE ./hcf/trace/com_ericsson_common_hcf_srv_hcfapplicationmanager.h

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(_COM_ERICSSON_COMMON_HCF_SRV_HCFAPPLICATIONMANAGER_H) || defined(TRACEPOINT_HEADER_MULTI_READ)
#define _COM_ERICSSON_COMMON_HCF_SRV_HCFAPPLICATIONMANAGER_H

#include <lttng/tracepoint.h>

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcfapplicationmanager, FunctionTracing,
		TP_ARGS(const char *, msg, const char *, function),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(FUNC, function)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcfapplicationmanager, FunctionTracing, TRACE_INFO)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcfapplicationmanager, StateChange,
		TP_ARGS(const char *, msg, int, old_s, int, new_s),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, OLD_STATE, old_s) ctf_integer(int, NEW_STATE, new_s)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcfapplicationmanager, StateChange, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcfapplicationmanager, RestartApplication,
		TP_ARGS(const char *, msg),
		TP_FIELDS(ctf_string(MSG, msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcfapplicationmanager, RestartApplication, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcfapplicationmanager, ActivateDebugMode,
		TP_ARGS(const char *, msg),
		TP_FIELDS(ctf_string(MSG, msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcfapplicationmanager, ActivateDebugMode, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcfapplicationmanager, ActivateDebugModeErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcfapplicationmanager, ActivateDebugModeErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcfapplicationmanager, ActivateApplication,
		TP_ARGS(const char *, msg),
		TP_FIELDS(ctf_string(MSG, msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcfapplicationmanager, ActivateApplication, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcfapplicationmanager, ActivateApplicationErr,
		TP_ARGS(const char *, msg, int, err, const char *, err_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err) ctf_string(ERROR_MSG, err_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcfapplicationmanager, ActivateApplicationErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcfapplicationmanager, ShutdownApplication,
		TP_ARGS(const char *, msg),
		TP_FIELDS(ctf_string(MSG, msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcfapplicationmanager, ShutdownApplication, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcfapplicationmanager, ShutdownApplicationErr,
		TP_ARGS(const char *, msg, int, err, const char *, err_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err) ctf_string(ERROR_MSG, err_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcfapplicationmanager, ShutdownApplicationErr, TRACE_ERR)

#endif /* _COM_ERICSSON_COMMON_HCF_SRV_HCFAPPLICATIONMANAGER_H */

/* This part must be outside protection */
#include <lttng/tracepoint-event.h>

#ifdef __cplusplus
}
#endif
