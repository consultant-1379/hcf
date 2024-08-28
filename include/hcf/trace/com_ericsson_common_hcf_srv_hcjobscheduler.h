/*
 *	@file com_ericsson_common_hcf_srv_hcjobscheduler.h
 *	@brief
 *	@author xcasale (Alessio Cascone)
 *	@date 2015-10-22
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
 *	| 2015-001 | 2015-10-22 | xcasale      | Creation and first revision.        |
 *	+==========+============+==============+=====================================+
 *
 */

#undef TRACEPOINT_PROVIDER
#define TRACEPOINT_PROVIDER com_ericsson_common_hcf_srv_hcjobscheduler

#undef TRACEPOINT_INCLUDE_FILE
#define TRACEPOINT_INCLUDE_FILE ./hcf/trace/com_ericsson_common_hcf_srv_hcjobscheduler.h

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(_COM_ERICSSON_COMMON_HCF_SRV_HCJOBSCHEDULER_H) || defined(TRACEPOINT_HEADER_MULTI_READ)
#define _COM_ERICSSON_COMMON_HCF_SRV_HCJOBSCHEDULER_H

#include <lttng/tracepoint.h>

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcjobscheduler, FunctionTracing,
		TP_ARGS(const char *, msg, const char *, function),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(FUNC, function)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcjobscheduler, FunctionTracing, TRACE_INFO)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcjobscheduler, SetObjectAttributes,
		TP_ARGS(const char *, msg, const char *, val),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(VALUE, val)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcjobscheduler, SetObjectAttributes, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcjobscheduler, SetObjectAttributesErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcjobscheduler, SetObjectAttributesErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcjobscheduler, FinalizeObject,
		TP_ARGS(const char *, msg, const char *, id, const char *, val),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(OBJECT_ID, id) ctf_string(VALUE, val)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcjobscheduler, FinalizeObject, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcjobscheduler, FinalizeObjectErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcjobscheduler, FinalizeObjectErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcjobscheduler, AbortObjectModification,
		TP_ARGS(const char *, msg, const char *, id, const char *, val),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(OBJECT_ID, id) ctf_string(VALUE, val)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcjobscheduler, AbortObjectModification, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcjobscheduler, AbortObjectModificationErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcjobscheduler, AbortObjectModificationErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcjobscheduler, HandleAlarm,
		TP_ARGS(const char *, msg, const char *, id),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(OBJECT_ID, id)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcjobscheduler, HandleAlarm, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcjobscheduler, HandleAlarmErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcjobscheduler, HandleAlarmErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcjobscheduler, GetObjectIdFromDn,
		TP_ARGS(const char *, msg, const char *, dn),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(DN, dn)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcjobscheduler, GetObjectIdFromDn, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcjobscheduler, AddSchedulingPolicy,
		TP_ARGS(const char *, msg, const char *, id, const char *, o_id),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(TIMEPLAN_ID, id) ctf_string(OBJECT_ID, o_id)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcjobscheduler, AddSchedulingPolicy, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcjobscheduler, AddSchedulingPolicyErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcjobscheduler, AddSchedulingPolicyErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcjobscheduler, ModifySchedulingPolicy,
		TP_ARGS(const char *, msg, const char *, id, const char *, o_id),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(TIMEPLAN_ID, id) ctf_string(OBJECT_ID, o_id)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcjobscheduler, ModifySchedulingPolicy, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcjobscheduler, ModifySchedulingPolicyErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcjobscheduler, ModifySchedulingPolicyErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcjobscheduler, RemoveSchedulingPolicy,
		TP_ARGS(const char *, msg, const char *, id, const char *, o_id),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(TIMEPLAN_ID, id) ctf_string(OBJECT_ID, o_id)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcjobscheduler, RemoveSchedulingPolicy, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcjobscheduler, RemoveSchedulingPolicyErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcjobscheduler, RemoveSchedulingPolicyErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcjobscheduler, ChooseNextScheduledTime,
		TP_ARGS(const char *, msg, const char *, val),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(VALUE, val)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcjobscheduler, ChooseNextScheduledTime, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcjobscheduler, ChooseNextScheduledTimeErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcjobscheduler, ChooseNextScheduledTimeErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcjobscheduler, SetNextScheduledTime,
		TP_ARGS(const char *, msg, const char *, val),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(VALUE, val)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcjobscheduler, SetNextScheduledTime, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcjobscheduler, SetNextScheduledTimeErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcjobscheduler, SetNextScheduledTimeErr, TRACE_ERR)

#endif /* _COM_ERICSSON_COMMON_HCF_SRV_HCJOBSCHEDULER_H */

/* This part must be outside protection */
#include <lttng/tracepoint-event.h>

#ifdef __cplusplus
}
#endif
