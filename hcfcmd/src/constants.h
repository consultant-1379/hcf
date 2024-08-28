#ifndef __INCLUDE_GUARD_FILE_CMD_CONSTANTS_H__
#define __INCLUDE_GUARD_FILE_CMD_CONSTANTS_H__

/*
 *	@file app_constants.h
 *	@brief
 *	@author xcasale (Alessio Cascone)
 *	@date 2014-12-29
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
 *	| 2014-001 | 2014-12-29 | xcasale      | Creation and first revision.        |
 *	+==========+============+==============+=====================================+
 */

#include "cmd_macros.h"

HCF_NS_HCF_CMD_BEGIN

enum CommandExitCodeConstants {
	CMD_EXIT_OK                              = 0
	, CMD_EXIT_GENERAL_ERROR                 = 1
	, CMD_EXIT_INCORRECT_USAGE               = 2
	, CMD_EXIT_NOT_EXISTING_FILE             = 3
	, CMD_EXIT_CONNECTION_FAILED             = 4
	, CMD_EXIT_BUILD_PRIMITIVE_FAILED        = 5
	, CMD_EXIT_SEND_PRIMITIVE_FAILED         = 6
	, CMD_EXIT_RECV_PRIMITIVE_FAILED         = 7
	, CMD_EXIT_UNPACK_PRIMITIVE_FAILED       = 8
	, CMD_EXIT_UNEXPECTED_PRIMITIVE_RECEIVED = 9
	, CMD_EXIT_NOT_REGULAR_FILE              = 10
	, CMD_EXIT_NOT_AUTHORIZED_USER           = 11
	, CMD_EXIT_RFS_ALREADY_INSTALLED         = 12
	, CMD_EXIT_RSF_MOVE_FAILED               = 13
};

const uid_t CMD_AUTHORAZIED_USER = 0; // The command can be executed only by root user

HCF_NS_HCF_CMD_END

#endif /* __INCLUDE_GUARD_FILE_CMD_CONSTANTS_H__ */
