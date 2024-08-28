/*
 *	@file com_ericsson_common_hcf_core_periodicalarmtimeplan.h
 *	@brief
 *	@author xcasale (Alessio Cascone)
 *	@date 2015-08-12
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
 *	| 2015-001 | 2015-08-12 | xcasale      | Creation and first revision.        |
 *	+==========+============+==============+=====================================+
 *
 */

#undef TRACEPOINT_PROVIDER
#define TRACEPOINT_PROVIDER com_ericsson_common_hcf_core_periodicalarmtimeplan

#undef TRACEPOINT_INCLUDE_FILE
#define TRACEPOINT_INCLUDE_FILE ./hcf/trace/com_ericsson_common_hcf_core_periodicalarmtimeplan.h

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(_COM_ERICSSON_COMMON_HCF_CORE_PERIODICALARMTIMEPLAN_H) || defined(TRACEPOINT_HEADER_MULTI_READ)
#define _COM_ERICSSON_COMMON_HCF_CORE_PERIODICALARMTIMEPLAN_H

#include <lttng/tracepoint.h>

TRACEPOINT_EVENT(com_ericsson_common_hcf_core_periodicalarmtimeplan, FunctionTracing,
		TP_ARGS(const char *, msg, const char *, function),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(FUNC, function)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_core_periodicalarmtimeplan, FunctionTracing, TRACE_INFO)

TRACEPOINT_EVENT(com_ericsson_common_hcf_core_periodicalarmtimeplan, GetNextTimerPoint,
		TP_ARGS(const char *, msg, const char *, time, unsigned, mo, unsigned, w, unsigned, d, unsigned, h, unsigned, mi),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(TIME, time) ctf_integer(unsigned, MONTHS, mo) ctf_integer(unsigned, WEEKS, w) ctf_integer(unsigned, DAYS, d) ctf_integer(unsigned, HOURS, h) ctf_integer(unsigned, MINUTES, mi)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_core_periodicalarmtimeplan, GetNextTimerPoint, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_core_periodicalarmtimeplan, GetNextTimerPointErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_core_periodicalarmtimeplan, GetNextTimerPointErr, TRACE_ERR)

#endif /* _COM_ERICSSON_COMMON_HCF_CORE_PERIODICALARMTIMEPLAN_H */

/* This part must be outside protection */
#include <lttng/tracepoint-event.h>

#ifdef __cplusplus
}
#endif
