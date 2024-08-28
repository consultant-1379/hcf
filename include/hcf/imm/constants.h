#ifndef __INCLUDE_GUARD_FILE_IMM_CONSTANTS_H__
#define __INCLUDE_GUARD_FILE_IMM_CONSTANTS_H__

/*
 *	@file constants.h
 *	@brief
 *	@author xcasale (Alessio Cascone)
 *	@date 2015-01-19
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
 *	| 2015-001 | 2015-01-19 | xcasale      | Creation and first revision.        |
 *	+==========+============+==============+=====================================+
 */

#include "imm_macros.h"

TSL_NS_HCF_IMM_BEGIN

enum ErrorConstants {
	ERR_NO_ERROR                               =  0
	, ERR_OM_HANDLER_INIT_FAILED               = -1
	, ERR_OM_HANDLER_FINALIZE_FAILED           = -2
	, ERR_OBJECT_ALREADY_EXISTS                = -3
	, ERR_OM_HANDLER_CREATE_OBJECT_FAILED      = -4
	, ERR_OM_HANDLER_MODIFY_OBJECT_FAILED      = -5
	, ERR_OM_HANDLER_DELETE_OBJECT_FAILED      = -6
	, ERR_OM_HANDLER_GET_INSTANCES_FAILED      = -7
	, ERR_OM_HANDLER_GET_OBJECT_FAILED         = -8
	, ERR_OM_HANDLER_SET_IMPLEMENTER_FAILED    = -9
	, ERR_OM_HANDLER_REMOVE_IMPLEMENTER_FAILED = -10
	, ERR_SET_OBJECT_ATTRIBUTES_FAILED         = -11
	, ERR_NULL_INSTANCE_RETRIEVED              = -12
};

TSL_NS_HCF_IMM_END

#endif /* __INCLUDE_GUARD_FILE_IMM_CONSTANTS_H__ */
