#ifndef __INCLUDE_GUARD_CLASS_INTERNALAPI_PROTOCOL_HANDLER__
#define __INCLUDE_GUARD_CLASS_INTERNALAPI_PROTOCOL_HANDLER__ protocol_handler

/*
 *	@file protocol_handler.h
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

#include <inttypes.h>

#include "hcf/core/error_info_provider.h"

#include "internalapi_macros.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_INTERNALAPI_PROTOCOL_HANDLER__

TSL_NS_HCF_INTERNALAPI_BEGIN

class __TSL_CLASS_NAME__ : public TSL_NS_HCF_CORE::error_info_provider {
	//==============//
	// Constructors //
	//==============//
public:
	inline TSL_CTOR ()
		TSL_CTOR_INIT_LIST(TSL_NS_HCF_CORE::error_info_provider(),
				_buffer(0), _buffer_length(0), _socket(-1)) {}

	inline explicit TSL_CTOR (int socket)
		TSL_CTOR_INIT_LIST(TSL_NS_HCF_CORE::error_info_provider(),
				_buffer(0), _buffer_length(0), _socket(socket)) {}

private:
	TSL_CTOR (const __TSL_CLASS_NAME__ &);

	//============//
	// Destructor //
	//============//
public:
	inline TSL_DTOR () { delete _buffer; _buffer = 0; }

	//===========//
	// Functions //
	//===========//
public:
	int make_primitive (uint32_t primitive_id, uint32_t msg_to_send_len, const char * msg_to_send);

	int unpack_primitive (uint32_t & primitive_id, char * primitive_msg, uint32_t primitive_msg_size);

	inline void bind (int socket) { _socket = socket; }

	int send_primitive ();

	int recv_primitive ();

	//===========//
	// Operators //
	//===========//
private:
	__TSL_CLASS_NAME__ & operator= (const __TSL_CLASS_NAME__ &);

	//========//
	// Fields //
	//========//
private:
	char * _buffer;
	size_t _buffer_length;
	int _socket;
};

TSL_NS_HCF_INTERNALAPI_END

#endif /* __INCLUDE_GUARD_CLASS_INTERNALAPI_PROTOCOL_HANDLER__ */
