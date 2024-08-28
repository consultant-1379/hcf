/*
 *	@file com_ericsson_common_hcf_core_utils.h
 *	@brief
 *	@author xcasale (Alessio Cascone)
 *	@date 2015-08-14
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
 *	| 2015-001 | 2015-08-14 | xcasale      | Creation and first revision.        |
 *	+==========+============+==============+=====================================+
 *
 */

#undef TRACEPOINT_PROVIDER
#define TRACEPOINT_PROVIDER com_ericsson_common_hcf_core_utils

#undef TRACEPOINT_INCLUDE_FILE
#define TRACEPOINT_INCLUDE_FILE ./hcf/trace/com_ericsson_common_hcf_core_utils.h

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(_COM_ERICSSON_COMMON_HCF_CORE_UTILS_H) || defined(TRACEPOINT_HEADER_MULTI_READ)
#define _COM_ERICSSON_COMMON_HCF_CORE_UTILS_H

#include <lttng/tracepoint.h>

TRACEPOINT_EVENT(com_ericsson_common_hcf_core_utils, FunctionTracing,
		TP_ARGS(const char *, msg, const char *, function),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(FUNC, function)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_core_utils, FunctionTracing, TRACE_INFO)

TRACEPOINT_EVENT(com_ericsson_common_hcf_core_utils, List,
		TP_ARGS(const char *, msg, const char *, val),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(VALUE, val)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_core_utils, List, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_core_utils, ListErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_core_utils, ListErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_core_utils, CreateDir,
		TP_ARGS(const char *, msg, const char *, path, const char *, parent, unsigned, mode),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(PATH, path) ctf_string(PARENT, parent) ctf_integer_hex(unsigned, MODE, mode)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_core_utils, CreateDir, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_core_utils, CreateDirErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_core_utils, CreateDirErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_core_utils, Copy,
		TP_ARGS(const char *, msg, const char *, from, const char *, to),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(FROM_FILE, from) ctf_string(TO_FILE, to)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_core_utils, Copy, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_core_utils, CopyErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_core_utils, CopyErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_core_utils, Move,
		TP_ARGS(const char *, msg, const char *, from, const char *, to),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(FROM_FILE, from) ctf_string(TO_FILE, to)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_core_utils, Move, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_core_utils, MoveErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_core_utils, MoveErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_core_utils, RemoveFolder,
		TP_ARGS(const char *, msg, const char *, val),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(VALUE, val)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_core_utils, RemoveFolder, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_core_utils, RemoveFolderErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_core_utils, RemoveFolderErr, TRACE_ERR)

#endif /* _COM_ERICSSON_COMMON_HCF_CORE_UTILS_H */

/* This part must be outside protection */
#include <lttng/tracepoint-event.h>

#ifdef __cplusplus
}
#endif
