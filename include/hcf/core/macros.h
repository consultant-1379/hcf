#ifndef __INCLUDE_GUARD_FILE_CORE_MACROS_H__
#define __INCLUDE_GUARD_FILE_CORE_MACROS_H__

/*
 *	@file macros.h
 *	@brief
 *	@author xnicmut (Nicola Muto)
 *	@date 2014-11-05
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
 *	| 2014-001 | 2014-11-05 | xnicmut      | Creation and first revision.        |
 *	+==========+============+==============+=====================================+
 *
 *	"If you want to find the secrets of the universe, think in terms of energy, frequency and vibration"
 */

//#	include <sys/types.h>
#if !defined (_GNU_SOURCE)
#	define _GNU_SOURCE
#endif
#include <unistd.h>
#include <sys/syscall.h>
#include <stdio.h>

#if !defined (TSL_ARRAY_SIZE)
#	define TSL_ARRAY_SIZE(array) (sizeof(array)/sizeof(*(array)))
#endif /* !defined (TSL_ARRAY_SIZE) */

#if !defined (TSL_STRINGIZE_MACROS)
#	define TSL_STRINGIZE_MACROS

#	define TSL_STRINGIZER_(s) #s
#	define TSL_STRINGIZE(s) TSL_STRINGIZER_(s)
#endif /* !defined (TSL_STRINGIZE_MACROS) */

#if !defined (TSL_CONCAT_MACROS)
#	define TSL_CONCAT_MACROS

#	define TSL_CONCAT_2_(a,b) a ## b
#	define TSL_CONCAT_2(a,b) TSL_CONCAT_2_(a,b)
#	define TSL_CONCAT_3_(a,b,c) a ## b ## c
#	define TSL_CONCAT_3(a,b,c) TSL_CONCAT_3_(a,b,c)
#endif /* !defined (TSL_CONCAT_MACROS) */

#if !defined (TSL_TRACING_MACROS)
#	define TSL_TRACE_ON_SCREEN ::printf("TRACE_POINT[TID=%ld,FUNCTION=%s,LINE=%d]\n", ::syscall(SYS_gettid),__func__,__LINE__)
#endif

#if !defined (TSL_CTOR_FUNCTION_MACROS)
#	define TSL_CTOR_FUNCTION_MACROS

#	define TSL_CTOR __TSL_CLASS_NAME__
#	define TSL_DTOR ~__TSL_CLASS_NAME__
#	define TSL_CTOR_INIT_LIST(...) : __VA_ARGS__
#endif /* !defined (TSL_CTOR_FUNCTION_MACROS) */

#if !defined (TSL_NS_CORE_MACROS)
#	define TSL_NS_CORE_MACROS

#	define TSL_NS_BEGIN(ns) namespace ns {
#	define TSL_NS_END }

#	if !defined(TSL_NS_HCF)
#		define TSL_NS_HCF hcf
#	endif /* !defined(TSL_NS_HCF) */

#	if !defined(TSL_NS_CORE)
#		define TSL_NS_CORE coreapi
#	endif /* !defined(TSL_NS_CORE) */

#	if !defined(TSL_NS_HCF_BEGIN)
#		define TSL_NS_HCF_BEGIN TSL_NS_BEGIN(TSL_NS_HCF)
#	endif /* !defined(TSL_NS_HCF_BEGIN) */

#	if !defined(TSL_NS_HCF_END)
#		define TSL_NS_HCF_END TSL_NS_END
#	endif /* !defined(TSL_NS_HCF_END) */

#	if !defined(TSL_NS_CORE_BEGIN)
#		define TSL_NS_CORE_BEGIN TSL_NS_BEGIN(TSL_NS_CORE)
#	endif /* !defined(TSL_NS_CORE_BEGIN) */

#	if !defined(TSL_NS_CORE_END)
#		define TSL_NS_CORE_END TSL_NS_END
#	endif /* !defined(TSL_NS_CORE_END) */

#	if !defined(TSL_NS_HCF_CORE_BEGIN)
#		define TSL_NS_HCF_CORE_BEGIN TSL_NS_HCF_BEGIN TSL_NS_CORE_BEGIN
#	endif /* !defined(TSL_NS_HCF_CORE_BEGIN) */

#	if !defined(TSL_NS_HCF_CORE_END)
#		define TSL_NS_HCF_CORE_END TSL_NS_CORE_END TSL_NS_HCF_END
#	endif /* !defined(TSL_NS_HCF_CORE_END) */

#	if !defined(TSL_NS_HCF_CORE)
#		define TSL_NS_HCF_CORE TSL_NS_HCF::TSL_NS_CORE
#	endif /* !defined(TSL_NS_HCF_CORE) */


//#	if !defined(TSL_PREFIX_ENUM_NAME_IF_NO_NAMESPACE)
//#		if defined(TSL_USE_NAMESPACE_FOR_OUT_SCOPE_CONSTANTS)
//#			define TSL_PREFIX_ENUM_NAME_IF_NO_NAMESPACE(n) n
//#		else
//#			define TSL_PREFIX_ENUM_NAME_IF_NO_NAMESPACE(n) TSL_CONCAT_2(TSL_,n)
//#		endif
//#	endif


#endif /* !defined (TSL_NS_CORE_MACROS) */

#if !defined (TSL_UNUSED_PAR)
#	define TSL_UNUSED_PAR(p)
#endif /* !defined (TSL_UNUSED_PAR) */

#endif /* __INCLUDE_GUARD_FILE_CORE_MACROS_H__ */
