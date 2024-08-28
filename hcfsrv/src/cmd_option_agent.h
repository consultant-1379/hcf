#ifndef __INCLUDE_GUARD_CLASS_SRV_CMD_OPTION_AGENT__
#define __INCLUDE_GUARD_CLASS_SRV_CMD_OPTION_AGENT__ cmd_option_agent

/*
 *	@file cmd_option_agent.h
 *	@brief
 *	@author xnicmut (Nicola Muto)
 *	@date 2014-11-06
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
 *	| 2014-001 | 2014-11-06 | xnicmut      | Creation and first revision.        |
 *	+==========+============+==============+=====================================+
 */

#include "hcf/core/cmd_option_parser.h"

#include "srv_macros.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_SRV_CMD_OPTION_AGENT__

HCF_NS_HCF_SRV_BEGIN

class __TSL_CLASS_NAME__ {
	//==============//
	// Constructors //
	//==============//
public:
	inline TSL_CTOR (int argc, char * const argv [])
	TSL_CTOR_INIT_LIST(_cmd_option_parser(argc, argv), _noha(0), _no_daemon(0)) {}

private:
	TSL_CTOR (const __TSL_CLASS_NAME__ &);

	//============//
	// Destructor //
	//============//
public:
	inline TSL_DTOR () {}

	//===========//
	// Functions //
	//===========//
public:
	inline int argc () const { return _cmd_option_parser.argc(); }

	inline const char * const * argv () const { return _cmd_option_parser.argv(); }

	inline const char * program_name () const { return _cmd_option_parser.program_name(); }

	inline int noha () const { return _noha; }

	inline int no_daemon () const { return _no_daemon; }

	inline int parse () { return _cmd_option_parser.parse_long(":", _long_options, *this); }

	//===========//
	// Operators //
	//===========//
public:
	int operator() (int opt_code, int long_index, int optind);

private:
	__TSL_CLASS_NAME__ & operator= (const __TSL_CLASS_NAME__ &);

	//========//
	// Fields //
	//========//
private:
	TSL_NS_HCF_CORE::cmd_option_parser _cmd_option_parser;
	int _noha;
	int _no_daemon;

	static struct option _long_options [];
};

HCF_NS_HCF_SRV_END

#endif /* __INCLUDE_GUARD_CLASS_SRV_CMD_OPTION_AGENT__ */
