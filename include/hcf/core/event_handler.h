#ifndef __INCLUDE_GUARD_CLASS_CORE_EVENT_HANDLER__
#define __INCLUDE_GUARD_CLASS_CORE_EVENT_HANDLER__ event_handler

/*
 *	@file event_handler.h
 *	@brief
 *	@author xnicmut (Nicola Muto)
 *	@date 2014-12-04
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
 *	| 2014-001 | 2014-12-04 | xnicmut      | Creation and first revision.        |
 *	+==========+============+==============+=====================================+
 */

#include "macros.h"

TSL_NS_HCF_CORE_BEGIN

//===========//
// Constants //
//===========//
enum {
	TSL_EVENT_NULL_MASK						=	0x0000

	, TSL_EVENT_READ_MASK					=	0x0001
	, TSL_EVENT_WRITE_MASK				=	0x0002

	, TSL_EVENT_EXCEPTION_MASK		=	0x0010

	, TSL_EVENT_DONT_CALL_MASK		= 0x1000
	, TSL_EVENT_EDGE_TRIGGER_MASK	=	0x2000		// EPOLLET = (1 << 31)

	/* TO BE DELETED */ // , TSL_EVENT_ONESHOT_MASK			=	0x4000		// EPOLLONESHOT = (1 << 30)

	, TSL_EVENT_RDWR_MASK					= TSL_EVENT_READ_MASK | TSL_EVENT_WRITE_MASK
	, TSL_EVENT_RWE_MASK					= TSL_EVENT_RDWR_MASK | TSL_EVENT_EXCEPTION_MASK

/*
	,	TSL_EVENT_INPUT_MASK				= 0x0001		// EPOLLIN = 0x001
	,	TSL_EVENT_INPUT_URG_MASK		= 0x0002		// EPOLLPRI = 0x002
	,	TSL_EVENT_INPUT_NORM_MASK		= 0x0004		// EPOLLRDNORM = 0x040
	,	TSL_EVENT_INPUT_BAND_MASK		= 0x0008		// EPOLLRDBAND = 0x080

	,	TSL_EVENT_OUTPUT_MASK				= 0x0010		// EPOLLOUT = 0x004
	,	TSL_EVENT_OUTPUT_NORM_MASK	= 0x0020		// EPOLLWRNORM = 0x100
	,	TSL_EVENT_OUTPUT_BAND_MASK	= 0x0040		// EPOLLWRBAND = 0x200

	,	TSL_EVENT_ERROR_MASK				= 0x0100		// EPOLLERR = 0x008
	,	TSL_EVENT_INPUT_HUP_MASK		= 0x0200		// EPOLLRDHUP = 0x2000
	,	TSL_EVENT_HUP_MASK					= 0x0400		// EPOLLHUP = 0x010

	,	TSL_EVENT_ONESHOT_MASK			= 0x1000		// EPOLLONESHOT = (1 << 30)
	,	TSL_EVENT_EDGE_TRIGGER_MASK	= 0x2000		// EPOLLET = (1 << 31)
*/

/*	,
  EPOLLMSG = 0x400,
*/
};

TSL_NS_HCF_CORE_END

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_CORE_EVENT_HANDLER__

TSL_NS_HCF_CORE_BEGIN

class __TSL_CLASS_NAME__ {
	//==============//
	// Constructors //
	//==============//
public:
	inline TSL_CTOR () {}

	inline TSL_CTOR (const __TSL_CLASS_NAME__ &) {}

	//============//
	// Destructor //
	//============//
public:
	inline virtual TSL_DTOR () {}

	//===========//
	// Functions //
	//===========//
public:
	inline virtual int get_handle () { return -1; }

	inline virtual int handle_input () { return -1; }

	inline virtual int handle_output () { return -1; }

	inline virtual int handle_exception (
			bool TSL_UNUSED_PAR(error_flag),
			bool TSL_UNUSED_PAR(hungup_flag),
			bool TSL_UNUSED_PAR(read_hungup_flag)) { return -1; }

	inline virtual int handle_close () { return -1; }

	//===========//
	// Operators //
	//===========//
public:
	inline __TSL_CLASS_NAME__ & operator= (const __TSL_CLASS_NAME__ &) { return *this; }

	//========//
	// Fields //
	//========//
private:
};

TSL_NS_HCF_CORE_END

#endif /* __INCLUDE_GUARD_CLASS_CORE_EVENT_HANDLER__ */
