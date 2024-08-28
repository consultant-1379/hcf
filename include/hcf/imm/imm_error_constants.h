#ifndef __INCLUDE_GUARD_FILE_IMM_ERROR_CONSTANTS__
#define __INCLUDE_GUARD_FILE_IMM_ERROR_CONSTANTS__ imm_error_constants

/**
 *  @file imm_error_constants.h
 *	@brief
 *	@author xcasale (Alessio Cascone)
 *	@date 2014-11-07
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
 *	| 2014-001 | 2014-11-07 | xcasale      | Creation and first revision.        |
 *	+==========+============+==============+=====================================+
 */

#include "imm_macros.h"

TSL_NS_HCF_IMM_BEGIN

const int IMM_API_INTERNAL_NUMBER_OF_ERROR_MESSAGES = 45;

const char * const IMM_API_NO_ERR                        = "NO_ERROR";
const char * const IMM_API_ERR_UNKNOW                    = "ERR_UNKNOW";
const char * const IMM_API_ERR_IMM_LIBRARY               = "ERR_IMM_LIBRARY";
const char * const IMM_API_ERR_IMM_VERSION               = "ERR_IMM_VERSION";
const char * const IMM_API_ERR_IMM_INIT                  = "ERR_IMM_INIT";
const char * const IMM_API_ERR_IMM_TIMEOUT               = "ERR_IMM_TIMEOUT";
const char * const IMM_API_ERR_IMM_TRY_AGAIN             = "ERR_IMM_TRY_AGAIN";
const char * const IMM_API_ERR_IMM_INVALID_PARAM         = "ERR_INVALID_PARAMETER";
const char * const IMM_API_ERR_IMM_NO_MEMORY             = "ERR_IMM_NO_MEMORY";
const char * const IMM_API_ERR_IMM_BAD_HANDLE            = "ERR_IMM_BAD_HANDLE";
const char * const IMM_API_ERR_IMM_BUSY                  = "ERR_IMM_BUSY";
const char * const IMM_API_ERR_IMM_ACCESS                = "ERR_IMM_ACCESS";
const char * const IMM_API_ERR_IMM_NOT_EXIST             = "ERR_ELEMENT_NOT_EXIST";
const char * const IMM_API_ERR_IMM_NAME_TOO_LONG         = "ERR_NAME_TOO_LONG";
const char * const IMM_API_ERR_IMM_EXIST                 = "ERR_ELEMENT_ALREADY_EXIST";
const char * const IMM_API_ERR_IMM_NO_SPACE              = "ERR_IMM_NO_SPACE";
const char * const IMM_API_ERR_IMM_INTERRUPT             = "ERR_IMM_INTERRUPT";
const char * const IMM_API_ERR_NAME_NOT_FOUND            = "ERR_NAME_NOT_FOUND";
const char * const IMM_API_ERR_IMM_NO_RESOURCES          = "ERR_IMM_NO_RESOURCES";
const char * const IMM_API_ERR_IMM_NOT_SUPPORTED         = "ERR_IMM_NOT_SUPPORTED";
const char * const IMM_API_ERR_IMM_BAD_OPERATION         = "ERR_IMM_BAD_OPERATION";
const char * const IMM_API_ERR_IMM_FAILED_OPERATION      = "ERR_IMM_FAILED_OPERATION";
const char * const IMM_API_ERR_IMM_MESSAGE_ERROR         = "ERR_IMM_MESSAGE_ERROR";
const char * const IMM_API_ERR_IMM_QUEUE_FULL            = "ERR_IMM_QUEUE_FULL";
const char * const IMM_API_ERR_IMM_QUEUE_NOT_AVAILABLE   = "ERR_IMM_QUEUE_NOT_AVAILABLE";
const char * const IMM_API_ERR_IMM_BAD_FLAGS             = "ERR_IMM_BAD_FLAGS";
const char * const IMM_API_ERR_IMM_TOO_BIG               = "ERR_IMM_TOO_BIG";
const char * const IMM_API_ERR_IMM_NO_SECTIONS           = "ERR_IMM_NO_SECTION";
const char * const IMM_API_ERR_IMM_NO_OP                 = "ERR_IMM_NO_OP";
const char * const IMM_API_ERR_IMM_REPAIR_PENDING        = "ERR_IMM_REPAIR_PENDING";
const char * const IMM_API_ERR_IMM_NO_BINDINGS           = "ERR_IMM_NO_BINDINGS";
const char * const IMM_API_ERR_IMM_UNAVAILABLE           = "ERR_IMM_UNAVAILABLE";
const char * const IMM_API_ERR_IMM_CAMPAIGN_ERR_DETECTED = "ERR_IMM_CAMPAIGN_ERR_DETECTED";
const char * const IMM_API_ERR_IMM_CAMPAIGN_PROC_FAILED  = "ERR_IMM_CAMPAIGN_PROC_FAILED";
const char * const IMM_API_ERR_IMM_CAMPAIGN_CANCELED     = "ERR_IMM_CAMPAIGN_CANCELED";
const char * const IMM_API_ERR_IMM_CAMPAIGN_FAILED       = "ERR_IMM_CAMPAIGN_FAILED";
const char * const IMM_API_ERR_IMM_CAMPAIGN_SUSPENDED    = "ERR_IMM_CAMPAIGN_SUSPENDED";
const char * const IMM_API_ERR_IMM_CAMPAIGN_SUSPENDING   = "ERR_IMM_CAMPAIGN_SUSPENDING";
const char * const IMM_API_ERR_IMM_ACCESS_DENIED         = "ERR_IMM_ACCESS_DENIED";
const char * const IMM_API_ERR_IMM_NOT_READY             = "ERR_IMM_NOT_READY";
const char * const IMM_API_ERR_IMM_DEPLOYMENT            = "ERR_IMM_DEPLOYMENT";
const char * const IMM_API_ERR_NOT_FOUND                 = "ELEMENT_NOT_FOUND";
const char * const IMM_API_ERR_TRANSACTION_NOT_FOUND     = "TRANSACTION_NOT_FOUND";
const char * const IMM_API_ERR_IMM_SAVE                  = "IMM-SAVE_FAILED";
const char * const IMM_API_ERR_GENERIC                   = "GENERIC_ERROR";

static const char * const IMM_API_ERROR_MESSAGES [IMM_API_INTERNAL_NUMBER_OF_ERROR_MESSAGES] = {
		IMM_API_NO_ERR,                        //0
		IMM_API_ERR_UNKNOW,                    //1
		IMM_API_ERR_IMM_LIBRARY,               //2
		IMM_API_ERR_IMM_VERSION,               //3
		IMM_API_ERR_IMM_INIT,                  //4
		IMM_API_ERR_IMM_TIMEOUT,               //5
		IMM_API_ERR_IMM_TRY_AGAIN,             //6
		IMM_API_ERR_IMM_INVALID_PARAM,         //7
		IMM_API_ERR_IMM_NO_MEMORY,             //8
		IMM_API_ERR_IMM_BAD_HANDLE,            //9
		IMM_API_ERR_IMM_BUSY,                  //10
		IMM_API_ERR_IMM_ACCESS,                //11
		IMM_API_ERR_IMM_NOT_EXIST,             //12
		IMM_API_ERR_IMM_NAME_TOO_LONG,         //13
		IMM_API_ERR_IMM_EXIST,                 //14
		IMM_API_ERR_IMM_NO_SPACE,              //15
		IMM_API_ERR_IMM_INTERRUPT,             //16
		IMM_API_ERR_NAME_NOT_FOUND,            //17
		IMM_API_ERR_IMM_NO_RESOURCES,          //18
		IMM_API_ERR_IMM_NOT_SUPPORTED,         //19
		IMM_API_ERR_IMM_BAD_OPERATION,         //20
		IMM_API_ERR_IMM_FAILED_OPERATION,      //21
		IMM_API_ERR_IMM_MESSAGE_ERROR,         //22
		IMM_API_ERR_IMM_QUEUE_FULL,            //23
		IMM_API_ERR_IMM_QUEUE_NOT_AVAILABLE,   //24
		IMM_API_ERR_IMM_BAD_FLAGS,             //25
		IMM_API_ERR_IMM_TOO_BIG,               //26
		IMM_API_ERR_IMM_NO_SECTIONS,           //27
		IMM_API_ERR_IMM_NO_OP,                 //28
		IMM_API_ERR_IMM_REPAIR_PENDING,        //29
		IMM_API_ERR_IMM_NO_BINDINGS,           //30
		IMM_API_ERR_IMM_UNAVAILABLE,           //31
		IMM_API_ERR_IMM_CAMPAIGN_ERR_DETECTED, //32
		IMM_API_ERR_IMM_CAMPAIGN_PROC_FAILED,  //33
		IMM_API_ERR_IMM_CAMPAIGN_CANCELED,     //34
		IMM_API_ERR_IMM_CAMPAIGN_FAILED,       //35
		IMM_API_ERR_IMM_CAMPAIGN_SUSPENDED,    //36
		IMM_API_ERR_IMM_CAMPAIGN_SUSPENDING,   //37
		IMM_API_ERR_IMM_ACCESS_DENIED,         //38
		IMM_API_ERR_IMM_NOT_READY,             //39
		IMM_API_ERR_IMM_DEPLOYMENT,            //40
		IMM_API_ERR_NOT_FOUND,                 //41
		IMM_API_ERR_TRANSACTION_NOT_FOUND,     //42
		IMM_API_ERR_IMM_SAVE,                  //43
		IMM_API_ERR_GENERIC                    //44
};

enum ImmApiInternalErrorCodes {
		IMM_API_ERROR_CODE_NO_ERR                    = 0,
		IMM_API_ERROR_CODE_UNKNOW                    = -1,
		IMM_API_ERROR_CODE_IMM_LIBRARY               = -2,
		IMM_API_ERROR_CODE_IMM_VERSION               = -3,
		IMM_API_ERROR_CODE_IMM_INIT                  = -4,
		IMM_API_ERROR_CODE_IMM_TIMEOUT               = -5,
		IMM_API_ERROR_CODE_IMM_TRY_AGAIN             = -6,
		IMM_API_ERROR_CODE_IMM_INVALID_PARAM         = -7,
		IMM_API_ERROR_CODE_IMM_NO_MEMORY             = -8,
		IMM_API_ERROR_CODE_IMM_BAD_HANDLE            = -9,
		IMM_API_ERROR_CODE_IMM_BUSY                  = -10,
		IMM_API_ERROR_CODE_IMM_ACCESS                = -11,
		IMM_API_ERROR_CODE_IMM_NOT_EXIST             = -12,
		IMM_API_ERROR_CODE_IMM_NAME_TOO_LONG         = -13,
		IMM_API_ERROR_CODE_IMM_EXIST                 = -14,
		IMM_API_ERROR_CODE_IMM_NO_SPACE              = -15,
		IMM_API_ERROR_CODE_IMM_INTERRUPT             = -16,
		IMM_API_ERROR_CODE_NAME_NOT_FOUND            = -17,
		IMM_API_ERROR_CODE_IMM_NO_RESOURCES          = -18,
		IMM_API_ERROR_CODE_IMM_NOT_SUPPORTED         = -19,
		IMM_API_ERROR_CODE_IMM_BAD_OPERATION         = -20,
		IMM_API_ERROR_CODE_IMM_FAILED_OPERATION      = -21,
		IMM_API_ERROR_CODE_IMM_MESSAGE_ERROR         = -22,
		IMM_API_ERROR_CODE_IMM_QUEUE_FULL            = -23,
		IMM_API_ERROR_CODE_IMM_QUEUE_NOT_AVAILABLE   = -24,
		IMM_API_ERROR_CODE_IMM_BAD_FLAGS             = -25,
		IMM_API_ERROR_CODE_IMM_TOO_BIG               = -26,
		IMM_API_ERROR_CODE_IMM_NO_SECTIONS           = -27,
		IMM_API_ERROR_CODE_IMM_NO_OP                 = -28,
		IMM_API_ERROR_CODE_IMM_REPAIR_PENDING        = -29,
		IMM_API_ERROR_CODE_IMM_NO_BINDINGS           = -30,
		IMM_API_ERROR_CODE_IMM_UNAVAILABLE           = -31,
		IMM_API_ERROR_CODE_IMM_CAMPAIGN_ERR_DETECTED = -32,
		IMM_API_ERROR_CODE_IMM_CAMPAIGN_PROC_FAILED  = -33,
		IMM_API_ERROR_CODE_IMM_CAMPAIGN_CANCELED     = -34,
		IMM_API_ERROR_CODE_IMM_CAMPAIGN_FAILED       = -35,
		IMM_API_ERROR_CODE_IMM_CAMPAIGN_SUSPENDED    = -36,
		IMM_API_ERROR_CODE_IMM_CAMPAIGN_SUSPENDING   = -37,
		IMM_API_ERROR_CODE_IMM_ACCESS_DENIED         = -38,
		IMM_API_ERROR_CODE_IMM_NOT_READY             = -39,
		IMM_API_ERROR_CODE_IMM_DEPLOYMENT            = -40,
		IMM_API_ERROR_CODE_NOT_FOUND                 = -41,
		IMM_API_ERROR_CODE_TRANSACTION_NOT_FOUND     = -42,
		IMM_API_ERROR_CODE_IMM_SAVE                  = -43,
		IMM_API_ERROR_CODE_GENERIC                   = -44
};

TSL_NS_HCF_IMM_END

#endif // __INCLUDE_GUARD_FILE_IMM_ERROR_CONSTANTS__
