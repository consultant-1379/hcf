/*
 *	@file com_ericsson_common_hcf_srv_hcperiodicevent.h
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
#define TRACEPOINT_PROVIDER com_ericsson_common_hcf_srv_hcperiodicevent

#undef TRACEPOINT_INCLUDE_FILE
#define TRACEPOINT_INCLUDE_FILE ./hcf/trace/com_ericsson_common_hcf_srv_hcperiodicevent.h

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(_COM_ERICSSON_COMMON_HCF_SRV_HCPERIODICEVENT_H) || defined(TRACEPOINT_HEADER_MULTI_READ)
#define _COM_ERICSSON_COMMON_HCF_SRV_HCPERIODICEVENT_H

#include <lttng/tracepoint.h>

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcperiodicevent, FunctionTracing,
		TP_ARGS(const char *, msg, const char *, function),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(FUNC, function)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcperiodicevent, FunctionTracing, TRACE_INFO)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcperiodicevent, SetObjectAttributes,
		TP_ARGS(const char *, msg, const char *, val),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(VALUE, val)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcperiodicevent, SetObjectAttributes, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcperiodicevent, SetObjectAttributesErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcperiodicevent, SetObjectAttributesErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcperiodicevent, FinalizeObject,
		TP_ARGS(const char *, msg, const char *, id, const char *, val),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(OBJECT_ID, id) ctf_string(VALUE, val)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcperiodicevent, FinalizeObject, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcperiodicevent, FinalizeObjectErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcperiodicevent, FinalizeObjectErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcperiodicevent, AbortObjectModification,
		TP_ARGS(const char *, msg, const char *, id, const char *, val),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(OBJECT_ID, id) ctf_string(VALUE, val)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcperiodicevent, AbortObjectModification, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcperiodicevent, AbortObjectModificationErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcperiodicevent, AbortObjectModificationErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcperiodicevent, AddTimeplan,
		TP_ARGS(const char *, msg, const char *, id, const char *, o_id),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(TIMEPLAN_ID, id) ctf_string(OBJECT_ID, o_id)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcperiodicevent, AddTimeplan, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcperiodicevent, AddTimeplanErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcperiodicevent, AddTimeplanErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcperiodicevent, ModifyTimeplan,
		TP_ARGS(const char *, msg, const char *, id, const char *, o_id),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(TIMEPLAN_ID, id) ctf_string(OBJECT_ID, o_id)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcperiodicevent, ModifyTimeplan, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcperiodicevent, ModifyTimeplanErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcperiodicevent, ModifyTimeplanErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcperiodicevent, RemoveTimeplan,
		TP_ARGS(const char *, msg, const char *, id, const char *, o_id),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(TIMEPLAN_ID, id) ctf_string(OBJECT_ID, o_id)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcperiodicevent, RemoveTimeplan, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcperiodicevent, RemoveTimeplanErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcperiodicevent, RemoveTimeplanErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcperiodicevent, GetNextScheduledTime,
		TP_ARGS(const char *, msg, const char *, val),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(VALUE, val)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcperiodicevent, GetNextScheduledTime, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcperiodicevent, GetNextScheduledTimeErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcperiodicevent, GetNextScheduledTimeErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcperiodicevent, GetObjectIdFromDn,
		TP_ARGS(const char *, msg, const char *, dn),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(DN, dn)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcperiodicevent, GetObjectIdFromDn, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcperiodicevent, ExecuteAugmentedOperations,
		TP_ARGS(const char *, msg, const char *, val),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(VALUE, val)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcperiodicevent, ExecuteAugmentedOperations, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcperiodicevent, ExecuteAugmentedOperationsErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcperiodicevent, ExecuteAugmentedOperationsErr, TRACE_ERR)

#endif /* _COM_ERICSSON_COMMON_HCF_SRV_HCPERIODICEVENT_H */

/* This part must be outside protection */
#include <lttng/tracepoint-event.h>

#ifdef __cplusplus
}
#endif
