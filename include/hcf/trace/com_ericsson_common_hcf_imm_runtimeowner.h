/*
 *	@file com_ericsson_common_hcf_imm_runtimeowner.h
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
#define TRACEPOINT_PROVIDER com_ericsson_common_hcf_imm_runtimeowner

#undef TRACEPOINT_INCLUDE_FILE
#define TRACEPOINT_INCLUDE_FILE ./hcf/trace/com_ericsson_common_hcf_imm_runtimeowner.h

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(_COM_ERICSSON_COMMON_HCF_IMM_RUNTIMEOWNER_H) || defined(TRACEPOINT_HEADER_MULTI_READ)
#define _COM_ERICSSON_COMMON_HCF_IMM_RUNTIMEOWNER_H

#include <lttng/tracepoint.h>

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_runtimeowner, FunctionTracing,
		TP_ARGS(const char *, msg, const char *, function),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(FUNC, function)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_runtimeowner, FunctionTracing, TRACE_INFO)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_runtimeowner, Init,
		TP_ARGS(const char *, msg, const char *, value),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(VALUE, value)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_runtimeowner, Init, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_runtimeowner, InitErr,
		TP_ARGS(const char *, msg, int, err, const char *, err_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err) ctf_string(ERROR_MSG, err_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_runtimeowner, InitErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_runtimeowner, Finalize,
		TP_ARGS(const char *, msg),
		TP_FIELDS(ctf_string(MSG, msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_runtimeowner, Finalize, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_runtimeowner, FinalizeErr,
		TP_ARGS(const char *, msg, int, err, const char *, err_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err) ctf_string(ERROR_MSG, err_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_runtimeowner, FinalizeErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_runtimeowner, CreateRuntimeObject,
		TP_ARGS(const char *, msg, const char *, classname, const char *, parent),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(CLASS, classname) ctf_string(PARENT, parent)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_runtimeowner, CreateRuntimeObject, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_runtimeowner, CreateRuntimeObjectErr,
		TP_ARGS(const char *, msg, int, err, const char *, err_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err) ctf_string(ERROR_MSG, err_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_runtimeowner, CreateRuntimeObjectErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_runtimeowner, DeleteRuntimeObject,
		TP_ARGS(const char *, msg, const char *, obj),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(OBJECT, obj)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_runtimeowner, DeleteRuntimeObject, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_runtimeowner, DeleteRuntimeObjectErr,
		TP_ARGS(const char *, msg, int, err, const char *, err_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err) ctf_string(ERROR_MSG, err_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_runtimeowner, DeleteRuntimeObjectErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_runtimeowner, ModifyRuntimeObject,
		TP_ARGS(const char *, msg, const char *, obj, const char *, attr),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(OBJECT, obj) ctf_string(ATTRIBUTE, attr)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_runtimeowner, ModifyRuntimeObject, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_runtimeowner, ModifyRuntimeObjectErr,
		TP_ARGS(const char *, msg, int, err, const char *, err_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err) ctf_string(ERROR_MSG, err_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_runtimeowner, ModifyRuntimeObjectErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_runtimeowner, Dispatch,
		TP_ARGS(const char *, msg, int, flag),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, FLAG, flag)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_runtimeowner, Dispatch, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_runtimeowner, DispatchErr,
		TP_ARGS(const char *, msg, int, err, const char *, err_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err) ctf_string(ERROR_MSG, err_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_runtimeowner, DispatchErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_runtimeowner, AdminOperationResult,
		TP_ARGS(const char *, msg, unsigned long long, handle, unsigned long long, inv, int, res),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(unsigned long long, HANDLE, handle) ctf_integer(unsigned long long, INVOCATION, inv) ctf_integer(int, RESULT, res)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_runtimeowner, AdminOperationResult, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_runtimeowner, AdminOperationResultErr,
		TP_ARGS(const char *, msg, int, err, const char *, err_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err) ctf_string(ERROR_MSG, err_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_runtimeowner, AdminOperationResultErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_runtimeowner, UpdateRuntimeCallback,
		TP_ARGS(const char *, msg, unsigned long long, handle, const char *, obj),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(unsigned long long, HANDLE, handle) ctf_string(OBJECT, obj)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_runtimeowner, UpdateRuntimeCallback, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_runtimeowner, UpdateRuntimeCallbackErr,
		TP_ARGS(const char *, msg, const char *, err_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(ERROR_MSG, err_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_runtimeowner, UpdateRuntimeCallbackErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_runtimeowner, AdminCallback,
		TP_ARGS(const char *, msg, unsigned long long, handle, unsigned long long, inv, unsigned long long, id, const char *, obj),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(unsigned long long, HANDLE, handle) ctf_integer(unsigned long long, INVOCATION, inv) ctf_integer(unsigned long long, OPERATION_ID, id) ctf_string(OBJECT, obj)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_runtimeowner, AdminCallback, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_runtimeowner, AdminCallbackErr,
		TP_ARGS(const char *, msg, const char *, err_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(ERROR_MSG, err_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_runtimeowner, AdminCallbackErr, TRACE_ERR)

#endif /* _COM_ERICSSON_COMMON_HCF_IMM_RUNTIMEOWNER_H */

/* This part must be outside protection */
#include <lttng/tracepoint-event.h>

#ifdef __cplusplus
}
#endif
