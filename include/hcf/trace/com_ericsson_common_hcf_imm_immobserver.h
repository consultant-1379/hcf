/*
 *	@file com_ericsson_common_hcf_imm_immobserver.h
 *	@brief
 *	@author xcasale (Alessio Cascone)
 *	@date 2015-08-07
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
 *	| 2015-001 | 2015-08-07 | xcasale      | Creation and first revision.        |
 *	+==========+============+==============+=====================================+
 *
 */

#undef TRACEPOINT_PROVIDER
#define TRACEPOINT_PROVIDER com_ericsson_common_hcf_imm_immobserver

#undef TRACEPOINT_INCLUDE_FILE
#define TRACEPOINT_INCLUDE_FILE ./hcf/trace/com_ericsson_common_hcf_imm_immobserver.h

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(_COM_ERICSSON_COMMON_HCF_IMM_IMMOBSERVER_H) || defined(TRACEPOINT_HEADER_MULTI_READ)
#define _COM_ERICSSON_COMMON_HCF_IMM_IMMOBSERVER_H

#include <lttng/tracepoint.h>

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_immobserver, FunctionTracing,
		TP_ARGS(const char *, msg, const char *, function),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(FUNC, function)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_immobserver, FunctionTracing, TRACE_INFO)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_immobserver, Initialize,
		TP_ARGS(const char *, msg, const char *, imp, int, num_classes),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(IMPLEMENTER, imp) ctf_integer(int, NUM_OF_CLASSES, num_classes)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_immobserver, Initialize, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_immobserver, InitializeErr,
		TP_ARGS(const char *, msg, int, err, const char *, err_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err) ctf_string(ERROR_MSG, err_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_immobserver, InitializeErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_immobserver, Finalize,
		TP_ARGS(const char *, msg, const char *, imp, int, num_classes),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(IMPLEMENTER, imp) ctf_integer(int, NUM_OF_CLASSES, num_classes)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_immobserver, Finalize, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_immobserver, FinalizeErr,
		TP_ARGS(const char *, msg, int, err, const char *, err_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err) ctf_string(ERROR_MSG, err_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_immobserver, FinalizeErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_immobserver, CreateCallback,
		TP_ARGS(const char *, msg, unsigned long long, handle, unsigned long long, ccbid, const char *, classname, const char *, parentname),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(unsigned long long, HANDLE, handle) ctf_integer(unsigned long long, CCB, ccbid) ctf_string(CLASS, classname) ctf_string(PARENT, parentname)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_immobserver, CreateCallback, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_immobserver, DeleteCallback,
		TP_ARGS(const char *, msg, unsigned long long, handle, unsigned long long, ccbid, const char *, obj),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(unsigned long long, HANDLE, handle) ctf_integer(unsigned long long, CCB, ccbid) ctf_string(OBJECT, obj)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_immobserver, DeleteCallback, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_immobserver, ModifyCallback,
		TP_ARGS(const char *, msg, unsigned long long, handle, unsigned long long, ccbid, const char *, obj),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(unsigned long long, HANDLE, handle) ctf_integer(unsigned long long, CCB, ccbid) ctf_string(OBJECT, obj)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_immobserver, ModifyCallback, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_immobserver, CompleteCallback,
		TP_ARGS(const char *, msg, unsigned long long, handle, unsigned long long, ccbid),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(unsigned long long, HANDLE, handle) ctf_integer(unsigned long long, CCB, ccbid)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_immobserver, CompleteCallback, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_immobserver, AbortCallback,
		TP_ARGS(const char *, msg, unsigned long long, handle, unsigned long long, ccbid),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(unsigned long long, HANDLE, handle) ctf_integer(unsigned long long, CCB, ccbid)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_immobserver, AbortCallback, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_immobserver, ApplyCallback,
		TP_ARGS(const char *, msg, unsigned long long, handle, unsigned long long, ccbid),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(unsigned long long, HANDLE, handle) ctf_integer(unsigned long long, CCB, ccbid)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_immobserver, ApplyCallback, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_immobserver, HandleInput,
		TP_ARGS(const char *, msg, int, val),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, VALUE, val)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_immobserver, HandleInput, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_immobserver, HandleOutput,
		TP_ARGS(const char *, msg, int, handle),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, HANDLE, handle)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_immobserver, HandleOutput, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_immobserver, HandleException,
		TP_ARGS(const char *, msg, int, handle, int, er_f, int, h_f, int, rh_f),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, HANDLE, handle) ctf_integer(int, ERROR_FLAG, er_f) ctf_integer(int, HUNGUP_FLAG, h_f) ctf_integer(int, READ_HUNGUP_FLAG, rh_f)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_immobserver, HandleException, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_imm_immobserver, HandleClose,
		TP_ARGS(const char *, msg, int, handle),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, HANDLE, handle)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_imm_immobserver, HandleClose, TRACE_NOTICE)

#endif /* _COM_ERICSSON_COMMON_HCF_IMM_IMMOBSERVER_H */

/* This part must be outside protection */
#include <lttng/tracepoint-event.h>

#ifdef __cplusplus
}
#endif
