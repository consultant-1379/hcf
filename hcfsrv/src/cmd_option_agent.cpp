#include "cmd_option_agent.h"

#include "hcf/trace/com_ericsson_common_hcf_srv_cmdoptionagent.h"
#include "hcf/trace/tracing_macros.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_SRV_CMD_OPTION_AGENT__

HCF_NS_HCF_SRV_BEGIN

HCF_CC_FUNCTION_TRACER_CLASS_DEFINE

struct option __TSL_CLASS_NAME__::_long_options [] = {
	{"noha", no_argument, 0, 0},
	{"no-daemon", no_argument, 0, 0},
	{0, 0, 0, 0}
};

int __TSL_CLASS_NAME__::operator() (int opt_code, int long_index, int optind) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(Parsing, opt_code, long_index, optind);
	int multi_instances_error = 0;

	if (opt_code == 0) { //A long option detected
		//We accept only an exact match.
		if (::strcmp(_long_options[long_index].name, argv()[optind - 1] + 2)) { //Unrecognized option
			HCF_CC_TRACE(ParsingErr, "The provided option is not supported!", argv()[optind - 1]);
			::fprintf(::stderr, "%s: Unrecognized option '%s'\n", program_name(), argv()[optind - 1]);
			return -1;
		}

		switch (long_index) {
		case 0:
			if (_noha++)			++multi_instances_error;
			break;

		case 1:
			if (_no_daemon++)	++multi_instances_error;
			break;

		default:
			HCF_CC_TRACE(ParsingErr, "Generic error while trying to parse the command line option!", "Default case!");
			::fprintf(::stderr, "%s: Generic error parsing command line options\n", program_name());
			return -1;
		}
	} else {
		//Other option found but not supported
		HCF_CC_TRACE(ParsingErr, "Generic error while trying to parse the command line option!", "Else case!");
		::fprintf(::stderr, "%s: Generic error parsing command line options\n", program_name());
		return -1;
	}

	if (multi_instances_error) {
		HCF_CC_TRACE(ParsingErr, "Multiple option instances were found on the command line!", argv()[::optind - 1]);
		::fprintf(::stderr, "%s: multiple instances of option '%s' \n", program_name(), argv()[::optind - 1]);
		return -1;
	}
	return 0;
}

HCF_NS_HCF_SRV_END
