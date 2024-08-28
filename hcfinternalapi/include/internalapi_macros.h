#ifndef __INCLUDE_GUARD_FILE_INTERNALAPI_MACROS_H__
#define __INCLUDE_GUARD_FILE_INTERNALAPI_MACROS_H__

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

#include "hcf/core/macros.h"

#if !defined (TSL_NS_INTERNALAPI_MACROS)
#	define TSL_NS_INTERNALAPI_MACROS

#	if !defined(TSL_NS_INTERNALAPI)
#		define TSL_NS_INTERNALAPI internalapi
#	endif /* !defined(TSL_NS_INTERNALAPI) */

#	if !defined(TSL_NS_INTERNALAPI_BEGIN)
#		define TSL_NS_INTERNALAPI_BEGIN TSL_NS_BEGIN(TSL_NS_INTERNALAPI)
#	endif /* !defined(TSL_NS_INTERNALAPI_BEGIN) */

#	if !defined(TSL_NS_INTERNALAPI_END)
#		define TSL_NS_INTERNALAPI_END TSL_NS_END
#	endif /* !defined(TSL_NS_INTERNALAPI_END) */

#	if !defined(TSL_NS_HCF_INTERNALAPI_BEGIN)
#		define TSL_NS_HCF_INTERNALAPI_BEGIN TSL_NS_HCF_BEGIN TSL_NS_INTERNALAPI_BEGIN
#	endif /* !defined(TSL_NS_HCF_INTERNALAPI_BEGIN) */

#	if !defined(TSL_NS_HCF_INTERNALAPI_END)
#		define TSL_NS_HCF_INTERNALAPI_END TSL_NS_INTERNALAPI_END TSL_NS_HCF_END
#	endif /* !defined(TSL_NS_HCF_INTERNALAPI_END) */

#	if !defined(TSL_NS_HCF_INTERNALAPI)
#		define TSL_NS_HCF_INTERNALAPI TSL_NS_HCF::TSL_NS_INTERNALAPI
#	endif
#endif /* !defined (TSL_NS_INTERNALAPI_MACROS) */

#endif /* __INCLUDE_GUARD_FILE_INTERNALAPI_MACROS_H__ */
