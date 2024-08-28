/*
 *	@file com_ericsson_common_hcf_srv_sftphandler.h
 *	@brief
 *	@author xcasale (Alessio Cascone)
 *	@date 2015-11-06
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
 *	| 2015-001 | 2015-11-06 | xcasale      | Creation and first revision.        |
 *	+==========+============+==============+=====================================+
 *
 */

#undef TRACEPOINT_PROVIDER
#define TRACEPOINT_PROVIDER com_ericsson_common_hcf_srv_sftphandler

#undef TRACEPOINT_INCLUDE_FILE
#define TRACEPOINT_INCLUDE_FILE ./hcf/trace/com_ericsson_common_hcf_srv_sftphandler.h

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(_COM_ERICSSON_COMMON_HCF_SRV_SFTPHANDLER_H) || defined(TRACEPOINT_HEADER_MULTI_READ)
#define _COM_ERICSSON_COMMON_HCF_SRV_SFTPHANDLER_H

#include <lttng/tracepoint.h>

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_sftphandler, FunctionTracing,
		TP_ARGS(const char *, msg, const char *, function),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(FUNC, function)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_sftphandler, FunctionTracing, TRACE_INFO)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_sftphandler, HandleInput,
		TP_ARGS(const char *, msg, const char *, val),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(VALUE, val)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_sftphandler, HandleInput, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_sftphandler, HandleInputErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_sftphandler, HandleInputErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_sftphandler, HandleOutput,
		TP_ARGS(const char *, msg),
		TP_FIELDS(ctf_string(MSG, msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_sftphandler, HandleOutput, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_sftphandler, HandleException,
		TP_ARGS(const char *, msg, const char *, err, const char *, hun, const char *, rhun),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(ERROR_FLAG, err) ctf_string(HUNGUP_FLAG, hun) ctf_string(READ_HUNGUP_FLAG, rhun)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_sftphandler, HandleException, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_sftphandler, HandleClose,
		TP_ARGS(const char *, msg),
		TP_FIELDS(ctf_string(MSG, msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_sftphandler, HandleClose, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_sftphandler, Initialize,
		TP_ARGS(const char *, msg, const char *, val),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(VALUE, val)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_sftphandler, Initialize, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_sftphandler, InitializeErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_sftphandler, InitializeErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_sftphandler, Finalize,
		TP_ARGS(const char *, msg),
		TP_FIELDS(ctf_string(MSG, msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_sftphandler, Finalize, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_sftphandler, AddFileToFTQ,
		TP_ARGS(const char *, msg, const char *, file),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(FILE, file)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_sftphandler, AddFileToFTQ, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_sftphandler, AddFileToFTQErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_sftphandler, AddFileToFTQErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_sftphandler, TransferFileViaSFTP,
		TP_ARGS(const char *, msg, const char *, value),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(value, value)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_sftphandler, TransferFileViaSFTP, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_sftphandler, TransferFileViaSFTPErr,
		TP_ARGS(const char *, msg, int, err, const char *, err_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err) ctf_string(ERROR_MSG, err_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_sftphandler, TransferFileViaSFTPErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_sftphandler, WaitSFTPTransferEnd,
		TP_ARGS(const char *, msg),
		TP_FIELDS(ctf_string(MSG, msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_sftphandler, WaitSFTPTransferEnd, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_sftphandler, WaitSFTPTransferEndErr,
		TP_ARGS(const char *, msg, int, err, const char *, err_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err) ctf_string(ERROR_MSG, err_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_sftphandler, WaitSFTPTransferEndErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_sftphandler, SignalSFTPTransferEnd,
		TP_ARGS(const char *, msg),
		TP_FIELDS(ctf_string(MSG, msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_sftphandler, SignalSFTPTransferEnd, TRACE_NOTICE)

#endif /* _COM_ERICSSON_COMMON_HCF_SRV_SFTPHANDLER_H */

/* This part must be outside protection */
#include <lttng/tracepoint-event.h>

#ifdef __cplusplus
}
#endif
