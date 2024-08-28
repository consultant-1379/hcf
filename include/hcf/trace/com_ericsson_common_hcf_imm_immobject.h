/*
 *	@file com_ericsson_common_hcf_imm_immobject.h
 *	@brief
 *	@author xcasale (Alessio Cascone)
 *	@date 2015-08-07
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
 *	| 2015-001 | 2015-08-07 | xcasale      | Creation and first revision.        |
 *	+==========+============+==============+=====================================+
 *
 */

#undef TRACEPOINT_PROVIDER
#define TRACEPOINT_PROVIDER com_ericsson_common_hcf_imm_immobject

#undef TRACEPOINT_INCLUDE_FILE
#define TRACEPOINT_INCLUDE_FILE ./hcf/trace/com_ericsson_common_hcf_imm_immobject.h

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(_COM_ERICSSON_COMMON_HCF_IMM_IMMOBJECT_H) || defined(TRACEPOINT_HEADER_MULTI_READ)
#define _COM_ERICSSON_COMMON_HCF_IMM_IMMOBJECT_H

#include <lttng/tracepoint.h>

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_immobject, FunctionTracing,
		TP_ARGS(const char *, msg, const char *, function),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(FUNC, function)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_immobject, FunctionTracing, TRACE_INFO)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_immobject, SetObjectAttributes,
		TP_ARGS(const char *, msg, const char *, val1, const char *, val2),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(VALUE_1, val1) ctf_string(VALUE_2, val2)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_immobject, SetObjectAttributes, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_immobject, SetObjectAttributesErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_immobject, SetObjectAttributesErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_immobject, GetObjIdFromDN,
		TP_ARGS(const char *, msg, const char *, dn, const char *, id),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(DN, dn) ctf_string(ID, id)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_immobject, GetObjIdFromDN, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_immobject, CreateObject,
		TP_ARGS(const char *, msg, const char *, class_name, const char *, parent),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(CLASS, class_name) ctf_string(PARENT, parent)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_immobject, CreateObject, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_immobject, CreateObjectErr,
		TP_ARGS(const char *, msg, int, err, const char *, err_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err) ctf_string(ERROR_MSG, err_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_immobject, CreateObjectErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_immobject, ModifyObject,
		TP_ARGS(const char *, msg, const char *, obj, const char *, attr, int, type),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(OBJECT, obj) ctf_string(ATTRIBUTE_NAME, attr) ctf_integer(int, ATTRIBUTE_TYPE, type)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_immobject, ModifyObject, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_immobject, ModifyObjectErr,
		TP_ARGS(const char *, msg, int, err, const char *, err_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err) ctf_string(ERROR_MSG, err_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_immobject, ModifyObjectErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_immobject, DeleteObject,
		TP_ARGS(const char *, msg, const char *, obj),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(OBJECT, obj)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_immobject, DeleteObject, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_immobject, DeleteObjectErr,
		TP_ARGS(const char *, msg, int, err, const char *, err_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err) ctf_string(ERROR_MSG, err_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_immobject, DeleteObjectErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_immobject, InitializeOM,
		TP_ARGS(const char *, msg, int, flag),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, IS_INITIALIZED, flag)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_immobject, InitializeOM, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_immobject, InitializeOMErr,
		TP_ARGS(const char *, msg, int, err, const char *, err_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err) ctf_string(ERROR_MSG, err_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_immobject, InitializeOMErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_immobject, FinalizeOM,
		TP_ARGS(const char *, msg, int, flag),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, IS_INITIALIZED, flag)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_immobject, FinalizeOM, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_immobject, FinalizeOMErr,
		TP_ARGS(const char *, msg, int, err, const char *, err_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err) ctf_string(ERROR_MSG, err_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_immobject, FinalizeOMErr, TRACE_ERR)


#endif /* _COM_ERICSSON_COMMON_HCF_IMM_IMMOBJECT_H */

/* This part must be outside protection */
#include <lttng/tracepoint-event.h>

#ifdef __cplusplus
}
#endif
