#include "hcf/core/cmd_option_parser.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_CORE_CMD_OPTION_PARSER__

TSL_NS_HCF_CORE_BEGIN

const char * __TSL_CLASS_NAME__::_program_name = 0;

#ifndef _GNU_SOURCE
const char * __TSL_CLASS_NAME__::program_name () const {
	if (!_program_name) {
		if ((_argc > 0) && _argv && *_argv) {
			const char * last_slash = ::strrchr(*_argv, '/');
			_program_name = last_slash ? last_slash + 1 : *_argv;
		}
	}

	return _program_name;
}
#endif

TSL_NS_HCF_CORE_END
