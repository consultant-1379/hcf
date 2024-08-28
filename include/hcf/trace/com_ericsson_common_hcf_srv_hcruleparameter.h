/*
 *	@file com_ericsson_common_hcf_srv_hcruleparameter.h
 *	@brief
 *	@author xcasale (Alessio Cascone)
 *	@date 2015-11-02
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
 *	| 2015-001 | 2015-11-02 | xcasale      | Creation and first revision.        |
 *	+==========+============+==============+=====================================+
 *
 */

#undef TRACEPOINT_PROVIDER
#define TRACEPOINT_PROVIDER com_ericsson_common_hcf_srv_hcruleparameter

#undef TRACEPOINT_INCLUDE_FILE
#define TRACEPOINT_INCLUDE_FILE ./hcf/trace/com_ericsson_common_hcf_srv_hcruleparameter.h

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(_COM_ERICSSON_COMMON_HCF_SRV_HCRULEPARAMETER_H) || defined(TRACEPOINT_HEADER_MULTI_READ)
#define _COM_ERICSSON_COMMON_HCF_SRV_HCRULEPARAMETER_H

#include <lttng/tracepoint.h>

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcruleparameter, FunctionTracing,
		TP_ARGS(const char *, msg, const char *, function),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(FUNC, function)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcruleparameter, FunctionTracing, TRACE_INFO)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcruleparameter, SetObjectAttributes,
		TP_ARGS(const char *, msg, const char *, val),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(VALUE, val)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcruleparameter, SetObjectAttributes, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcruleparameter, SetObjectAttributesErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcruleparameter, SetObjectAttributesErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcruleparameter, FinalizeObject,
		TP_ARGS(const char *, msg, const char *, id, const char *, val),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(OBJECT_ID, id) ctf_string(VALUE, val)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcruleparameter, FinalizeObject, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcruleparameter, FinalizeObjectErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcruleparameter, FinalizeObjectErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcruleparameter, AbortObjectModification,
		TP_ARGS(const char *, msg, const char *, id, const char *, val),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(OBJECT_ID, id) ctf_string(VALUE, val)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcruleparameter, AbortObjectModification, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcruleparameter, AbortObjectModificationErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcruleparameter, AbortObjectModificationErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcruleparameter, GetObjectIdFromDn,
		TP_ARGS(const char *, msg, const char *, dn),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(DN, dn)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcruleparameter, GetObjectIdFromDn, TRACE_NOTICE)

#endif /* _COM_ERICSSON_COMMON_HCF_SRV_HCRULEPARAMETER_H */

/* This part must be outside protection */
#include <lttng/tracepoint-event.h>

#ifdef __cplusplus
}
#endif
