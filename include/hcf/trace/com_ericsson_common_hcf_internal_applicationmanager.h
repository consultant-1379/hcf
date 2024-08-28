/*
 *	@file com_ericsson_common_hcf_internal_applicationmanager.h
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
#define TRACEPOINT_PROVIDER com_ericsson_common_hcf_internal_applicationmanager

#undef TRACEPOINT_INCLUDE_FILE
#define TRACEPOINT_INCLUDE_FILE ./hcf/trace/com_ericsson_common_hcf_internal_applicationmanager.h

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(_COM_ERICSSON_COMMON_HCF_INTERNAL_APPLICATIONMANAGER_H) || defined(TRACEPOINT_HEADER_MULTI_READ)
#define _COM_ERICSSON_COMMON_HCF_INTERNAL_APPLICATIONMANAGER_H

#include <lttng/tracepoint.h>

TRACEPOINT_EVENT(com_ericsson_common_hcf_internal_applicationmanager, FunctionTracing,
		TP_ARGS(const char *, msg, const char *, function),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(FUNC, function)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_internal_applicationmanager, FunctionTracing, TRACE_INFO)

TRACEPOINT_EVENT(com_ericsson_common_hcf_internal_applicationmanager, ActivateHAMode,
		TP_ARGS(const char *, msg, const char *, daemon),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(DAEMON_NAME, daemon)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_internal_applicationmanager, ActivateHAMode, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_internal_applicationmanager, ActivateHAModeErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_internal_applicationmanager, ActivateHAModeErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_internal_applicationmanager, ComponentError,
		TP_ARGS(const char *, msg, int, act),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, RECOVERY_ACTION, act)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_internal_applicationmanager, ComponentError, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_internal_applicationmanager, ComponentErrorErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_internal_applicationmanager, ComponentErrorErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_internal_applicationmanager, Initialize,
		TP_ARGS(const char *, msg, const char *, val),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(VALUE, val)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_internal_applicationmanager, Initialize, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_internal_applicationmanager, InitializeErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_internal_applicationmanager, InitializeErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_internal_applicationmanager, Finalize,
		TP_ARGS(const char *, msg, const char *, daemon),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(DAEMON_NAME, daemon)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_internal_applicationmanager, Finalize, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_internal_applicationmanager, FinalizeErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_internal_applicationmanager, FinalizeErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_internal_applicationmanager, Dispatch,
		TP_ARGS(const char *, msg, int, flag),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, FLAGS, flag)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_internal_applicationmanager, Dispatch, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_internal_applicationmanager, DispatchErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_internal_applicationmanager, DispatchErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_internal_applicationmanager, SignalHandler,
		TP_ARGS(const char *, msg, int, signum),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, SIGNAL, signum)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_internal_applicationmanager, SignalHandler, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_internal_applicationmanager, SignalHandlerErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_internal_applicationmanager, SignalHandlerErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_internal_applicationmanager, CSISetCallback,
		TP_ARGS(const char *, msg, unsigned long long, inv, const char *, comp, int, old_s, int, new_s),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(unsigned long long, INVOCATION, inv) ctf_string(COMPONENT_NAME, comp) ctf_integer(int, OLD_STATE, old_s) ctf_integer(int, NEW_STATE, new_s)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_internal_applicationmanager, CSISetCallback, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_internal_applicationmanager, CSISetCallbackErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_internal_applicationmanager, CSISetCallbackErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_internal_applicationmanager, CSIRemoveCallback,
		TP_ARGS(const char *, msg, unsigned long long, inv, const char *, comp, const char *, csi, int, flag),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(unsigned long long, INVOCATION, inv) ctf_string(COMPONENT_NAME, comp) ctf_string(CSI_NAME, csi) ctf_integer(int, FLAGS, flag)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_internal_applicationmanager, CSIRemoveCallback, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_internal_applicationmanager, CSIRemoveCallbackErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_internal_applicationmanager, CSIRemoveCallbackErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_internal_applicationmanager, CSITerminateCallback,
		TP_ARGS(const char *, msg, unsigned long long, inv, const char *, comp),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(unsigned long long, INVOCATION, inv) ctf_string(COMPONENT_NAME, comp)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_internal_applicationmanager, CSITerminateCallback, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_internal_applicationmanager, CSITerminateCallbackErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_internal_applicationmanager, CSITerminateCallbackErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_internal_applicationmanager, CSIHealthCheckCallback,
		TP_ARGS(const char *, msg, unsigned long long, inv, const char *, comp, const char *, key),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(unsigned long long, INVOCATION, inv) ctf_string(COMPONENT_NAME, comp) ctf_string(KEY, key)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_internal_applicationmanager, CSIHealthCheckCallback, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_internal_applicationmanager, CSIHealthCheckCallbackErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_internal_applicationmanager, CSIHealthCheckCallbackErr, TRACE_ERR)

#endif /* _COM_ERICSSON_COMMON_HCF_INTERNAL_APPLICATIONMANAGER_H */

/* This part must be outside protection */
#include <lttng/tracepoint-event.h>

#ifdef __cplusplus
}
#endif
