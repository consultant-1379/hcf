/*
 *	@file com_ericsson_common_hcf_imm_oiinterface.h
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
#define TRACEPOINT_PROVIDER com_ericsson_common_hcf_imm_oiinterface

#undef TRACEPOINT_INCLUDE_FILE
#define TRACEPOINT_INCLUDE_FILE ./hcf/trace/com_ericsson_common_hcf_imm_oiinterface.h

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(_COM_ERICSSON_COMMON_HCF_IMM_OIINTERFACE_H) || defined(TRACEPOINT_HEADER_MULTI_READ)
#define _COM_ERICSSON_COMMON_HCF_IMM_OIINTERFACE_H

#include <lttng/tracepoint.h>

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_oiinterface, FunctionTracing,
		TP_ARGS(const char *, msg, const char *, function),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(FUNC, function)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_oiinterface, FunctionTracing, TRACE_INFO)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_oiinterface, Dispatch,
		TP_ARGS(const char *, msg, int, flags),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, FLAGS, flags)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_oiinterface, Dispatch, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_oiinterface, DispatchErr,
		TP_ARGS(const char *, msg, int, err, const char *, err_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err) ctf_string(ERROR_MSG, err_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_oiinterface, DispatchErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_oiinterface, ModifyRuntimeObj,
		TP_ARGS(const char *, msg, const char *, obj, const char *, attr),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(OBJECT, obj) ctf_string(ATTRIBUTE, attr)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_oiinterface, ModifyRuntimeObj, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_oiinterface, ModifyRuntimeObjErr,
		TP_ARGS(const char *, msg, int, err, const char *, err_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err) ctf_string(ERROR_MSG, err_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_oiinterface, ModifyRuntimeObjErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_oiinterface, AdminOperationalResult,
		TP_ARGS(const char *, msg, unsigned long long, handle, unsigned long long, inv, int, res),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(unsigned long long, HANDLE, handle) ctf_integer(unsigned long long, INVOCATION, inv) ctf_integer(int, RESULT, res)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_oiinterface, AdminOperationalResult, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_oiinterface, AdminOperationalResultErr,
		TP_ARGS(const char *, msg, int, err, const char *, err_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err) ctf_string(ERROR_MSG, err_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_oiinterface, AdminOperationalResultErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_oiinterface, ExitCode,
		TP_ARGS(const char *, msg, int, exit_code, const char *, exit_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, EXIT_CODE, exit_code) ctf_string(EXIT_MSG, exit_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_oiinterface, ExitCode, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_oiinterface, CcbAugmentationInitialize,
		TP_ARGS(const char *, msg, unsigned long long, handle, unsigned long long, ccb, unsigned long long, ccb_handle, unsigned long long, admin_handle),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(unsigned long long, HANDLE, handle) ctf_integer(unsigned long long, CCB, ccb) ctf_integer(unsigned long long, CCB_HANDLE, ccb_handle) ctf_integer(unsigned long long, ADMIN_HANDLE, admin_handle)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_oiinterface, CcbAugmentationInitialize, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_oiinterface, CcbAugmentationInitializeErr,
		TP_ARGS(const char *, msg, int, err, const char *, err_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err) ctf_string(ERROR_MSG, err_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_oiinterface, CcbAugmentationInitializeErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_oiinterface, CreateObjectAugmentCcb,
		TP_ARGS(const char *, msg, unsigned long long, ccb_handle, unsigned long long, admin_handle, const char *, classname, const char *, parent),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(unsigned long long, CCB_HANDLE, ccb_handle) ctf_integer(unsigned long long, ADMIN_HANDLE, admin_handle) ctf_string(CLASS, classname) ctf_string(PARENT, parent)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_oiinterface, CreateObjectAugmentCcb, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_oiinterface, CreateObjectAugmentCcbErr,
		TP_ARGS(const char *, msg, int, err, const char *, err_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err) ctf_string(ERROR_MSG, err_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_oiinterface, CreateObjectAugmentCcbErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_oiinterface, ModifyObjectAugmentCcb,
		TP_ARGS(const char *, msg, unsigned long long, ccb_handle, unsigned long long, admin_handle, const char *, obj, const char *, attr),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(unsigned long long, CCB_HANDLE, ccb_handle) ctf_integer(unsigned long long, ADMIN_HANDLE, admin_handle) ctf_string(OBJECT, obj) ctf_string(ATTRIBUTE, attr)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_oiinterface, ModifyObjectAugmentCcb, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_oiinterface, ModifyObjectAugmentCcbErr,
		TP_ARGS(const char *, msg, int, err, const char *, err_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err) ctf_string(ERROR_MSG, err_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_oiinterface, ModifyObjectAugmentCcbErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_oiinterface, DeleteObjectAugmentCcb,
		TP_ARGS(const char *, msg, unsigned long long, ccb_handle, unsigned long long, admin_handle, const char *, obj),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(unsigned long long, CCB_HANDLE, ccb_handle) ctf_integer(unsigned long long, ADMIN_HANDLE, admin_handle) ctf_string(OBJECT, obj)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_oiinterface, DeleteObjectAugmentCcb, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_oiinterface, DeleteObjectAugmentCcbErr,
		TP_ARGS(const char *, msg, int, err, const char *, err_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err) ctf_string(ERROR_MSG, err_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_oiinterface, DeleteObjectAugmentCcbErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_oiinterface, ApplyAugmentCcb,
		TP_ARGS(const char *, msg, unsigned long long, ccb),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(unsigned long long, CCB, ccb)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_oiinterface, ApplyAugmentCcb, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_oiinterface, ApplyAugmentCcbErr,
		TP_ARGS(const char *, msg, int, err, const char *, err_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err) ctf_string(ERROR_MSG, err_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_oiinterface, ApplyAugmentCcbErr, TRACE_ERR)

#endif /* _COM_ERICSSON_COMMON_HCF_IMM_OIINTERFACE_H */

/* This part must be outside protection */
#include <lttng/tracepoint-event.h>

#ifdef __cplusplus
}
#endif
