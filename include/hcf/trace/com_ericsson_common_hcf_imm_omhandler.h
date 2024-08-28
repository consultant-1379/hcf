/*
 *	@file com_ericsson_common_hcf_imm_omhandler.h
 *	@brief
 *	@author xcasale (Alessio Cascone)
 *	@date 2015-08-05
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
 *	| 2015-001 | 2015-08-05 | xcasale      | Creation and first revision.        |
 *	+==========+============+==============+=====================================+
 *
 */

#undef TRACEPOINT_PROVIDER
#define TRACEPOINT_PROVIDER com_ericsson_common_hcf_imm_omhandler

#undef TRACEPOINT_INCLUDE_FILE
#define TRACEPOINT_INCLUDE_FILE ./hcf/trace/com_ericsson_common_hcf_imm_omhandler.h

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(_COM_ERICSSON_COMMON_HCF_IMM_OMHANDLER_H) || defined(TRACEPOINT_HEADER_MULTI_READ)
#define _COM_ERICSSON_COMMON_HCF_IMM_OMHANDLER_H

#include <lttng/tracepoint.h>

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_omhandler, FunctionTracing,
		TP_ARGS(const char *, msg, const char *, function),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(FUNC, function)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_omhandler, FunctionTracing, TRACE_INFO)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_omhandler, Init,
		TP_ARGS(const char *, msg, int, flag),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, FLAG, flag)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_omhandler, Init, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_omhandler, InitErr,
		TP_ARGS(const char *, msg, int, err, const char *, err_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err) ctf_string(ERROR_MSG, err_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_omhandler, InitErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_omhandler, Finalize,
		TP_ARGS(const char *, msg),
		TP_FIELDS(ctf_string(MSG, msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_omhandler, Finalize, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_omhandler, FinalizeErr,
		TP_ARGS(const char *, msg, int, err, const char *, err_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err) ctf_string(ERROR_MSG, err_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_omhandler, FinalizeErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_omhandler, DefineClass,
		TP_ARGS(const char *, msg, const char *, classname, int, cat, int, n_attrs),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(CLASS, classname) ctf_integer(int, CATEGORY, cat) ctf_integer(int, NUM_OF_ATTRS, n_attrs)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_omhandler, DefineClass, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_omhandler, DefineClassErr,
		TP_ARGS(const char *, msg, int, err, const char *, err_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err) ctf_string(ERROR_MSG, err_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_omhandler, DefineClassErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_omhandler, RemoveClass,
		TP_ARGS(const char *, msg, const char *, classname),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(CLASS, classname)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_omhandler, RemoveClass, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_omhandler, RemoveClassErr,
		TP_ARGS(const char *, msg, int, err, const char *, err_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err) ctf_string(ERROR_MSG, err_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_omhandler, RemoveClassErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_omhandler, CreateObject,
		TP_ARGS(const char *, msg, const char *, classname, const char *, parent, const char *, trans),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(CLASS, classname) ctf_string(PARENT, parent) ctf_string(TRANSACTION, trans)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_omhandler, CreateObject, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_omhandler, CreateObjectErr,
		TP_ARGS(const char *, msg, int, err, const char *, err_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err) ctf_string(ERROR_MSG, err_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_omhandler, CreateObjectErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_omhandler, DeleteObject,
		TP_ARGS(const char *, msg, const char *, obj, int, scope, const char *, trans),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(OBJECT, obj) ctf_integer(int, SCOPE, scope) ctf_string(TRANSACTION, trans)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_omhandler, DeleteObject, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_omhandler, DeleteObjectErr,
		TP_ARGS(const char *, msg, int, err, const char *, err_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err) ctf_string(ERROR_MSG, err_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_omhandler, DeleteObjectErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_omhandler, GetAttribute,
		TP_ARGS(const char *, msg, const char *, obj, const char *, attr),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(OBJECT, obj) ctf_string(ATTRIBUTE, attr)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_omhandler, GetAttribute, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_omhandler, GetAttributeErr,
		TP_ARGS(const char *, msg, int, err, const char *, err_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err) ctf_string(ERROR_MSG, err_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_omhandler, GetAttributeErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_omhandler, GetObject,
		TP_ARGS(const char *, msg, const char *, obj),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(OBJECT, obj)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_omhandler, GetObject, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_omhandler, GetObjectErr,
		TP_ARGS(const char *, msg, int, err, const char *, err_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err) ctf_string(ERROR_MSG, err_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_omhandler, GetObjectErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_omhandler, GetChildren,
		TP_ARGS(const char *, msg, const char *, root, int, scope),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(ROOT, root) ctf_integer(int, SCOPE, scope)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_omhandler, GetChildren, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_omhandler, GetChildrenErr,
		TP_ARGS(const char *, msg, int, err, const char *, err_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err) ctf_string(ERROR_MSG, err_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_omhandler, GetChildrenErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_omhandler, GetClassInstances,
		TP_ARGS(const char *, msg, const char *, classname),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(CLASS, classname)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_omhandler, GetClassInstances, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_omhandler, GetClassInstancesErr,
		TP_ARGS(const char *, msg, int, err, const char *, err_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err) ctf_string(ERROR_MSG, err_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_omhandler, GetClassInstancesErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_omhandler, ModifyAttribute,
		TP_ARGS(const char *, msg, const char *, obj, const char *, attr, const char *, trans),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(OBJECT, obj) ctf_string(ATTRIBUTE, attr) ctf_string(TRANSACTION, trans)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_omhandler, ModifyAttribute, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_omhandler, ModifyAttributeErr,
		TP_ARGS(const char *, msg, int, err, const char *, err_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err) ctf_string(ERROR_MSG, err_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_omhandler, ModifyAttributeErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_omhandler, ApplyRequest,
		TP_ARGS(const char *, msg, const char *, trans),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(TRANSACTION, trans)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_omhandler, ApplyRequest, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_omhandler, ApplyRequestErr,
		TP_ARGS(const char *, msg, int, err, const char *, err_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err) ctf_string(ERROR_MSG, err_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_omhandler, ApplyRequestErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_omhandler, ResetRequest,
		TP_ARGS(const char *, msg, const char *, trans),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(TRANSACTION, trans)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_omhandler, ResetRequest, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_omhandler, ResetRequestErr,
		TP_ARGS(const char *, msg, int, err, const char *, err_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err) ctf_string(ERROR_MSG, err_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_omhandler, ResetRequestErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_omhandler, ExitCode,
		TP_ARGS(const char *, msg, int, code, const char *, text, const char *, trans),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, code) ctf_string(ERROR_TEXT, text) ctf_string(TRANSACTION, trans)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_omhandler, ExitCode, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_omhandler, ExitCodeErr,
		TP_ARGS(const char *, msg, int, err, const char *, err_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err) ctf_string(ERROR_MSG, err_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_omhandler, ExitCodeErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_omhandler, GetCcbHandle,
		TP_ARGS(const char *, msg, const char *, trans),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(TRANSACTION, trans)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_omhandler, GetCcbHandle, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_omhandler, GetCcbHandleErr,
		TP_ARGS(const char *, msg, int, err, const char *, err_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err) ctf_string(ERROR_MSG, err_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_omhandler, GetCcbHandleErr, TRACE_ERR)

#endif /* _COM_ERICSSON_COMMON_HCF_IMM_OMHANDLER_H */

/* This part must be outside protection */
#include <lttng/tracepoint-event.h>

#ifdef __cplusplus
}
#endif
