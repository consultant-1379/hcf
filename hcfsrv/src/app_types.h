#ifndef __INCLUDE_GUARD_FILE_SRV_APP_TYPES_H__
#define __INCLUDE_GUARD_FILE_SRV_APP_TYPES_H__

/*
 *	@file app_types.h
 *	@brief
 *	@author xcasale (Alessio Cascone)
 *	@date 2014-11-26
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
 *	| 2014-001 | 2014-11-26 | xcasale      | Creation and first revision.        |
 *	+==========+============+==============+=====================================+
 */

#include "srv_macros.h"

HCF_NS_HCF_SRV_BEGIN

typedef enum {
	HEALTH_STATUS_HEALTHY         = 0
	, HEALTH_STATUS_NOT_HEALTHY   = 1
	, HEALTH_STATUS_WARNING       = 2
	, HEALTH_STATUS_NOT_AVAILABLE = 3
} health_status_t;

typedef enum {
	ADMINISTRATIVE_STATE_LOCKED      = 0
	, ADMINISTRATIVE_STATE_UNLOCKED  = 1
	, ADMINISTRATIVE_STATE_UNDEFINED = 2
} administrative_state_t;

typedef enum {
	SEVERITY_CRITICAL  = 0
	, SEVERITY_WARNING = 1
} severity_t;

typedef enum {
	CATEGORY_BACKOFFICE     = 0
	, CATEGORY_DAILY        = 1
	, CATEGORY_MANDATORY    = 2
	, CATEGORY_POSTUPDATE   = 3
	, CATEGORY_POSTUPGRADE  = 4
	, CATEGORY_PREINSTALL   = 5
	, CATEGORY_PREUPDATE    = 6
	, CATEGORY_PREUPGRADE   = 7
	, CATEGORY_SHORT        = 8
	, CATEGORY_TROUBLESHOOT = 9
	, CATEGORY_OTHER        = 10
} category_t;

typedef enum {
	RESULT_SUCCESS         = 1
	, RESULT_FAILURE       = 2
	, RESULT_NOT_AVAILABLE = 3
} action_result_t;

typedef enum {
	STATE_CANCELLING  = 1
	, STATE_RUNNING   = 2
	, STATE_FINISHED  = 3
	, STATE_CANCELLED = 4
} action_state_t;

typedef enum {
	ALL_DAYS    = 0
	, MONDAY    = 1
	, TUESDAY   = 2
	, WEDNESDAY = 3
	, THURSDAY  = 4
	, FRIDAY    = 5
	, SATURDAY  = 6
	, SUNDAY    = 7
} dayofweek_t;

typedef enum {
	ALL_OCCURRENCES = 0
	, FIRST         = 1
	, SECOND        = 2
	, THIRD         = 3
	, FOURTH        = 4
	, LAST          = 5
} dayofweek_occurrence_t;

typedef enum {
  NO_SKIP  = 0
  , SKIP_COMMAND  = 1
  , SKIP_PRINTOUT = 2
  , SKIP_ALL
} skip_log_t;

HCF_NS_HCF_SRV_END

#endif /* __INCLUDE_GUARD_FILE_SRV_APP_TYPES_H__ */
