/*
 *	@file com_ericsson_common_hcf_srv_jobexecutor.h
 *	@brief
 *	@author xcasale (Alessio Cascone)
 *	@date 2015-11-10
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
 *	| 2015-001 | 2015-11-10 | xcasale      | Creation and first revision.        |
 *	+==========+============+==============+=====================================+
 *
 */

#undef TRACEPOINT_PROVIDER
#define TRACEPOINT_PROVIDER com_ericsson_common_hcf_srv_jobexecutor

#undef TRACEPOINT_INCLUDE_FILE
#define TRACEPOINT_INCLUDE_FILE ./hcf/trace/com_ericsson_common_hcf_srv_jobexecutor.h

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(_COM_ERICSSON_COMMON_HCF_SRV_JOBEXECUTOR_H) || defined(TRACEPOINT_HEADER_MULTI_READ)
#define _COM_ERICSSON_COMMON_HCF_SRV_JOBEXECUTOR_H

#include <lttng/tracepoint.h>

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_jobexecutor, FunctionTracing,
		TP_ARGS(const char *, msg, const char *, function),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(FUNC, function)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_jobexecutor, FunctionTracing, TRACE_INFO)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_jobexecutor, OutputFileLesser,
		TP_ARGS(const char *, msg, const char *, f1, const char *, f2),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(FILE_1, f1) ctf_string(FILE_2, f2)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_jobexecutor, OutputFileLesser, TRACE_INFO)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_jobexecutor, HandleInput,
		TP_ARGS(const char *, msg, const char *, val),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(VALUE, val)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_jobexecutor, HandleInput, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_jobexecutor, HandleInputErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_jobexecutor, HandleInputErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_jobexecutor, HandleOutput,
		TP_ARGS(const char *, msg),
		TP_FIELDS(ctf_string(MSG, msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_jobexecutor, HandleOutput, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_jobexecutor, HandleException,
		TP_ARGS(const char *, msg, const char *, err, const char *, hun, const char *, rhun),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(ERROR_FLAG, err) ctf_string(HUNGUP_FLAG, hun) ctf_string(READ_HUNGUP_FLAG, rhun)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_jobexecutor, HandleException, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_jobexecutor, HandleClose,
		TP_ARGS(const char *, msg),
		TP_FIELDS(ctf_string(MSG, msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_jobexecutor, HandleClose, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_jobexecutor, Initialize,
		TP_ARGS(const char *, msg),
		TP_FIELDS(ctf_string(MSG, msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_jobexecutor, Initialize, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_jobexecutor, InitializeErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_jobexecutor, InitializeErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_jobexecutor, Finalize,
		TP_ARGS(const char *, msg),
		TP_FIELDS(ctf_string(MSG, msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_jobexecutor, Finalize, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_jobexecutor, Stop,
		TP_ARGS(const char *, msg),
		TP_FIELDS(ctf_string(MSG, msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_jobexecutor, Stop, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_jobexecutor, StopErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_jobexecutor, StopErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_jobexecutor, ExecuteJob,
		TP_ARGS(const char *, msg, const char *, id, int, n_cat, const char *, uri, const char *, job_to_tr, int, is_sched),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(JOB_ID, id) ctf_integer(int, NUM_OF_CATEGORIES, n_cat) ctf_string(EXPORT_URI, uri) ctf_string(JOB_TO_TRIGGER, job_to_tr) ctf_integer(int, IS_SCHEDULED, is_sched)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_jobexecutor, ExecuteJob, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_jobexecutor, ExecuteJobErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_jobexecutor, ExecuteJobErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_jobexecutor, Execute,
		TP_ARGS(const char *, msg, const char *, val),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(VALUE, val)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_jobexecutor, Execute, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_jobexecutor, ExecuteErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_jobexecutor, ExecuteErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_jobexecutor, CheckExecEnv,
		TP_ARGS(const char *, msg, const char *, value),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(VALUE, value)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_jobexecutor, CheckExecEnv, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_jobexecutor, CheckExecEnvErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_jobexecutor, CheckExecEnvErr, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_jobexecutor, ExecuteCommand,
		TP_ARGS(const char *, msg),
		TP_FIELDS(ctf_string(MSG, msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_jobexecutor, ExecuteCommand, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_jobexecutor, ExecuteCommandErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_jobexecutor, ExecuteCommandErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_jobexecutor, RemoveNewLineCharacters,
		TP_ARGS(const char *, msg),
		TP_FIELDS(ctf_string(MSG, msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_jobexecutor, RemoveNewLineCharacters, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_jobexecutor, IsNodeReachable,
		TP_ARGS(const char *, msg, const char *, value),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(VALUE, value)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_jobexecutor, IsNodeReachable, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_jobexecutor, IsNodeReachableErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_jobexecutor, IsNodeReachableErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_jobexecutor, ExtractTimestampFromFilename,
		TP_ARGS(const char *, msg, const char *, value),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(VALUE, value)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_jobexecutor, ExtractTimestampFromFilename, TRACE_NOTICE)

#endif /* _COM_ERICSSON_COMMON_HCF_SRV_JOBEXECUTOR_H */

/* This part must be outside protection */
#include <lttng/tracepoint-event.h>

#ifdef __cplusplus
}
#endif
