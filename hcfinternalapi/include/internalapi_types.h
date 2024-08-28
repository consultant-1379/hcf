#ifndef __INCLUDE_GUARD_FILE_INTERNALAPI_APP_TYPES_H__
#define __INCLUDE_GUARD_FILE_INTERNALAPI_APP_TYPES_H__

/*
 *	@file internalapi_types.h
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

#include <saAmf.h>

#include "internalapi_macros.h"

TSL_NS_HCF_INTERNALAPI_BEGIN

typedef unsigned long long selectable_obj_t;

enum amf_recommended_recovery_t {
	RECOMMENDED_RECOVERY_NO_RECOMMENDATION     = 1
	, RECOMMENDED_RECOVERY_COMPONENT_RESTART   = 2
	, RECOMMENDED_RECOVERY_COMPONENT_FAILOVER  = 3
	, RECOMMENDED_RECOVERY_NODE_SWITCHOVER     = 4
	,	RECOMMENDED_RECOVERY_NODE_FAILOVER       = 5
	, RECOMMENDED_RECOVERY_NODE_FAILFAST       = 6
	, RECOMMENDED_RECOVERY_CLUSTER_RESET       = 7
	, RECOMMENDED_RECOVERY_APPLICATION_RESTART = 8
	, RECOMMENDED_RECOVERY_CONTAINER_RESTART   = 9
};

enum amf_dispatch_flags_t {
	AMF_DISPATCH_ONE        = 1
	, AMF_DISPATCH_ALL      = 2
	, AMF_DISPATCH_BLOCKING = 3
};

enum amf_ha_state_t {
	AMF_HA_ACTIVE      = 1
	, AMF_HA_STANDBY   = 2
	, AMF_HA_QUIESCED  = 3
	, AMF_HA_QUIESCING = 4
	, AMF_HA_UNDEFINED = 5
};

struct amf_health_check_key_t {
	unsigned char key [SA_AMF_HEALTHCHECK_KEY_MAX];
	unsigned short keyLen;
};

TSL_NS_HCF_INTERNALAPI_END

#endif /* __INCLUDE_GUARD_FILE_INTERNALAPI_APP_TYPES_H__ */
