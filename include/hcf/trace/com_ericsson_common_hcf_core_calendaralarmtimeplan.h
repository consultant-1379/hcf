/*
 *	@file com_ericsson_common_hcf_core_calendaralarmtimeplan.h
 *	@brief
 *	@author xcasale (Alessio Cascone)
 *	@date 2015-08-10
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
 *	| 2015-001 | 2015-08-10 | xcasale      | Creation and first revision.        |
 *	+==========+============+==============+=====================================+
 *
 */

#undef TRACEPOINT_PROVIDER
#define TRACEPOINT_PROVIDER com_ericsson_common_hcf_core_calendaralarmtimeplan

#undef TRACEPOINT_INCLUDE_FILE
#define TRACEPOINT_INCLUDE_FILE ./hcf/trace/com_ericsson_common_hcf_core_calendaralarmtimeplan.h

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(_COM_ERICSSON_COMMON_HCF_CORE_CALENDARALARMTIMEPLAN_H) || defined(TRACEPOINT_HEADER_MULTI_READ)
#define _COM_ERICSSON_COMMON_HCF_CORE_CALENDARALARMTIMEPLAN_H

#include <lttng/tracepoint.h>

TRACEPOINT_EVENT(com_ericsson_common_hcf_core_calendaralarmtimeplan, FunctionTracing,
		TP_ARGS(const char *, msg, const char *, function),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(FUNC, function)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_core_calendaralarmtimeplan, FunctionTracing, TRACE_INFO)

TRACEPOINT_EVENT(com_ericsson_common_hcf_core_calendaralarmtimeplan, GetNextTimerPoint,
		TP_ARGS(const char *, msg, const char *, time, unsigned, h, unsigned, mi, unsigned, s, unsigned, mo, unsigned, d, unsigned, dw, unsigned, dwo),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(TIME, time) ctf_integer(unsigned, HOUR, h) ctf_integer(unsigned, MINUTE, mi) ctf_integer(unsigned, SECOND, s)
				ctf_integer(unsigned, MONTH, mo) ctf_integer(unsigned, DAY_OF_MONTH, d) ctf_integer(unsigned, DAY_OF_WEEK, dw) ctf_integer(unsigned, DAY_OF_WEEK_OCCURRENCE, dwo)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_core_calendaralarmtimeplan, GetNextTimerPoint, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_core_calendaralarmtimeplan, GetNextTimerPointErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_core_calendaralarmtimeplan, GetNextTimerPointErr, TRACE_ERR)

#endif /* _COM_ERICSSON_COMMON_HCF_CORE_CALENDARALARMTIMEPLAN_H */

/* This part must be outside protection */
#include <lttng/tracepoint-event.h>

#ifdef __cplusplus
}
#endif
