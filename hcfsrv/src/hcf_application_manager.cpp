#include <syslog.h>
#include <new>

#include "hcf/core/logger.h"

#include "hcf/trace/com_ericsson_common_hcf_srv_hcfapplicationmanager.h"
#include "hcf/trace/tracing_macros.h"

#include "constants.h"
#include "hcf_application_manager.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_SRV_HCF_APPLICATION_MANAGER__

HCF_NS_HCF_SRV_BEGIN

HCF_CC_FUNCTION_TRACER_CLASS_DEFINE

int __TSL_CLASS_NAME__::perform_state_transition_to_active (TSL_NS_HCF_INTERNALAPI::amf_ha_state_t previous_state) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(StateChange, "Received a request to change the HA status!", previous_state, get_ha_state());
	HCF_CC_SYSLOG(LOG_DEBUG, LEVEL_NOTICE, "Received a request to pass from %d state to ACTIVE!", previous_state);
	return activate_application();
}

int __TSL_CLASS_NAME__::perform_state_transition_to_passive (TSL_NS_HCF_INTERNALAPI::amf_ha_state_t previous_state) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(StateChange, "Received a request to change the HA status!", previous_state, get_ha_state());
	HCF_CC_SYSLOG(LOG_DEBUG, LEVEL_NOTICE, "Received a request to pass from %d state to PASSIVE!", previous_state);
	return shutdown_application();
}

int __TSL_CLASS_NAME__::perform_state_transition_to_quiesing (TSL_NS_HCF_INTERNALAPI::amf_ha_state_t previous_state) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(StateChange, "Received a request to change the HA status!", previous_state, get_ha_state());
	HCF_CC_SYSLOG(LOG_DEBUG, LEVEL_NOTICE, "Received a request to pass from %d state to QUIESING!", previous_state);
	return shutdown_application();
}

int __TSL_CLASS_NAME__::perform_state_transition_to_quiesced (TSL_NS_HCF_INTERNALAPI::amf_ha_state_t previous_state) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(StateChange, "Received a request to change the HA status!", previous_state, get_ha_state());
	HCF_CC_SYSLOG(LOG_DEBUG, LEVEL_NOTICE, "Received a request to pass from %d state to QUIESCED!", previous_state);
	return shutdown_application();
}

int __TSL_CLASS_NAME__::perform_component_health_check () {
	HCF_CC_TRACE_FUNCTION;
	// Nothing to do for now!!!
	return ERR_NO_ERROR;
}

int __TSL_CLASS_NAME__::perform_component_terminate () {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(StateChange, "Received a request to terminate the component!", -1, get_ha_state());
	HCF_CC_SYSLOG(LOG_DEBUG, LEVEL_NOTICE, "Received a request to terminate the application!");
	return shutdown_application();
}

int __TSL_CLASS_NAME__::perform_component_remove () {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(StateChange, "Received a request to remove the component!", -1, get_ha_state());
	HCF_CC_SYSLOG(LOG_DEBUG, LEVEL_NOTICE, "Received a request to remove the application!");
	return shutdown_application();
}

int __TSL_CLASS_NAME__::perform_application_shutdown () {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(StateChange, "Received a request to shutdown the component!", -1, get_ha_state());
	HCF_CC_SYSLOG(LOG_DEBUG, LEVEL_NOTICE, "Received a request to shutdown the application!");
	return shutdown_application();
}

void __TSL_CLASS_NAME__::restart_application () {
	HCF_CC_TRACE_FUNCTION;
	if (!_ha_mode) {
		HCF_CC_TRACE(RestartApplication, "Received a request to restart the application, but we aren't running in HA mode, nothing to do!");
		return;
	}

	HCF_CC_TRACE(RestartApplication, "Received a request to restart the application, asking AMF to restart the component!");
	HCF_CC_SYSLOG(LOG_ERR, LEVEL_CRITICAL, "Received a request to restart the application!");
	component_report_error(TSL_NS_HCF_INTERNALAPI::RECOMMENDED_RECOVERY_COMPONENT_RESTART);
}

int __TSL_CLASS_NAME__::activate_debug_mode () {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(ActivateDebugMode, "Received a request to activate the application in DEBUG mode: activating it!");

	if (const int call_result = activate_application()) {
		HCF_CC_TRACE(ActivateDebugModeErr, "Call 'activate_application' failed.", call_result);
		HCF_CC_SYSLOG(LOG_ERR, LEVEL_ERROR, "Call 'activate_application' failed with error_code == %d", call_result);
		return call_result;
	}
	HCF_CC_TRACE(ActivateDebugMode, "Application is now activated, waiting the Root Worker Thread termination!");
	HCF_CC_SYSLOG(LOG_DEBUG, LEVEL_NOTICE, "Waiting the Root Worker Thread termination!");

	int root_worker_thread_exit_code = 0;
	if (const int call_result = _root_worker_thread->join(root_worker_thread_exit_code)) {
		HCF_CC_TRACE(ActivateDebugModeErr, "Call '_root_worker_thread->join' failed.", call_result);
		HCF_CC_SYSLOG(LOG_ERR, LEVEL_ERROR, "Failed to wait the Root Worker Thread, call_result == %d!", call_result);
		return ERR_ROOT_WORKER_THREAD_WAIT_FAILED;
	}
	HCF_CC_TRACE(ActivateDebugMode, "Root Worker Thread terminated, freeing allocated memory!");
	HCF_CC_SYSLOG(LOG_DEBUG, LEVEL_NOTICE, "Root Worker Thread terminated with exit_code == %d!", root_worker_thread_exit_code);

	delete _root_worker_thread; _root_worker_thread = 0;
	return ERR_NO_ERROR;
}

int __TSL_CLASS_NAME__::activate_application () {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(ActivateApplication, "Received a request to activate the application!");
	HCF_CC_SYSLOG(LOG_DEBUG, LEVEL_NOTICE, "Received a request to activate the application!");

	if (_root_worker_thread) {
		HCF_CC_TRACE(ActivateApplication, "The application is already active, nothing to do!");
		HCF_CC_SYSLOG(LOG_DEBUG, LEVEL_NOTICE, "The application is already active, nothing to do!");
		return ERR_NO_ERROR;
	}

	HCF_CC_TRACE(ActivateApplication, "Allocating memory for the Root Worker Thread object!");
	if (!(_root_worker_thread = new (std::nothrow) root_worker_thread())) {
		HCF_CC_TRACE(ActivateApplicationErr, "Memory allocation failed for the Root Worker Thread object!", ERR_ROOT_WORKER_THREAD_CREATE_FAILED, "");
		HCF_CC_SYSLOG(LOG_ERR, LEVEL_ERROR, "Memory allocation failed for the Root Worker Thread object!");
		return ERR_ROOT_WORKER_THREAD_CREATE_FAILED;
	}

	HCF_CC_TRACE(ActivateApplication, "Starting the Root Worker Thread...");
	HCF_CC_SYSLOG(LOG_DEBUG, LEVEL_NOTICE, "Starting the Root Worker Thread...");
	if (const int call_result = _root_worker_thread->activate(this)) {
		HCF_CC_TRACE(ActivateApplicationErr, "Call '_root_worker_thread->activate' failed.", _root_worker_thread->error(), _root_worker_thread->error_text());
		HCF_CC_SYSLOG(LOG_ERR, LEVEL_ERROR, "Failed to start the Root Worker Thread, call_result == %d, errno == %d, "
				"error == %d, error_text == '%s'!", call_result, _root_worker_thread->sys_error(), _root_worker_thread->error(), _root_worker_thread->error_text());
		return ERR_ROOT_WORKER_THREAD_START_FAILED;
	}

	HCF_CC_TRACE(ActivateApplication, "Root Worker Thread correctly started!");
	return ERR_NO_ERROR;
}

int __TSL_CLASS_NAME__::shutdown_application () {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(ShutdownApplication, "Received a request to shutdown the application!");
	HCF_CC_SYSLOG(LOG_DEBUG, LEVEL_NOTICE, "Received a request to stop the application!");

	if (!_root_worker_thread) {
		HCF_CC_TRACE(ShutdownApplication, "The application is already passive, nothing to do!");
		HCF_CC_SYSLOG(LOG_DEBUG, LEVEL_NOTICE, "The application is already passive, nothing to do!");
		return ERR_NO_ERROR;
	}

	HCF_CC_TRACE(ShutdownApplication, "Stopping the Root Worker Thread...");
	HCF_CC_SYSLOG(LOG_DEBUG, LEVEL_NOTICE, "Stopping the Root Worker Thread...");
	_root_worker_thread->stop();

	if (_ha_mode) {
		// The thread must be joined and deleted only in HA mode
		HCF_CC_TRACE(ShutdownApplication, "Waiting the Root Worker Thread termination...");
		int root_worker_thread_exit_code = 0;
		if (const int call_result = _root_worker_thread->join(root_worker_thread_exit_code)) {
			HCF_CC_TRACE(ShutdownApplicationErr, "Call '_root_worker_thread->join' failed.", _root_worker_thread->error(), _root_worker_thread->error_text());
			HCF_CC_SYSLOG(LOG_ERR, LEVEL_ERROR, "Failed to wait the Root Worker Thread, call_result == %d, errno == %d, "
					"error == %d, error_text == '%s'!", call_result, _root_worker_thread->sys_error(),
					_root_worker_thread->error(), _root_worker_thread->error_text());
			return ERR_ROOT_WORKER_THREAD_WAIT_FAILED;
		}

		HCF_CC_TRACE(ShutdownApplication, "Root Worker Thread terminated, freeing allocated memory!");
		delete _root_worker_thread; _root_worker_thread = 0;
	}

	return ERR_NO_ERROR;
}

HCF_NS_HCF_SRV_END
