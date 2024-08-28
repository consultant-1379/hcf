#ifndef __INCLUDE_GUARD_FILE_INTERNALAPI_PROTOCOL_PRIMITIVES_H__
#define __INCLUDE_GUARD_FILE_INTERNALAPI_PROTOCOL_PRIMITIVES_H__

/*
 *	@file internalapi_macros.h
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

#include "internalapi_macros.h"
#include "internalapi_constants.h"

TSL_NS_HCF_INTERNALAPI_BEGIN

const int HCF_RULESET_FILE_OPERATION_SUCCESS = 1;
const int HCF_RULESET_FILE_OPERATION_FAILURE = 2;

enum HCFPrimitiveIdConstants {
	HCF_RULESET_FILE_DELETE_REQUEST_ID       = 1
	, HCF_RULESET_FILE_DELETE_REPLY_OK_ID    = 2
	, HCF_RULESET_FILE_DELETE_REPLY_NOK_ID   = 3
	, HCF_RULESET_FILE_INSTALL_REQUEST_ID    = 4
	, HCF_RULESET_FILE_INSTALL_REPLY_OK_ID   = 5
	, HCF_RULESET_FILE_INSTALL_REPLY_NOK_ID  = 6
	, HCF_RULESET_FILE_LIST_REQUEST_ID       = 7
	, HCF_RULESET_FILE_LIST_REPLY_OK_ID      = 8
	, HCF_RULESET_FILE_LIST_REPLY_NOK_ID     = 9
	, HCF_RULESET_FILE_UPGRADE_REQUEST_ID    = 10
	, HCF_RULESET_FILE_UPGRADE_REPLY_OK_ID   = 11
	, HCF_RULESET_FILE_UPGRADE_REPLY_NOK_ID  = 12
	, HCF_RULESET_FILE_VALIDATE_REQUEST_ID   = 13
	, HCF_RULESET_FILE_VALIDATE_REPLY_OK_ID  = 14
	, HCF_RULESET_FILE_VALIDATE_REPLY_NOK_ID = 15
};

TSL_NS_HCF_INTERNALAPI_END

#endif /* __INCLUDE_GUARD_FILE_INTERNALAPI_PROTOCOL_PRIMITIVES_H__ */
