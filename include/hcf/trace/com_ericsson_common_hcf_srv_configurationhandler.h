/*
 *	@file com_ericsson_common_hcf_srv_configurationhandler.h
 *	@brief
 *	@author xcasale (Alessio Cascone)
 *	@date 2015-10-13
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
 *	| 2015-001 | 2015-10-13 | xcasale      | Creation and first revision.        |
 *	+==========+============+==============+=====================================+
 *
 */

#undef TRACEPOINT_PROVIDER
#define TRACEPOINT_PROVIDER com_ericsson_common_hcf_srv_configurationhandler

#undef TRACEPOINT_INCLUDE_FILE
#define TRACEPOINT_INCLUDE_FILE ./hcf/trace/com_ericsson_common_hcf_srv_configurationhandler.h

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(_COM_ERICSSON_COMMON_HCF_SRV_CONFIGURATIONHANDLER_H) || defined(TRACEPOINT_HEADER_MULTI_READ)
#define _COM_ERICSSON_COMMON_HCF_SRV_CONFIGURATIONHANDLER_H

#include <lttng/tracepoint.h>

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_configurationhandler, FunctionTracing,
		TP_ARGS(const char *, msg, const char *, function),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(FUNC, function)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_configurationhandler, FunctionTracing, TRACE_INFO)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_configurationhandler, Initialize,
		TP_ARGS(const char *, msg),
		TP_FIELDS(ctf_string(MSG, msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_configurationhandler, Initialize, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_configurationhandler, InitializeErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_configurationhandler, InitializeErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_configurationhandler, LoadManagedElementIdValue,
		TP_ARGS(const char *, msg, const char *, val),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(VALUE, val)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_configurationhandler, LoadManagedElementIdValue, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_configurationhandler, LoadManagedElementIdValueErr,
		TP_ARGS(const char *, msg, int, err, const char *, err_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err) ctf_string(ERROR_MSG, err_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_configurationhandler, LoadManagedElementIdValueErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_configurationhandler, HandleManagedElementClassNotification,
		TP_ARGS(const char *, msg, const char *, val),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(VALUE, val)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_configurationhandler, HandleManagedElementClassNotification, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_configurationhandler, NotifyObjectCreation,
		TP_ARGS(const char *, msg, const char *, class_name, const char *, parent),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(CLASS_NAME, class_name) ctf_string(PARENT_DN, parent)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_configurationhandler, NotifyObjectCreation, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_configurationhandler, NotifyObjectModification,
		TP_ARGS(const char *, msg, const char *, obj),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(OBJECT, obj)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_configurationhandler, NotifyObjectModification, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_configurationhandler, NotifyObjectDeletion,
		TP_ARGS(const char *, msg, const char *, obj),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(OBJECT, obj)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_configurationhandler, NotifyObjectDeletion, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_configurationhandler, NotifyObjectModificationApplication,
		TP_ARGS(const char *, msg),
		TP_FIELDS(ctf_string(MSG, msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_configurationhandler, NotifyObjectModificationApplication, TRACE_NOTICE)

#endif /* _COM_ERICSSON_COMMON_HCF_SRV_CONFIGURATIONHANDLER_H */

/* This part must be outside protection */
#include <lttng/tracepoint-event.h>

#ifdef __cplusplus
}
#endif
