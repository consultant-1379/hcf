#ifndef __INCLUDE_GUARD_FILE_CORE_ERROR_CONSTANTS_H__
#define __INCLUDE_GUARD_FILE_CORE_ERROR_CONSTANTS_H__

/*
 *	@file error_constants.h
 *	@brief
 *	@author xnicmut (Nicola Muto)
 *	@date 2014-11-18
 *
 *	COPYRIGHT Ericsson AB, 2014
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
 *	| 2014-001 | 2014-11-18 | xnicmut      | Creation and first revision.        |
 *	+==========+============+==============+=====================================+
 */

#include "macros.h"

//#if defined(TSL_USE_NAMESPACE_FOR_CONSTANTS)
TSL_NS_HCF_CORE_BEGIN
//#endif

//enum TSL_PREFIX_ENUM_NAME_IF_NO_NAMESPACE(ErrorConstants) {
enum ErrorConstants {
	/* INFO CODES */
	ERR_NO_ERROR = 0

	/* WARNING CODES */
	,	ERR_WARNING_CODES_BASE	= 0x4000  // == 16384

	/* ERROR CODES */
	,	ERR_ERROR_CODES_BASE	= 0x8000  // == 32768
	,	ERR_INVALID_ARGUMENT
	,	ERR_NULL_ARGUMENT
	, ERR_MISSING_ARGUMENT
	, ERR_TOO_MANY_ARGUMENTS
	, ERR_UNRECOGNIZED_OPTION
	, ERR_INVALID_HANDLE
	, ERR_UNSUPPORTED_OPTION
	, ERR_ALREADY_INITIALIZED
	, ERR_ALREADY_OPEN
	, ERR_ALREADY_CLOSED
	, ERR_NOT_FOUND
	, ERR_FOUND
	, ERR_ON_INIT
	, ERR_INVALID_OPERATION
	, ERR_INTERNAL_ERROR
	, ERR_IMPLEMENTATION_NOT_FOUND

	,	ERR_REGISTERING_HANDLER
	, ERR_CHANGING_HANDLER

	, ERR_NO_RESOURCES_AVAILABLE

	, ERR_LOGGER_INITIALIZE_FAILED
	, ERR_SEND_LOG_BUFFER_FAILED

	, ERR_TIME_IS_OVER

//	, ERR_EVENT_MONITOR_INIT
//	, ERR_HANDLER_NOT_FOUND
//	,	ERR_EPOLL_EVENT_ADD
//	, ERR_EPOLL_EVENT_CHANGE
//	, ERR_EVENT_MONITOR_CLOSED
//	, ERR_EVENT_MONITOR_INIT
//	, ERR_EVENT_HANDLER_NOT_FOUND
//	, ERR_EVENT_HANDLER_BUSY_ON_HANDLING
//	, ERR_MUTEX_LOCK

	/* FATAL CODES */
	,	ERR_FATAL_CODES_BASE	= 0xC000
};

//#if defined(TSL_USE_NAMESPACE_FOR_CONSTANTS)
TSL_NS_HCF_CORE_END
//#endif

#endif /* __INCLUDE_GUARD_FILE_CORE_ERROR_CONSTANTS_H__ */
