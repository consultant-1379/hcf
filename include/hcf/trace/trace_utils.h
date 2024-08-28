#ifndef __INCLUDE_GUARD_CLASS_TRACE_TRACE_UTILS__
#define __INCLUDE_GUARD_CLASS_TRACE_TRACE_UTILS__ trace_utils

/*
 *	@file trace_utils.h
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
 */

#include <stdint.h>
#include <string>

#include "trace_macros.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_TRACE_TRACE_UTILS__

TSL_NS_HCF_TRACE_BEGIN

class __TSL_CLASS_NAME__ {
	//==============//
	// Constructors //
	//==============//
private:
	TSL_CTOR ();
	TSL_CTOR (const __TSL_CLASS_NAME__ & rhs);

	//============//
	// Destructor //
	//============//
public:
	inline virtual TSL_DTOR () {}

	//==========//
	// Costants //
	//==========//
public:
	static const int HCF_CC_TRACE_DEFAULT_DUMP_STRING_SIZE    = 128;
	static const int HCF_CC_TRACE_DEFAULT_DUMPING_LINE_LENGTH = 16;

	//===========//
	// Functions //
	//===========//
public:
	/**
	 * This static method is a helper function used to provide a hexadecimal
	 * representation of the buffer provided as input argument.
	 * The dumped buffer is then returned into the output string argument.
	 * The user can also provide a value for the maximum size of the dumped string
	 * and a value for the number of characters to be included into each line.
	 *
	 * @param[in]  buffer - The buffer which content must be dumped.
	 * @param[in]  size - The number of characters contained into the input buffer.
	 * @param[in]  dumping_size - The maximum size of the dumped string.
	 * @param[in]  dumping_line_length - The maximum number of characters to be included into each line.
	 * @return The string in which the dumped buffer must be written.
	 */
	static const std::string dump_buffer (char * buffer,
			size_t size,
			size_t dumping_size = HCF_CC_TRACE_DEFAULT_DUMP_STRING_SIZE,
			size_t dumping_line_length = HCF_CC_TRACE_DEFAULT_DUMPING_LINE_LENGTH);

	//===========//
	// Operators //
	//===========//
private:
	__TSL_CLASS_NAME__ & operator= (const __TSL_CLASS_NAME__ & rhs);
};

TSL_NS_HCF_TRACE_END

#endif /* __INCLUDE_GUARD_CLASS_TRACE_TRACE_UTILS__ */
