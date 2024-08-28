#include "cmd_option_agent.h"

#include "hcf/trace/com_ericsson_common_hcf_hcrsfm.h"
#include "hcf/trace/tracing_macros.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_CMD_CMD_OPTION_AGENT__

HCF_NS_HCF_CMD_BEGIN

HCF_CC_FUNCTION_TRACER_CLASS_DEFINE

struct option __TSL_CLASS_NAME__::_long_options [] = {
	{"erase", required_argument, 0, 'e'},
	{"install", required_argument, 0, 'i'},
	{"list", no_argument, 0, 'l'},
	{"upgrade", required_argument, 0, 'u'},
	{"validate", required_argument, 0, 'v'},
	{0, 0, 0, 0}
};

int __TSL_CLASS_NAME__::parse () {
	HCF_CC_TRACE_FUNCTION;
	if (_cmd_option_parser.parse_long(":e:i:lu:v:", _long_options, *this) < 0) {
		char error_msg[1024];
		::strncpy(error_msg, _cmd_option_parser.error_text(), TSL_ARRAY_SIZE(error_msg));
		TSL_SET_ERROR_INFO_AND_RETURN(-1, _cmd_option_parser.error(), error_msg);
	}
	TSL_SET_ERROR_INFO_AND_RETURN(0);
}

int __TSL_CLASS_NAME__::operator() (int opt_code, int long_index, int optind) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(Parsing, opt_code, long_index, optind);

	// Check if a combined option has been provided. In this case, return error
	int multi_instances_error = 0;
	char opt[3] = { '-', static_cast<char>(opt_code), 0 };
	if ((long_index < 0) && (argv()[optind - 1][0] == '-') && (::strcmp(argv()[optind - 1], opt))) {
		TSL_SET_ERROR_INFO_AND_RETURN(-1, TSL_NS_HCF_CORE::ERR_UNSUPPORTED_OPTION, "Unsupported option");
	}

	// We accept only an exact match.
	const int idx = optind - ((opt_code == 'l') ? 1: 2);
	if ((long_index >= 0) && (::strcmp(_long_options[long_index].name, argv()[idx] + 2))) { //Unrecognized option
		TSL_SET_ERROR_INFO_AND_RETURN(-1, TSL_NS_HCF_CORE::ERR_UNSUPPORTED_OPTION, "Unsupported option");
	}

	switch (opt_code) {
	case 'e':
		if (_erase++) 		++multi_instances_error;
		break;

	case 'i':
		if (_install++)		++multi_instances_error;
		break;

	case 'l':
		if (_list++)			++multi_instances_error;
		break;

	case 'u':
		if (_upgrade++)		++multi_instances_error;
		break;

	case 'v':
		if (_validate++)	++multi_instances_error;
		break;

	default: // Other option found but not supported: very strange
		TSL_SET_ERROR_INFO_AND_RETURN(-1, TSL_NS_HCF_CORE::ERR_UNSUPPORTED_OPTION, "Generic error parsing command line options");
	}

	if (multi_instances_error) {
		TSL_SET_ERROR_INFO_AND_RETURN(-1, TSL_NS_HCF_CORE::ERR_TOO_MANY_ARGUMENTS, "Multiple option instances");
	}
	TSL_SET_ERROR_INFO_AND_RETURN(0);
}

HCF_NS_HCF_CMD_END
