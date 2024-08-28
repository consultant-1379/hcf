/*
 *	@file com_ericsson_common_hcf_core_oneshotalarmtimeplan.h
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
#define TRACEPOINT_PROVIDER com_ericsson_common_hcf_core_oneshotalarmtimeplan

#undef TRACEPOINT_INCLUDE_FILE
#define TRACEPOINT_INCLUDE_FILE ./hcf/trace/com_ericsson_common_hcf_core_oneshotalarmtimeplan.h

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(_COM_ERICSSON_COMMON_HCF_CORE_ONESHOTALARMTIMEPLAN_H) || defined(TRACEPOINT_HEADER_MULTI_READ)
#define _COM_ERICSSON_COMMON_HCF_CORE_ONESHOTALARMTIMEPLAN_H

#include <lttng/tracepoint.h>

TRACEPOINT_EVENT(com_ericsson_common_hcf_core_oneshotalarmtimeplan, FunctionTracing,
		TP_ARGS(const char *, msg, const char *, function),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(FUNC, function)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_core_oneshotalarmtimeplan, FunctionTracing, TRACE_INFO)

TRACEPOINT_EVENT(com_ericsson_common_hcf_core_oneshotalarmtimeplan, GetNextTimerPoint,
		TP_ARGS(const char *, msg, const char *, time),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(TIME, time)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_core_oneshotalarmtimeplan, GetNextTimerPoint, TRACE_NOTICE)

#endif /* _COM_ERICSSON_COMMON_HCF_CORE_ONESHOTALARMTIMEPLAN_H */

/* This part must be outside protection */
#include <lttng/tracepoint-event.h>

#ifdef __cplusplus
}
#endif
