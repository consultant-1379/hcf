#ifndef __INCLUDE_GUARD_CLASS_CORE_ERROR_INFO_PROVIDER__
#define __INCLUDE_GUARD_CLASS_CORE_ERROR_INFO_PROVIDER__ error_info_provider

/*
 *	@file error_info_provider.h
 *	@brief
 *	@author xnicmut (Nicola Muto)
 *	@date 2014-11-17
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
 *	| 2014-001 | 2014-11-17 | xnicmut      | Creation and first revision.        |
 *	+==========+============+==============+=====================================+
 */

#include <errno.h>
#include <stdio.h>
#include <stdarg.h>

#include "macros.h"

#if !defined (TSL_SET_ERROR_INFO_AND_RETURN)
#	define TSL_SET_ERROR_INFO_AND_RETURN(return_code, ...) do { set_error_info(__VA_ARGS__); return return_code; } while (0)

#	define TSL_SET_ERROR_INFO_OK_AND_RETURN(return_code) do { set_error_info(); return return_code; } while (0)
#endif

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_CORE_ERROR_INFO_PROVIDER__

TSL_NS_HCF_CORE_BEGIN

class __TSL_CLASS_NAME__ {
	//===========//
	// Constants //
	//===========//
private:
	enum {
		ERROR_TEXT_SIZE_MAX = 256
	};

	//==============//
	// Constructors //
	//==============//
public:
	// Here the default constructor will be used
	// Also the default assignment operator will be used foe that class

	//============//
	// Destructor //
	//============//
public:
	inline virtual TSL_DTOR () {}

	//===========//
	// Functions //
	//===========//
public:
	inline int error () const { return _error; }

	inline const char * error_text () const { return _error_text; }

	inline int sys_error () const { return _sys_error; }

protected:
	inline int set_error_info (int error = 0) { _sys_error = errno; *_error_text = 0; return (_error = error); }

	inline int set_error_info (const char * format, ...) __attribute__ ((format (printf, 2, 3))) {
		va_list argp;
		va_start(argp, format);
		const int return_code = set_error_info(errno, -1, format, argp);
		va_end(argp);
		return return_code;
	}

	inline int set_error_info (int error, const char * format, ...) __attribute__ ((format (printf, 3, 4))) {
		va_list argp;
		va_start(argp, format);
		const int return_code = set_error_info(errno, error, format, argp);
		va_end(argp);
		return return_code;
	}

	inline int set_error_info (int sys_error, int error) { _sys_error = sys_error; *_error_text = 0; return (_error = error); }

	inline int set_error_info (int sys_error, int error, const char * format, ...) __attribute__ ((format (printf, 4, 5))) {
		va_list argp;
		va_start(argp, format);
		const int return_code = set_error_info(sys_error, error, format, argp);
		va_end(argp);
		return return_code;
	}

private:
	inline int set_error_info (int sys_error, int error, const char * format, va_list ap) {
		_sys_error = sys_error;
		::vsnprintf(_error_text, TSL_ARRAY_SIZE(_error_text), format, ap);
		_error_text[TSL_ARRAY_SIZE(_error_text) - 1] = 0;
		return (_error = error);
	}

	//========//
	// Fields //
	//========//
private:
	static __thread int _sys_error;
	static __thread int _error;
	static __thread char _error_text[ERROR_TEXT_SIZE_MAX];
};

TSL_NS_HCF_CORE_END

#endif /* __INCLUDE_GUARD_CLASS_CORE_ERROR_INFO_PROVIDER__ */
