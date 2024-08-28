#ifndef __INCLUDE_GUARD_FILE_IMM_MACROS_H__
#define __INCLUDE_GUARD_FILE_IMM_MACROS_H__

/**
 *  @file imm_macros.h
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

#include "hcf/core/macros.h"

#if !defined (TSL_NS_IMM_MACROS)
#	define TSL_NS_IMM_MACROS

#	if !defined(TSL_NS_IMM)
#		define TSL_NS_IMM immapi
#	endif /* !defined(TSL_NS_IMM) */

#	if !defined(TSL_NS_IMM_BEGIN)
#		define TSL_NS_IMM_BEGIN TSL_NS_BEGIN(TSL_NS_IMM)
#	endif /* !defined(TSL_NS_IMM_BEGIN) */

#	if !defined(TSL_NS_IMM_END)
#		define TSL_NS_IMM_END TSL_NS_END
#	endif /* !defined(TSL_NS_IMM_END) */

#	if !defined(TSL_NS_HCF_IMM_BEGIN)
#		define TSL_NS_HCF_IMM_BEGIN TSL_NS_HCF_BEGIN TSL_NS_IMM_BEGIN
#	endif /* !defined(TSL_NS_HCF_IMM_BEGIN) */

#	if !defined(TSL_NS_HCF_IMM_END)
#		define TSL_NS_HCF_IMM_END TSL_NS_IMM_END TSL_NS_HCF_END
#	endif /* !defined(TSL_NS_HCF_IMM_END) */

#	if !defined(TSL_NS_HCF_IMM)
#		define TSL_NS_HCF_IMM TSL_NS_HCF::TSL_NS_IMM
#	endif
#endif /* !defined (TSL_NS_IMM_MACROS) */

#endif /* __INCLUDE_GUARD_FILE_SRV_MACROS_H__ */
