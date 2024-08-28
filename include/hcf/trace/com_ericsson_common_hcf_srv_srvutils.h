/*
 *	@file com_ericsson_common_hcf_srv_srvutils.h
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
#define TRACEPOINT_PROVIDER com_ericsson_common_hcf_srv_srvutils

#undef TRACEPOINT_INCLUDE_FILE
#define TRACEPOINT_INCLUDE_FILE ./hcf/trace/com_ericsson_common_hcf_srv_srvutils.h

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(_COM_ERICSSON_COMMON_HCF_SRV_SRVUTILS_H) || defined(TRACEPOINT_HEADER_MULTI_READ)
#define _COM_ERICSSON_COMMON_HCF_SRV_SRVUTILS_H

#include <lttng/tracepoint.h>

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_srvutils, FunctionTracing,
		TP_ARGS(const char *, msg, const char *, function),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(FUNC, function)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_srvutils, FunctionTracing, TRACE_INFO)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_srvutils, GetCurrentTimestamp,
		TP_ARGS(const char *, msg, const char *, value),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(VALUE, value)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_srvutils, GetCurrentTimestamp, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_srvutils, MapCategoryStringToCategoryValue,
		TP_ARGS(const char *, msg, const char *, cat),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(CATEGORY, cat)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_srvutils, MapCategoryStringToCategoryValue, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_srvutils, MapCategoryValueToCategoryString,
		TP_ARGS(const char *, msg, int, cat),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, CATEGORY, cat)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_srvutils, MapCategoryValueToCategoryString, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_srvutils, MapHealthStatusToString,
		TP_ARGS(const char *, msg, int, status),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, HEALTH_STATUS, status)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_srvutils, MapHealthStatusToString, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_srvutils, IsSpecialCharacter,
		TP_ARGS(const char *, msg, char, c),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(char, CHARACTER, c)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_srvutils, IsSpecialCharacter, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_srvutils, EscapeSpecialCharacters,
		TP_ARGS(const char *, msg, char, c),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(char, CHARACTER, c)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_srvutils, EscapeSpecialCharacters, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_srvutils, MapSkipLogStringToSkipLogValue,
		TP_ARGS(const char *, msg, const char *, value),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(VALUE, value)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_srvutils, MapSkipLogStringToSkipLogValue, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_srvutils, MapSkipLogValueToSkipLogString,
		TP_ARGS(const char *, msg, int, value),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, VALUE, value)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_srvutils, MapSkipLogValueToSkipLogString, TRACE_NOTICE)

#endif /* _COM_ERICSSON_COMMON_HCF_SRV_SRVUTILS_H */

/* This part must be outside protection */
#include <lttng/tracepoint-event.h>

#ifdef __cplusplus
}
#endif
