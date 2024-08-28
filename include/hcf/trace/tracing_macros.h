#ifndef __INCLUDE_GUARD_FILE_TRACE_TRACING_MACROS_H__
#define __INCLUDE_GUARD_FILE_TRACE_TRACING_MACROS_H__

/*
 *	@file tracing_macros.h
 *	@brief
 *	@author xcasale (Alessio Cascone)
 *	@date 2015-07-03
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
 *	| 2015-001 | 2015-07-03 | xcasale      | Creation and first revision.        |
 *	+==========+============+==============+=====================================+
 *
 */

#include "trace_macros.h"
#include "trace_utils.h"

// This macro is needed by the following macros: for that reason, raise a warning showing that its value is missing!
#if !defined (TRACEPOINT_PROVIDER)
# warning "Undefined value for TRACEPOINT_PROVIDER macro: please define it!!!"
#endif /* !defined (TRACEPOINT_PROVIDER) */

/*
 * The following macros set is used to provide some facilities in
 * order to make easier the LTTnG API usage.
 */
#if !defined (HCF_CC_TRACING_MACROS)
# define HCF_CC_TRACING_MACROS
# define HCF_CC_TRACE(e, ...) HCF_CC_TRACE_(TRACEPOINT_PROVIDER, e, __VA_ARGS__)
# define HCF_CC_TRACE_(d, e, ...) tracepoint(d, e, __VA_ARGS__)
#endif /* !defined (HCF_CC_TRACING_MACROS) */

/*
 * The following macros set provides a facility to print
 * buffer of characters in their hexadecimal format.
 */
#if !defined (HCF_CC_DUMPING_MACROS)
# define HCF_CC_DUMPING_MACROS
# define HCF_CC_TRACE_UTILS_CLASS_NAME TSL_NS_HCF_TRACE::trace_utils
# define HCF_CC_TRACE_DUMPING_METHOD_NAME dump_buffer
# define HCF_CC_DUMP(e, b, l) HCF_CC_TRACE(e, HCF_CC_TRACE_UTILS_CLASS_NAME::HCF_CC_TRACE_DUMPING_METHOD_NAME(b, l).c_str())
#endif /* !defined (HCF_CC_DUMPING_MACROS) */

/*
 * The following macros set provides a facility to trace
 * an entering/leaving message for each function in which
 * the macro is used.
 */
#if !defined (HCF_CC_FUNCTION_TRACING_MACROS)
# define HCF_CC_FUNCTION_TRACING_MACROS
# define HCF_CC_FUNCTION_TRACER_CLASS_NAME TSL_CONCAT_3(TRACEPOINT_PROVIDER, __, function_tracer)
# define HCF_CC_FUNCTION_TRACER_TRACE_EVENT_NAME FunctionTracing
# define HCF_CC_FUNCTION_TRACER_ENTERING_MSG "Entering function:"
# define HCF_CC_FUNCTION_TRACER_LEAVING_MSG "Leaving function:"
# define HCF_CC_FUNCTION_TRACER_CLASS_DEFINE \
	namespace {\
	class HCF_CC_FUNCTION_TRACER_CLASS_NAME {\
	public:\
		inline HCF_CC_FUNCTION_TRACER_CLASS_NAME(const char * f) : func(f) {\
			HCF_CC_TRACE(HCF_CC_FUNCTION_TRACER_TRACE_EVENT_NAME, HCF_CC_FUNCTION_TRACER_ENTERING_MSG, func); }\
		inline virtual ~HCF_CC_FUNCTION_TRACER_CLASS_NAME() {\
			HCF_CC_TRACE(HCF_CC_FUNCTION_TRACER_TRACE_EVENT_NAME, HCF_CC_FUNCTION_TRACER_LEAVING_MSG, func); }\
	private:\
		const char * func;\
	};}
# define HCF_CC_TRACE_FUNCTION HCF_CC_FUNCTION_TRACER_CLASS_NAME TSL_CONCAT_2(HCF_CC_FUNCTION_TRACER_CLASS_NAME, __LINE__)(__PRETTY_FUNCTION__)
#endif /* !defined (HCF_CC_FUNCTION_TRACING_MACROS) */

#endif /* __INCLUDE_GUARD_FILE_TRACE_TRACING_MACROS_H__ */
