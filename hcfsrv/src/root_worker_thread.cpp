#include <linux/limits.h>
#include <string.h>
#include <fcntl.h>

#include "hcf/imm/imm_config_class.h"
#include "hcf/core/logger.h"
#include "hcf/core/utils.h"
#include "hcf/core/event_handler.h"
#include "hcf/core/event_reactor.h"

#include "imm_object_healthcheckm.h"
#include "imm_object_hcrule.h"
#include "imm_object_hcrule_inputparameter.h"
#include "imm_object_hcjob.h"
#include "imm_object_hcjob_scheduler.h"
#include "imm_object_hcrule_parameter.h"
#include "imm_object_hcsingle_event.h"
#include "imm_object_hcperiodic_event.h"
#include "imm_object_hccalendar_event.h"
#include "ruleset_file_parser.h"
#include "cmd_handler.h"
#include "configurator.h"
#include "worker_thread_set.h"
#include "job_executor.h"
#include "sftp_handler.h"
#include "hcf_application_manager.h"
#include "clock_alarm_notifier.h"
#include "configuration_handler.h"

#include "hcf/trace/com_ericsson_common_hcf_srv_rootworkerthread.h"
#include "hcf/trace/tracing_macros.h"

#include "root_worker_thread.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_SRV_ROOT_WORKER_THREAD__

/***************** BEGIN *****************/
/*       GLOBAL OBJECTS DECLARATION      */
/*****************************************/
TSL_NS_HCF_IMM::imm_config_class<HCF_NS_HCF_SRV::imm_object_healthcheckm>          healthcheckm_class_handler (HCF_NS_HCF_SRV::imm_object_healthcheckm::HCF_HEALTHCHECKM_CLASS_NAME, HCF_NS_HCF_SRV::imm_object_healthcheckm::HCF_HEALTHCHECKM_IMPLEMENTER_NAME);
TSL_NS_HCF_IMM::imm_config_class<HCF_NS_HCF_SRV::imm_object_hcrule_inputparameter> hcrule_input_parameter_class_handler (HCF_NS_HCF_SRV::imm_object_hcrule_inputparameter::HCF_HCRULE_INPUTPARAMETER_CLASS_NAME, HCF_NS_HCF_SRV::imm_object_hcrule_inputparameter::HCF_HCRULE_INPUTPARAMETER_IMPLEMENTER_NAME);
TSL_NS_HCF_IMM::imm_config_class<HCF_NS_HCF_SRV::imm_object_hcrule>                hcrule_class_handler (HCF_NS_HCF_SRV::imm_object_hcrule::HCF_HCRULE_CLASS_NAME, HCF_NS_HCF_SRV::imm_object_hcrule::HCF_HCRULE_IMPLEMENTER_NAME);
TSL_NS_HCF_IMM::imm_config_class<HCF_NS_HCF_SRV::imm_object_hcrule_parameter>      hcrule_parameter_class_handler (HCF_NS_HCF_SRV::imm_object_hcrule_parameter::HCF_HCRULEPARAMETER_CLASS_NAME, HCF_NS_HCF_SRV::imm_object_hcrule_parameter::HCF_HCRULEPARAMETER_IMPLEMENTER_NAME);
TSL_NS_HCF_IMM::imm_config_class<HCF_NS_HCF_SRV::imm_object_hcjob_scheduler>       hcjob_scheduler_class_handler (HCF_NS_HCF_SRV::imm_object_hcjob_scheduler::HCF_HCJOB_SCHEDULER_CLASS_NAME, HCF_NS_HCF_SRV::imm_object_hcjob_scheduler::HCF_HCJOB_SCHEDULER_IMPLEMENTER_NAME);
TSL_NS_HCF_IMM::imm_config_class<HCF_NS_HCF_SRV::imm_object_hcjob>                 hcjob_class_handler (HCF_NS_HCF_SRV::imm_object_hcjob::HCF_HCJOB_CLASS_NAME, HCF_NS_HCF_SRV::imm_object_hcjob::HCF_HCJOB_IMPLEMENTER_NAME);
TSL_NS_HCF_IMM::imm_config_class<HCF_NS_HCF_SRV::imm_object_hcsingle_event>        hcsingle_event_class_handler (HCF_NS_HCF_SRV::imm_object_hcsingle_event::HCF_HCSINGLE_EVENT_CLASS_NAME, HCF_NS_HCF_SRV::imm_object_hcsingle_event::HCF_HCSINGLE_EVENT_IMPLEMENTER_NAME);
TSL_NS_HCF_IMM::imm_config_class<HCF_NS_HCF_SRV::imm_object_hcperiodic_event>      hcperiodic_event_class_handler (HCF_NS_HCF_SRV::imm_object_hcperiodic_event::HCF_HCPERIODIC_EVENT_CLASS_NAME, HCF_NS_HCF_SRV::imm_object_hcperiodic_event::HCF_HCPERIODIC_EVENT_IMPLEMENTER_NAME);
TSL_NS_HCF_IMM::imm_config_class<HCF_NS_HCF_SRV::imm_object_hccalendar_event>      hccalendar_event_class_handler (HCF_NS_HCF_SRV::imm_object_hccalendar_event::HCF_HCCALENDAR_EVENT_CLASS_NAME, HCF_NS_HCF_SRV::imm_object_hccalendar_event::HCF_HCCALENDAR_EVENT_IMPLEMENTER_NAME);
HCF_NS_HCF_SRV::job_executor hcjob_executor;
HCF_NS_HCF_SRV::sftp_handler sftpHandler;
HCF_NS_HCF_SRV::clock_alarm_notifier alarm_notifier;
char HCF_LOCAL_FILE_STORE_PATH_VALUE[PATH_MAX + 1] = {0};
/*****************************************/
/*       GLOBAL OBJECTS DECLARATION      */
/*****************  END  *****************/

HCF_NS_HCF_SRV_BEGIN

HCF_CC_FUNCTION_TRACER_CLASS_DEFINE

// Static fields initialization
int __TSL_CLASS_NAME__::_hcf_server_stopping = 0;

int __TSL_CLASS_NAME__::main (void * arg) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(Main, "Root worker thread, starting to work with the following TID:", std::to_string(tid()).c_str());
	HCF_CC_LOG(LEVEL_NOTICE, "[%d, %lu] Root Worker Thread starting to work!", tid(), id());

	int return_code = ERR_NO_ERROR;
	std::list<TSL_NS_HCF_CORE::event_handler *> event_handlers;
	hcf_application_manager * app_manager = reinterpret_cast<hcf_application_manager *>(arg);

	// Cleanup the HCF temporary folder (if needed)
	HCF_CC_TRACE(Main, "Removing all the temporary data stored under the following HCF temporary folder:", HCF_TEMP_FOLDER_PATH);
	TSL_NS_HCF_CORE::utils::remove_folder_recursive(HCF_TEMP_FOLDER_PATH);

	/****************************************/
	/*         OUTPUT FOLDER SETUP          */
	/***************** BEGIN ****************/
	// Create the HCF output folder
	HCF_CC_TRACE(Main, "Creating the HCF output folder!", "");
	if (configurator::create_output_folder()) {
		HCF_CC_TRACE(MainErr, "Call 'configurator::create_output_folder' failed, asking AMF to restart the application!", ERR_OUTPUT_FOLDER_CREATION_FAILED);
		HCF_CC_LOG(LEVEL_ERROR, "Call 'configurator::create_output_folder' failed!");
		app_manager->restart_application();
		return ERR_OUTPUT_FOLDER_CREATION_FAILED;
	}
	/****************************************/
	/*         OUTPUT FOLDER SETUP          */
	/*****************  END  ****************/

	/***************** BEGIN *****************/
	/*       INITIALIZE EVENT HANDLERS       */
	/*****************************************/
	HCF_CC_TRACE(Main, "Opening the Clock Alarm Notifier object!", "");
	int call_result = 0;
	if ((call_result = alarm_notifier.open())) {
		HCF_CC_TRACE(MainErr, "Call 'alarm_notifier.open' failed.", alarm_notifier.error());
		HCF_CC_LOG(LEVEL_ERROR, "Call 'alarm_notifier.open' failed, return_code == %d, error == %d, error_message == '%s'",
				call_result, alarm_notifier.error(), alarm_notifier.error_text());
		return_code = (return_code != ERR_NO_ERROR) ? return_code : alarm_notifier.error();
		HCF_CC_TRACE(MainErr, "Call 'alarm_notifier.open' failed.", return_code);
	}
	event_handlers.push_back(&alarm_notifier);

	HCF_CC_TRACE(Main, "Initializing the Class Handler object for the following class:", "HealthCheckM");
	if ((call_result = healthcheckm_class_handler.initialize())) {
		HCF_CC_TRACE(MainErr, "Call 'healthcheckm_class_handler.initialize' failed.", call_result);
		HCF_CC_LOG(LEVEL_ERROR, "Call 'healthcheckm_class_handler.initialize' failed, return_code == %d", call_result);
		return_code = (return_code != ERR_NO_ERROR) ? return_code : ERR_CLASS_HANDLER_INITIALIZE_FAILED;
	}
	event_handlers.push_back(&healthcheckm_class_handler);

	HCF_CC_TRACE(Main, "Initializing the Class Handler object for the following class:", "HcRuleInputParameter");
	if ((call_result = hcrule_input_parameter_class_handler.initialize())) {
		HCF_CC_TRACE(MainErr, "Call 'hcrule_input_parameter_class_handler.initialize' failed.", call_result);
		HCF_CC_LOG(LEVEL_ERROR, "Call 'hcrule_input_parameter_class_handler.initialize' failed, return_code == %d", call_result);
		return_code = (return_code != ERR_NO_ERROR) ? return_code : ERR_CLASS_HANDLER_INITIALIZE_FAILED;
	}
	event_handlers.push_back(&hcrule_input_parameter_class_handler);

	HCF_CC_TRACE(Main, "Initializing the Class Handler object for the following class:", "HcRule");
	if ((call_result = hcrule_class_handler.initialize())) {
		HCF_CC_TRACE(MainErr, "Call 'hcrule_class_handler.initialize' failed.", call_result);
		HCF_CC_LOG(LEVEL_ERROR, "Call 'hcrule_class_handler.initialize' failed, return_code == %d", call_result);
		return_code = (return_code != ERR_NO_ERROR) ? return_code : ERR_CLASS_HANDLER_INITIALIZE_FAILED;
	}
	event_handlers.push_back(&hcrule_class_handler);

	HCF_CC_TRACE(Main, "Initializing the Class Handler object for the following class:", "HcRuleParameter");
	if ((call_result = hcrule_parameter_class_handler.initialize())) {
		HCF_CC_TRACE(MainErr, "Call 'hcrule_parameter_class_handler.initialize' failed.", call_result);
		HCF_CC_LOG(LEVEL_ERROR, "Call 'hcrule_parameter_class_handler.initialize' failed, return_code == %d", call_result);
		return_code = (return_code != ERR_NO_ERROR) ? return_code : ERR_CLASS_HANDLER_INITIALIZE_FAILED;
	}
	event_handlers.push_back(&hcrule_parameter_class_handler);

	HCF_CC_TRACE(Main, "Initializing the Class Handler object for the following class:", "HcJobScheduler");
	if ((call_result = hcjob_scheduler_class_handler.initialize())) {
		HCF_CC_TRACE(MainErr, "Call 'hcjob_scheduler_class_handler.initialize' failed.", call_result);
		HCF_CC_LOG(LEVEL_ERROR, "Call 'hcjob_scheduler_class_handler.initialize' failed, return_code == %d", call_result);
		return_code = (return_code != ERR_NO_ERROR) ? return_code : ERR_CLASS_HANDLER_INITIALIZE_FAILED;
	}
	event_handlers.push_back(&hcjob_scheduler_class_handler);

	HCF_CC_TRACE(Main, "Initializing the Class Handler object for the following class:", "HcJob");
	if ((call_result = hcjob_class_handler.initialize())) {
		HCF_CC_TRACE(MainErr, "Call 'hcjob_class_handler.initialize' failed.", call_result);
		HCF_CC_LOG(LEVEL_ERROR, "Call 'hcjob_class_handler.initialize' failed, return_code == %d", call_result);
		return_code = (return_code != ERR_NO_ERROR) ? return_code : ERR_CLASS_HANDLER_INITIALIZE_FAILED;
	}
	event_handlers.push_back(&hcjob_class_handler);

	HCF_CC_TRACE(Main, "Initializing the Class Handler object for the following class:", "HcSingleEvent");
	if ((call_result = hcsingle_event_class_handler.initialize())) {
		HCF_CC_TRACE(MainErr, "Call 'hcsingle_event_class_handler.initialize' failed.", call_result);
		HCF_CC_LOG(LEVEL_ERROR, "Call 'hcsingle_event_class_handler.initialize' failed, return_code == %d", call_result);
		return_code = (return_code != ERR_NO_ERROR) ? return_code : ERR_CLASS_HANDLER_INITIALIZE_FAILED;
	}
	event_handlers.push_back(&hcsingle_event_class_handler);

	HCF_CC_TRACE(Main, "Initializing the Class Handler object for the following class:", "HcPeriodicEvent");
	if ((call_result = hcperiodic_event_class_handler.initialize())) {
		HCF_CC_TRACE(MainErr, "Call 'hcperiodic_event_class_handler.initialize' failed.", call_result);
		HCF_CC_LOG(LEVEL_ERROR, "Call 'hcperiodic_event_class_handler.initialize' failed, return_code == %d", call_result);
		return_code = (return_code != ERR_NO_ERROR) ? return_code : ERR_CLASS_HANDLER_INITIALIZE_FAILED;
	}
	event_handlers.push_back(&hcperiodic_event_class_handler);

	HCF_CC_TRACE(Main, "Initializing the Class Handler object for the following class:", "HcCalendarBasedPeriodicEvent");
	if ((call_result = hccalendar_event_class_handler.initialize())) {
		HCF_CC_TRACE(MainErr, "Call 'hccalendar_event_class_handler.initialize' failed.", call_result);
		HCF_CC_LOG(LEVEL_ERROR, "Call 'hccalendar_event_class_handler.initialize' failed, return_code == %d", call_result);
		return_code = (return_code != ERR_NO_ERROR) ? return_code : ERR_CLASS_HANDLER_INITIALIZE_FAILED;
	}
	event_handlers.push_back(&hccalendar_event_class_handler);

	HCF_CC_TRACE(Main, "Initializing the Command Handler object!", "");
	HCF_NS_HCF_SRV::cmd_handler command_handler;
	if ((call_result = command_handler.initialize())) {
		HCF_CC_TRACE(MainErr, "Call 'command_handler.initialize' failed.", call_result);
		HCF_CC_LOG(LEVEL_ERROR, "Call 'command_handler.initialize' failed, return_code == %d", call_result);
		return_code = (return_code != ERR_NO_ERROR) ? return_code : ERR_CMD_HANDLER_INITIALIZE_FAILED;
	}
	event_handlers.push_back(&command_handler);

	HCF_CC_TRACE(Main, "Initializing the Job Executor object!", "");
	if ((call_result = hcjob_executor.initialize())) {
		HCF_CC_TRACE(MainErr, "Call 'hcjob_executor.initialize' failed.", call_result);
		HCF_CC_LOG(LEVEL_ERROR, "Call 'hcjob_executor.initialize' failed, return_code == %d", call_result);
		return_code = (return_code != ERR_NO_ERROR) ? return_code : ERR_JOB_EXECUTOR_INITIALIZE_FAILED;
	}
	event_handlers.push_back(&hcjob_executor);

	HCF_CC_TRACE(Main, "Initializing the SFTP Handler object!", "");
	if ((call_result = sftpHandler.initialize())) {
		HCF_CC_TRACE(MainErr, "Call 'sftpHandler.initialize' failed.", call_result);
		HCF_CC_LOG(LEVEL_ERROR, "Call 'sftpHandler.initialize' failed, return_code == %d", call_result);
		return_code = (return_code != ERR_NO_ERROR) ? return_code : ERR_SFTP_HANDLER_INITIALIZE_FAILED;
	}
	event_handlers.push_back(&sftpHandler);

	HCF_CC_TRACE(Main, "Initializing the Configuration Handler object!", "");
	configuration_handler conf_handler (HCF_NS_HCF_SRV::configuration_handler::HCF_CONFIGURATION_HANDLER_IMPLEMENTER_NAME);
	if ((call_result = conf_handler.initialize())) {
		HCF_CC_TRACE(MainErr, "Call 'conf_handler.initialize' failed.", call_result);
		HCF_CC_LOG(LEVEL_ERROR, "Call 'conf_handler.initialize' failed, return_code == %d", call_result);
		return_code = (return_code != ERR_NO_ERROR) ? return_code : ERR_CONFIG_HANDLER_INIT_FAILED;
	}
	event_handlers.push_back(&conf_handler);

	// In case of errors, finalize the event handlers and order AMF to restart HCF
	if (return_code != ERR_NO_ERROR) {
		HCF_CC_TRACE(MainErr, "One or more of the previously executed initializations failed, "
				"finalizing all the data structures and asking AMF to restart the application.", return_code);
		alarm_notifier.close();
		healthcheckm_class_handler.finalize();
		hcrule_input_parameter_class_handler.finalize();
		hcrule_class_handler.finalize();
		hcrule_parameter_class_handler.finalize();
		hcjob_scheduler_class_handler.finalize();
		hcjob_class_handler.finalize();
		hcsingle_event_class_handler.finalize();
		hcperiodic_event_class_handler.finalize();
		hccalendar_event_class_handler.finalize();
		command_handler.finalize();
		hcjob_executor.finalize();
		sftpHandler.finalize();
		conf_handler.finalize();
		app_manager->restart_application();
		return return_code;
	}
	/*****************************************/
	/*       INITIALIZE EVENT HANDLERS       */
	/*****************  END  *****************/

	/*****************************************/
	/*      EVENT HANDLERS REGISTRATION      */
	/****************** BEGIN ****************/
	// Create and open the event reactor object
	HCF_CC_TRACE(Main, "Opening the Event Reactor object!", "");
	TSL_NS_HCF_CORE::event_reactor event_reactor;
	if ((call_result = event_reactor.open())) {
		HCF_CC_TRACE(MainErr, "Call 'event_reactor.open' failed, asking AMF to restart the application!", event_reactor.error());
		HCF_CC_LOG(LEVEL_ERROR, "Call 'event_reactor.open' failed, call_result == %d, error == %d, error_message == '%s'",
				call_result, event_reactor.error(), event_reactor.error_text());
		app_manager->restart_application();
		return event_reactor.error();
	}

	// For each defined event handler, register it on the created reactor
	HCF_CC_TRACE(Main, "Registering the Event Handler objects into the Event Reactor object!", "");
	for (std::list<TSL_NS_HCF_CORE::event_handler *>::iterator e = event_handlers.begin(); e != event_handlers.end(); ++e) {
		if ((call_result = event_reactor.register_handler(*e, TSL_NS_HCF_CORE::TSL_EVENT_READ_MASK | TSL_NS_HCF_CORE::TSL_EVENT_EXCEPTION_MASK))) {
			HCF_CC_TRACE(MainErr, "Call 'event_reactor.register_handler' failed, asking AMF to restart the application!", event_reactor.error());
			HCF_CC_LOG(LEVEL_ERROR, "Call 'event_reactor.register_handler' failed, call_result == %d, error == %d, error_message == '%s'",
					call_result, event_reactor.error(), event_reactor.error_text());
			app_manager->restart_application();
			return event_reactor.error();
		}
	}
	/*****************************************/
	/*      EVENT HANDLERS REGISTRATION      */
	/******************  END  ****************/

	/****************************************/
	/* WORKER THREAD SET CREATION AND START */
	/***************** BEGIN ****************/
	const int HCF_SERVER_NUMBER_OF_THREADS = 8;
	HCF_CC_TRACE(Main, "Activating the Worker Thread Set with the following number of threads:", std::to_string(HCF_SERVER_NUMBER_OF_THREADS).c_str());
	worker_thread_set thread_set;
	if ((call_result = thread_set.activate(TSL_NS_HCF_CORE::TSL_THREAD_NULL_MASK, HCF_SERVER_NUMBER_OF_THREADS, &event_reactor)) != HCF_SERVER_NUMBER_OF_THREADS) {
		HCF_CC_TRACE(MainErr, "Call 'thread_set.activate' failed, asking AMF to restart the application!", thread_set.error());
		HCF_CC_LOG(LEVEL_ERROR, "Call 'thread_set.activate' for %d threads failed, call_result == %d, error == %d, error_message == '%s'",
				HCF_SERVER_NUMBER_OF_THREADS, call_result, thread_set.error(), thread_set.error_text());
		app_manager->restart_application();
		return thread_set.error();
	}
	/****************************************/
	/* WORKER THREAD SET CREATION AND START */
	/*****************  END  ****************/

	/***************************************/
	/*  RULE SET FILES STARTUP OPERATIONS  */
	/**************** BEGIN ****************/
	HCF_CC_TRACE(Main, "Executing the Command Handler startup operations!", "");
	if ((call_result = command_handler.handle_startup_operations())) {
		HCF_CC_TRACE(MainErr, "Call 'command_handler.handle_startup_operations' failed, asking AMF to restart the application!", call_result);
		HCF_CC_LOG(LEVEL_ERROR, "Call 'command_handler.handle_startup_operations' failed, call_result == %d", call_result);
		app_manager->restart_application();
		return call_result;
	}
	/***************************************/
	/*  RULE SET FILES STARTUP OPERATIONS  */
	/****************  END  ****************/

	/********************************************/
	/* ROOT WORKER THREAD: WAIT FOR TERMINATION */
	/****************** BEGIN *******************/
	// Block the main thread until the stop condition is signaled
	__sync_lock_test_and_set(&_hcf_server_stopping, 0);
	HCF_CC_TRACE(Main, "All the initialization operations were correctly performed, let's wait the termination signal!", "");
	HCF_CC_LOG(LEVEL_NOTICE, "Waiting until the stop condition is signaled...");

	TSL_CRITICAL_SECTION_BEGIN(sync_t, _stop_condition_mutex);
		while (!_stop_signaled) {
			HCF_CC_TRACE(Main, "Stop condition wasn't yet signaled, let's wait!", "");
			if (_stop_condition.wait(_stop_condition_mutex) < 0) {
				const int errno_save = errno;
				HCF_CC_TRACE(MainErr, "Call '_stop_condition.wait' failed", errno_save);
				HCF_CC_LOG(LEVEL_ERROR, "Call '_stop_condition.wait' returned -1, errno == %d", errno_save);
			}
		}
	TSL_CRITICAL_SECTION_END;

	// When the stop condition is signaled, signal the stop event to each Class Handler thread
	__sync_lock_test_and_set(&_hcf_server_stopping, 1);
	HCF_CC_TRACE(Main, "The stop condition was signaled, let's propagate the information to all threads!", "");
	HCF_CC_LOG(LEVEL_NOTICE, "Received a request to stop, signaling the stop to the thread set!");
	/********************************************/
	/* ROOT WORKER THREAD: WAIT FOR TERMINATION */
	/******************  END  *******************/

	/*******************************************/
	/*   ROOT WORKER THREAD: STOP SIGNALLING   */
	/****************** BEGIN ******************/
	HCF_CC_TRACE(Main, "Stopping the Event Reactor object, in order to don't accept events anymore!", "");
	if ((call_result = event_reactor.end_event_loop())) {
		HCF_CC_TRACE(MainErr, "Call 'event_reactor.end_event_loop' failed", event_reactor.error());
		HCF_CC_LOG(LEVEL_ERROR, "Call 'event_reactor.end_event_loop' failed, call_result == %d, error == %d, error_message == '%s'",
				call_result, event_reactor.error(), event_reactor.error_text());
		return event_reactor.error();
	}

	// Signal the stop event to the Job Executor, since a job execution could be ongoing
	HCF_CC_TRACE(Main, "Stopping the Job Executor object!", "");
	if ((call_result = hcjob_executor.stop()) < 0) {
		HCF_CC_TRACE(MainErr, "Call 'hcjob_executor.stop' failed", call_result);
		HCF_CC_LOG(LEVEL_ERROR, "Call 'hcjob_executor.stop' failed, call_result == %d", call_result);
		return call_result;
	}

	HCF_CC_TRACE(Main, "Waiting the termination of all the threads belonging to the Worker Thread set!", "");
	if ((call_result = thread_set.join()) < 0) {
		HCF_CC_TRACE(MainErr, "Call 'thread_set.join' failed", thread_set.error());
		HCF_CC_LOG(LEVEL_ERROR, "Call 'thread_set.join' failed, call_result == %d, error == %d, error_message == '%s'",
				call_result, thread_set.error(), thread_set.error_text());
		return thread_set.error();
	}
	/*******************************************/
	/*   ROOT WORKER THREAD: STOP SIGNALLING   */
	/******************  END  ******************/

	/*****************************************/
	/*     EVENT HANDLERS DEREGISTRATION     */
	/****************** BEGIN ****************/
	HCF_CC_TRACE(Main, "Removing the Event Handler objects from the Event Reactor object!", "");
	for (std::list<TSL_NS_HCF_CORE::event_handler *>::iterator e = event_handlers.begin(); e != event_handlers.end(); ++e) {
		if ((call_result = event_reactor.remove_handler(*e))) {
			HCF_CC_TRACE(MainErr, "Call 'event_reactor.remove_handler' failed", event_reactor.error());
			HCF_CC_LOG(LEVEL_ERROR, "Call 'event_reactor.remove_handler' failed, call_result == %d, error_code == %d, error_message == '%s",
					call_result, event_reactor.error(), event_reactor.error_text());
			return_code = event_reactor.error();
		}
	}
	/*****************************************/
	/*     EVENT HANDLERS DEREGISTRATION     */
	/******************  END  ****************/

	/*****************************************/
	/*        FINALIZE EVENT HANDLERS        */
	/***************** BEGIN *****************/
	HCF_CC_TRACE(Main, "Finalizing the Configuration Handler object!", "");
	if ((call_result = conf_handler.finalize())) {
		HCF_CC_TRACE(MainErr, "Call 'conf_handler.finalize' failed", call_result);
		HCF_CC_LOG(LEVEL_ERROR, "Call 'conf_handler.finalize' failed, return_code == %d", call_result);
		return_code = (return_code != ERR_NO_ERROR) ? return_code : ERR_CONFIG_HANDLER_FINALIZE_FAILED;
	}

	HCF_CC_TRACE(Main, "Finalizing the Job Executor object!", "");
	if ((call_result = hcjob_executor.finalize())) {
		HCF_CC_TRACE(MainErr, "Call 'hcjob_executor.finalize' failed", call_result);
		HCF_CC_LOG(LEVEL_ERROR, "Call 'hcjob_executor.finalize' failed, return_code == %d", call_result);
		return_code = (return_code != ERR_NO_ERROR) ? return_code : ERR_JOB_EXECUTOR_FINALIZE_FAILED;
	}

	HCF_CC_TRACE(Main, "Finalizing the SFTP Handler object!", "");
	if ((call_result = sftpHandler.finalize())) {
		HCF_CC_TRACE(MainErr, "Call 'sftpHandler.finalize' failed", call_result);
		HCF_CC_LOG(LEVEL_ERROR, "Call 'sftpHandler.finalize' failed, return_code == %d", call_result);
		return_code = (return_code != ERR_NO_ERROR) ? return_code : ERR_SFTP_HANDLER_FINALIZE_FAILED;
	}

	HCF_CC_TRACE(Main, "Finalizing the Command Handler object!", "");
	if ((call_result = command_handler.finalize())) {
		HCF_CC_TRACE(MainErr, "Call 'command_handler.finalize' failed", call_result);
		HCF_CC_LOG(LEVEL_ERROR, "Call 'command_handler.finalize' failed, return_code == %d", call_result);
		return_code = (return_code != ERR_NO_ERROR) ? return_code : ERR_CMD_HANDLER_FINALIZE_FAILED;
	}

	HCF_CC_TRACE(Main, "Finalizing the Class Handler object for the following class:", "HcCalendarBasedPeriodicEvent");
	if ((call_result = hccalendar_event_class_handler.finalize())) {
		HCF_CC_TRACE(MainErr, "Call 'hccalendar_event_class_handler.finalize' failed", call_result);
		HCF_CC_LOG(LEVEL_ERROR, "Call 'hccalendar_event_class_handler.finalize' failed, return_code == %d", call_result);
		return_code = (return_code != ERR_NO_ERROR) ? return_code : ERR_CLASS_HANDLER_FINALIZE_FAILED;
	}

	HCF_CC_TRACE(Main, "Finalizing the Class Handler object for the following class:", "HcPeriodicEvent");
	if ((call_result = hcperiodic_event_class_handler.finalize())) {
		HCF_CC_TRACE(MainErr, "Call 'hcperiodic_event_class_handler.finalize' failed", call_result);
		HCF_CC_LOG(LEVEL_ERROR, "Call 'hcperiodic_event_class_handler.finalize' failed, return_code == %d", call_result);
		return_code = (return_code != ERR_NO_ERROR) ? return_code : ERR_CLASS_HANDLER_FINALIZE_FAILED;
	}

	HCF_CC_TRACE(Main, "Finalizing the Class Handler object for the following class:", "HcSingleEvent");
	if ((call_result = hcsingle_event_class_handler.finalize())) {
		HCF_CC_TRACE(MainErr, "Call 'hcsingle_event_class_handler.finalize' failed", call_result);
		HCF_CC_LOG(LEVEL_ERROR, "Call 'hcsingle_event_class_handler.finalize' failed, return_code == %d", call_result);
		return_code = (return_code != ERR_NO_ERROR) ? return_code : ERR_CLASS_HANDLER_FINALIZE_FAILED;
	}

	HCF_CC_TRACE(Main, "Finalizing the Class Handler object for the following class:", "HcJob");
	if ((call_result = hcjob_class_handler.finalize())) {
		HCF_CC_TRACE(MainErr, "Call 'hcjob_class_handler.finalize' failed", call_result);
		HCF_CC_LOG(LEVEL_ERROR, "Call 'hcjob_class_handler.finalize' failed, return_code == %d", call_result);
		return_code = (return_code != ERR_NO_ERROR) ? return_code : ERR_CLASS_HANDLER_FINALIZE_FAILED;
	}

	HCF_CC_TRACE(Main, "Finalizing the Class Handler object for the following class:", "HcJobScheduler");
	if ((call_result = hcjob_scheduler_class_handler.finalize())) {
		HCF_CC_TRACE(MainErr, "Call 'hcjob_scheduler_class_handler.finalize' failed", call_result);
		HCF_CC_LOG(LEVEL_ERROR, "Call 'hcjob_scheduler_class_handler.finalize' failed, return_code == %d", call_result);
		return_code = (return_code != ERR_NO_ERROR) ? return_code : ERR_CLASS_HANDLER_FINALIZE_FAILED;
	}

	HCF_CC_TRACE(Main, "Finalizing the Class Handler object for the following class:", "HcRuleParameter");
	if ((call_result = hcrule_parameter_class_handler.finalize())) {
		HCF_CC_TRACE(MainErr, "Call 'hcrule_parameter_class_handler.finalize' failed", call_result);
		HCF_CC_LOG(LEVEL_ERROR, "Call 'hcrule_parameter_class_handler.finalize' failed, return_code == %d", call_result);
		return_code = (return_code != ERR_NO_ERROR) ? return_code : ERR_CLASS_HANDLER_FINALIZE_FAILED;
	}

	HCF_CC_TRACE(Main, "Finalizing the Class Handler object for the following class:", "HcRule");
	if ((call_result = hcrule_class_handler.finalize())) {
		HCF_CC_TRACE(MainErr, "Call 'hcrule_class_handler.finalize' failed", call_result);
		HCF_CC_LOG(LEVEL_ERROR, "Call 'hcrule_class_handler.finalize' failed, return_code == %d", call_result);
		return_code = (return_code != ERR_NO_ERROR) ? return_code : ERR_CLASS_HANDLER_FINALIZE_FAILED;
	}

	HCF_CC_TRACE(Main, "Finalizing the Class Handler object for the following class:", "HcRuleInputParameter");
	if ((call_result = hcrule_input_parameter_class_handler.finalize())) {
		HCF_CC_TRACE(MainErr, "Call 'hcrule_input_parameter_class_handler.finalize' failed", call_result);
		HCF_CC_LOG(LEVEL_ERROR, "Call 'hcrule_input_parameter_class_handler.finalize' failed, return_code == %d", call_result);
		return_code = (return_code != ERR_NO_ERROR) ? return_code : ERR_CLASS_HANDLER_FINALIZE_FAILED;
	}

	HCF_CC_TRACE(Main, "Finalizing the Class Handler object for the following class:", "HealthCheckM");
	if ((call_result = healthcheckm_class_handler.finalize())) {
		HCF_CC_TRACE(MainErr, "Call 'healthcheckm_class_handler.finalize' failed", call_result);
		HCF_CC_LOG(LEVEL_ERROR, "Call 'healthcheckm_class_handler.finalize' failed, return_code == %d", call_result);
		return_code = (return_code != ERR_NO_ERROR) ? return_code : ERR_CLASS_HANDLER_FINALIZE_FAILED;
	}

	HCF_CC_TRACE(Main, "Closing the Clock Alarm Notifier object!", "");
	if ((call_result = alarm_notifier.close())) {
		HCF_CC_TRACE(MainErr, "Call 'alarm_notifier.close' failed", call_result);
		HCF_CC_LOG(LEVEL_ERROR, "Call 'alarm_notifier.close' failed, return_code == %d, error == %d, error_message == '%s'",
				call_result, alarm_notifier.error(), alarm_notifier.error_text());
		return_code = (return_code != ERR_NO_ERROR) ? return_code : ERR_JOB_EXECUTOR_FINALIZE_FAILED;
	}
	/*****************************************/
	/*        FINALIZE EVENT HANDLERS        */
	/*****************  END  *****************/
	return return_code;
}

HCF_NS_HCF_SRV_END
