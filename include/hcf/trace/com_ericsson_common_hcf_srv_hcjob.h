/*
 *	@file com_ericsson_common_hcf_srv_hcjob.h
 *	@brief
 *	@author xcasale (Alessio Cascone)
 *	@date 2015-10-23
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
 *	| 2015-001 | 2015-10-23 | xcasale      | Creation and first revision.        |
 *	+==========+============+==============+=====================================+
 *
 */

#undef TRACEPOINT_PROVIDER
#define TRACEPOINT_PROVIDER com_ericsson_common_hcf_srv_hcjob

#undef TRACEPOINT_INCLUDE_FILE
#define TRACEPOINT_INCLUDE_FILE ./hcf/trace/com_ericsson_common_hcf_srv_hcjob.h

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(_COM_ERICSSON_COMMON_HCF_SRV_HCJOB_H) || defined(TRACEPOINT_HEADER_MULTI_READ)
#define _COM_ERICSSON_COMMON_HCF_SRV_HCJOB_H

#include <lttng/tracepoint.h>

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcjob, FunctionTracing,
		TP_ARGS(const char *, msg, const char *, function),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(FUNC, function)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcjob, FunctionTracing, TRACE_INFO)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcjob, SetObjectAttributes,
		TP_ARGS(const char *, msg, const char *, val),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(VALUE, val)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcjob, SetObjectAttributes, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcjob, SetObjectAttributesErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcjob, SetObjectAttributesErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcjob, FinalizeObject,
		TP_ARGS(const char *, msg, const char *, id),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(OBJECT_ID, id)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcjob, FinalizeObject, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcjob, FinalizeObjectErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcjob, FinalizeObjectErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcjob, ExecuteAdminOperation,
		TP_ARGS(const char *, msg, const char *, id, unsigned long, op_id),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(OBJECT_ID, id) ctf_integer(unsigned long, OPERATION_ID, op_id)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcjob, ExecuteAdminOperation, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcjob, ExecuteAdminOperationErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcjob, ExecuteAdminOperationErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcjob, ClearFailedRules,
		TP_ARGS(const char *, msg, const char *, val),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(VALUE, val)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcjob, ClearFailedRules, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcjob, ClearFailedRulesErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcjob, ClearFailedRulesErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcjob, SetFailedRule,
		TP_ARGS(const char *, msg, const char *, id, const char *, rule, const char *, reason),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(OBJECT_ID, id) ctf_string(RULE_ID, rule) ctf_string(REASON, reason)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcjob, SetFailedRule, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcjob, SetFailedRuleErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcjob, SetFailedRuleErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcjob, Execute,
		TP_ARGS(const char *, msg, const char *, id, const char *, job, const char *, uri, const char *, pwd, int, sch),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(OBJECT_ID, id) ctf_string(JOB_TO_TRIGGER, job) ctf_string(EXPORT_URI, uri) ctf_string(EXPORT_PASSWORD, pwd) ctf_integer(int, SCHEDULED_EXEC, sch)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcjob, Execute, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcjob, ExecuteAugmentedOperations,
		TP_ARGS(const char *, msg, const char *, val),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(VALUE, val)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcjob, ExecuteAugmentedOperations, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcjob, ExecuteAugmentedOperationsErr,
		TP_ARGS(const char *, msg, int, err, const char *, err_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err) ctf_string(ERROR_MSG, err_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcjob, ExecuteAugmentedOperationsErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcjob, CreateProgressReportAndAttachToParent,
		TP_ARGS(const char *, msg, const char *, parent, const char *, dn),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(PARENT_DN, parent) ctf_string(OBJECT_DN, dn)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcjob, CreateProgressReportAndAttachToParent, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcjob, CreateProgressReportAndAttachToParentErr,
		TP_ARGS(const char *, msg, int, err, const char *, err_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err) ctf_string(ERROR_MSG, err_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcjob, CreateProgressReportAndAttachToParentErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcjob, InitProgressReport,
		TP_ARGS(const char *, msg, const char *, dn, const char *, val),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(OBJECT_DN, dn) ctf_string(VALUE, val)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcjob, InitProgressReport, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcjob, InitProgressReportErr,
		TP_ARGS(const char *, msg, int, err, const char *, err_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err) ctf_string(ERROR_MSG, err_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcjob, InitProgressReportErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcjob, UpdateProgressReport,
		TP_ARGS(const char *, msg, const char *, dn, unsigned, perc, const char *, val),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(OBJECT_DN, dn) ctf_integer(unsigned, PERCENTAGE, perc) ctf_string(VALUE, val)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcjob, UpdateProgressReport, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcjob, UpdateProgressReportErr,
		TP_ARGS(const char *, msg, int, err, const char *, err_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err) ctf_string(ERROR_MSG, err_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcjob, UpdateProgressReportErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcjob, FinalizeProgressReport,
		TP_ARGS(const char *, msg, const char *, dn, int, succ, const char *, mess, const char *, val),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(OBJECT_DN, dn) ctf_integer(int, IS_SUCCESS, succ) ctf_string(MESSAGE, mess) ctf_string(VALUE, val)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcjob, FinalizeProgressReport, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcjob, FinalizeProgressReportErr,
		TP_ARGS(const char *, msg, int, err, const char *, err_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err) ctf_string(ERROR_MSG, err_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcjob, FinalizeProgressReportErr, TRACE_ERR)

#endif /* _COM_ERICSSON_COMMON_HCF_SRV_HCJOB_H */

/* This part must be outside protection */
#include <lttng/tracepoint-event.h>

#ifdef __cplusplus
}
#endif
