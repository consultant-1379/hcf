#ifndef __INCLUDE_GUARD_FILE_TRACE_WARNING_MACROS_H__
#define __INCLUDE_GUARD_FILE_TRACE_WARNING_MACROS_H__

/*
 *	@file warning_macros.h
 *	@brief
 *	@author xcasale (Alessio Cascone)
 *	@date 2015-07-07
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
 *	| 2015-001 | 2015-07-07 | xcasale      | Creation and first revision.        |
 *	+==========+============+==============+=====================================+
 *
 */

#include "hcf/core/macros.h"

/**
 * The following macros set is used to turn off/on a particular family of compiler
 * warnings. That can be useful to disable some warnings for particular pieces of
 * source code in which we cannot change anything to solve the warning root cause,
 * as for example external libraries header files.
 */
#if !defined (WARNINGS_HANDLING_MACROS)
#	define WARNINGS_HANDLING_MACROS
# define _GCC_DIAGNOSTIC GCC diagnostic
# define _DIAGNOSTIC_IGNORED _GCC_DIAGNOSTIC ignored
# define _DIAGNOSTIC_WARNING _GCC_DIAGNOSTIC warning
# define _TURN_OFF_COMPILER_WARNING(w) _DIAGNOSTIC_IGNORED TSL_STRINGIZE(TSL_CONCAT_2(-W,w))
# define _TURN_ON_COMPILER_WARNING(w) _DIAGNOSTIC_WARNING TSL_STRINGIZE(TSL_CONCAT_2(-W,w))
# define TURN_OFF_COMPILER_WARNING(w) _Pragma(TSL_STRINGIZE(_TURN_OFF_COMPILER_WARNING(w)))
# define TURN_ON_COMPILER_WARNING(w) _Pragma(TSL_STRINGIZE(_TURN_ON_COMPILER_WARNING(w)))
#endif /* !defined (WARNINGS_HANDLING_MACROS) */

/**
 * The following macros set is used to avoid getting warnings from the compiler
 * when a code that includes LTTnG library header files is compiled with
 * -Wall && -Wextra flags. In that case, since we cannot do anything to correct
 * the warning root cause into LTTnG library, it's better to avoid to show those
 * warnings, in order to focus to the own code warnings.
 * LTTnG library header files generate the following warnings:
 * 		- Unused functions parameters.
 * 		- Unused variables.
 * 		- Always true/false comparison due to the data types range.
 */
#if !defined (LTTNG_WARNING_REMOVAL_MACROS)
# define LTTNG_WARNING_REMOVAL_MACROS
# define TURN_OFF_WARNINGS_INTO_LTTNG_LIBRARY_BEGIN \
	TURN_OFF_COMPILER_WARNING(unused-parameter) \
	TURN_OFF_COMPILER_WARNING(unused-variable) \
	TURN_OFF_COMPILER_WARNING(type-limits)
# define TURN_OFF_WARNINGS_INTO_LTTNG_LIBRARY_END \
	TURN_ON_COMPILER_WARNING(unused-parameter) \
	TURN_ON_COMPILER_WARNING(unused-variable) \
	TURN_ON_COMPILER_WARNING(type-limits)
#endif /* !defined (LTTNG_WARNING_REMOVAL_MACROS) */

#endif /* __INCLUDE_GUARD_FILE_TRACE_WARNING_MACROS_H__ */
