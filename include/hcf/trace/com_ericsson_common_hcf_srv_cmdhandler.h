/*
 *	@file com_ericsson_common_hcf_srv_cmdhandler.h
 *	@brief
 *	@author xcasale (Alessio Cascone)
 *	@date 2015-10-08
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
 *	| 2015-001 | 2015-10-08 | xcasale      | Creation and first revision.        |
 *	+==========+============+==============+=====================================+
 *
 */

#undef TRACEPOINT_PROVIDER
#define TRACEPOINT_PROVIDER com_ericsson_common_hcf_srv_cmdhandler

#undef TRACEPOINT_INCLUDE_FILE
#define TRACEPOINT_INCLUDE_FILE ./hcf/trace/com_ericsson_common_hcf_srv_cmdhandler.h

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(_COM_ERICSSON_COMMON_HCF_SRV_CMDHANDLER_H) || defined(TRACEPOINT_HEADER_MULTI_READ)
#define _COM_ERICSSON_COMMON_HCF_SRV_CMDHANDLER_H

#include <lttng/tracepoint.h>

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_cmdhandler, FunctionTracing,
		TP_ARGS(const char *, msg, const char *, function),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(FUNC, function)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_cmdhandler, FunctionTracing, TRACE_INFO)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_cmdhandler, HandleInput,
		TP_ARGS(const char *, msg, int, prim, const char *, val),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, PRIMITIVE, prim) ctf_string(VALUE, val)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_cmdhandler, HandleInput, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_cmdhandler, HandleInputErr,
		TP_ARGS(const char *, msg, int, err, const char *, err_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err) ctf_string(ERROR_MSG, err_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_cmdhandler, HandleInputErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_cmdhandler, HandleOutput,
		TP_ARGS(const char *, msg),
		TP_FIELDS(ctf_string(MSG, msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_cmdhandler, HandleOutput, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_cmdhandler, HandleException,
		TP_ARGS(const char *, msg, const char *, err, const char *, hun, const char *, rhun),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(ERROR_FLAG, err) ctf_string(HUNGUP_FLAG, hun) ctf_string(READ_HUNGUP_FLAG, rhun)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_cmdhandler, HandleException, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_cmdhandler, HandleClose,
		TP_ARGS(const char *, msg),
		TP_FIELDS(ctf_string(MSG, msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_cmdhandler, HandleClose, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_cmdhandler, Initialize,
		TP_ARGS(const char *, msg, const char *, val),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(VALUE, val)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_cmdhandler, Initialize, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_cmdhandler, InitializeErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_cmdhandler, InitializeErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_cmdhandler, Finalize,
		TP_ARGS(const char *, msg),
		TP_FIELDS(ctf_string(MSG, msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_cmdhandler, Finalize, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_cmdhandler, HandleStartupOperations,
		TP_ARGS(const char *, msg, const char *, val),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(VALUE, val)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_cmdhandler, HandleStartupOperations, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_cmdhandler, HandleStartupOperationsErr,
		TP_ARGS(const char *, msg, int, err, const char *, err_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err) ctf_string(ERROR_MSG, err_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_cmdhandler, HandleStartupOperationsErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_cmdhandler, HandleInstall,
		TP_ARGS(const char *, msg, unsigned, prim, const char *, prim_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(unsigned, PRIMITIVE, prim) ctf_string(PRIMITIVE_MSG, prim_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_cmdhandler, HandleInstall, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_cmdhandler, HandleInstallErr,
		TP_ARGS(const char *, msg, int, err, const char *, err_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err) ctf_string(ERROR_MSG, err_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_cmdhandler, HandleInstallErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_cmdhandler, HandleDelete,
		TP_ARGS(const char *, msg, unsigned, prim, const char *, prim_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(unsigned, PRIMITIVE, prim) ctf_string(PRIMITIVE_MSG, prim_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_cmdhandler, HandleDelete, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_cmdhandler, HandleDeleteErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_cmdhandler, HandleDeleteErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_cmdhandler, HandleValidate,
		TP_ARGS(const char *, msg, unsigned, prim, const char *, prim_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(unsigned, PRIMITIVE, prim) ctf_string(PRIMITIVE_MSG, prim_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_cmdhandler, HandleValidate, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_cmdhandler, HandleListing,
		TP_ARGS(const char *, msg, unsigned, prim, const char *, prim_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(unsigned, PRIMITIVE, prim) ctf_string(PRIMITIVE_MSG, prim_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_cmdhandler, HandleListing, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_cmdhandler, HandleUpdate,
		TP_ARGS(const char *, msg, unsigned, prim, const char *, prim_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(unsigned, PRIMITIVE, prim) ctf_string(PRIMITIVE_MSG, prim_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_cmdhandler, HandleUpdate, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_cmdhandler, HandleUpdateErr,
		TP_ARGS(const char *, msg, int, err, const char *, err_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err) ctf_string(ERROR_MSG, err_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_cmdhandler, HandleUpdateErr, TRACE_ERR)

#endif /* _COM_ERICSSON_COMMON_HCF_SRV_CMDHANDLER_H */

/* This part must be outside protection */
#include <lttng/tracepoint-event.h>

#ifdef __cplusplus
}
#endif
