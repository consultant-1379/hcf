/*
 *	@file com_ericsson_common_hcf_srv_datetimeparser.h
 *	@brief
 *	@author xcasale (Alessio Cascone)
 *	@date 2015-10-15
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
 *	| 2015-001 | 2015-10-15 | xcasale      | Creation and first revision.        |
 *	+==========+============+==============+=====================================+
 *
 */

#undef TRACEPOINT_PROVIDER
#define TRACEPOINT_PROVIDER com_ericsson_common_hcf_srv_datetimeparser

#undef TRACEPOINT_INCLUDE_FILE
#define TRACEPOINT_INCLUDE_FILE ./hcf/trace/com_ericsson_common_hcf_srv_datetimeparser.h

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(_COM_ERICSSON_COMMON_HCF_SRV_DATETIMEPARSER_H) || defined(TRACEPOINT_HEADER_MULTI_READ)
#define _COM_ERICSSON_COMMON_HCF_SRV_DATETIMEPARSER_H

#include <lttng/tracepoint.h>

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_datetimeparser, FunctionTracing,
		TP_ARGS(const char *, msg, const char *, function),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(FUNC, function)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_datetimeparser, FunctionTracing, TRACE_INFO)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_datetimeparser, GetDateTimeFormatToScan,
		TP_ARGS(const char *, msg, const char *, val),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(VALUE, val)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_datetimeparser, GetDateTimeFormatToScan, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_datetimeparser, SetDateTimeParsed,
		TP_ARGS(const char *, msg, int, fmt_pos, const char *, fmt, const char *, date),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer_hex(int, FORMAT_POSITION, fmt_pos) ctf_string(FORMAT, fmt) ctf_string(DATE, date)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_datetimeparser, SetDateTimeParsed, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_datetimeparser, SetDateTimeParsedErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_datetimeparser, SetDateTimeParsedErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_datetimeparser, SetDateTime,
		TP_ARGS(const char *, msg, const char *, fmt, const char *, date, const char *, t),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(FORMAT, fmt) ctf_string(DATE, date) ctf_string(TIME, t)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_datetimeparser, SetDateTime, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_datetimeparser, SetDateTimeErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_datetimeparser, SetDateTimeErr, TRACE_ERR)

#endif /* _COM_ERICSSON_COMMON_HCF_SRV_DATETIMEPARSER_H */

/* This part must be outside protection */
#include <lttng/tracepoint-event.h>

#ifdef __cplusplus
}
#endif
