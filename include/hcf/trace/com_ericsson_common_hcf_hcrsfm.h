/*
 *	@file com_ericsson_common_hcf_hcrsfm.h
 *	@brief
 *	@author xcasale (Alessio Cascone)
 *	@date 2015-07-22
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
 *	| 2015-001 | 2015-07-22 | xcasale      | Creation and first revision.        |
 *	+==========+============+==============+=====================================+
 *
 */

#undef TRACEPOINT_PROVIDER
#define TRACEPOINT_PROVIDER com_ericsson_common_hcf_hcrsfm

#undef TRACEPOINT_INCLUDE_FILE
#define TRACEPOINT_INCLUDE_FILE ./hcf/trace/com_ericsson_common_hcf_hcrsfm.h

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(_COM_ERICSSON_COMMON_HCF_HCRSFM_H) || defined(TRACEPOINT_HEADER_MULTI_READ)
#define _COM_ERICSSON_COMMON_HCF_HCRSFM_H

#include <lttng/tracepoint.h>

TRACEPOINT_EVENT(com_ericsson_common_hcf_hcrsfm, FunctionTracing,
		TP_ARGS(const char *, msg, const char *, function),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(FUNC, function)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_hcrsfm, FunctionTracing, TRACE_INFO)

TRACEPOINT_EVENT(com_ericsson_common_hcf_hcrsfm, User,
		TP_ARGS(const char *, msg, unsigned int, uid),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(unsigned int, UID, uid)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_hcrsfm, User, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_hcrsfm, CmdUsageError,
		TP_ARGS(const char *, msg, const char *, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(ERR, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_hcrsfm, CmdUsageError, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_hcrsfm, FileArgument,
		TP_ARGS(const char *, file),
		TP_FIELDS(ctf_string(FILE, file)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_hcrsfm, FileArgument, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_hcrsfm, FileArgumentError,
		TP_ARGS(const char *, err, const char *, file),
		TP_FIELDS(ctf_string(ERR, err) ctf_string(FILE, file)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_hcrsfm, FileArgumentError, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_hcrsfm, ServerConnection,
		TP_ARGS(const char *, msg, const char *, addr),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(ADDR, addr)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_hcrsfm, ServerConnection, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_hcrsfm, ServerConnectionError,
		TP_ARGS(const char *, msg, int, error),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR, error)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_hcrsfm, ServerConnectionError, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_hcrsfm, Primitive,
		TP_ARGS(const char *, msg, unsigned int, prim_id, unsigned int, msg_len, const char *, prim_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(unsigned int, PRIMITIVE_ID, prim_id) ctf_integer(unsigned int, MESSAGE_LENGTH, msg_len) ctf_string(PRIMITIVE_MESSAGE, prim_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_hcrsfm, Primitive, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_hcrsfm, PrimitiveError,
		TP_ARGS(const char *, msg, int, error, const char *, error_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR, error) ctf_string(ERROR_MESSAGE, error_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_hcrsfm, PrimitiveError, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_hcrsfm, CmdExitCode,
		TP_ARGS(const char *, msg, int, exit_code),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, EXIT_CODE, exit_code)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_hcrsfm, CmdExitCode, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_hcrsfm, Parsing,
		TP_ARGS(int, opt_code, int, long_index, int, optind),
		TP_FIELDS(ctf_integer(int, OPT_CODE, opt_code) ctf_integer(int, LONG_INDEX, long_index) ctf_integer(int, OPTIND, optind)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_hcrsfm, Parsing, TRACE_NOTICE)

#endif /* _COM_ERICSSON_COMMON_HCF_HCRSFM_H */

/* This part must be outside protection */
#include <lttng/tracepoint-event.h>

#ifdef __cplusplus
}
#endif
