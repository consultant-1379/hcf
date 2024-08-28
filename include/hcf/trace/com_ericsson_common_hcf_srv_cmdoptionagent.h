/*
 *	@file com_ericsson_common_hcf_srv_cmdoptionagent.h
 *	@brief
 *	@author xcasale (Alessio Cascone)
 *	@date 2015-10-13
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
 *	| 2015-001 | 2015-10-13 | xcasale      | Creation and first revision.        |
 *	+==========+============+==============+=====================================+
 *
 */

#undef TRACEPOINT_PROVIDER
#define TRACEPOINT_PROVIDER com_ericsson_common_hcf_srv_cmdoptionagent

#undef TRACEPOINT_INCLUDE_FILE
#define TRACEPOINT_INCLUDE_FILE ./hcf/trace/com_ericsson_common_hcf_srv_cmdoptionagent.h

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(_COM_ERICSSON_COMMON_HCF_SRV_CMDOPTIONAGENT_H) || defined(TRACEPOINT_HEADER_MULTI_READ)
#define _COM_ERICSSON_COMMON_HCF_SRV_CMDOPTIONAGENT_H

#include <lttng/tracepoint.h>

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_cmdoptionagent, FunctionTracing,
		TP_ARGS(const char *, msg, const char *, function),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(FUNC, function)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_cmdoptionagent, FunctionTracing, TRACE_INFO)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_cmdoptionagent, Parsing,
		TP_ARGS(int, opt_code, int, long_index, int, optind),
		TP_FIELDS(ctf_integer(int, OPT_CODE, opt_code) ctf_integer(int, LONG_INDEX, long_index) ctf_integer(int, OPTIND, optind)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_cmdoptionagent, Parsing, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_cmdoptionagent, ParsingErr,
		TP_ARGS(const char *, msg, const char *, val),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(VALUE, val)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_cmdoptionagent, ParsingErr, TRACE_ERR)

#endif /* _COM_ERICSSON_COMMON_HCF_SRV_CMDOPTIONAGENT_H */

/* This part must be outside protection */
#include <lttng/tracepoint-event.h>

#ifdef __cplusplus
}
#endif
