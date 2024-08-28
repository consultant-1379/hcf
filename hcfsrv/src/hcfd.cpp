#include <sys/stat.h>
#include <stdio.h>
#include <signal.h>
#include <syslog.h>
#include <new>

#include "libxml/parser.h"
#include "hcf/core/process_daemonizer.h"
#include "hcf/core/logger.h"

#include "constants.h"
#include "app_types.h"
#include "cmd_option_agent.h"
#include "hcf_application_manager.h"
#include "configurator.h"

#include "hcf/trace/com_ericsson_common_hcf_srv_hcfd.h"
#include "hcf/trace/tracing_macros.h"

HCF_CC_FUNCTION_TRACER_CLASS_DEFINE

namespace {
	HCF_NS_HCF_SRV::hcf_application_manager * application_manager;

	inline void print_command_usage (const char * program_name) {
		::fprintf(stderr, "Incorrect usage:\nUsage:\n%s [--noha] [--no-daemon]\n", program_name);
	}

	void signal_handler (int signum) {
		::syslog(LOG_DEBUG, "Received signal '%d' (%s)", signum, ::strsignal(signum));

		if ((signum == SIGINT) && (!application_manager->ha_mode())) {
			application_manager->perform_application_shutdown();
		}
	}
}

int main (int argc, char * argv[]) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(Main, "HCF server starting!", TSL_STRINGIZE(HCF_CC_PRODUCT_REVISION));
	::syslog(LOG_DEBUG, "HCF server (REVISION == %s) starting!", TSL_STRINGIZE(HCF_CC_PRODUCT_REVISION));
	HCF_NS_HCF_SRV::cmd_option_agent cmd_opt_agent(argc, argv);

	HCF_CC_TRACE(Main, "Parsing the command-line options", TSL_STRINGIZE(HCF_CC_PRODUCT_REVISION));
	if (const int call_result = cmd_opt_agent.parse() < 0) { // Error parsing command line
		HCF_CC_TRACE(MainErr, "Call 'cmd_opt_agent.parse' failed.", call_result);
		print_command_usage(cmd_opt_agent.program_name());
		return HCF_NS_HCF_SRV::APPLICATION_EXIT_BAD_INVOCATION;
	}
	const bool ha_mode = (cmd_opt_agent.noha() == 0);
	const bool daemonize_required = (cmd_opt_agent.no_daemon() == 0);
	HCF_CC_TRACE(Main, "Starting HCF server with the following HA mode:", ((ha_mode) ? "ENABLED" : "DISABLED"));
	HCF_CC_TRACE(Main, "Starting HCF server with the following daemonization configuration:", ((daemonize_required) ? "REQUIRED" : "NOT REQUIRED"));

	if (daemonize_required) {
		HCF_CC_TRACE(Main, "Daemonization is required, running as not root user is:",
#ifdef HCF_CC_RUNS_WITH_NO_ROOT_PERMISSIONS
				"ENABLED"
#else
				"DISABLED"
#endif
		);
		if (TSL_NS_HCF_CORE::process_daemonizer::daemonize(
				0, (ha_mode ? 0 : 1)
#ifdef HCF_CC_RUNS_WITH_NO_ROOT_PERMISSIONS
				, 0, HCF_NS_HCF_SRV::HCF_SRV_USER_NAME, HCF_NS_HCF_SRV::HCF_SRV_GROUP_NAME
				, HCF_NS_HCF_SRV::HCF_SRV_SECONDARY_GROUPS_NAMES
#endif
		)) {	// ERROR: Daemonizing this process
			const int errno_save = errno;
			HCF_CC_TRACE(MainErr, "Call 'process_daemonizer::daemonize' failed.", errno_save);
			::syslog(LOG_DEBUG, "ERROR: Call 'process_daemonizer::daemonize()' failed: errno == %d", errno_save);
			return HCF_NS_HCF_SRV::APPLICATION_EXIT_DEMONIZATION_FAILED;
		}
	}
	HCF_CC_TRACE(Main, "Registering the signal handler for the SIGINT and SIGPIPE signals!", "");
	::signal(SIGINT, &signal_handler);
	::signal(SIGPIPE, &signal_handler);

	if (!ha_mode) {
		HCF_CC_TRACE(Main, "HCF started in debug mode, checking if the Trace CC is installed!", "");
		/*
		 * Check if TraceEA executables and LTTnG libraries are present:
		 * in that case check if the LD_PRELOAD environment variable has been correctly
		 * exported in order to use the tracing functionalities provided by LTTnG.
		 * This check must be done only in no-HA mode, since normally it's performed by
		 * the HCF proxy script.
		 */
		struct stat st;
		if (!::stat("/opt/trace_ea/bin/TraceEa", &st)) {
			// TraceEA (and LTTnG) is installed: check if the env variable has been correctly exported!
			const char * ld_preload_value = ::getenv("LD_PRELOAD");
			HCF_CC_TRACE(Main, "Trace CC is installed, the value of LD_PRELOAD environment variable is:", (ld_preload_value ?: "EMPTY"));

			if (!ld_preload_value) {
				// No value for LD_PRELOAD provided: print a proper error message
				HCF_CC_TRACE(MainErr, "Trace CC is installed, but the LD_PRELOAD environment variable is not set!", HCF_NS_HCF_SRV::APPLICATION_EXIT_BAD_INVOCATION);
				::fprintf(stderr, "\nThe TraceCC is installed on the node, but the environment variable LD_PRELOAD "
						"is empty: please set it!\nexport LD_PRELOAD=/opt/hcf/lib/libhcftrace.so:/usr/lib64/liblttng-ust-fork.so.0.0.0\n");
				return HCF_NS_HCF_SRV::APPLICATION_EXIT_BAD_INVOCATION;
			}
			else {
				// A value has been provided for LD_PRELOAD: check if the correct libraries are present!
				const std::string ld_preload_string(ld_preload_value);
				HCF_CC_TRACE(Main, "Trace CC is installed, checking if Trace libraries have been preloaded!", ld_preload_value);

				if ((ld_preload_string.find("libhcftrace.so") == std::string::npos) || (ld_preload_string.find("liblttng-ust-fork.so") == std::string::npos)) {
					// No value for LD_PRELOAD provided: print a proper error message
					HCF_CC_TRACE(MainErr, "Trace CC is installed, but the LD_PRELOAD environment variable is not correctly set!", HCF_NS_HCF_SRV::APPLICATION_EXIT_BAD_INVOCATION);
					::fprintf(stderr, "\nThe TraceCC is installed on the node, but the environment variable LD_PRELOAD has a bad value (%s): "
							"please set it!\nexport LD_PRELOAD=/opt/hcf/lib/libhcftrace.so:/usr/lib64/liblttng-ust-fork.so.0.0.0\n", ld_preload_value);
					return HCF_NS_HCF_SRV::APPLICATION_EXIT_BAD_INVOCATION;
				}
			}
		}
	}

	HCF_CC_TRACE(Main, "Initializing the logging framework with application name:", HCF_NS_HCF_SRV::HCF_APPLICATION_NAME);
	TSL_NS_HCF_CORE::logger::initialize(HCF_NS_HCF_SRV::HCF_APPLICATION_NAME);

	HCF_CC_TRACE(Main, "Allocating memory for the Application Manager object!", "");
	if (!(application_manager = new (std::nothrow) HCF_NS_HCF_SRV::hcf_application_manager(cmd_opt_agent.program_name(), ha_mode))) {
		HCF_CC_TRACE(MainErr, "Memory allocation failed for Application Manager object!", HCF_NS_HCF_SRV::APPLICATION_EXIT_MEMORY_ALLOCATION_ERROR);
		HCF_CC_SYSLOG(LOG_ERR, LEVEL_ERROR, "Memory allocation failed for Application Manager object!");
		TSL_NS_HCF_CORE::logger::finalize();
		return HCF_NS_HCF_SRV::APPLICATION_EXIT_MEMORY_ALLOCATION_ERROR;
	}

	HCF_CC_SYSLOG(LOG_DEBUG, LEVEL_NOTICE, "Starting HCF server (REVISION == %s) in %s mode (UID == %u, GID == %u)!",
			TSL_STRINGIZE(HCF_CC_PRODUCT_REVISION), ((ha_mode) ? "HA" : "DEBUG"), ::getuid(), ::getgid());

	// Create the link to the output folder into FileM path
	HCF_CC_TRACE(Main, "Linking the HCF output folder to the NBI folder!", "");
	if (HCF_NS_HCF_SRV::configurator::link_nbi_to_output_folder()) {
		const int errno_save = errno;
		HCF_CC_TRACE(MainErr, "Call 'configurator::link_nbi_to_output_folder' failed.", errno_save);
		HCF_CC_LOG(LEVEL_ERROR, "Call 'configurator::link_nbi_to_output_folder' failed, error == %d!", errno_save);
	}

  HCF_CC_LOG(LEVEL_NOTICE, "Initializing the XML2 library...");
  LIBXML_TEST_VERSION

  // Initialize the parser library
	HCF_CC_TRACE(Main, "Initializing the XML2 library...", "");
  xmlInitParser();

  HCF_CC_TRACE(Main, "Activating the Application Manager object.", "");
	if (const int call_result = application_manager->activate()) {
		HCF_CC_TRACE(MainErr, "Call 'application_manager->activate' failed.", call_result);
		HCF_CC_SYSLOG(LOG_ERR, LEVEL_ERROR, "Call 'application_manager->activate' failed, call_result == %d", call_result);
	}

	HCF_CC_TRACE(Main, "The Application Manager object has been finalized, freeing the allocated memory!", "");
	delete application_manager;

	HCF_CC_TRACE(Main, "HCF server stopping!", TSL_STRINGIZE(HCF_CC_PRODUCT_REVISION));
	HCF_CC_SYSLOG(LOG_DEBUG, LEVEL_NOTICE, "HCF server (REVISION == %s) exiting!", TSL_STRINGIZE(HCF_CC_PRODUCT_REVISION));

	HCF_CC_LOG(LEVEL_NOTICE, "Finalizing the XML2 library...");
  HCF_CC_TRACE(Main, "Finalizing the XML2 library...", "");

  // Finalize the parser library
	xmlCleanupParser();

  HCF_CC_TRACE(Main, "Finalizing the logging framework.", HCF_NS_HCF_SRV::HCF_APPLICATION_NAME);
	TSL_NS_HCF_CORE::logger::finalize();
	return HCF_NS_HCF_SRV::APPLICATION_EXIT_OK;
}
