/*
 *	@file com_ericsson_common_hcf_srv_ruleexecutor.h
 *	@brief
 *	@author xcasale (Alessio Cascone)
 *	@date 2015-11-04
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
 *	| 2015-001 | 2015-11-04 | xcasale      | Creation and first revision.        |
 *	+==========+============+==============+=====================================+
 *
 */

#undef TRACEPOINT_PROVIDER
#define TRACEPOINT_PROVIDER com_ericsson_common_hcf_srv_ruleexecutor

#undef TRACEPOINT_INCLUDE_FILE
#define TRACEPOINT_INCLUDE_FILE ./hcf/trace/com_ericsson_common_hcf_srv_ruleexecutor.h

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(_COM_ERICSSON_COMMON_HCF_SRV_RULEEXECUTOR_H) || defined(TRACEPOINT_HEADER_MULTI_READ)
#define _COM_ERICSSON_COMMON_HCF_SRV_RULEEXECUTOR_H

#include <lttng/tracepoint.h>

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_ruleexecutor, FunctionTracing,
		TP_ARGS(const char *, msg, const char *, function),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(FUNC, function)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_ruleexecutor, FunctionTracing, TRACE_INFO)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_ruleexecutor, CheckHeaders,
		TP_ARGS(const char *, msg, const char *, value),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(VALUE, value)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_ruleexecutor, CheckHeaders, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_ruleexecutor, CheckHeadersErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_ruleexecutor, CheckHeadersErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_ruleexecutor, CheckRequiredColumnsByHeader,
		TP_ARGS(const char *, msg, const char *, value),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(VALUE, value)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_ruleexecutor, CheckRequiredColumnsByHeader, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_ruleexecutor, CheckRequiredColumnsByHeaderErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_ruleexecutor, CheckRequiredColumnsByHeaderErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_ruleexecutor, ExtractPrintoutByHeader,
		TP_ARGS(const char *, msg, const char *, value),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(VALUE, value)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_ruleexecutor, ExtractPrintoutByHeader, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_ruleexecutor, ExtractPrintoutByHeaderErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_ruleexecutor, ExtractPrintoutByHeaderErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_ruleexecutor, LoadMatrix,
		TP_ARGS(const char *, msg, const char *, value),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(VALUE, value)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_ruleexecutor, LoadMatrix, TRACE_INFO)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_ruleexecutor, LoadMatrixErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_ruleexecutor, LoadMatrixErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_ruleexecutor, PrintMatrix,
		TP_ARGS(const char *, msg, const char *, value),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(VALUE, value)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_ruleexecutor, PrintMatrix, TRACE_INFO)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_ruleexecutor, FindHeaderIndexByString,
		TP_ARGS(const char *, msg, const char *, value),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(VALUE, value)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_ruleexecutor, FindHeaderIndexByString, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_ruleexecutor, FindHeaderIndexByStringErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_ruleexecutor, FindHeaderIndexByStringErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_ruleexecutor, ApplyOperationOnFilters,
		TP_ARGS(const char *, msg, int, op),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, OPERATION, op)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_ruleexecutor, ApplyOperationOnFilters, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_ruleexecutor, ApplyOperationOnFiltersErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_ruleexecutor, ApplyOperationOnFiltersErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_ruleexecutor, AnalyzeOSContainer,
		TP_ARGS(const char *, msg),
		TP_FIELDS(ctf_string(MSG, msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_ruleexecutor, AnalyzeOSContainer, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_ruleexecutor, AnalyzeOSContainerErr,
		TP_ARGS(const char *, msg),
		TP_FIELDS(ctf_string(MSG, msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_ruleexecutor, AnalyzeOSContainerErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_ruleexecutor, AnalyzeFilterContainer,
		TP_ARGS(const char *, msg),
		TP_FIELDS(ctf_string(MSG, msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_ruleexecutor, AnalyzeFilterContainer, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_ruleexecutor, AnalyzeFilterContainerErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_ruleexecutor, AnalyzeFilterContainerErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_ruleexecutor, AnalyzeEvaluationContainerByLogicalOperator,
		TP_ARGS(const char *, msg, int, val),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, VALUE, val)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_ruleexecutor, AnalyzeEvaluationContainerByLogicalOperator, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_ruleexecutor, AnalyzeEvaluationContainer,
		TP_ARGS(const char *, msg, int, val),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, VALUE, val)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_ruleexecutor, AnalyzeEvaluationContainer, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_ruleexecutor, AnalyzeRuleContainer,
		TP_ARGS(const char *, msg, const char *, value),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(VALUE, value)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_ruleexecutor, AnalyzeRuleContainer, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_ruleexecutor, AnalyzeRuleContainerErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_ruleexecutor, AnalyzeRuleContainerErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_ruleexecutor, EvaluateRule,
		TP_ARGS(const char *, msg, int, val),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, VALUE, val)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_ruleexecutor, EvaluateRule, TRACE_NOTICE)

#endif /* _COM_ERICSSON_COMMON_HCF_SRV_RULEEXECUTOR_H */

/* This part must be outside protection */
#include <lttng/tracepoint-event.h>

#ifdef __cplusplus
}
#endif
