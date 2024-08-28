#ifndef __INCLUDE_GUARD_FILE_SRV_CONSTANTS_H__
#define __INCLUDE_GUARD_FILE_SRV_CONSTANTS_H__

/*
 *	@file constants.h
 *	@brief
 *	@author xcasale (Alessio Cascone)
 *	@date 2014-12-02
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
 *	| 2014-001 | 2014-12-02 | xcasale      | Creation and first revision.        |
 *	+==========+============+==============+=====================================+
 */

#include "srv_macros.h"

HCF_NS_HCF_SRV_BEGIN

/*
 * HCF server return codes.
 */
enum ApplicationExitCodeConstants {
	APPLICATION_EXIT_OK = 0
	, APPLICATION_EXIT_ANOTHER_SERVER_RUNNING
	, APPLICATION_EXIT_BAD_INVOCATION
	, APPLICATION_EXIT_DEMONIZATION_FAILED
	, APPLICATION_EXIT_MEMORY_ALLOCATION_ERROR
};

/*
 * HCF server internal error codes.
 */
enum ErrorConstants {
	ERR_NO_ERROR                            = 0
	, ERR_RSF_XML_PARSING_FAILED            = 1
	, ERR_RSF_XML_FILE_NOT_VALID            = 2
	, ERR_RSF_XML_FILE_EMPTY                = 3
	, ERR_RSF_XML_BAD_ELEMENT               = 4
	, ERR_RSF_IMM_RULE_CREATE_FAILED        = 5
	, ERR_RSF_IMM_RULE_GET_INSTANCE_FAILED  = 6
	, ERR_RSF_IMM_RULE_DUPLICATED           = 7
	, ERR_RSF_XSD_CTX_INIT_FAILED           = 8
	, ERR_RSF_XSD_PARSING_FAILED            = 9
	, ERR_RSF_XSD_VALIDATION_FAILED         = 10
	, ERR_IMM_MANDATORY_VALUE_MISSING       = 11
	, ERR_UNDEFINED_CATEGORY                = 12
	, ERR_NULL_INSTANCE_RETRIEVED           = 13
	, ERR_OPERATION_NOT_IMPLEMENTED         = 14
	, ERR_AUGMENTED_CCB_INIT_FAILED         = 15
	, ERR_AUGMENTED_CCB_CREATE_FAILED       = 16
	, ERR_AUGMENTED_CCB_APPLY_FAILED        = 17
	, ERR_AUGMENTED_CCB_MODIFY_FAILED       = 18
	, ERR_PROGRESS_REPORT_INIT_FAILED       = 19
	, ERR_PROGRESS_REPORT_MODIFY_FAILED     = 20
	, ERR_PROGRESS_REPORT_FINALIZE_FAILED   = 21
	, ERR_FAILED_RULE_CREATE_FAILED         = 22
	, ERR_FAILED_RULE_MODIFY_FAILED         = 23
	, ERR_FAILED_RULE_DELETE_FAILED         = 24
	, ERR_ANOTHER_JOB_ONGOING               = 25
	, ERR_UNDEFINED_RULE_ATTRIBUTE          = 26
	, ERR_DUPLICATED_RULE_ATTRIBUTE         = 27
	, ERR_OUT_XML_FILE_CREATE_FAILED        = 28
	, ERR_OUT_XML_FILE_INDENT_FAILED        = 29
	, ERR_OUT_XML_FILE_ELEMENT_WRITE_FAILED = 30
	, ERR_CLASS_HANDLER_INITIALIZE_FAILED   = 31
	, ERR_CLASS_HANDLER_FINALIZE_FAILED     = 32
	, ERR_OUTPUT_FOLDER_CREATION_FAILED     = 33
	, ERR_BAD_VALUE_PROVIDED                = 34
	, ERR_CMD_HANDLER_INITIALIZE_FAILED     = 35
	, ERR_CMD_HANDLER_FINALIZE_FAILED       = 36
	, ERR_FILEM_PATH_RETRIEVE_FAILED        = 37
	, ERR_OUTPUT_FOLDER_LINK_CREATE_FAILED  = 38
	, ERR_ROOT_WORKER_THREAD_CREATE_FAILED  = 39
	, ERR_ROOT_WORKER_THREAD_START_FAILED   = 40
	, ERR_ROOT_WORKER_THREAD_WAIT_FAILED    = 41
	, ERR_GET_CONFIGURATION_INFO_FAILED     = 42
	, ERR_FILE_SYSTEM_OPERATION_FAILED      = 43
	, ERR_DUPLICATED_RULE_SET_FILE          = 44
	, ERR_UNDEFINED_RULE_SET_FILE           = 45
	, ERR_JOB_EXECUTOR_INITIALIZE_FAILED    = 46
	, ERR_JOB_EXECUTOR_FINALIZE_FAILED      = 47
	, ERR_JOB_EXECUTOR_ADD_JOB_FAILED       = 48
	, ERR_JOB_LINKED_TO_ANOTHER_JOB         = 49
	, ERR_JOB_EXECUTION_ONGOING             = 50
	, ERR_STOP_EVENT_SIGNALLING_FAILED      = 51
	, ERR_CONFIG_HANDLER_INIT_FAILED        = 52
	, ERR_CONFIG_HANDLER_FINALIZE_FAILED    = 53
	, ERR_SFTP_HANDLER_INITIALIZE_FAILED    = 54
	, ERR_SFTP_HANDLER_FINALIZE_FAILED      = 55
	, ERR_OPERATION_NOT_PERMITTED           = 56
};

/*
 * HCF server internal integer constants.
 */
const int HCF_MAX_NUMBER_OF_CATEGORIES       = 11;
const int HCF_NUMBER_OF_OUTPUT_FILES_FOR_JOB = 2;

/*
 * HCF server internal string constants.
 */
#ifdef HCF_CC_RUNS_WITH_NO_ROOT_PERMISSIONS
extern const char * const HCF_SRV_USER_NAME;
extern const char * const HCF_SRV_GROUP_NAME;
extern const char * const HCF_SYSTEM_NBI_DATA_GROUP_NAME;
extern const char * const HCF_CMW_IMM_USERS_GROUP_NAME;
extern const char * HCF_SRV_SECONDARY_GROUPS_NAMES[];
#endif
extern const char * const HCF_APPLICATION_NAME;
extern const char * const HCF_NBI_API_CONFIGURATION_FILE_PATH;
extern const char * const HCF_FILEM_CONFIGURATION_FILE_PATH;
extern const char * const HCF_FILEM_INTERNAL_ROOT_DEFAULT_PATH;
extern const char * const HCF_OUTPUT_FOLDER_NAME;
extern const char * const HCF_TEMP_FOLDER_PATH;
extern const char * const HCF_RULE_LOG_FILE_EXTENSION;
extern const char * const HCF_OUTPUT_ARCHIVE_EXTENSION;
extern const char * const HCF_GENERIC_ERROR_ERROR_MSG;
extern const char * const HCF_MANDATORY_VALUE_MISSING_ERROR_MSG;
extern const char * const HCF_UNDEFINED_CATEGORY_ERROR_MSG;
extern const char * const HCF_CHILDREN_OBJECTS_CREATION_FAILED_ERROR_MSG;
extern const char * const HCF_OPERATION_NOT_IMPLEMENTED_ERROR_MSG;
extern const char * const HCF_JOB_ALREADY_RUNNING_ERROR_MSG;
extern const char * const HCF_INVALID_RULE_RDN_ERROR_MSG;
extern const char * const HCF_UNDEFINED_RULE_RDN_ERROR_MSG;
extern const char * const HCF_UNDEFINED_RULE_ATTRIBUTE_ERROR_MSG;
extern const char * const HCF_DUPLICATED_RULE_ATTRIBUTE_ERROR_MSG;
extern const char * const HCF_INVALID_JOBTOTRIGGER_VALUE_ERROR_MSG;
extern const char * const HCF_NOT_EXISTING_JOBTOTRIGGER_ERROR_MSG;
extern const char * const HCF_JOB_LINKED_TO_ANOTHER_JOB_ERROR_MSG;
extern const char * const HCF_JOB_EXECUTION_ONGOING_ERROR_MSG;
extern const char * const HCF_INVALID_JOB_RDN_ERROR_MSG;
extern const char * const HCF_INVALID_TIME_VALUE_ERROR_MSG;
extern const char * const HCF_PAST_TIME_VALUE_ERROR_MSG;
extern const char * const HCF_INVALID_TIME_INTERVAL_ERROR_MSG;
extern const char * const HCF_INVALID_VALUE_PROVIDED_ERROR_MSG;
extern const char * const HCF_INVALID_DAYOFWEEK_OCCURRENCE_ERROR_MSG;
extern const char * const HCF_INPUTPARAMETER_OP_NOT_PERMITTED_ERROR_MSG;

HCF_NS_HCF_SRV_END

#endif /* __INCLUDE_GUARD_FILE_SRV_CONSTANTS_H__ */
