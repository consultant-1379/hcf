/*
 *	@file com_ericsson_common_hcf_srv_datetime.h
 *	@brief
 *	@author xcasale (Alessio Cascone)
 *	@date 2015-10-20
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
 *	| 2015-001 | 2015-10-20 | xcasale      | Creation and first revision.        |
 *	+==========+============+==============+=====================================+
 *
 */

#undef TRACEPOINT_PROVIDER
#define TRACEPOINT_PROVIDER com_ericsson_common_hcf_srv_datetime

#undef TRACEPOINT_INCLUDE_FILE
#define TRACEPOINT_INCLUDE_FILE ./hcf/trace/com_ericsson_common_hcf_srv_datetime.h

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(_COM_ERICSSON_COMMON_HCF_SRV_DATETIME_H) || defined(TRACEPOINT_HEADER_MULTI_READ)
#define _COM_ERICSSON_COMMON_HCF_SRV_DATETIME_H

#include <lttng/tracepoint.h>

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_datetime, FunctionTracing,
		TP_ARGS(const char *, msg, const char *, function),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(FUNC, function)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_datetime, FunctionTracing, TRACE_INFO)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_datetime, SetDate,
		TP_ARGS(const char *, msg, unsigned, y, unsigned, m, unsigned, d),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(unsigned, YEAR, y) ctf_integer(unsigned, MONTH, m) ctf_integer(unsigned, DAY, d)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_datetime, SetDate, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_datetime, SetDateErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_datetime, SetDateErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_datetime, SetTime,
		TP_ARGS(const char *, msg, unsigned, h, unsigned, m, unsigned, s),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(unsigned, HOUR, h) ctf_integer(unsigned, MINUTE, m) ctf_integer(unsigned, SECOND, s)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_datetime, SetTime, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_datetime, SetTimeErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_datetime, SetTimeErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_datetime, SetDateTime,
		TP_ARGS(const char *, msg, unsigned, y, unsigned, mo, unsigned, d, unsigned, h, unsigned, mi, unsigned, s),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(unsigned, YEAR, y) ctf_integer(unsigned, MONTH, mo) ctf_integer(unsigned, DAY, d) ctf_integer(unsigned, HOUR, h) ctf_integer(unsigned, MINUTE, mi) ctf_integer(unsigned, SECOND, s)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_datetime, SetDateTime, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_datetime, SetDateTimeErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_datetime, SetDateTimeErr, TRACE_ERR)

#endif /* _COM_ERICSSON_COMMON_HCF_SRV_DATETIME_H */

/* This part must be outside protection */
#include <lttng/tracepoint-event.h>

#ifdef __cplusplus
}
#endif
