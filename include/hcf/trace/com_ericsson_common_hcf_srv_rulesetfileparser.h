/*
 *	@file com_ericsson_common_hcf_srv_rulesetfileparser.h
 *	@brief
 *	@author xcasale (Alessio Cascone)
 *	@date 2015-11-09
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
 *	| 2015-001 | 2015-11-09 | xcasale      | Creation and first revision.        |
 *	+==========+============+==============+=====================================+
 *
 */

#undef TRACEPOINT_PROVIDER
#define TRACEPOINT_PROVIDER com_ericsson_common_hcf_srv_rulesetfileparser

#undef TRACEPOINT_INCLUDE_FILE
#define TRACEPOINT_INCLUDE_FILE ./hcf/trace/com_ericsson_common_hcf_srv_rulesetfileparser.h

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(_COM_ERICSSON_COMMON_HCF_SRV_RULESETFILEPARSER_H) || defined(TRACEPOINT_HEADER_MULTI_READ)
#define _COM_ERICSSON_COMMON_HCF_SRV_RULESETFILEPARSER_H

#include <lttng/tracepoint.h>

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_rulesetfileparser, FunctionTracing,
		TP_ARGS(const char *, msg, const char *, function),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(FUNC, function)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_rulesetfileparser, FunctionTracing, TRACE_INFO)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_rulesetfileparser, Initialize,
		TP_ARGS(const char *, msg),
		TP_FIELDS(ctf_string(MSG, msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_rulesetfileparser, Initialize, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_rulesetfileparser, Finalize,
		TP_ARGS(const char *, msg),
		TP_FIELDS(ctf_string(MSG, msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_rulesetfileparser, Finalize, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_rulesetfileparser, Parse,
		TP_ARGS(const char *, msg, const char *, value),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(VALUE, value)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_rulesetfileparser, Parse, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_rulesetfileparser, ParseErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_rulesetfileparser, ParseErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_rulesetfileparser, Validate,
		TP_ARGS(const char *, msg, const char *, value),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(VALUE, value)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_rulesetfileparser, Validate, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_rulesetfileparser, ValidateErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_rulesetfileparser, ValidateErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_rulesetfileparser, ParseRule,
		TP_ARGS(const char *, msg, const char *, value),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(VALUE, value)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_rulesetfileparser, ParseRule, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_rulesetfileparser, ParseRuleErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_rulesetfileparser, ParseRuleErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_rulesetfileparser, ParseRuleAttributes,
		TP_ARGS(const char *, msg, const char *, value),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(VALUE, value)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_rulesetfileparser, ParseRuleAttributes, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_rulesetfileparser, ParseRuleAttributesErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_rulesetfileparser, ParseRuleAttributesErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_rulesetfileparser, ParseRuleCheck,
		TP_ARGS(const char *, msg, const char *, value),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(VALUE, value)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_rulesetfileparser, ParseRuleCheck, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_rulesetfileparser, ParseRuleCheckErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_rulesetfileparser, ParseRuleCheckErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_rulesetfileparser, ParseRulePrintout,
		TP_ARGS(const char *, msg, const char *, value),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(VALUE, value)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_rulesetfileparser, ParseRulePrintout, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_rulesetfileparser, ParseRulePrintoutErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_rulesetfileparser, ParseRulePrintoutErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_rulesetfileparser, ParseRuleFilter,
		TP_ARGS(const char *, msg, const char *, value),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(VALUE, value)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_rulesetfileparser, ParseRuleFilter, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_rulesetfileparser, ParseRuleFilterErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_rulesetfileparser, ParseRuleFilterErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_rulesetfileparser, ParseRuleEval,
		TP_ARGS(const char *, msg, const char *, value),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(VALUE, value)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_rulesetfileparser, ParseRuleEval, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_rulesetfileparser, ParseRuleEvalErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_rulesetfileparser, ParseRuleEvalErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_rulesetfileparser, LogXMLMessage,
		TP_ARGS(const char *, msg),
		TP_FIELDS(ctf_string(MSG, msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_rulesetfileparser, LogXMLMessage, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_rulesetfileparser, GetRSFInfo,
		TP_ARGS(const char *, msg, const char *, value),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(VALUE, value)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_rulesetfileparser, GetRSFInfo, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_rulesetfileparser, GetRSFInfoErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_rulesetfileparser, GetRSFInfoErr, TRACE_ERR)

#endif /* _COM_ERICSSON_COMMON_HCF_SRV_RULESETFILEPARSER_H */

/* This part must be outside protection */
#include <lttng/tracepoint-event.h>

#ifdef __cplusplus
}
#endif
