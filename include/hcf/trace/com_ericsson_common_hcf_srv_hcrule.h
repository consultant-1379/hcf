/*
 *	@file com_ericsson_common_hcf_srv_hcrule.h
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
#define TRACEPOINT_PROVIDER com_ericsson_common_hcf_srv_hcrule

#undef TRACEPOINT_INCLUDE_FILE
#define TRACEPOINT_INCLUDE_FILE ./hcf/trace/com_ericsson_common_hcf_srv_hcrule.h

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(_COM_ERICSSON_COMMON_HCF_SRV_HCRULE_H) || defined(TRACEPOINT_HEADER_MULTI_READ)
#define _COM_ERICSSON_COMMON_HCF_SRV_HCRULE_H

#include <lttng/tracepoint.h>

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcrule, FunctionTracing,
		TP_ARGS(const char *, msg, const char *, function),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(FUNC, function)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcrule, FunctionTracing, TRACE_INFO)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcrule, SetObjectAttributes,
		TP_ARGS(const char *, msg, const char *, val),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(VALUE, val)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcrule, SetObjectAttributes, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcrule, SetObjectAttributesErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcrule, SetObjectAttributesErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcrule, FinalizeObject,
		TP_ARGS(const char *, msg, const char *, id),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(OBJECT_ID, id)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcrule, FinalizeObject, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcrule, AbortObjectModification,
		TP_ARGS(const char *, msg, const char *, id),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(OBJECT_ID, id)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcrule, AbortObjectModification, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcrule, CreateHcRule,
		TP_ARGS(const char *, msg, const char *, id, int, num_of_cat, int, sev, const char *, recomm, const char *, desc, const char *, name, int, err_if),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(RULE_ID, id) ctf_integer(int, NUM_OF_CATEGORIES, num_of_cat) ctf_integer(int, SEVERITY, sev) ctf_string(RECOMMENDED_ACTION, recomm) ctf_string(DESCRIPTION, desc) ctf_string(NAME, name) ctf_integer(int, ERROR_IF_EXISTS, err_if)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcrule, CreateHcRule, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcrule, CreateHcRuleErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcrule, CreateHcRuleErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcrule, AddRuleAttribute,
		TP_ARGS(const char *, msg, const char *, job, const char *, name, const char *, value),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(JOB, job) ctf_string(NAME, name) ctf_string(VALUE, value)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcrule, AddRuleAttribute, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcrule, AddRuleAttributeErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcrule, AddRuleAttributeErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcrule, ModifyRuleAttribute,
		TP_ARGS(const char *, msg, const char *, job, const char *, name, const char *, value),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(JOB, job) ctf_string(NAME, name) ctf_string(VALUE, value)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcrule, ModifyRuleAttribute, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcrule, RemoveRuleAttribute,
		TP_ARGS(const char *, msg, const char *, job, const char *, name),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(JOB, job) ctf_string(NAME, name)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcrule, RemoveRuleAttribute, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcrule, GetRuleAttributes,
		TP_ARGS(const char *, msg, const char *, val1, const char *, val2),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(VALUE1, val1) ctf_string(VALUE2, val2)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcrule, GetRuleAttributes, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcrule, CalculateTargets,
		TP_ARGS(const char *, msg, const char *, val),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(VALUE, val)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcrule, CalculateTargets, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_hcrule, CalculateTargetsErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_hcrule, CalculateTargetsErr, TRACE_ERR)

#endif /* _COM_ERICSSON_COMMON_HCF_SRV_HCRULE_H */

/* This part must be outside protection */
#include <lttng/tracepoint-event.h>

#ifdef __cplusplus
}
#endif
