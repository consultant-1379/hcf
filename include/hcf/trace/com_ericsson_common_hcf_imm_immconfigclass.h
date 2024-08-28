/*
 *	@file com_ericsson_common_hcf_imm_immconfigclass.h
 *	@brief
 *	@author xcasale (Alessio Cascone)
 *	@date 2015-08-10
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
 *	| 2015-001 | 2015-08-10 | xcasale      | Creation and first revision.        |
 *	+==========+============+==============+=====================================+
 *
 */

#undef TRACEPOINT_PROVIDER
#define TRACEPOINT_PROVIDER com_ericsson_common_hcf_imm_immconfigclass

#undef TRACEPOINT_INCLUDE_FILE
#define TRACEPOINT_INCLUDE_FILE ./hcf/trace/com_ericsson_common_hcf_imm_immconfigclass.h

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(_COM_ERICSSON_COMMON_HCF_IMM_IMMCONFIGCLASS_H) || defined(TRACEPOINT_HEADER_MULTI_READ)
#define _COM_ERICSSON_COMMON_HCF_IMM_IMMCONFIGCLASS_H

#include <lttng/tracepoint.h>

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_immconfigclass, FunctionTracing,
		TP_ARGS(const char *, msg, const char *, function),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(FUNC, function)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_immconfigclass, FunctionTracing, TRACE_INFO)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_immconfigclass, Initialize,
		TP_ARGS(const char *, msg, const char *, val, int, flag),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(VALUE, val) ctf_integer(int, IS_INITIALIZED, flag)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_immconfigclass, Initialize, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_immconfigclass, InitializeErr,
		TP_ARGS(const char *, msg, int, err, const char *, err_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err) ctf_string(ERROR_MSG, err_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_immconfigclass, InitializeErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_immconfigclass, Finalize,
		TP_ARGS(const char *, msg, const char *, class_name, int, flag),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(CLASS, class_name) ctf_integer(int, IS_INITIALIZED, flag)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_immconfigclass, Finalize, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_immconfigclass, FinalizeErr,
		TP_ARGS(const char *, msg, int, err, const char *, err_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err) ctf_string(ERROR_MSG, err_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_immconfigclass, FinalizeErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_immconfigclass, RegisterImplementer,
		TP_ARGS(const char *, msg, const char *, class_name, const char *, impl_name),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(CLASS, class_name) ctf_string(IMPLEMENTER, impl_name)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_immconfigclass, RegisterImplementer, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_immconfigclass, RegisterImplementerErr,
		TP_ARGS(const char *, msg, int, err, const char *, err_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err) ctf_string(ERROR_MSG, err_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_immconfigclass, RegisterImplementerErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_immconfigclass, RemoveImplementer,
		TP_ARGS(const char *, msg, const char *, class_name, const char *, impl_name),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(CLASS, class_name) ctf_string(IMPLEMENTER, impl_name)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_immconfigclass, RemoveImplementer, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_immconfigclass, RemoveImplementerErr,
		TP_ARGS(const char *, msg, int, err, const char *, err_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err) ctf_string(ERROR_MSG, err_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_immconfigclass, RemoveImplementerErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_immconfigclass, AddObjectToInternals,
		TP_ARGS(const char *, msg, const char *, val),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(VALUE, val)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_immconfigclass, AddObjectToInternals, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_immconfigclass, AddObjectToInternalsErr,
		TP_ARGS(const char *, msg, int, err, const char *, err_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err) ctf_string(ERROR_MSG, err_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_immconfigclass, AddObjectToInternalsErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_immconfigclass, CreateCallback,
		TP_ARGS(const char *, msg, unsigned long long, handle, unsigned long long, ccb, const char *, class_name, const char *, parent),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(unsigned long long, HANDLE, handle) ctf_integer(unsigned long long, CCB, ccb) ctf_string(CLASS, class_name) ctf_string(PARENT, parent)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_immconfigclass, CreateCallback, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_immconfigclass, CreateCallbackErr,
		TP_ARGS(const char *, msg, int, err, const char *, err_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err) ctf_string(ERROR_MSG, err_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_immconfigclass, CreateCallbackErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_immconfigclass, DeleteCallback,
		TP_ARGS(const char *, msg, unsigned long long, handle, unsigned long long, ccb, const char *, obj),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(unsigned long long, HANDLE, handle) ctf_integer(unsigned long long, CCB, ccb) ctf_string(OBJECT, obj)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_immconfigclass, DeleteCallback, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_immconfigclass, DeleteCallbackErr,
		TP_ARGS(const char *, msg, int, err, const char *, err_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err) ctf_string(ERROR_MSG, err_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_immconfigclass, DeleteCallbackErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_immconfigclass, ModifyCallback,
		TP_ARGS(const char *, msg, unsigned long long, handle, unsigned long long, ccb, const char *, obj),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(unsigned long long, HANDLE, handle) ctf_integer(unsigned long long, CCB, ccb) ctf_string(OBJECT, obj)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_immconfigclass, ModifyCallback, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_immconfigclass, ModifyCallbackErr,
		TP_ARGS(const char *, msg, int, err, const char *, err_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err) ctf_string(ERROR_MSG, err_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_immconfigclass, ModifyCallbackErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_immconfigclass, CompleteCallback,
		TP_ARGS(const char *, msg, unsigned long long, handle, unsigned long long, ccb, const char *, obj),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(unsigned long long, HANDLE, handle) ctf_integer(unsigned long long, CCB, ccb) ctf_string(OBJECT, obj)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_immconfigclass, CompleteCallback, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_immconfigclass, CompleteCallbackErr,
		TP_ARGS(const char *, msg, int, err, const char *, err_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err) ctf_string(ERROR_MSG, err_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_immconfigclass, CompleteCallbackErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_immconfigclass, AbortCallback,
		TP_ARGS(const char *, msg, unsigned long long, handle, unsigned long long, ccb, const char *, obj),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(unsigned long long, HANDLE, handle) ctf_integer(unsigned long long, CCB, ccb) ctf_string(OBJECT, obj)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_immconfigclass, AbortCallback, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_immconfigclass, AbortCallbackErr,
		TP_ARGS(const char *, msg, int, err, const char *, err_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err) ctf_string(ERROR_MSG, err_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_immconfigclass, AbortCallbackErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_immconfigclass, ApplyCallback,
		TP_ARGS(const char *, msg, unsigned long long, handle, unsigned long long, ccb, const char *, obj),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(unsigned long long, HANDLE, handle) ctf_integer(unsigned long long, CCB, ccb) ctf_string(OBJECT, obj)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_immconfigclass, ApplyCallback, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_immconfigclass, ApplyCallbackErr,
		TP_ARGS(const char *, msg, int, err, const char *, err_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err) ctf_string(ERROR_MSG, err_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_immconfigclass, ApplyCallbackErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_immconfigclass, UpdateRuntimeCallback,
		TP_ARGS(const char *, msg, const char *, obj, const char *, attr),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(OBJECT, obj) ctf_string(ATTRIBUTE, attr)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_immconfigclass, UpdateRuntimeCallback, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_immconfigclass, UpdateRuntimeCallbackErr,
		TP_ARGS(const char *, msg, int, err, const char *, err_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err) ctf_string(ERROR_MSG, err_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_immconfigclass, UpdateRuntimeCallbackErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_immconfigclass, AdminOperationCallback,
		TP_ARGS(const char *, msg, unsigned long long, handle, unsigned long long, inv, const char *, obj, unsigned long long, id),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(unsigned long long, HANDLE, handle) ctf_integer(unsigned long long, INVOCATION, inv) ctf_string(OBJECT, obj) ctf_integer(unsigned long long, OPERATION_ID, id)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_immconfigclass, AdminOperationCallback, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_immconfigclass, AdminOperationCallbackErr,
		TP_ARGS(const char *, msg, int, err, const char *, err_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err) ctf_string(ERROR_MSG, err_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_immconfigclass, AdminOperationCallbackErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_immconfigclass, HandleInput,
		TP_ARGS(const char *, msg, int, val),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, VALUE, val)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_immconfigclass, HandleInput, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_immconfigclass, HandleOutput,
		TP_ARGS(const char *, msg, int, handle),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, HANDLE, handle)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_immconfigclass, HandleOutput, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_immconfigclass, HandleException,
		TP_ARGS(const char *, msg, int, handle, int, er_f, int, h_f, int, rh_f),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, HANDLE, handle) ctf_integer(int, ERROR_FLAG, er_f) ctf_integer(int, HUNGUP_FLAG, h_f) ctf_integer(int, READ_HUNGUP_FLAG, rh_f)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_immconfigclass, HandleException, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_immconfigclass, HandleClose,
		TP_ARGS(const char *, msg, int, handle),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, HANDLE, handle)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_immconfigclass, HandleClose, TRACE_NOTICE)

#endif /* _COM_ERICSSON_COMMON_HCF_IMM_IMMCONFIGCLASS_H */

/* This part must be outside protection */
#include <lttng/tracepoint-event.h>

#ifdef __cplusplus
}
#endif
