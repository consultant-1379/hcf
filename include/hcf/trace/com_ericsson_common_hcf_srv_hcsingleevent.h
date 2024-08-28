/*
 *	@file com_ericsson_common_hcf_srv_hcsingleevent.h
 *	@brief
 *	@author xcasale (Alessio Cascone)
 *	@date 2015-11-03
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
 *	| 2015-001 | 2015-11-03 | xcasale      | Creation and first revision.        |
 *	+==========+============+==============+=====================================+
 *
 */

#undef TRACEPOINT_PROVIDER
#define TRACEPOINT_PROVIDER com_ericsson_common_hcf_srv_hcsingleevent

#undef TRACEPOINT_INCLUDE_FILE
#define TRACEPOINT_INCLUDE_FILE ./hcf/trace/com_ericsson_common_hcf_srv_hcsingleevent.h

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(_COM_ERICSSON_COMMON_HCF_SRV_HCSINGLEEVENT_H) || defined(TRACEPOINT_HEADER_MULTI_READ)
#define _COM_ERICSSON_COMMON_HCF_SRV_HCSINGLEEVENT_H

#include <lttng/tracepoint.h>

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcsingleevent, FunctionTracing,
		TP_ARGS(const char *, msg, const char *, function),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(FUNC, function)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcsingleevent, FunctionTracing, TRACE_INFO)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcsingleevent, SetObjectAttributes,
		TP_ARGS(const char *, msg, const char *, val),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(VALUE, val)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcsingleevent, SetObjectAttributes, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcsingleevent, SetObjectAttributesErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcsingleevent, SetObjectAttributesErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcsingleevent, FinalizeObject,
		TP_ARGS(const char *, msg, const char *, id, const char *, val),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(OBJECT_ID, id) ctf_string(VALUE, val)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcsingleevent, FinalizeObject, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcsingleevent, FinalizeObjectErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcsingleevent, FinalizeObjectErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcsingleevent, AbortObjectModification,
		TP_ARGS(const char *, msg, const char *, id, const char *, val),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(OBJECT_ID, id) ctf_string(VALUE, val)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcsingleevent, AbortObjectModification, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcsingleevent, AbortObjectModificationErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcsingleevent, AbortObjectModificationErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcsingleevent, AddTimeplan,
		TP_ARGS(const char *, msg, const char *, id, const char *, o_id),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(TIMEPLAN_ID, id) ctf_string(OBJECT_ID, o_id)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcsingleevent, AddTimeplan, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcsingleevent, AddTimeplanErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcsingleevent, AddTimeplanErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcsingleevent, ModifyTimeplan,
		TP_ARGS(const char *, msg, const char *, id, const char *, o_id),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(TIMEPLAN_ID, id) ctf_string(OBJECT_ID, o_id)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcsingleevent, ModifyTimeplan, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcsingleevent, ModifyTimeplanErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcsingleevent, ModifyTimeplanErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcsingleevent, RemoveTimeplan,
		TP_ARGS(const char *, msg, const char *, id, const char *, o_id),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(TIMEPLAN_ID, id) ctf_string(OBJECT_ID, o_id)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcsingleevent, RemoveTimeplan, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcsingleevent, RemoveTimeplanErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcsingleevent, RemoveTimeplanErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcsingleevent, GetNextScheduledTime,
		TP_ARGS(const char *, msg, const char *, val),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(VALUE, val)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcsingleevent, GetNextScheduledTime, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcsingleevent, GetNextScheduledTimeErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcsingleevent, GetNextScheduledTimeErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcsingleevent, GetObjectIdFromDn,
		TP_ARGS(const char *, msg, const char *, dn),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(DN, dn)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcsingleevent, GetObjectIdFromDn, TRACE_NOTICE)

#endif /* _COM_ERICSSON_COMMON_HCF_SRV_HCSINGLEEVENT_H */

/* This part must be outside protection */
#include <lttng/tracepoint-event.h>

#ifdef __cplusplus
}
#endif
