#ifndef __INCLUDE_GUARD_CLASS_CMD_CMD_OPTION_AGENT__
#define __INCLUDE_GUARD_CLASS_CMD_CMD_OPTION_AGENT__ cmd_option_agent

/*
 *	@file cmd_option_agent.h
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

#include "hcf/core/cmd_option_parser.h"
#include "hcf/core/error_info_provider.h"
#include "hcf/core/error_constants.h"

#include "cmd_macros.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_CMD_CMD_OPTION_AGENT__

HCF_NS_HCF_CMD_BEGIN

class __TSL_CLASS_NAME__ : public TSL_NS_HCF_CORE::error_info_provider {
	//==============//
	// Constructors //
	//==============//
public:
	inline TSL_CTOR (int argc, char * const argv [])
		TSL_CTOR_INIT_LIST(TSL_NS_HCF_CORE::error_info_provider(), _cmd_option_parser(argc, argv),
			_erase(0), _install(0), _list(0), _upgrade(0), _validate(0)) {}

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

	inline int erase () const { return _erase; }

	inline int install () const { return _install; }

	inline int list () const { return _list; }

	inline int upgrade () const { return _upgrade; }

	inline int validate () const { return _validate; }

	int parse ();

	inline bool is_cmd_line_valid () {
		const int how_many_options = (_erase + _install + _list + _upgrade + _validate);
		const bool is_valid = (how_many_options == 1);

		TSL_SET_ERROR_INFO_AND_RETURN(
				is_valid,
				(is_valid) ? 0 : ((how_many_options == 0) ? TSL_NS_HCF_CORE::ERR_MISSING_ARGUMENT : TSL_NS_HCF_CORE::ERR_TOO_MANY_ARGUMENTS),
				(is_valid) ? "" : ((how_many_options == 0) ? "No options have been provided" : "More than one option has been provided")
		);
	}

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
	int _erase;
	int _install;
	int _list;
	int _upgrade;
	int _validate;

	static struct option _long_options [];
};

HCF_NS_HCF_CMD_END

#endif /* __INCLUDE_GUARD_CLASS_CMD_CMD_OPTION_AGENT__ */
