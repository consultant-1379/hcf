/*
 *	@file com_ericsson_common_hcf_srv_rulecallbackshandler.h
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
#define TRACEPOINT_PROVIDER com_ericsson_common_hcf_srv_rulecallbackshandler

#undef TRACEPOINT_INCLUDE_FILE
#define TRACEPOINT_INCLUDE_FILE ./hcf/trace/com_ericsson_common_hcf_srv_rulecallbackshandler.h

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(_COM_ERICSSON_COMMON_HCF_SRV_RULECALLBACKSHANDLER_H) || defined(TRACEPOINT_HEADER_MULTI_READ)
#define _COM_ERICSSON_COMMON_HCF_SRV_RULECALLBACKSHANDLER_H

#include <lttng/tracepoint.h>

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_rulecallbackshandler, FunctionTracing,
		TP_ARGS(const char *, msg, const char *, function),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(FUNC, function)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_rulecallbackshandler, FunctionTracing, TRACE_INFO)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_rulecallbackshandler, ApplyFilter,
		TP_ARGS(const char *, msg, int, op),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, OPERATION_ID, op)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_rulecallbackshandler, ApplyFilter, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_rulecallbackshandler, Apply,
		TP_ARGS(const char *, msg, const char *, filter, const char *, value),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(FILTER_NAME, filter) ctf_string(VALUE, value)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_rulecallbackshandler, Apply, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_rulecallbackshandler, ApplyErr,
		TP_ARGS(const char *, msg, const char *, filter, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(FILTER_NAME, filter) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_rulecallbackshandler, ApplyErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_rulecallbackshandler, RuleCallbacksHandler,
		TP_ARGS(const char *, msg),
		TP_FIELDS(ctf_string(MSG, msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_rulecallbackshandler, RuleCallbacksHandler, TRACE_NOTICE)

#endif /* _COM_ERICSSON_COMMON_HCF_SRV_RULECALLBACKSHANDLER_H */

/* This part must be outside protection */
#include <lttng/tracepoint-event.h>

#ifdef __cplusplus
}
#endif
