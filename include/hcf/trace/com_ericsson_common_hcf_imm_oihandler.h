/*
 *	@file com_ericsson_common_hcf_imm_oihandler.h
 *	@brief
 *	@author xcasale (Alessio Cascone)
 *	@date 2015-08-03
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
 *	| 2015-001 | 2015-08-03 | xcasale      | Creation and first revision.        |
 *	+==========+============+==============+=====================================+
 *
 */

#undef TRACEPOINT_PROVIDER
#define TRACEPOINT_PROVIDER com_ericsson_common_hcf_imm_oihandler

#undef TRACEPOINT_INCLUDE_FILE
#define TRACEPOINT_INCLUDE_FILE ./hcf/trace/com_ericsson_common_hcf_imm_oihandler.h

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(_COM_ERICSSON_COMMON_HCF_IMM_OIHANDLER_H) || defined(TRACEPOINT_HEADER_MULTI_READ)
#define _COM_ERICSSON_COMMON_HCF_IMM_OIHANDLER_H

#include <lttng/tracepoint.h>

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_oihandler, FunctionTracing,
		TP_ARGS(const char *, msg, const char *, function),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(FUNC, function)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_oihandler, FunctionTracing, TRACE_INFO)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_oihandler, AddObjectImpl,
		TP_ARGS(const char *, msg, const char *, value),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(VALUE, value)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_oihandler, AddObjectImpl, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_oihandler, AddObjectImplErr,
		TP_ARGS(const char *, msg, int, err, const char *, err_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err) ctf_string(ERROR_MSG, err_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_oihandler, AddObjectImplErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_oihandler, RemoveObjectImpl,
		TP_ARGS(const char *, msg, const char *, value),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(VALUE, value)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_oihandler, RemoveObjectImpl, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_oihandler, RemoveObjectImplErr,
		TP_ARGS(const char *, msg, int, err, const char *, err_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err) ctf_string(ERROR_MSG, err_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_oihandler, RemoveObjectImplErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_oihandler, AddClassImpl,
		TP_ARGS(const char *, msg, const char *, value),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(VALUE, value)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_oihandler, AddClassImpl, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_oihandler, AddClassImplErr,
		TP_ARGS(const char *, msg, int, err, const char *, err_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err) ctf_string(ERROR_MSG, err_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_oihandler, AddClassImplErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_oihandler, RemoveClassImpl,
		TP_ARGS(const char *, msg, const char *, value),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(VALUE, value)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_oihandler, RemoveClassImpl, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_oihandler, RemoveClassImplErr,
		TP_ARGS(const char *, msg, int, err, const char *, err_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err) ctf_string(ERROR_MSG, err_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_oihandler, RemoveClassImplErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_oihandler, Finalize,
		TP_ARGS(const char *, msg, unsigned long long, handle),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(unsigned long long, HANDLE, handle)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_oihandler, Finalize, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_oihandler, FinalizeErr,
		TP_ARGS(const char *, msg, int, err, const char *, err_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err) ctf_string(ERROR_MSG, err_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_oihandler, FinalizeErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_oihandler, CreateCallback,
		TP_ARGS(const char *, msg, unsigned long long, handle, unsigned long long, ccbid, const char *, classname, const char *, parentname),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(unsigned long long, HANDLE, handle) ctf_integer(unsigned long long, CCB, ccbid) ctf_string(CLASS, classname) ctf_string(PARENT, parentname)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_oihandler, CreateCallback, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_oihandler, CreateCallbackErr,
		TP_ARGS(const char *, msg, const char *, err_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(ERROR_MSG, err_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_oihandler, CreateCallbackErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_oihandler, DeleteCallback,
		TP_ARGS(const char *, msg, unsigned long long, handle, unsigned long long, ccbid, const char *, obj),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(unsigned long long, HANDLE, handle) ctf_integer(unsigned long long, CCB, ccbid) ctf_string(OBJECT, obj)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_oihandler, DeleteCallback, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_oihandler, DeleteCallbackErr,
		TP_ARGS(const char *, msg, const char *, err_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(ERROR_MSG, err_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_oihandler, DeleteCallbackErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_oihandler, ModifyCallback,
		TP_ARGS(const char *, msg, unsigned long long, handle, unsigned long long, ccbid, const char *, obj),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(unsigned long long, HANDLE, handle) ctf_integer(unsigned long long, CCB, ccbid) ctf_string(OBJECT, obj)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_oihandler, ModifyCallback, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_oihandler, ModifyCallbackErr,
		TP_ARGS(const char *, msg, const char *, err_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(ERROR_MSG, err_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_oihandler, ModifyCallbackErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_oihandler, CompletedCallback,
		TP_ARGS(const char *, msg, unsigned long long, handle, unsigned long long, ccbid),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(unsigned long long, HANDLE, handle) ctf_integer(unsigned long long, CCB, ccbid)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_oihandler, CompletedCallback, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_oihandler, CompletedCallbackErr,
		TP_ARGS(const char *, msg, const char *, err_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(ERROR_MSG, err_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_oihandler, CompletedCallbackErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_oihandler, AbortCallback,
		TP_ARGS(const char *, msg, unsigned long long, handle, unsigned long long, ccbid),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(unsigned long long, HANDLE, handle) ctf_integer(unsigned long long, CCB, ccbid)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_oihandler, AbortCallback, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_oihandler, AbortCallbackErr,
		TP_ARGS(const char *, msg, const char *, err_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(ERROR_MSG, err_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_oihandler, AbortCallbackErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_oihandler, ApplyCallback,
		TP_ARGS(const char *, msg, unsigned long long, handle, unsigned long long, ccbid),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(unsigned long long, HANDLE, handle) ctf_integer(unsigned long long, CCB, ccbid)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_oihandler, ApplyCallback, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_oihandler, ApplyCallbackErr,
		TP_ARGS(const char *, msg, const char *, err_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(ERROR_MSG, err_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_oihandler, ApplyCallbackErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_oihandler, UpdateRuntimeCallback,
		TP_ARGS(const char *, msg, unsigned long long, handle, const char *, obj),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(unsigned long long, HANDLE, handle) ctf_string(OBJECT, obj)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_oihandler, UpdateRuntimeCallback, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_oihandler, UpdateRuntimeCallbackErr,
		TP_ARGS(const char *, msg, const char *, err_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(ERROR_MSG, err_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_oihandler, UpdateRuntimeCallbackErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_oihandler, AdminCallback,
		TP_ARGS(const char *, msg, unsigned long long, handle, unsigned long long, inv, unsigned long long, id, const char *, obj),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(unsigned long long, HANDLE, handle) ctf_integer(unsigned long long, INVOCATION, inv) ctf_integer(unsigned long long, OPERATION_ID, id) ctf_string(OBJECT, obj)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_oihandler, AdminCallback, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_oihandler, AdminCallbackErr,
		TP_ARGS(const char *, msg, const char *, err_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(ERROR_MSG, err_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_oihandler, AdminCallbackErr, TRACE_ERR)

#endif /* _COM_ERICSSON_COMMON_HCF_IMM_OIHANDLER_H */

/* This part must be outside protection */
#include <lttng/tracepoint-event.h>

#ifdef __cplusplus
}
#endif
