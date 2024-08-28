#ifndef __INCLUDE_GUARD_CLASS_CORE_CMD_OPTION_PARSER__
#define __INCLUDE_GUARD_CLASS_CORE_CMD_OPTION_PARSER__ cmd_option_parser

/*
 *	@file cmd_option_parser.h
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
 */

#include <getopt.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

#include "macros.h"
#include "error_constants.h"
#include "error_info_provider.h"

#include "hcf/trace/com_ericsson_common_hcf_core_cmdoptionparser.h"
#include "hcf/trace/tracing_macros.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_CORE_CMD_OPTION_PARSER__

TSL_NS_HCF_CORE_BEGIN

HCF_CC_FUNCTION_TRACER_CLASS_DEFINE

class __TSL_CLASS_NAME__ : public error_info_provider {
	//==============//
	// Constructors //
	//==============//
public:
	inline TSL_CTOR (int argc, char * const argv [])
	TSL_CTOR_INIT_LIST(error_info_provider(), _argc(argc), _argv(argv)) {}

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
	inline int argc () const { return _argc; }

	inline const char * const * argv () const { return _argv; }

#ifdef _GNU_SOURCE
	inline
#endif
	const char * program_name () const
#ifdef _GNU_SOURCE
	{ return ::program_invocation_short_name; }
#endif
	;

	template <typename checker_t>
	int parse_long (const char * optstring, const struct ::option * longopts, checker_t & checker) {
		HCF_CC_TRACE_FUNCTION;
		int long_index = -1;
		int opt_code = 0;
		int return_code = 0;

		opterr = 0; // Preventing the print of a system error message
		HCF_CC_TRACE(ParseLong, "Starting to parse command line options!", optstring, opt_code, long_index, -1);

		while ((return_code == 0) && (opt_code = ::getopt_long(_argc, _argv, optstring, longopts, &long_index)) != -1) {
			if (opt_code == '?') { // The user provided an option not supported here
				HCF_CC_TRACE(ParseLongErr, "Call 'getopt_long' returned the option code '?': an unsupported option was found!", ERR_UNSUPPORTED_OPTION);
				TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_UNSUPPORTED_OPTION, "Unsupported option");
			} else if (opt_code == ':') { // The user missed the option argument.
				HCF_CC_TRACE(ParseLongErr, "Call 'getopt_long' returned the option code ':': an argument required by a command line option is missing!", ERR_MISSING_ARGUMENT);
				TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_MISSING_ARGUMENT, "Argument missed for the option '%s'", _argv[::optind - 1]);
			} else { // Call the checker to handle the current option info
				HCF_CC_TRACE(ParseLong, "A valid command line option has been found: invoking checker object!", optstring, opt_code, long_index, ::optind);
				return_code = checker(opt_code, long_index, ::optind);
			}
		}

		if (return_code == 0) {
			// Rejecting the '--' option on command line
			// Note: if "--" command line argument is found, getopt_long() increments <optind> variable and returns -1.
			if ((::optind > 1) && !::strcmp(_argv[::optind - 1], "--")) {
				HCF_CC_TRACE(ParseLongErr, "The '--' command line argument was found: it is not supported!", ERR_UNRECOGNIZED_OPTION);
				TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_UNRECOGNIZED_OPTION, "Unrecognized option '%s'", _argv[::optind - 1]);
			}

			// We must check the command has no argument on its line
			if (::optind < _argc) {
				HCF_CC_TRACE(ParseLongErr, "One or more arguments were found on the command line: the option doens't require them!", ERR_TOO_MANY_ARGUMENTS);
				TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_TOO_MANY_ARGUMENTS, "One or more argument found on command line");
			}
		}

		return return_code;
	}

	//===========//
	// Operators //
	//===========//
private:
	__TSL_CLASS_NAME__ & operator= (const __TSL_CLASS_NAME__ &);

	//========//
	// Fields //
	//========//
private:
	int _argc;
	char * const * const _argv;

	static const char * _program_name;
};

TSL_NS_HCF_CORE_END

#endif /* __INCLUDE_GUARD_CLASS_CORE_CMD_OPTION_PARSER__ */
