#ifndef __INCLUDE_GUARD_FILE_SRV_MACROS_H__
#define __INCLUDE_GUARD_FILE_SRV_MACROS_H__

/*
 *	@file srv_macros.h
 *	@brief
 *	@author xnicmut (Nicola Muto)
 *	@date 2014-11-06
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
 *	| 2014-001 | 2014-11-06 | xnicmut      | Creation and first revision.        |
 *	+==========+============+==============+=====================================+
 */

#include "hcf/core/macros.h"

#if !defined (HCF_NS_SRV_MACROS)
#	define HCF_NS_SRV_MACROS

//#	if !defined(HCF_NS_HCF)
//#		define HCF_NS_HCF TSL_NS_HCF
//#	endif

//#	if !defined(HCF_NS_CORE)
//#		define HCF_NS_CORE TSL_NS_CORE
//#	endif

#	if !defined(HCF_NS_SRV)
#		define HCF_NS_SRV srv
#	endif /* !defined(HCF_NS_SRV) */

//#	if !defined(HCF_NS_HCF_BEGIN)
//#		define HCF_NS_HCF_BEGIN TSL_NS_BEGIN(HCF_NS_HCF)
//#	endif

//#	if !defined(HCF_NS_HCF_END)
//#		define HCF_NS_HCF_END TSL_NS_END
//#	endif

//#	if !defined(HCF_NS_CORE_BEGIN)
//#		define HCF_NS_CORE_BEGIN TSL_NS_BEGIN(HCF_NS_CORE)
//#	endif

//#	if !defined(HCF_NS_CORE_END)
//#		define HCF_NS_CORE_END TSL_NS_END
//#	endif

#	if !defined(HCF_NS_SRV_BEGIN)
#		define HCF_NS_SRV_BEGIN TSL_NS_BEGIN(HCF_NS_SRV)
#	endif /* !defined(HCF_NS_SRV_BEGIN) */

#	if !defined(HCF_NS_SRV_END)
#		define HCF_NS_SRV_END TSL_NS_END
#	endif /* !defined(HCF_NS_SRV_END) */

#	if !defined(HCF_NS_HCF_SRV_BEGIN)
#		define HCF_NS_HCF_SRV_BEGIN TSL_NS_HCF_BEGIN HCF_NS_SRV_BEGIN
#	endif /* !defined(HCF_NS_HCF_SRV_BEGIN) */

#	if !defined(HCF_NS_HCF_SRV_END)
#		define HCF_NS_HCF_SRV_END HCF_NS_SRV_END TSL_NS_HCF_END
#	endif /* !defined(HCF_NS_HCF_SRV_END) */

#	if !defined(HCF_NS_HCF_SRV)
#		define HCF_NS_HCF_SRV TSL_NS_HCF::HCF_NS_SRV
#	endif /* !defined(HCF_NS_HCF_SRV) */
#endif /* !defined (HCF_NS_SRV_MACROS) */

#endif /* __INCLUDE_GUARD_FILE_SRV_MACROS_H__ */
