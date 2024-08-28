/*
 *	@file com_ericsson_common_hcf_core_processdaemonizer.h
 *	@brief
 *	@author xcasale (Alessio Cascone)
 *	@date 2015-08-13
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
 *	| 2015-001 | 2015-08-13 | xcasale      | Creation and first revision.        |
 *	+==========+============+==============+=====================================+
 *
 */

#undef TRACEPOINT_PROVIDER
#define TRACEPOINT_PROVIDER com_ericsson_common_hcf_core_processdaemonizer

#undef TRACEPOINT_INCLUDE_FILE
#define TRACEPOINT_INCLUDE_FILE ./hcf/trace/com_ericsson_common_hcf_core_processdaemonizer.h

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(_COM_ERICSSON_COMMON_HCF_CORE_PROCESSDAEMONIZER_H) || defined(TRACEPOINT_HEADER_MULTI_READ)
#define _COM_ERICSSON_COMMON_HCF_CORE_PROCESSDAEMONIZER_H

#include <lttng/tracepoint.h>

TRACEPOINT_EVENT(com_ericsson_common_hcf_core_processdaemonizer, FunctionTracing,
		TP_ARGS(const char *, msg, const char *, function),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(FUNC, function)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_core_processdaemonizer, FunctionTracing, TRACE_INFO)

TRACEPOINT_EVENT(com_ericsson_common_hcf_core_processdaemonizer, Daemonize,
		TP_ARGS(const char *, msg, int, nochdir, int, noclose, const char *, name, const char *, user, const char *, grp),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, NOCHDIR, nochdir) ctf_integer(int, NOCLOSE, noclose) ctf_string(DAEMON_NAME, name) ctf_string(USERNAME, user) ctf_string(GROUP, grp)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_core_processdaemonizer, Daemonize, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_core_processdaemonizer, DaemonizeErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_core_processdaemonizer, DaemonizeErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_core_processdaemonizer, SetUserID,
		TP_ARGS(const char *, msg, const char *, user),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(USERNAME, user)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_core_processdaemonizer, SetUserID, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_core_processdaemonizer, SetUserIDErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_core_processdaemonizer, SetUserIDErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_core_processdaemonizer, SetGroupID,
		TP_ARGS(const char *, msg, const char *, grp),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(GROUP, grp)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_core_processdaemonizer, SetGroupID, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_core_processdaemonizer, SetGroupIDErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_core_processdaemonizer, SetGroupIDErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_core_processdaemonizer, SetSecondaryGroupsIDs,
		TP_ARGS(const char *, msg, const char *, val),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(VALUE, val)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_core_processdaemonizer, SetSecondaryGroupsIDs, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_core_processdaemonizer, SetSecondaryGroupsIDsErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_core_processdaemonizer, SetSecondaryGroupsIDsErr, TRACE_ERR)

#endif /* _COM_ERICSSON_COMMON_HCF_CORE_PROCESSDAEMONIZER_H */

/* This part must be outside protection */
#include <lttng/tracepoint-event.h>

#ifdef __cplusplus
}
#endif
