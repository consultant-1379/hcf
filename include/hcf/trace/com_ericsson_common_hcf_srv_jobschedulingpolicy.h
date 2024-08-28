/*
 *	@file com_ericsson_common_hcf_srv_jobschedulingpolicy.h
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
#define TRACEPOINT_PROVIDER com_ericsson_common_hcf_srv_jobschedulingpolicy

#undef TRACEPOINT_INCLUDE_FILE
#define TRACEPOINT_INCLUDE_FILE ./hcf/trace/com_ericsson_common_hcf_srv_jobschedulingpolicy.h

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(_COM_ERICSSON_COMMON_HCF_SRV_JOBSCHEDULINGPOLICY_H) || defined(TRACEPOINT_HEADER_MULTI_READ)
#define _COM_ERICSSON_COMMON_HCF_SRV_JOBSCHEDULINGPOLICY_H

#include <lttng/tracepoint.h>

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_jobschedulingpolicy, FunctionTracing,
		TP_ARGS(const char *, msg, const char *, function),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(FUNC, function)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_jobschedulingpolicy, FunctionTracing, TRACE_INFO)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_jobschedulingpolicy, ConvertDatetimeIntoIso8601Format,
		TP_ARGS(const char *, msg, const char *, val),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(VALUE, val)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_jobschedulingpolicy, ConvertDatetimeIntoIso8601Format, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_jobschedulingpolicy, ConvertIso8601IntoDatetimeFormat,
		TP_ARGS(const char *, msg, const char *, val),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(VALUE, val)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_jobschedulingpolicy, ConvertIso8601IntoDatetimeFormat, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_jobschedulingpolicy, ConvertIso8601IntoDatetimeFormatErr,
		TP_ARGS(const char *, msg, int, retval, int, expval),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, RETURN_VALUE, retval) ctf_integer(int, EXPECTED_VALUE, expval)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_jobschedulingpolicy, ConvertIso8601IntoDatetimeFormatErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_jobschedulingpolicy, ConvertTimeStringIntoDatetimeFormat,
		TP_ARGS(const char *, msg, const char *, val),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(VALUE, val)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_jobschedulingpolicy, ConvertTimeStringIntoDatetimeFormat, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_jobschedulingpolicy, ConvertTimeStringIntoDatetimeFormatErr,
		TP_ARGS(const char *, msg, int, retval, int, expval),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, RETURN_VALUE, retval) ctf_integer(int, EXPECTED_VALUE, expval)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_jobschedulingpolicy, ConvertTimeStringIntoDatetimeFormatErr, TRACE_ERR)

#endif /* _COM_ERICSSON_COMMON_HCF_SRV_JOBSCHEDULINGPOLICY_H */

/* This part must be outside protection */
#include <lttng/tracepoint-event.h>

#ifdef __cplusplus
}
#endif
