#ifndef __INCLUDE_GUARD_CLASS_TEST_01_FIFO_READER__
#define __INCLUDE_GUARD_CLASS_TEST_01_FIFO_READER__ fifo_reader

/*
 *	@file fifo_reader.h
 *	@brief
 *	@author xnicmut (Nicola Muto)
 *	@date 2014-11-11
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
 *	| 2014-001 | 2014-11-11 | xnicmut      | Creation and first revision.        |
 *	+==========+============+==============+=====================================+
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>

#include "hcf/core/event_handler.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_TEST_01_FIFO_READER__

class __TSL_CLASS_NAME__ : public TSL_NS_HCF_CORE::event_handler {
	//==============//
	// Constructors //
	//==============//
public:
	inline TSL_CTOR ()
	TSL_CTOR_INIT_LIST(event_handler(), _fifo_fd(-1), _kilo_bytes_read(0), _bytes_read(0)) { TSL_TRACE_ON_SCREEN; }

private:
	TSL_CTOR (const __TSL_CLASS_NAME__ &);

	//============//
	// Destructor //
	//============//
public:
	inline virtual TSL_DTOR () { TSL_TRACE_ON_SCREEN; close(); }

	//===========//
	// Functions //
	//===========//
public:
	inline int open (const char * path) {
		TSL_TRACE_ON_SCREEN;
		const int fd = ::open(path, O_RDONLY | O_NONBLOCK);
		if (fd >= 0) {
			::close(_fifo_fd);
			_fifo_fd = fd;
			return 0;
		}
		return -1;
	}

	inline int close () {
		TSL_TRACE_ON_SCREEN;
		int return_code = ::close(_fifo_fd);
		return (return_code ?: (_fifo_fd = -1, 0));
	}

	//====================================//
	// epoll_event_handler implementation //
	//====================================//
public:
	inline virtual int get_handle () { TSL_TRACE_ON_SCREEN; return _fifo_fd; }

	virtual int handle_input ();

	virtual int handle_exception (
			bool TSL_UNUSED_PAR(error_flag),
			bool TSL_UNUSED_PAR(hungup_flag),
			bool TSL_UNUSED_PAR(read_hungup_flag));

	virtual int handle_close ();

	//===========//
	// Operators //
	//===========//
private:
	__TSL_CLASS_NAME__ & operator= (const __TSL_CLASS_NAME__ &);

	//========//
	// Fields //
	//========//
private:
	int _fifo_fd;
	uint32_t _kilo_bytes_read;
	uint32_t _bytes_read;
};

#endif /* __INCLUDE_GUARD_CLASS_TEST_01_FIFO_READER__ */
