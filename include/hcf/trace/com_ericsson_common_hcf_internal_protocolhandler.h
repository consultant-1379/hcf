/*
 *	@file com_ericsson_common_hcf_internal_protocolhandler.h
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
#define TRACEPOINT_PROVIDER com_ericsson_common_hcf_internal_protocolhandler

#undef TRACEPOINT_INCLUDE_FILE
#define TRACEPOINT_INCLUDE_FILE ./hcf/trace/com_ericsson_common_hcf_internal_protocolhandler.h

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(_COM_ERICSSON_COMMON_HCF_INTERNAL_PROTOCOLHANDLER_H) || defined(TRACEPOINT_HEADER_MULTI_READ)
#define _COM_ERICSSON_COMMON_HCF_INTERNAL_PROTOCOLHANDLER_H

#include <lttng/tracepoint.h>

TRACEPOINT_EVENT(com_ericsson_common_hcf_internal_protocolhandler, FunctionTracing,
		TP_ARGS(const char *, msg, const char *, function),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(FUNC, function)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_internal_protocolhandler, FunctionTracing, TRACE_INFO)

TRACEPOINT_EVENT(com_ericsson_common_hcf_internal_protocolhandler, MakePrimitive,
		TP_ARGS(const char *, msg, unsigned, id, unsigned, len, const char *, buf),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(unsigned, PRIMITIVE_ID, id) ctf_integer(unsigned, MESSAGE_LENGTH, len) ctf_string(PRIMITIVE_MESSAGE, buf)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_internal_protocolhandler, MakePrimitive, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_internal_protocolhandler, MakePrimitiveErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_internal_protocolhandler, MakePrimitiveErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_internal_protocolhandler, UnpackPrimitive,
		TP_ARGS(const char *, msg, unsigned, id, unsigned, len, const char *, buf),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(unsigned, PRIMITIVE_ID, id) ctf_integer(unsigned, MESSAGE_LENGTH, len) ctf_string(PRIMITIVE_MESSAGE, buf)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_internal_protocolhandler, UnpackPrimitive, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_internal_protocolhandler, UnpackPrimitiveErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_internal_protocolhandler, UnpackPrimitiveErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_internal_protocolhandler, SendPrimitive,
		TP_ARGS(const char *, msg, int, sock, unsigned, len, const char *, buf),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, SOCKET, sock) ctf_integer(unsigned, BUFFER_LENGTH, len) ctf_string(BUFFER, buf)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_internal_protocolhandler, SendPrimitive, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_internal_protocolhandler, SendPrimitiveErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_internal_protocolhandler, SendPrimitiveErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_internal_protocolhandler, ReceivePrimitive,
		TP_ARGS(const char *, msg, int, sock, unsigned, len, const char *, buf),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, SOCKET, sock) ctf_integer(unsigned, BUFFER_LENGTH, len) ctf_string(BUFFER, buf)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_internal_protocolhandler, ReceivePrimitive, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_internal_protocolhandler, ReceivePrimitiveErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_internal_protocolhandler, ReceivePrimitiveErr, TRACE_ERR)

#endif /* _COM_ERICSSON_COMMON_HCF_INTERNAL_PROTOCOLHANDLER_H */

/* This part must be outside protection */
#include <lttng/tracepoint-event.h>

#ifdef __cplusplus
}
#endif
