/*
 *	@file com_ericsson_common_hcf_internal_internalapiconstants.h
 *	@brief
 *	@author xcasale (Alessio Cascone)
 *	@date 2015-10-27
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
 *	| 2015-001 | 2015-10-27 | xcasale      | Creation and first revision.        |
 *	+==========+============+==============+=====================================+
 *
 */

#undef TRACEPOINT_PROVIDER
#define TRACEPOINT_PROVIDER com_ericsson_common_hcf_internal_internalapiconstants

#undef TRACEPOINT_INCLUDE_FILE
#define TRACEPOINT_INCLUDE_FILE ./hcf/trace/com_ericsson_common_hcf_internal_internalapiconstants.h

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(_COM_ERICSSON_COMMON_HCF_INTERNAL_INTERNALAPICONSTANTS_H) || defined(TRACEPOINT_HEADER_MULTI_READ)
#define _COM_ERICSSON_COMMON_HCF_INTERNAL_INTERNALAPICONSTANTS_H

#include <lttng/tracepoint.h>

TRACEPOINT_EVENT(com_ericsson_common_hcf_internal_internalapiconstants, FunctionTracing,
		TP_ARGS(const char *, msg, const char *, function),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(FUNC, function)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_internal_internalapiconstants, FunctionTracing, TRACE_INFO)

TRACEPOINT_EVENT(com_ericsson_common_hcf_internal_internalapiconstants, GetClusterNoBackupPath,
		TP_ARGS(const char *, msg, const char *, val),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(VALUE, val)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_internal_internalapiconstants, GetClusterNoBackupPath, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_internal_internalapiconstants, GetClusterNoBackupPathErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_internal_internalapiconstants, GetClusterNoBackupPathErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_internal_internalapiconstants, GetClusterBackupPath,
		TP_ARGS(const char *, msg, const char *, val),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(VALUE, val)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_internal_internalapiconstants, GetClusterBackupPath, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_internal_internalapiconstants, GetClusterBackupPathErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_internal_internalapiconstants, GetClusterBackupPathErr, TRACE_ERR)

#endif /* _COM_ERICSSON_COMMON_HCF_INTERNAL_INTERNALAPICONSTANTS_H */

/* This part must be outside protection */
#include <lttng/tracepoint-event.h>

#ifdef __cplusplus
}
#endif
