#ifndef __INCLUDE_GUARD_FILE_CMD_MACROS_H__
#define __INCLUDE_GUARD_FILE_CMD_MACROS_H__

/*
 *	@file cmd_macros.h
 *	@brief
 *	@author xcasale (Alessio Cascone)
 *	@date 2014-12-23
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
 *	| 2014-001 | 2014-12-23 | xcasale      | Creation and first revision.        |
 *	+==========+============+==============+=====================================+
 */

#include "hcf/core/macros.h"

#if !defined (HCF_NS_CMD_MACROS)
#	define HCF_NS_CMD_MACROS

#	if !defined(HCF_NS_CMD)
#		define HCF_NS_CMD cmd
#	endif /* !defined(HCF_NS_CMD) */

#	if !defined(HCF_NS_CMD_BEGIN)
#		define HCF_NS_CMD_BEGIN TSL_NS_BEGIN(HCF_NS_CMD)
#	endif /* !defined(HCF_NS_CMD_BEGIN) */

#	if !defined(HCF_NS_CMD_END)
#		define HCF_NS_CMD_END TSL_NS_END
#	endif /* !defined(HCF_NS_CMD_END) */

#	if !defined(HCF_NS_HCF_CMD_BEGIN)
#		define HCF_NS_HCF_CMD_BEGIN TSL_NS_HCF_BEGIN HCF_NS_CMD_BEGIN
#	endif /* !defined(HCF_NS_HCF_CMD_BEGIN) */

#	if !defined(HCF_NS_HCF_CMD_END)
#		define HCF_NS_HCF_CMD_END HCF_NS_CMD_END TSL_NS_HCF_END
#	endif /* !defined(HCF_NS_HCF_CMD_END) */

#	if !defined(HCF_NS_HCF_CMD)
#		define HCF_NS_HCF_CMD TSL_NS_HCF::HCF_NS_CMD
#	endif /* !defined(HCF_NS_HCF_CMD) */
#endif /* !defined (HCF_NS_CMD_MACROS) */

#endif /* __INCLUDE_GUARD_FILE_CMD_MACROS_H__ */
