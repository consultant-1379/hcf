/*
 *	@file com_ericsson_common_hcf_srv_outputfilewriter.h
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
#define TRACEPOINT_PROVIDER com_ericsson_common_hcf_srv_outputfilewriter

#undef TRACEPOINT_INCLUDE_FILE
#define TRACEPOINT_INCLUDE_FILE ./hcf/trace/com_ericsson_common_hcf_srv_outputfilewriter.h

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(_COM_ERICSSON_COMMON_HCF_SRV_OUTPUTFILEWRITER_H) || defined(TRACEPOINT_HEADER_MULTI_READ)
#define _COM_ERICSSON_COMMON_HCF_SRV_OUTPUTFILEWRITER_H

#include <lttng/tracepoint.h>

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_outputfilewriter, FunctionTracing,
		TP_ARGS(const char *, msg, const char *, function),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(FUNC, function)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_outputfilewriter, FunctionTracing, TRACE_INFO)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_outputfilewriter, Write,
		TP_ARGS(const char *, msg, const char *, file),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(FILE, file)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_outputfilewriter, Write, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_outputfilewriter, WriteErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_outputfilewriter, WriteErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_outputfilewriter, WriteFailedRules,
		TP_ARGS(const char *, msg, const char *, value),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(VALUE, value)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_outputfilewriter, WriteFailedRules, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_outputfilewriter, WriteFailedRulesErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_outputfilewriter, WriteFailedRulesErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_outputfilewriter, WriteSuccessRules,
		TP_ARGS(const char *, msg, const char *, value),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(VALUE, value)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_outputfilewriter, WriteSuccessRules, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_outputfilewriter, WriteSuccessRulesErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_outputfilewriter, WriteSuccessRulesErr, TRACE_ERR)

#endif /* _COM_ERICSSON_COMMON_HCF_SRV_OUTPUTFILEWRITER_H */

/* This part must be outside protection */
#include <lttng/tracepoint-event.h>

#ifdef __cplusplus
}
#endif
