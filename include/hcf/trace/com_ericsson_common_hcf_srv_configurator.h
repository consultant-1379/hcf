/*
 *	@file com_ericsson_common_hcf_srv_configurator.h
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
#define TRACEPOINT_PROVIDER com_ericsson_common_hcf_srv_configurator

#undef TRACEPOINT_INCLUDE_FILE
#define TRACEPOINT_INCLUDE_FILE ./hcf/trace/com_ericsson_common_hcf_srv_configurator.h

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(_COM_ERICSSON_COMMON_HCF_SRV_CONFIGURATOR_H) || defined(TRACEPOINT_HEADER_MULTI_READ)
#define _COM_ERICSSON_COMMON_HCF_SRV_CONFIGURATOR_H

#include <lttng/tracepoint.h>

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_configurator, FunctionTracing,
		TP_ARGS(const char *, msg, const char *, function),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(FUNC, function)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_configurator, FunctionTracing, TRACE_INFO)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_configurator, GetSystemControllersSet,
		TP_ARGS(const char *, msg),
		TP_FIELDS(ctf_string(MSG, msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_configurator, GetSystemControllersSet, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_configurator, GetSystemControllersSetErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_configurator, GetSystemControllersSetErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_configurator, GetPayloadsSet,
		TP_ARGS(const char *, msg),
		TP_FIELDS(ctf_string(MSG, msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_configurator, GetPayloadsSet, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_configurator, GetPayloadsSetErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_configurator, GetPayloadsSetErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_configurator, GetNodesSet,
		TP_ARGS(const char *, msg),
		TP_FIELDS(ctf_string(MSG, msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_configurator, GetNodesSet, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_configurator, GetNodesSetErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_configurator, GetNodesSetErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_configurator, GetHostname,
		TP_ARGS(const char *, msg, const char *, val),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(VALUE, val)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_configurator, GetHostname, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_configurator, GetHostnameErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_configurator, GetHostnameErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_configurator, GetAddrByHostname,
		TP_ARGS(const char *, msg, const char *, host),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(HOSTNAME, host)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_configurator, GetAddrByHostname, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_configurator, GetAddrByHostnameErr,
		TP_ARGS(const char *, msg, int, err, const char *, err_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err) ctf_string(ERROR_MSG, err_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_configurator, GetAddrByHostnameErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_configurator, GetInternalOutputFolder,
		TP_ARGS(const char *, msg, const char *, folder),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(OUTPUT_FOLDER, folder)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_configurator, GetInternalOutputFolder, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_configurator, CreateOutputFolder,
		TP_ARGS(const char *, msg, const char *, folder),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(OUTPUT_FOLDER, folder)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_configurator, CreateOutputFolder, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_configurator, CreateOutputFolderErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_configurator, CreateOutputFolderErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_configurator, GetOutputFolder,
		TP_ARGS(const char *, msg, const char *, val),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(VALUE, val)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_configurator, GetOutputFolder, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_configurator, LinkNBIToOutputFolder,
		TP_ARGS(const char *, msg, const char *, folder, const char *, link),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(OUTPUT_FOLDER, folder) ctf_string(LINK_NAME, link)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_configurator, LinkNBIToOutputFolder, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_configurator, LinkNBIToOutputFolderErr,
		TP_ARGS(const char *, msg, int, err),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_configurator, LinkNBIToOutputFolderErr, TRACE_ERR)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_configurator, ClearLoadedConfiguration,
		TP_ARGS(const char *, msg, int, mask),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer_hex(int, DELETE_MASK, mask)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_configurator, ClearLoadedConfiguration, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_configurator, LoadNodesFromImm,
		TP_ARGS(const char *, msg, const char *, val),
		TP_FIELDS(ctf_string(MSG, msg) ctf_string(VALUE, val)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_configurator, LoadNodesFromImm, TRACE_NOTICE)

TRACEPOINT_EVENT(com_ericsson_common_hcf_srv_configurator, LoadNodesFromImmErr,
		TP_ARGS(const char *, msg, int, err, const char *, err_msg),
		TP_FIELDS(ctf_string(MSG, msg) ctf_integer(int, ERROR_CODE, err) ctf_string(ERROR_MSG, err_msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_common_hcf_srv_configurator, LoadNodesFromImmErr, TRACE_ERR)

#endif /* _COM_ERICSSON_COMMON_HCF_SRV_CONFIGURATOR_H */

/* This part must be outside protection */
#include <lttng/tracepoint-event.h>

#ifdef __cplusplus
}
#endif
