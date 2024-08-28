#include <syslog.h>
#include <signal.h>
#include <errno.h>
#ifndef _GNU_SOURCE
# define _GNU_SOURCE
#endif
#include <unistd.h>
#include <poll.h>
#include <fcntl.h>
#include <new>

#include "hcf/core/logger.h"

#include "hcf/trace/com_ericsson_common_hcf_internal_applicationmanager.h"
#include "hcf/trace/tracing_macros.h"

#include "application_manager.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_INTERNALAPI_APPLICATION_MANAGER__

TSL_NS_HCF_INTERNALAPI_BEGIN

HCF_CC_FUNCTION_TRACER_CLASS_DEFINE

// Static fields initialization
int __TSL_CLASS_NAME__::_shutdown_pipe_fd[2];
__TSL_CLASS_NAME__ * __TSL_CLASS_NAME__::_application_manager = 0;

int __TSL_CLASS_NAME__::activate_ha_mode () {
	HCF_CC_TRACE_FUNCTION;

	int call_result = 0;
	_termination_event_received = false;

	// Initialize the CoreMW framework
	HCF_CC_TRACE(ActivateHAMode, "Initializing the CoreMW Framework!", _daemon_name);
	if ((call_result = initialize())) {
		HCF_CC_SYSLOG(LOG_ERR, LEVEL_ERROR, "Call 'initialize' failed with error == %d!", call_result);
		HCF_CC_TRACE(ActivateHAModeErr, "Call 'initialize' failed.", call_result);
		return -1;
	}

	// Create the pipe to handle the shutdown mechanism and set it to non-blocking mode
	HCF_CC_TRACE(ActivateHAMode, "Creating the shutdown pipe!", _daemon_name);
	if (::pipe2(_shutdown_pipe_fd, O_NONBLOCK) < 0) {
		const int errno_save = errno;
		HCF_CC_SYSLOG(LOG_ERR, LEVEL_ERROR, "Call 'pipe' failed, errno == %d!", errno_save);
		HCF_CC_TRACE(ActivateHAModeErr, "Call 'pipe' failed.", errno_save);
		return -1;
	}

	// Create and fill the poll file descriptors array
	const nfds_t num_of_fds = 2;
	struct pollfd poll_fds[num_of_fds];

	poll_fds[0].fd = get_selectable_obj();
	poll_fds[0].events = POLLIN | POLLPRI | POLLRDHUP | POLLHUP | POLLERR;
	poll_fds[1].fd = _shutdown_pipe_fd[0];
	poll_fds[1].events = POLLIN | POLLPRI | POLLRDHUP | POLLHUP | POLLERR;

	// Application manager main loop: handle the AMF state assignment callbacks
	while (true) {
		HCF_CC_TRACE(ActivateHAMode, "Waiting for new events!", _daemon_name);
		const int poll_result = ::poll(poll_fds, num_of_fds, -1);

		if (poll_result < 0) {	// ERROR: Call 'poll' failed
			const int errno_save = errno;
			if (errno_save != EINTR) {
				HCF_CC_SYSLOG(LOG_ERR, LEVEL_ERROR, "Call 'poll' failed, errno == %d!", errno_save);
				HCF_CC_TRACE(ActivateHAModeErr, "Call 'poll' failed.", errno_save);
				break;
			}
		}
		else if (poll_result == 0) {}	// Timeout occurred, nothing to do (it shouldn't occur since no timeout is used)
		else {	// Data ready to be read: check what file descriptor has been signaled
			if (poll_fds[0].revents & (POLLIN | POLLPRI)) {
				HCF_CC_TRACE(ActivateHAMode, "Event found on the AMF handle, dispatching it!", _daemon_name);

				// Good event found on the AMF handle: dispatch it!
				if ((call_result = dispatch(AMF_DISPATCH_ALL))) {
					HCF_CC_SYSLOG(LOG_ERR, LEVEL_ERROR, "Call 'dispatch' failed with error == %d!", call_result);
					HCF_CC_TRACE(ActivateHAModeErr, "Call 'dispatch' failed.", call_result);
					break;
				}

				// After dispatching the AMF event, check if the application must be stopped
				if (_termination_event_received)	break;
			}

			if (poll_fds[1].revents & (POLLIN | POLLPRI)) {
				HCF_CC_TRACE(ActivateHAMode, "Event found on the shutdown handle, stopping application!", _daemon_name);

				// Stop event signaled: stop the application calling the shutdown callback
				if ((call_result = perform_application_shutdown())) {
					HCF_CC_SYSLOG(LOG_ERR, LEVEL_ERROR, "Call 'perform_application_shutdown' failed with error == %d!", call_result);
					HCF_CC_TRACE(ActivateHAModeErr, "Call 'perform_application_shutdown' failed.", call_result);
				}
				_termination_event_received = true;
				break;
			}
		}
	}

	// Close the pipe handles previously created
	::close(_shutdown_pipe_fd[0]);
	::close(_shutdown_pipe_fd[1]);

	// Finalize the CoreMW framework
	HCF_CC_TRACE(ActivateHAMode, "Finalizing the CoreMW Framework!", _daemon_name);
	if ((call_result = finalize(_termination_event_received))) {
		HCF_CC_SYSLOG(LOG_ERR, LEVEL_ERROR, "Call 'finalize' failed with error == %d!", call_result);
		HCF_CC_TRACE(ActivateHAModeErr, "Call 'finalize' failed.", call_result);
		return -2;
	}
	return (_termination_event_received) ? 0 : -3;
}

int __TSL_CLASS_NAME__::component_report_error (amf_recommended_recovery_t recovery_action) {
	HCF_CC_TRACE_FUNCTION;

	SaAisErrorT error_code = SA_AIS_OK;
	SaTimeT detected_time = 0;

	HCF_CC_TRACE(ComponentError, "Calling 'saAmfComponentErrorReport' function to report a component error!", recovery_action);
	if ((error_code = saAmfComponentErrorReport(_amf_handle, &_component_name, detected_time, static_cast<SaAmfRecommendedRecoveryT>(recovery_action), SA_NTF_IDENTIFIER_UNUSED)) != SA_AIS_OK) {
		HCF_CC_SYSLOG(LOG_ERR, LEVEL_ERROR, "Call 'saAmfComponentErrorReport' failed with error == %d!", error_code);
		HCF_CC_TRACE(ComponentErrorErr, "Call 'saAmfComponentErrorReport' failed.", error_code);
		return -1;
	}
	return 0;
}

int __TSL_CLASS_NAME__::component_clear_error () {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(ComponentError, "Calling 'saAmfComponentErrorClear' function to clear a component error!", RECOMMENDED_RECOVERY_NO_RECOMMENDATION);

	SaAisErrorT error_code = SA_AIS_OK;
	if ((error_code = saAmfComponentErrorClear(_amf_handle, &_component_name, SA_NTF_IDENTIFIER_UNUSED)) != SA_AIS_OK) {
		HCF_CC_SYSLOG(LOG_ERR, LEVEL_ERROR, "Call 'saAmfComponentErrorClear' failed with error == %d!", error_code);
		return -1;
	}
	return 0;
}

int __TSL_CLASS_NAME__::initialize () {
	HCF_CC_TRACE_FUNCTION;

	// Register a signal handler for the shutdown (via SIGUSR2 signal)
	HCF_CC_TRACE(Initialize, "Registering a signal handler for the SIGUSR2 signal for the following process:", _daemon_name);
	if (::signal(SIGUSR2, application_shutdown_handler) == SIG_ERR) {
		const int errno_save = errno;
		HCF_CC_SYSLOG(LOG_ERR, LEVEL_ERROR, "Call 'signal' failed for SIGUSR2, errno == %d!", errno_save);
		HCF_CC_TRACE(InitializeErr, "Call 'signal' failed.", errno_save);
		return -1;
	}

	// Set AMF version
	SaVersionT version;
	version.releaseCode = 'B';
	version.majorVersion = 0x01;
	version.minorVersion = 0x01;

	// Initialize callbacks structure and fill it
	SaAmfCallbacksT callbacks;
	::memset(&callbacks, 0, sizeof(SaAmfCallbacksT));
	callbacks.saAmfCSISetCallback             = coremw_csi_set_callback;
	callbacks.saAmfCSIRemoveCallback          = coremw_csi_remove_callback;
	callbacks.saAmfHealthcheckCallback        = coremw_healthcheck_callback;
	callbacks.saAmfComponentTerminateCallback = coremw_csi_terminate_callback;

	_application_manager = this;

	// Initialize the application with CoreMW framework
	SaAisErrorT error_code = SA_AIS_OK;
	HCF_CC_TRACE(Initialize, "Initializing AMF Framework for the following process:", _daemon_name);
	if ((error_code = saAmfInitialize(&_amf_handle, &callbacks, &version)) != SA_AIS_OK) {
		HCF_CC_SYSLOG(LOG_ERR, LEVEL_ERROR, "Call 'saAmfInitialize' failed with error == %d!", error_code);
		HCF_CC_TRACE(InitializeErr, "Call 'saAmfInitialize' failed.", error_code);
		return -2;
	}

	/*
	 * Call saAmfSelectionObjectGet to get the operating system handle.
	 * This handle will be used by application to wait on the callbacks.
	 */
	HCF_CC_TRACE(Initialize, "Retrieving the AMF handle associated to the following process:", _daemon_name);
	if ((error_code = saAmfSelectionObjectGet(_amf_handle, &_selectable_obj)) != SA_AIS_OK) {
		HCF_CC_SYSLOG(LOG_ERR, LEVEL_ERROR, "Call 'saAmfSelectionObjectGet' failed with error == %d!", error_code);
		HCF_CC_TRACE(InitializeErr, "Call 'saAmfSelectionObjectGet' failed: finalizing AMF handle!", error_code);
		saAmfFinalize(_amf_handle);
		return -3;
	}

	// Call saAmfCompNameGet to get the name of the component
	HCF_CC_TRACE(Initialize, "Retrieving the component name for the following process:", _daemon_name);
	if ((error_code = saAmfComponentNameGet(_amf_handle, &_component_name)) != SA_AIS_OK) {
		HCF_CC_SYSLOG(LOG_ERR, LEVEL_ERROR, "Call 'saAmfComponentNameGet' failed with error == %d!", error_code);
		HCF_CC_TRACE(InitializeErr, "Call 'saAmfComponentNameGet' failed: finalizing AMF handle!", error_code);
		saAmfFinalize(_amf_handle);
		return -4;
	}

	// Call saAmfComponentRegister to register the component
	HCF_CC_TRACE(Initialize, "Registering the following component name:", reinterpret_cast<const char *> (_component_name.value));
	if ((error_code = saAmfComponentRegister(_amf_handle, &_component_name, 0)) != SA_AIS_OK) {
		HCF_CC_SYSLOG(LOG_ERR, LEVEL_ERROR, "Call 'saAmfComponentRegister' failed with error == %d!", error_code);
		HCF_CC_TRACE(InitializeErr, "Call 'saAmfComponentRegister' failed: finalizing AMF handle!", error_code);
		saAmfFinalize(_amf_handle);
		return -5;
	}

	// Form the application health check key
	HCF_CC_TRACE(Initialize, "Creating the health check key object for the following component:", reinterpret_cast<const char *> (_component_name.value));
	if (!(_health_check_key = new (std::nothrow) amf_health_check_key_t)) {
		HCF_CC_SYSLOG(LOG_ERR, LEVEL_ERROR, "Memory allocation failed for amf_health_check_key!");
		HCF_CC_TRACE(InitializeErr, "Failed to allocate memory for the health check key object: unregistering component and finalizing AMF handle!", 0);
		saAmfComponentUnregister(_amf_handle, &_component_name, 0);
		saAmfFinalize(_amf_handle);
		return -6;
	}
	size_t daemon_name_length = ::strlen(_daemon_name);
	::memcpy(_health_check_key->key, _daemon_name, daemon_name_length);
	::strcpy(reinterpret_cast<char *>(_health_check_key->key + daemon_name_length), "_hck");
	_health_check_key->keyLen = ::strlen(reinterpret_cast<char *>(_health_check_key->key));

	// Start the application health check
	SaAmfHealthcheckInvocationT health_check_invocation = SA_AMF_HEALTHCHECK_AMF_INVOKED;
	SaAmfRecommendedRecoveryT recommended_recovery = SA_AMF_COMPONENT_RESTART;

	HCF_CC_TRACE(Initialize, "Starting the health check procedure on the component with the following key:", reinterpret_cast<const char *> (_health_check_key->key));
	if ((error_code = saAmfHealthcheckStart(_amf_handle, &_component_name, reinterpret_cast<SaAmfHealthcheckKeyT *>(_health_check_key), health_check_invocation, recommended_recovery)) != SA_AIS_OK) {
		HCF_CC_SYSLOG(LOG_ERR, LEVEL_ERROR, "Call 'saAmfHealthcheckStart' failed with error == %d!", error_code);
		HCF_CC_TRACE(InitializeErr, "Call '' failed: Failed to allocate memory for the health check key object: unregistering component and finalizing AMF handle!", error_code);
		saAmfComponentUnregister(_amf_handle, &_component_name, 0);
		saAmfFinalize(_amf_handle);
		return -7;
	}
	return 0;
}

int __TSL_CLASS_NAME__::finalize (bool termination_event_received) {
	HCF_CC_TRACE_FUNCTION;

	SaAisErrorT error_code = SA_AIS_OK;
	int call_result = 0;

	// Some cleanup operations must be performed only in case of not gracefully termination
	if (!termination_event_received) {
		// Stop the application health check
		HCF_CC_TRACE(Finalize, "Received a request to terminate this process: stopping health check!", _daemon_name);
		if ((error_code = saAmfHealthcheckStop(_amf_handle, &_component_name, reinterpret_cast<SaAmfHealthcheckKeyT *>(_health_check_key))) != SA_AIS_OK) {
			HCF_CC_SYSLOG(LOG_ERR, LEVEL_ERROR, "Call 'saAmfHealthcheckStop' failed with error == %d!", error_code);
			HCF_CC_TRACE(FinalizeErr, "Call 'saAmfHealthcheckStop' failed.", error_code);
			call_result = -1;
		}

		// Unregister the component
		HCF_CC_TRACE(Finalize, "Received a request to terminate this process: unregistering component!", _daemon_name);
		if ((error_code = saAmfComponentUnregister(_amf_handle, &_component_name, 0)) != SA_AIS_OK) {
			HCF_CC_SYSLOG(LOG_ERR, LEVEL_ERROR, "Call 'saAmfComponentUnregister' failed with error == %d!", error_code);
			HCF_CC_TRACE(FinalizeErr, "Call 'saAmfComponentUnregister' failed.", error_code);
			call_result = -2;
		}
	}

	delete _health_check_key;

	// Finalize the AMF framework
	HCF_CC_TRACE(Finalize, "Received a request to terminate this process: finalizing AMF Framework!", _daemon_name);
	if ((error_code = saAmfFinalize(_amf_handle)) != SA_AIS_OK) {
		HCF_CC_SYSLOG(LOG_ERR, LEVEL_ERROR, "Call 'saAmfFinalize' failed with error == %d!", error_code);
		HCF_CC_TRACE(FinalizeErr, "Call 'saAmfFinalize' failed.", error_code);
		call_result = -3;
	}
	return call_result;
}

int __TSL_CLASS_NAME__::dispatch (amf_dispatch_flags_t dispatch_flags) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(Dispatch, "Dispatching an AMF callback!", dispatch_flags);

	SaAisErrorT error_code = SA_AIS_OK;
	if ((error_code = saAmfDispatch(_amf_handle, static_cast<SaDispatchFlagsT>(dispatch_flags))) != SA_AIS_OK) {
		HCF_CC_SYSLOG(LOG_ERR, LEVEL_ERROR, "Call 'saAmfDispatch' failed with error == %d!", error_code);
		HCF_CC_TRACE(DispatchErr, "Call 'saAmfDispatch' failed.", error_code);
		return -1;
	}
	return 0;
}

void __TSL_CLASS_NAME__::application_shutdown_handler (int signum) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_SYSLOG(LOG_DEBUG, LEVEL_NOTICE, "Shutting down the application, with signal %d (%s)", signum, ::strsignal(signum));
	HCF_CC_TRACE(SignalHandler, "The application received a signal: shutting down it by signaling the shutdown handle!", signum);

	// Write the received signal on the shutdown pipe
	if (::write(_shutdown_pipe_fd[1], &signum, sizeof(signum)) < 0) {
		const int errno_save = errno;
		HCF_CC_SYSLOG(LOG_ERR, LEVEL_ERROR, "Call 'write' failed, errno == %d!", errno_save);
		HCF_CC_TRACE(SignalHandlerErr, "Call 'write' failed.", errno_save);
	}
}

void __TSL_CLASS_NAME__::coremw_csi_set_callback (SaInvocationT invocation, const SaNameT * comp_name, SaAmfHAStateT ha_state, SaAmfCSIDescriptorT TSL_UNUSED_PAR(csi_descr)) {
	HCF_CC_TRACE_FUNCTION;

	// Save the old HA state and get the new one
	amf_ha_state_t previous_state = _application_manager->_ha_state;
	_application_manager->_ha_state = static_cast<amf_ha_state_t>(ha_state);
	HCF_CC_TRACE(CSISetCallback, "Received a state change callback: calling the right change-state method according to "
			"the new HA state!", invocation, reinterpret_cast<const char *> (comp_name->value), previous_state, ha_state);

	int call_result = 0;

	// Invoke the right callback method according to the new state
	switch (ha_state) {
		case SA_AMF_HA_ACTIVE:
			call_result = _application_manager->perform_state_transition_to_active(previous_state);
			break;

		case SA_AMF_HA_STANDBY:
			call_result = _application_manager->perform_state_transition_to_passive(previous_state);
			break;

		case SA_AMF_HA_QUIESCED:
			call_result = _application_manager->perform_state_transition_to_quiesced(previous_state);
			_application_manager->_ha_state = AMF_HA_UNDEFINED;
			break;

		case SA_AMF_HA_QUIESCING: {
			SaAisErrorT return_code = SA_AIS_OK;
			SaAisErrorT error_code = SA_AIS_OK;
			HCF_CC_TRACE(CSISetCallback, "The new HA state is QUIESCING, sending immediate response before quiescing the application!",
					invocation, reinterpret_cast<const char *> (comp_name->value), previous_state, ha_state);

			if ((return_code = saAmfResponse(_application_manager->_amf_handle, invocation, error_code)) != SA_AIS_OK) {
				HCF_CC_SYSLOG(LOG_ERR, LEVEL_ERROR, "Call 'saAmfResponse' failed with error == %d!", return_code);
				HCF_CC_TRACE(CSISetCallbackErr, "Call 'saAmfResponse' failed.", return_code);
			}

			if ((call_result = _application_manager->perform_state_transition_to_quiesced(previous_state))) {
				error_code = SA_AIS_ERR_FAILED_OPERATION;
				HCF_CC_TRACE(CSISetCallbackErr, "Call 'perform_state_transition_to_quiesced' failed.", call_result);
			}
			_application_manager->_ha_state = AMF_HA_UNDEFINED;
			HCF_CC_TRACE(CSISetCallback, "The application is now quiesced, signaling that to AMF!",
					invocation, reinterpret_cast<const char *> (comp_name->value), previous_state, ha_state);

			if ((return_code = saAmfCSIQuiescingComplete(_application_manager->_amf_handle, invocation, error_code)) != SA_AIS_OK) {
				HCF_CC_SYSLOG(LOG_ERR, LEVEL_ERROR, "Call 'saAmfCSIQuiescingComplete' failed with error == %d,"
						" restoring previous state!", return_code);
				HCF_CC_TRACE(CSISetCallbackErr, "Call 'saAmfCSIQuiescingComplete' failed.", return_code);
				_application_manager->_ha_state = previous_state;
			}

			return;
		}
	}

	// After executing the right callback, return the error code back to AMF
	SaAisErrorT error_code = (call_result) ? SA_AIS_ERR_FAILED_OPERATION : SA_AIS_OK;
	SaAisErrorT return_code = SA_AIS_OK;
	HCF_CC_TRACE(CSISetCallback, "After executing the right state-change function, signal the result to AMF!",
			invocation, reinterpret_cast<const char *> (comp_name->value), previous_state, ha_state);

	if ((return_code = saAmfResponse(_application_manager->_amf_handle, invocation, error_code)) != SA_AIS_OK) {
		HCF_CC_SYSLOG(LOG_ERR, LEVEL_ERROR, "Call 'saAmfResponse' failed with error == %d,"
				" restoring previous state!", return_code);
		HCF_CC_TRACE(CSISetCallbackErr, "Call 'saAmfResponse' failed.", return_code);
		_application_manager->_ha_state = previous_state;
	}
}

void __TSL_CLASS_NAME__::coremw_csi_remove_callback(SaInvocationT invocation, const SaNameT * comp_name, const SaNameT * csi_name, SaAmfCSIFlagsT flags) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(CSIRemoveCallback, "Received a request to remove the Component Service Instance: performing the remove operation!",
			invocation, reinterpret_cast<const char *> (comp_name->value), reinterpret_cast<const char *> (csi_name->value), flags);

	// When this callback is called, the HA state must be undefined and the remove callback invoked
	_application_manager->_ha_state = AMF_HA_UNDEFINED;
	const int call_result = _application_manager->perform_component_remove();

	// After executing the callback, return the error code back to AMF
	SaAisErrorT error_code = (call_result) ? SA_AIS_ERR_FAILED_OPERATION : SA_AIS_OK;
	SaAisErrorT return_code = SA_AIS_OK;
	HCF_CC_TRACE(CSIRemoveCallback, "Remove operation performed: signaling the result to AMF!", invocation,
			reinterpret_cast<const char *> (comp_name->value), reinterpret_cast<const char *> (csi_name->value), flags);

	if ((return_code = saAmfResponse(_application_manager->_amf_handle, invocation, error_code)) != SA_AIS_OK) {
		HCF_CC_SYSLOG(LOG_ERR, LEVEL_ERROR, "Call 'saAmfResponse' failed with error == %d!", return_code);
		HCF_CC_TRACE(CSIRemoveCallbackErr, "Call 'saAmfResponse' failed.", return_code);
	}
}

void __TSL_CLASS_NAME__::coremw_csi_terminate_callback(SaInvocationT invocation, const SaNameT * comp_name) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(CSITerminateCallback, "Received a request to terminate the Component Service Instance: "
			"performing the terminate operation!", invocation, reinterpret_cast<const char *> (comp_name->value));

	// When this callback is called, the HA state must be undefined and the terminate callback invoked
	_application_manager->_ha_state = AMF_HA_UNDEFINED;
	const int call_result = _application_manager->perform_component_terminate();

	// After executing the callback, return the error code back to AMF
	SaAisErrorT error_code = (call_result) ? SA_AIS_ERR_FAILED_OPERATION : SA_AIS_OK;
	SaAisErrorT return_code = SA_AIS_OK;
	HCF_CC_TRACE(CSITerminateCallback, "Terminate operation performed: signaling the result to AMF!",
			invocation, reinterpret_cast<const char *> (comp_name->value));

	if ((return_code = saAmfResponse(_application_manager->_amf_handle, invocation, error_code)) != SA_AIS_OK) {
		HCF_CC_SYSLOG(LOG_ERR, LEVEL_ERROR, "Call 'saAmfResponse' failed with error == %d!", return_code);
		HCF_CC_TRACE(CSITerminateCallbackErr, "Call 'saAmfResponse' failed.", return_code);
	}

	_application_manager->_termination_event_received = true;
}

void __TSL_CLASS_NAME__::coremw_healthcheck_callback(SaInvocationT invocation, const SaNameT * comp_name, SaAmfHealthcheckKeyT * health_check_key) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(CSIHealthCheckCallback, "Received a health check callback: checking keys match!", invocation,
			reinterpret_cast<const char *> (comp_name->value), reinterpret_cast<const char *> (health_check_key->key));

	// First, check if the provided health check key matches with the current one
	if (::strcmp((char *)(health_check_key->key), (char *)(_application_manager->_health_check_key->key))) {
		HCF_CC_SYSLOG(LOG_ERR, LEVEL_ERROR, "Health Check Key mismatch: '%s' instead of '%s'",
				health_check_key->key, _application_manager->_health_check_key->key);
		HCF_CC_TRACE(CSIHealthCheckCallbackErr, "The received health check key does not match with the application one!", 0);
		return;
	}

	HCF_CC_TRACE(CSIHealthCheckCallback, "Received a health check callback: calling health check callback!", invocation,
			reinterpret_cast<const char *> (comp_name->value), reinterpret_cast<const char *> (health_check_key->key));

	// After checking the health check key, invoke the right callback
	const int call_result = _application_manager->perform_component_health_check();

	// Only in case of successful health check, return the information back to AMF
	if (!call_result) {
		HCF_CC_TRACE(CSIHealthCheckCallback, "Health check callback correcly executed, signaling that to AMF!", invocation,
				reinterpret_cast<const char *> (comp_name->value), reinterpret_cast<const char *> (health_check_key->key));

		SaAisErrorT return_code = SA_AIS_OK;
		if ((return_code = saAmfResponse(_application_manager->_amf_handle, invocation, SA_AIS_OK)) != SA_AIS_OK) {
			HCF_CC_SYSLOG(LOG_ERR, LEVEL_ERROR, "Call 'saAmfResponse' failed with error == %d!", return_code);
			HCF_CC_TRACE(CSIHealthCheckCallbackErr, "Call 'saAmfResponse' failed.", return_code);
		}
	}
	else {
		HCF_CC_SYSLOG(LOG_ERR, LEVEL_ERROR, "Call 'perform_component_health_check' failed with error == %d!", call_result);
		HCF_CC_TRACE(CSIHealthCheckCallbackErr, "Call 'perform_component_health_check' failed.", call_result);
	}
}

TSL_NS_HCF_INTERNALAPI_END
