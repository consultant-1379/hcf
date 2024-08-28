#ifndef __INCLUDE_GUARD_FILE_TRACE_TRACE_MACROS_H__
#define __INCLUDE_GUARD_FILE_TRACE_TRACE_MACROS_H__

/**
 *  @file trace_macros.h
 *	@brief
 *	@author xcasale (Alessio Cascone)
 *	@date 2015-07-14
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
 *	| 2015-001 | 2015-07-14 | xcasale      | Creation and first revision.        |
 *	+==========+============+==============+=====================================+
 */

#include "hcf/core/macros.h"

#if !defined (TSL_NS_TRACE_MACROS)
#	define TSL_NS_TRACE_MACROS

#	if !defined(TSL_NS_TRACE)
#		define TSL_NS_TRACE trace
#	endif /* !defined(TSL_NS_TRACE) */

#	if !defined(TSL_NS_TRACE_BEGIN)
#		define TSL_NS_TRACE_BEGIN TSL_NS_BEGIN(TSL_NS_TRACE)
#	endif /* !defined(TSL_NS_TRACE_BEGIN) */

#	if !defined(TSL_NS_TRACE_END)
#		define TSL_NS_TRACE_END TSL_NS_END
#	endif /* !defined(TSL_NS_TRACE_END) */

#	if !defined(TSL_NS_HCF_TRACE_BEGIN)
#		define TSL_NS_HCF_TRACE_BEGIN TSL_NS_HCF_BEGIN TSL_NS_TRACE_BEGIN
#	endif /* !defined(TSL_NS_HCF_TRACE_BEGIN) */

#	if !defined(TSL_NS_HCF_TRACE_END)
#		define TSL_NS_HCF_TRACE_END TSL_NS_TRACE_END TSL_NS_HCF_END
#	endif /* !defined(TSL_NS_HCF_TRACE_END) */

#	if !defined(TSL_NS_HCF_TRACE)
#		define TSL_NS_HCF_TRACE TSL_NS_HCF::TSL_NS_TRACE
#	endif
#endif /* !defined (TSL_NS_TRACE_MACROS) */

#endif /* __INCLUDE_GUARD_FILE_TRACE_TRACE_MACROS_H__ */
