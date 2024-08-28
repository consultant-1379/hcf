#include <sys/eventfd.h>
#include <sys/wait.h>
#ifndef _GNU_SOURCE
# define _GNU_SOURCE
#endif
#include <unistd.h>
#include <fcntl.h>
#include <syslog.h>
#include <poll.h>

#include <libxml/uri.h>

#include "hcf/core/logger.h"
#include "hcf/core/guard.h"
#include "hcf/core/utils.h"
#include "hcf/imm/imm_config_class.h"
#include "imm_object_healthcheckm.h"
#include "output_file_writer.h"
#include "configurator.h"
#include "configuration_handler.h"
#include "root_worker_thread.h"
#include "rule_executor.h"
#include "sftp_handler.h"
#include "job_executor.h"

#include "hcf/trace/com_ericsson_common_hcf_srv_jobexecutor.h"
#include "hcf/trace/tracing_macros.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_SRV_JOB_EXECUTOR__

// Extern objects
extern char ** environ;
extern TSL_NS_HCF_IMM::imm_config_class<HCF_NS_HCF_SRV::imm_object_healthcheckm> healthcheckm_class_handler;
extern TSL_NS_HCF_IMM::imm_config_class<HCF_NS_HCF_SRV::imm_object_hcrule> hcrule_class_handler;
extern TSL_NS_HCF_IMM::imm_config_class<HCF_NS_HCF_SRV::imm_object_hcjob> hcjob_class_handler;
extern HCF_NS_HCF_SRV::sftp_handler sftpHandler;

HCF_NS_HCF_SRV_BEGIN

HCF_CC_FUNCTION_TRACER_CLASS_DEFINE

bool __TSL_CLASS_NAME__::output_file_lesser::operator ()(const std::string & first, const std::string & second) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(OutputFileLesser, "Received a request to check if the first file is older than the second one!", first.c_str(), second.c_str());
	/*
	 * The output file name could follow one of the following formats:
	 *  - MEID_JOBID_timestamp_man
	 *  - MEID_JOBID_timestamp
	 * First, extract only the timestamp part from each input parameter
	 */
	std::string first_timestamp = extract_timestamp_from_filename(first);
	std::string second_timestamp = extract_timestamp_from_filename(second);
	return first_timestamp < second_timestamp;
}

int __TSL_CLASS_NAME__::handle_input () {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_LOG(LEVEL_NOTICE, "Received a request to execute a health check job!");

	// FIRST: consume the value stored into the event file descriptor
	uint64_t value;
	HCF_CC_TRACE(HandleInput, "Received a request to handle a job execution operation!", "");
	if (::read(_event_fd, &value, sizeof(value)) < 0) {
		const int errno_save = errno;
		HCF_CC_LOG(LEVEL_ERROR, "Call 'read' failed, removing the Event Handler from the reactor, errno == %d", errno_save);
		HCF_CC_TRACE(HandleInputErr, "Call 'read' failed.", errno_save);
		return (errno_save != EINTR) ? -1 : 0;
	}

	// SECOND: take the information about the job to be executed from the internal queue
	job_to_execute job;
	TSL_CRITICAL_SECTION_BEGIN(sync_t, _jobs_mutex);
		job = _jobs.front();
	TSL_CRITICAL_SECTION_END;

	// THIRD: execute the job having the retrieved info
	HCF_CC_TRACE(HandleInput, "Executing the following job:", job.id.c_str());
	execute(job);

	// FOURTH: remove from the internal queue the executed job
	TSL_CRITICAL_SECTION_BEGIN(sync_t, _jobs_mutex);
		_jobs.pop();
	TSL_CRITICAL_SECTION_END;
	return 0;
}

int __TSL_CLASS_NAME__::handle_output () {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(HandleOutput, "Received an output I/O event for the Job Executor, it "
			"shouldn't occur, removing the Event Handler from the reactor");
	HCF_CC_LOG(LEVEL_WARNING, "Received an output I/O event for the Job Executor, it "
			"shouldn't occur, removing the Event Handler from the reactor");
	return -1;
}

int __TSL_CLASS_NAME__::handle_exception (bool error_flag, bool hungup_flag, bool read_hungup_flag) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(HandleException, "Received an error on the Job Executor I/O handle, removing the Event Handler "
			"from the reactor: error_flag == %s, hungup_flag == %s, read_hungup_flag == %s",
			(error_flag ? "TRUE" : "FALSE"), (hungup_flag ? "TRUE" : "FALSE"), (read_hungup_flag ? "TRUE" : "FALSE"));
	HCF_CC_LOG(LEVEL_WARNING, "Received an error on the Job Executor I/O handle, removing the Event Handler "
			"from the reactor: error_flag == %s, hungup_flag == %s, read_hungup_flag == %s",
			(error_flag ? "TRUE" : "FALSE"), (hungup_flag ? "TRUE" : "FALSE"), (read_hungup_flag ? "TRUE" : "FALSE"));
	return -1;
}

int __TSL_CLASS_NAME__::handle_close () {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(HandleClose, "Removing the Job Executor from the reactor!");
	HCF_CC_LOG(LEVEL_NOTICE, "Removing the Job Executor from the reactor!");
	return 0;
}

int __TSL_CLASS_NAME__::initialize () {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_LOG(LEVEL_NOTICE, "Initializing the Job Executor object!");

	// Create the file descriptor to be used to signal the events to the Job Executor
	HCF_CC_TRACE(Initialize, "Received a request to initialize the Job Executor object, creating the internal file descriptor!");
	_event_fd = ::eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK | EFD_SEMAPHORE);
	if (_event_fd < 0) {
		const int errno_save = errno;
		HCF_CC_LOG(LEVEL_ERROR, "Call 'eventfd' failed, errno == %d", errno_save);
		HCF_CC_TRACE(InitializeErr, "Call 'eventfd' failed.", errno_save);
		return ERR_JOB_EXECUTOR_INITIALIZE_FAILED;
	}

	return ERR_NO_ERROR;
}

int __TSL_CLASS_NAME__::finalize () {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_LOG(LEVEL_NOTICE, "Finalizing the Job Executor object!");
	HCF_CC_TRACE(Finalize, "Received a request to finalize the SFTP Handler object: closing internal file descriptor!");
	::close(_event_fd);
	return ERR_NO_ERROR;
}

int __TSL_CLASS_NAME__::stop () {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(Stop, "Received a request to stop the Job Executor object!");

	// Before removing the Job Executor object from the reactor, signal a forced stop to the child process (if needed)
	if (_child_process_pid >= 0) {
		// Valid child process pid found, kill it in order to unlock the father process (that is waiting the child termination)
		HCF_CC_TRACE(Stop, "There is a job execution operation ongoing, signaling to the child process the stop!");
		if (::kill(_child_process_pid, SIGKILL) < 0) {
			const int errno_save = errno;
			HCF_CC_LOG(LEVEL_ERROR, "Call 'kill' failed, errno == %d", errno_save);
			HCF_CC_TRACE(StopErr, "Call 'kill' failed.", errno_save);
			return ERR_STOP_EVENT_SIGNALLING_FAILED;
		}
	}
	return ERR_NO_ERROR;
}

int __TSL_CLASS_NAME__::execute_job (const std::string & job_id, const std::set<category_t> & rules_categories, const std::string & export_uri, const std::string & export_password, const std::string & job_to_trigger, bool is_scheduled_execution) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(ExecuteJob, "Received a request to add a new health check job to the queue of jobs!", job_id.c_str(),
			rules_categories.size(), export_uri.c_str(), job_to_trigger.c_str(), is_scheduled_execution);

	// FIRST: store the info about the job into the internal queue
	job_to_execute job;
	job.id = job_id;
	job.categories = rules_categories;
	job.uri = export_uri;
	job.pwd = export_password;
	job.job_to_trigger = job_to_trigger;
	job.is_scheduled_execution = is_scheduled_execution;

	TSL_CRITICAL_SECTION_BEGIN(sync_t, _jobs_mutex);
		_jobs.push(job);
	TSL_CRITICAL_SECTION_END;

	// SECOND: signal to the job executor that there is a new job to be executed
	const uint64_t value = 1;
	HCF_CC_TRACE(ExecuteJob, "Signaling to the Job Executor object that a new job is available!", job_id.c_str(),
			rules_categories.size(), export_uri.c_str(), job_to_trigger.c_str(), is_scheduled_execution);
	if (::write(_event_fd, &value, sizeof(value)) < 0) {
		const int errno_save = errno;
		HCF_CC_LOG(LEVEL_ERROR, "Call 'write' failed, errno == %d", errno_save);
		HCF_CC_TRACE(ExecuteJobErr, "Call 'write' failed.", errno_save);
		return ERR_JOB_EXECUTOR_ADD_JOB_FAILED;
	}

	return ERR_NO_ERROR;
}

void __TSL_CLASS_NAME__::execute (job_to_execute & job) {
	HCF_CC_TRACE_FUNCTION;
	int call_result = 0;
	output_file_writer writer;

	// Retrieve the pointer to the HcJob instance having the given id
	HCF_CC_TRACE(Execute, "Retrieving the object instance for the following job:", job.id.c_str());
	imm_object_hcjob * job_ptr = hcjob_class_handler.get_instance(job.id);
	if (!job_ptr) {
		HCF_CC_LOG(LEVEL_ERROR, "Retrieved a NULL instance for the HcJob having ID == '%s'!", job.id.c_str());
		HCF_CC_TRACE(ExecuteErr, "Call 'hcjob_class_handler.get_instance' failed.", ERR_NULL_INSTANCE_RETRIEVED);
		return;
	}

	// First of all, signal to all the other jobs that one job is running
	HCF_CC_TRACE(Execute, "Signaling that the following job is being executing:", job.id.c_str());
	job_ptr->job_execution_ongoing(job_ptr->get_dn());

	// Before starting to execute the job, initialize the progressReport structure
	HCF_CC_TRACE(Execute, "Initialize the data for the progressReport structure for the following job:", job.id.c_str());
	job_ptr->init_progress_report();

	/*
	 * Calculate the output folder where the output file must be stored.
	 * From HCF 1.1 the possible cases are:
	 * 		1. Default output folder (no exportUri provided), provided into localFileStorePath attribute of HcJob MOC.
	 * 		2. Local absolute URI: the output file is stored under the provided folder.
	 * 		3. Local relative URI: the provided URI is added as suffix to the default output folder.
	 * 		4. SFTP URI: the output file is first stored locally in the default output folder (as in CASE 1),
	 * 					 then it is also transferred via SFTP protocol according to the data provided in the URI.
	 * 		5. URI scheme not supported: the provided URI scheme is not supported.
	 */

	// Parse the URI string
	xmlURIPtr uri;
	if((uri = xmlParseURI(job.uri.c_str())))
	{
		HCF_CC_LOG(LEVEL_INFO, "The provided export URI format '%s' is valid!", job.uri.c_str());
		HCF_CC_TRACE(Execute, "The following exportUri value has been correctly parsed:", job.uri.c_str());
	}
	else
	{
		HCF_CC_LOG(LEVEL_ERROR, "The provided export URI format '%s' is not valid!", job.uri.c_str());
		job_ptr->job_execution_ongoing();
		job_ptr->finalize_progress_report(false, "The export URI format is not valid.");
		HCF_CC_TRACE(ExecuteErr, "Invalid exporUri value provided.", -1);
		// No memory allocated by xmlParseURI
		return;
	}

	// Check the URI scheme
	char output_folder[PATH_MAX + 1] = {0};
	bool isSftpUri = false;
	file_to_transfer report_file;
	file_to_transfer archive_file;
	HCF_CC_TRACE(Execute, "Checking the type of the provided URI, according to the parsed scheme!", "");
	if(uri->scheme == NULL)
	{
		if (uri->path == NULL) { // CASE 1: set the default output folder
			::strncpy(output_folder, job_ptr->get_local_file_store_path().c_str(), TSL_ARRAY_SIZE(output_folder));
			HCF_CC_TRACE(Execute, "URI scheme is NULL and also URI path is NULL: using the following default output folder:", output_folder);
		}
		else if (uri->path[0] == '/') { // CASE 2: local absolute path
			::strncpy(output_folder, uri->path, TSL_ARRAY_SIZE(output_folder));
			HCF_CC_TRACE(Execute, "URI scheme is NULL and URI path points to an absolute path: using the following output folder:", output_folder);
		}
		else { // CASE 3: local relative path: concat the default value with the provided URI
			::snprintf(output_folder, TSL_ARRAY_SIZE(output_folder), "%s/%s", job_ptr->get_local_file_store_path().c_str(), uri->path);
			HCF_CC_TRACE(Execute, "URI scheme is NULL and URI path points to a relative path: using the following output folder:", output_folder);
		}
	}
	else if (string(uri->scheme).compare("sftp") == 0)	// CASE 4: SFTP URI
	{
		isSftpUri = true;
		// Set the default output folder
		::strncpy(output_folder, job_ptr->get_local_file_store_path().c_str(), TSL_ARRAY_SIZE(output_folder));
		HCF_CC_TRACE(Execute, "A SFTP URI has been provided, using as local output folder the following value:", output_folder);

		// Save parameters for SFTP transfer
		report_file.server = (!uri->server) ? "" : std::string(uri->server);
		report_file.port = uri->port;
		report_file.user = (!uri->user) ? "" : std::string(uri->user);
		report_file.password = job.pwd;
		report_file.remoteFile = (!uri->path) ? "" : std::string(uri->path); // As soon as possible file name will be concatenated.

		// Save common values in archive file
		archive_file = report_file;
	}
	else // CASE 5: URI scheme not supported
	{
		HCF_CC_LOG(LEVEL_ERROR, "The provided export URI scheme '%s' is not supported!", uri->scheme);
		job_ptr->job_execution_ongoing();
		job_ptr->finalize_progress_report(false, "The export URI scheme is not supported.");
		HCF_CC_TRACE(ExecuteErr, "The provided exporUri value is valid, but its scheme is not supported!", -1);
		xmlFreeURI(uri);
		return;
	}

	// Not any longer needed, free allocated memory
	xmlFreeURI(uri);

	// After calculating the output folder, check if it exists and if it is a directory
	HCF_CC_LOG(LEVEL_NOTICE, "Using as output folder '%s'", output_folder);
	HCF_CC_TRACE(Execute, "Checking if the following output folder exists:", output_folder);

	struct stat st;
	if (::stat(output_folder, &st)) {	// ERROR: Failed to check the output folder type
		const int errno_save = errno;
		job_ptr->job_execution_ongoing();
		HCF_CC_TRACE(ExecuteErr, "Call 'stat' failed.", errno_save);

		if (errno_save == ENOENT) {
			HCF_CC_LOG(LEVEL_WARNING, "The provided output folder '%s' doesn't exist!", output_folder);
			job_ptr->finalize_progress_report(false, "The provided output folder doesn't exist.");
		}
		else {
			HCF_CC_LOG(LEVEL_ERROR, "Call 'stat' failed for output folder '%s', errno == %d", output_folder, errno_save);
			job_ptr->finalize_progress_report(false, "Failed to retrieve info about the output folder.");
		}
		return;
	}

	// In case the output folder isn't a directory (or does not exist), the job is failed
	HCF_CC_TRACE(Execute, "Checking if the following output folder is a directory:", output_folder);
	if (!S_ISDIR(st.st_mode)) {
		HCF_CC_LOG(LEVEL_ERROR, "The provided output folder '%s' is not a directory (st.st_mode == 0x%X)!", output_folder, st.st_mode);
		job_ptr->job_execution_ongoing();
		job_ptr->finalize_progress_report(false, "The output folder is not valid.");
		HCF_CC_TRACE(ExecuteErr, "The provided output folder is not a directory, mode is:", st.st_mode);
		return;
	}

	// Create the temporary folder to store the commands printouts
	char tmp_output_folder[PATH_MAX + 1];
	::snprintf(tmp_output_folder, TSL_ARRAY_SIZE(tmp_output_folder), "%s/%s", HCF_TEMP_FOLDER_PATH, job_ptr->get_id().c_str());
	HCF_CC_TRACE(Execute, "The command printouts will be temporary stored in the following folder:", tmp_output_folder);

	// Before creating it, remove its eventual pre-existing content
	TSL_NS_HCF_CORE::utils::remove_folder_recursive(tmp_output_folder);

	HCF_CC_TRACE(Execute, "Creating the temporary output folder:", tmp_output_folder);
	if (TSL_NS_HCF_CORE::utils::create_dir_recursive(tmp_output_folder) < 0) {
		const int errno_save = errno;
		HCF_CC_LOG(LEVEL_ERROR, "Failed to create the temporary folder to store commands printouts (%s), errno == %d!",
				tmp_output_folder, errno_save);
		job_ptr->job_execution_ongoing();
		job_ptr->finalize_progress_report(false, "Failed to create the temporary output folder.");
		HCF_CC_TRACE(ExecuteErr, "Call 'utils::create_dir_recursive' failed.", errno_save);
		return;
	}

	// Get the name of the current host
	HCF_CC_TRACE(Execute, "Retrieving the name of the current host!", "");
	size_t current_hostname_len = PATH_MAX + 1;
	char current_hostname[current_hostname_len];
	if (configurator::get_current_hostname(current_hostname, current_hostname_len) < 0) {
		HCF_CC_LOG(LEVEL_WARNING, "Call 'configurator::get_current_hostname' failed!");
		HCF_CC_TRACE(ExecuteErr, "Call 'configurator::get_current_hostname' failed.", -1);
	}
	HCF_CC_TRACE(Execute, "The job will be executed on the following host:", current_hostname);

	// Then, provide to the output file handler the ID of the job to be executed and the list of categories
	writer.set_job_id(job_ptr->get_id());
	for (std::set<category_t>::const_iterator cat = job.categories.begin(); cat != job.categories.end(); ++cat) {
		HCF_CC_TRACE(Execute, "Adding the following category to the job to be executed:", utils::map_category_value_to_category_string(*cat));
		writer.add_category(*cat);
	}

	// Extract the list of rules to be executed based on the provided categories
	HCF_CC_TRACE(Execute, "Retrieving the list of rules associated to the provided number of categories:", std::to_string(job.categories.size()).c_str());
	std::set<std::string> rules_to_execute = imm_object_hcrule::get_rules_by_categories(job.categories);

	if (rules_to_execute.empty()) {
		HCF_CC_LOG(LEVEL_WARNING, "There are no rules for the provided categories!");
		HCF_CC_TRACE(Execute, "WARNING: There are no rules for the provided categories!", "");
	}

	// Calculate the progress percentage at each step
	const size_t num_of_rules = rules_to_execute.size();
	uint32_t progress_step = 100 / ((num_of_rules > 0) ? ((num_of_rules > 100) ? 100 : num_of_rules) : 1);
	uint32_t progress_percentage = 0;

	std::list<imm_object_hcrule *> failed_rules;
	health_status_t status = HEALTH_STATUS_HEALTHY;
	imm_object_hcrule * rule_ptr = 0;
	std::set<std::string> remote_nodes_not_reachable;

	for (std::set<std::string>::const_iterator rule_id = rules_to_execute.begin();
			(rule_id != rules_to_execute.end()) && (!root_worker_thread::is_hcf_server_stopping()); ++rule_id) {
		HCF_CC_TRACE(Execute, "It's time to execute the following rule:", rule_id->c_str());

		// For each rule to be executed, increment the progress percentage and update the progressReport structure
		progress_percentage = ((progress_percentage + progress_step) >= 100) ? 99 : (progress_percentage + progress_step);
		HCF_CC_TRACE(Execute, "Updating the progressReport attribute with the following value:", std::to_string(progress_percentage).c_str());
		job_ptr->update_progress_report(progress_percentage);

		// Retrieve the HcRule instance having the provided ID
		HCF_CC_TRACE(Execute, "Retrieving the object instance for the following rule:", rule_id->c_str());
		if (!(rule_ptr = hcrule_class_handler.get_instance(*rule_id))) {
			HCF_CC_LOG(LEVEL_ERROR, "Call 'get_instance' returned a NULL pointer for the rule having id == '%s'", rule_id->c_str());
			HCF_CC_TRACE(ExecuteErr, "Call 'hcrule_class_handler.get_instance' failed.", ERR_NULL_INSTANCE_RETRIEVED);
			continue;
		}

		// Check the HcRule administrative state, in case it's locked, skip this rule
		HCF_CC_TRACE(Execute, "Checking the adminsitrativeState for following rule:", rule_id->c_str());
		if (rule_ptr->is_locked()) {
			HCF_CC_LOG(LEVEL_WARNING, "The rule having id == '%s' is locked, skipping it!", rule_id->c_str());
			HCF_CC_TRACE(Execute, "The following rule is LOCKED, skipping it!", rule_id->c_str());
			continue;
		}

		// Before executing the rule, check if its preconditions are met or not
		HCF_CC_TRACE(Execute, "Checking if the preconditions for the following rule are satisfied:", rule_id->c_str());
		if (check_exec_env(rule_ptr) < 0) {
			HCF_CC_LOG(LEVEL_WARNING, "Rule skipped since it cannot be executed in this environment.");
			HCF_CC_TRACE(Execute, "The rule preconditions are not fulfilled, skipping the rule!", rule_id->c_str());
			continue;
		}

		// Extract from the HcRule object the information about the targets on which the rule must be executed
		HCF_CC_TRACE(Execute, "Retrieving the list of rule targets for the following rule:", rule_id->c_str());
		std::set<std::string> rule_targets = rule_ptr->get_targets();
		if (rule_targets.size() == 0) {
			HCF_CC_LOG(LEVEL_WARNING, "The rule having id == '%s' hasn't available targets, skipping it!", rule_id->c_str());
			HCF_CC_TRACE(Execute, "The following rule hasn't available targers, skipping it!", rule_id->c_str());
			continue;
		}

		/*
		 * Before executing the rule, create a log file in a temporary folder in order to save the command printout.
		 * The following operations must be executed:
		 * 	1. Log file path generation (depending from rule ID)
		 * 	2. Log file creation
		 */
		char log_file_path[PATH_MAX + 1];
		::snprintf(log_file_path, TSL_ARRAY_SIZE(log_file_path), "%s/%s%s", tmp_output_folder, rule_ptr->get_id().c_str(), HCF_RULE_LOG_FILE_EXTENSION);

		int log_file_fd = -1;
		HCF_CC_TRACE(Execute, "Opening the log file for the following rule:", log_file_path);
		if ((log_file_fd = ::open(log_file_path, O_RDWR | O_CREAT, 0640)) < 0) {
			const int errno_save = errno;
			HCF_CC_LOG(LEVEL_ERROR, "Call 'open' failed for file %s, errno == %d", log_file_path, errno_save);
			HCF_CC_TRACE(ExecuteErr, "Call 'open' failed.", errno_save);
			continue;
		}

		// Extract from the HcRule object all the info needed for the rule evaluation
		std::map<std::string, std::pair<std::string, std::string>> rule_attrs = rule_ptr->get_rule_attributes(job_ptr->get_id());

		// By default, the rule is executed with success
		bool rule_failed = false;

		// Execute the rule on each of the provided targets
		for (std::set<std::string>::const_iterator target = rule_targets.begin(); (target != rule_targets.end()) && (!root_worker_thread::is_hcf_server_stopping()); ++target) {
			char complete_command[2048];
			const char * target_hostname = configurator::get_node_hostname(*target);
			bool is_local_node = true;
			HCF_CC_TRACE(Execute, "Executing the rule on the following target:", target_hostname);

			/*
			 * Before creating the command string, check if the target node is local or
			 * remote. In case of remote node, check if it is reachable or not.
			 * In case of not reachable node, set the healthiness status to NOT_HEALTHY
			 * (since when a node cannot be reached the cluster is not in a good state),
			 * store the information about the lack of connection towards a remote node
			 * and continue with the remaining health check rules.
			 */
			if (::strncmp(current_hostname, target_hostname, current_hostname_len)) { // Remote node, check if it is reachable
				is_local_node = false;

				HCF_CC_TRACE(Execute, "The rule target is a remote node, checking if it's reachable!", target_hostname);
				if (!is_node_reachable(target_hostname)) {
					remote_nodes_not_reachable.insert(target_hostname);
					status = HEALTH_STATUS_NOT_HEALTHY;
					HCF_CC_TRACE(Execute, "The rule target is not reachable, the healthiness status is NOT_HEALTHY!", target_hostname);
					continue;
				}
			}

			for (std::list<SUFFIX_CONCAT(os,SUFFIX_C)>::iterator it_os = rule_ptr->get_parameter_container()->os_list.begin();
					it_os != rule_ptr->get_parameter_container()->os_list.end(); ++it_os){

				if (is_local_node) {
					// The rule must be executed on the current node, don't use SSH!
					::snprintf(complete_command, TSL_ARRAY_SIZE(complete_command),
#ifdef HCF_CC_RUNS_WITH_NO_ROOT_PERMISSIONS
							"sudo -- sh -c "
#endif
							"\"%s\"", it_os->command.c_str());
				}
				else {
					char escaped_command[2048];
					::strncpy(escaped_command, it_os->command.c_str(), TSL_ARRAY_SIZE(escaped_command));
					utils::escape_special_characters(escaped_command, TSL_ARRAY_SIZE(escaped_command));

					// The rule must be executed on a remote node, use SSH!
					::snprintf(complete_command, TSL_ARRAY_SIZE(complete_command),
#ifdef HCF_CC_RUNS_WITH_NO_ROOT_PERMISSIONS
							"sudo "
#endif
							"ssh -t -q root@%s \"%s\"", target_hostname, escaped_command);
				}

				// Remove from the command string all the unwanted characters (for example new lines)
				remove_new_line_characters(complete_command, ::strlen(complete_command));
				HCF_CC_LOG(LEVEL_NOTICE, "The Log level is set to %d", rule_ptr->get_skip_log());

				if (rule_ptr->get_skip_log() != SKIP_COMMAND && rule_ptr->get_skip_log() != SKIP_ALL) {
				  HCF_CC_LOG(LEVEL_NOTICE, "Executing the rule having id == '%s' (command == %s)", rule_id->c_str(), complete_command);
					HCF_CC_TRACE(Execute, "Executing the following command:", complete_command);
				}
				char buffer[1024 * 1024]; // 1 MB buffer to store command printout

				// Execute the command associated to the current HcRule
				if (execute_command(is_local_node, complete_command, buffer, TSL_ARRAY_SIZE(buffer), rule_ptr->get_env_variables()) < 0) {
					HCF_CC_LOG(LEVEL_ERROR, "Call 'execute_command' failed, skipping this command!");
					HCF_CC_TRACE(ExecuteErr, "Call 'execute_command' failed.", -1);
					continue;
				}

				// The command execution could take long time, so check if the stop has been signaled
				if (root_worker_thread::is_hcf_server_stopping()) {
					HCF_CC_TRACE(Execute, "The stop condition has been signaled, breaking!", "");
					break;
				}

				// Before evaluating the rule, save its printout in the log file for further use
				char msg_to_write[(1024 * 1024) + 128];	// To store buffer + header message
				::snprintf(msg_to_write, TSL_ARRAY_SIZE(msg_to_write), "EXECUTION on %s\n%s\n", target_hostname, buffer);

				ssize_t bytes_written = -1;
				const ssize_t bytes_to_write = ::strlen(msg_to_write);
				HCF_CC_TRACE(Execute, "Adding the information on the command previously executed to the following rule log file:", log_file_path);
				if ((log_file_fd != -1) && ((bytes_written = ::write(log_file_fd, msg_to_write, bytes_to_write)) != bytes_to_write)) {
					const int errno_save = errno;
					HCF_CC_LOG(LEVEL_ERROR, "Call 'write' failed for file %s, bytes_written == %zd, bytes_to_write == %zd, errno == %d",
							log_file_path, bytes_written, bytes_to_write, errno_save);
					::close(log_file_fd);
					HCF_CC_TRACE(ExecuteErr, "Call 'write' failed.", errno_save);
				}
				it_os->command_printout = buffer;
			}

			// Set the rule attributes list into the filter parameter containers
			rule_ptr->get_parameter_container()->set_rule_attributes(&rule_attrs);
			rule_executor rule_executor;

			//Set logs to skip in rule executor
			rule_executor.set_skip_log(rule_ptr->get_skip_log());

			//Evaluate the rule
			HCF_CC_TRACE(Execute, "After executing all the commands associated to the couple (rule, target), let's evaluate it!", "");
			int evaluation_result = rule_executor.evaluate_rule(rule_ptr->get_parameter_container());

			if (evaluation_result == exec_operation::evaluation_return_code::EVAL_RC_FALSE) {
				HCF_CC_LOG(LEVEL_NOTICE, "The rule result was: FAILURE!");
				HCF_CC_TRACE(Execute, "The rule was executed successfully and its result is FAILURE", "");
				rule_failed = true;
			}else if(evaluation_result == exec_operation::evaluation_return_code::EVAL_RC_TRUE){
				HCF_CC_LOG(LEVEL_NOTICE, "The rule result was: SUCCESS!");
				HCF_CC_TRACE(Execute, "The rule was executed successfully and its result is SUCCESS", "");
			}else {
				HCF_CC_LOG(LEVEL_NOTICE, "The rule result was: ERROR(%d)!",evaluation_result);
				job_ptr->job_execution_ongoing();
				job_ptr->finalize_progress_report(false, "Job execution stopped due to error.");
				HCF_CC_TRACE(ExecuteErr, "Call 'evaluate_rule' failed.", evaluation_result);
				return;
			}

			// Update the current node healthiness value according to the rule result and to its severity
			HCF_CC_TRACE(Execute, "The current (partial) healthiness status is the following one, checking if it must be updated!", utils::map_health_status_to_string(status));
			if ((rule_failed) && (status != HEALTH_STATUS_NOT_HEALTHY)) {
				status = (rule_ptr->is_critical()) ? HEALTH_STATUS_NOT_HEALTHY : HEALTH_STATUS_WARNING;
				HCF_CC_TRACE(Execute, "The rule is failed, and the old status wasn't NOT_HEALTHY, the new value is:", utils::map_health_status_to_string(status));
			}
		}
		::close(log_file_fd);

		// Depending to the rule result, add the HcRule ID to the output file handler
		if (rule_failed) {
			HCF_CC_TRACE(Execute, "The rule was failed on one or more targets, adding it to the list of failed rules!", rule_id->c_str());
			writer.add_failed_rule(*rule_id);

			/*
			 * When the rule result is not the expected one, the rule must
			 * be also added to the list of failed rules to be showed under HcJob MOC.
			 */
			failed_rules.push_back(rule_ptr);
		}
		else {
			HCF_CC_TRACE(Execute, "The rule was successfully executed on all targets, adding it to the list of success rules!", rule_id->c_str());
			writer.add_success_rule(*rule_id);
		}
	}

	if (root_worker_thread::is_hcf_server_stopping()) {
		job_ptr->job_execution_ongoing();
		job_ptr->finalize_progress_report(false, "Job execution stopped due to AMF request.");
		HCF_CC_LOG(LEVEL_WARNING, "Job not completed, stop has been signaled!");
		HCF_CC_TRACE(Execute, "The stop condition has been signaled, the job wasn't completed!", "");
		return;
	}
	HCF_CC_LOG(LEVEL_NOTICE, "The HcJob execution has been successfully completed, the node status is %s!", utils::map_health_status_to_string(status));
	HCF_CC_TRACE(Execute, "The job execution was completed, the node status is:", utils::map_health_status_to_string(status));

	// Prepare a brief string to be logged into system log file
	std::string categories_msg_string;
	if (status != HEALTH_STATUS_HEALTHY) {
		bool is_first_time = true;
		for (std::set<category_t>::const_iterator c = job.categories.begin(); c != job.categories.end(); ++c) {
			if (is_first_time)	is_first_time = false;
			else categories_msg_string += ", ";
			categories_msg_string += utils::map_category_value_to_category_string(*c);
		}
	}
	else categories_msg_string = "NONE";

	HCF_CC_TRACE(Execute, "Sending the following information about the categories of executed job to the system log:", categories_msg_string.c_str());
	::syslog(LOG_INFO, "HCJOB=%s HCSTATUS=%s FAILEDCATEGORIES: %s", job_ptr->get_id().c_str(),
			utils::map_health_status_to_string(status), categories_msg_string.c_str());

	// Calculate the job execution end timestamp
	char job_end_timestamp[1024];
	time_t curr_time = ::time(0);
	struct tm * current_time = ::localtime(&curr_time);
	::snprintf(job_end_timestamp, TSL_ARRAY_SIZE(job_end_timestamp), "%04d-%02d-%02dT%02d:%02d:%02d", (current_time->tm_year + 1900),
			(current_time->tm_mon + 1), current_time->tm_mday, current_time->tm_hour, current_time->tm_min, current_time->tm_sec);
	HCF_CC_TRACE(Execute, "The following value will be used as job end timestamp:", job_end_timestamp);

	/*
	 * After the HcJob has been successfully executed, the MOM must be aligned
	 * with all the info deriving from it. The following attributes must be modified:
	 *
	 * In HcJob MOC:
	 * 	- failedRules
	 *  - lastReportFileName
	 *  - lastRunTime
	 *  - progressReport
	 *  - status
	 *
	 * In HealthCheckM MOC:
	 *  - lastExecutedJob
	 *  - lastStatus
	 *  - lastUpdateTime
	 */
	// HcJob failedRules attribute update: first delete the already existing failed rules
	HCF_CC_TRACE(Execute, "Clearing all the previously created instances of the failedRules structure for the following job:", job_ptr->get_id().c_str());
	job_ptr->clear_failed_rules();

	// HcJob failedRules attribute update: then, for each failed rule, add the info into the HcJob MOC
	for (std::list<imm_object_hcrule *>::iterator it = failed_rules.begin(); it != failed_rules.end(); ++it) {
		imm_object_hcrule * rule = *it;
		HCF_CC_LOG(LEVEL_NOTICE, "Adding the rule having id == %s to the failed rules of job "
				"having id == %s!", rule->get_id().c_str(), job_ptr->get_id().c_str());
		HCF_CC_TRACE(Execute, "Creating a failedRule instance for the following rule:", rule->get_id().c_str());
		job_ptr->set_failed_rule(rule->get_id(), rule->get_reason());
	}

	// To calculate the lastReportFileName, retrieve the Managed Element ID
	HCF_CC_TRACE(Execute, "Before creating the report filename, retrieve the MEId!", "");
	size_t meid_buffer_size = 2048;
	char meid[meid_buffer_size];
	if ((call_result = configurator::get_managed_element_id(meid, meid_buffer_size))) {
		HCF_CC_LOG(LEVEL_ERROR, "Call 'configurator::get_managed_element_id' failed "
				"(using the default value), call_result == %d", call_result);
		HCF_CC_TRACE(ExecuteErr, "Call 'configurator::get_managed_element_id' failed, using default MEId!", call_result);
		::strncpy(meid, configuration_handler::HCF_MANAGED_ELEMENT_ID_DEFAULT_VALUE, meid_buffer_size);
	}
	HCF_CC_TRACE(Execute, "The value retrieved for the MEId is:", meid);

	// HcJob lastReportFileName attribute update: calculate the file name and update the HcJob MOC
	char report_file_name[PATH_MAX + 1];
	::snprintf(report_file_name, TSL_ARRAY_SIZE(report_file_name), "%s_%s_%04d%02d%02dT%02d%02d%02d%s",
			meid, job_ptr->get_id().c_str(), (current_time->tm_year + 1900), (current_time->tm_mon + 1),
			current_time->tm_mday, current_time->tm_hour, current_time->tm_min, current_time->tm_sec,
			(job.is_scheduled_execution ? "" : "_man"));
	HCF_CC_TRACE(Execute, "Setting the following value as report file name:", report_file_name);
	job_ptr->set_last_report_file_name(report_file_name);

	// HcJob lastRunTime attribute update
	HCF_CC_TRACE(Execute, "Setting the following value as lastRunTime:", job_end_timestamp);
	job_ptr->set_last_run_time(job_end_timestamp);

	// HcJob status attribute update
	HCF_CC_TRACE(Execute, "Setting the following value as healthiness status:", utils::map_health_status_to_string(status));
	job_ptr->set_status(status);

	// To update the HealthCheckM MOC, get the pointer to the root MOC
	HCF_CC_TRACE(Execute, "Retrieving the root MOC instance.", "");
	imm_object_healthcheckm * hcm_root = healthcheckm_class_handler.get_instance(imm_object_healthcheckm::HCF_HEALTHCHECKM_ID_DEFAULT_VALUE);
	if (!hcm_root) {
		// In case the retrieving fails, update accordingly the progressReport attribute
		HCF_CC_LOG(LEVEL_ERROR, "Warning received a NULL pointer for the root MOC, updating progressReport");
		HCF_CC_TRACE(ExecuteErr, "Call 'healthcheckm_class_handler.get_instance' failed.", ERR_NULL_INSTANCE_RETRIEVED);
		job_ptr->job_execution_ongoing();
		job_ptr->finalize_progress_report(false, "Failed to update the HealthCheckM MOC");
		return;
	}

	// HealthCheckM lastExecutedJob attribute update
	HCF_CC_TRACE(Execute, "Updating the lastExecutedJob attribute of the root MOC with the following value:", job_ptr->get_dn().c_str());
	hcm_root->set_last_executed_job(job_ptr->get_dn());

	// HealthCheckM lastStatus attribute update
	HCF_CC_TRACE(Execute, "Updating the lastStatus attribute of the root MOC with the following value:", utils::map_health_status_to_string(status));
	hcm_root->set_last_status(status);

	// HealthCheckM lastUpdateTime attribute update
	HCF_CC_TRACE(Execute, "Updating the lastUpdatetime attribute of the root MOC with the following value:", job_end_timestamp);
	hcm_root->set_last_update_time(job_end_timestamp);

	// After updating the model, provide all the remaining info to the output file handler
	writer.set_date(job_end_timestamp);
	writer.set_status(status);

	// Calculate the path of the output file
	char report_file_path[PATH_MAX + 1];
	::snprintf(report_file_path, TSL_ARRAY_SIZE(report_file_path), "%s/%s", output_folder, report_file_name);

	// Create the output file having the calculated path
	HCF_CC_TRACE(Execute, "Creating the job output file at the following path:", report_file_path);
	if ((call_result = writer.write(report_file_path))) {
		// In case the write fails, update accordingly the progressReport attribute
		HCF_CC_LOG(LEVEL_ERROR, "Call failed to write the output file having path == %s, call_result == %d", report_file_path, call_result);
		job_ptr->job_execution_ongoing();
		job_ptr->finalize_progress_report(false, "Failed to write the output file");
		HCF_CC_TRACE(ExecuteErr, "Call 'writer.write' failed.", call_result);
		return;
	}

	// Calculate the archive file name
	char archive_file_name[PATH_MAX + 1];
	::snprintf(archive_file_name, TSL_ARRAY_SIZE(archive_file_name), "%s%s", report_file_name, HCF_OUTPUT_ARCHIVE_EXTENSION);
	HCF_CC_TRACE(Execute, "The job output archive file will have the following name:", archive_file_name);

	// Calculate the path of the archive file
	char archive_file_path[PATH_MAX + 1];
	::snprintf(archive_file_path, TSL_ARRAY_SIZE(archive_file_path), "%s/%s", output_folder, archive_file_name);
	HCF_CC_TRACE(Execute, "The job output archive file will have the following path:", archive_file_path);

	// Create the archive containing all the commands printouts
	char archive_cmd[PATH_MAX + 1];
	::snprintf(archive_cmd, TSL_ARRAY_SIZE(archive_cmd), "cd %s; tar czf %s * &> /dev/null; chmod 640 %s",
			tmp_output_folder, archive_file_path, archive_file_path);

	// Execute the command to create the archive file
	HCF_CC_TRACE(Execute, "Creating the archive file using the following command:", archive_cmd);
	if ((call_result = ::system(archive_cmd)) < 0) {
		HCF_CC_LOG(LEVEL_ERROR, "Command '%s' failed with error code == %d", archive_cmd, call_result);
		HCF_CC_TRACE(ExecuteErr, "Call 'system' failed.", call_result);
	}

	// After creating the archive file, remove the temporary folder used to store the commands printouts
	HCF_CC_TRACE(Execute, "Removing the temporary output folder:", tmp_output_folder);
	if (TSL_NS_HCF_CORE::utils::remove_folder_recursive(tmp_output_folder) < 0) {
		const int errno_save = errno;
		HCF_CC_LOG(LEVEL_ERROR, "Failed to remove the temporary output folder '%s'", tmp_output_folder);
		HCF_CC_TRACE(ExecuteErr, "Call 'utils::create_dir_recursive' failed.", errno_save);
	}

	int reportSent = FILE_TRANSFER_OK; // OK as default
	std::string report_sftp_msg;
	int archiveSent = FILE_TRANSFER_OK; // OK as default
	std::string archive_sftp_msg;
	if(isSftpUri) {
		// Output files are copied into the sftp exporting folder and, then, given in charge to the SFTP handler.
		std::string sftp_folder = sftpHandler.get_sftp_folder();

		// Handle transfer of report file
		std::string sftp_report_file_path = sftp_folder + "/" + report_file_name;
		HCF_CC_TRACE(Execute, "Moving the previously created output file to the following SFTP private folder:", sftp_report_file_path.c_str());
		if( TSL_NS_HCF_CORE::utils::copy(report_file_path, sftp_report_file_path.c_str()) == 0) {
			//Complete remote file path
			report_file.remoteFile = (report_file.remoteFile.empty() ? report_file_name : (report_file.remoteFile + "/" + report_file_name));
			HCF_CC_TRACE(Execute, "Requesting to transfer to the following remote file:", report_file.remoteFile.c_str());

			// Set local file path
			report_file.localFile = sftp_report_file_path;
			HCF_CC_TRACE(Execute, "Requesting to transfer the following local file:", report_file.localFile.c_str());

			// File is given in charge to SFTP Handler
			HCF_CC_TRACE(Execute, "Adding the report file to the list of files to be transferred over SFTP protocol!", "");
			sftpHandler.add_file_to_ftq(report_file);

			// Wait for sftp transfer end
			HCF_CC_TRACE(Execute, "Waiting the termination (or timeout expiration) of the report file sending operation!", "");
			sftpHandler.wait_sftp_transfer_end(reportSent, report_sftp_msg);
		}
		else {
			const int errno_save = errno;
			reportSent = FILE_TRANSFER_ERROR;
			report_sftp_msg = "I/O error";
			HCF_CC_LOG(LEVEL_ERROR, "Failed to copy report file '%s' into sftp exporting folder '%s', "
					"report file will not be exported!", report_file_path, sftp_folder.c_str());
			HCF_CC_TRACE(ExecuteErr, "Call 'utils::copy' failed for report file.", errno_save);
		}

		// In case of report file transfer error, skip archive file transfer
		if (reportSent == FILE_TRANSFER_OK) {
			// Handle transfer of logs archive file
			std::string sftp_archive_file_path = sftp_folder + "/" + archive_file_name;
			if( TSL_NS_HCF_CORE::utils::copy(archive_file_path, sftp_archive_file_path.c_str()) == 0) {
				//Complete remote file path
				archive_file.remoteFile = (archive_file.remoteFile.empty() ? archive_file_name : (archive_file.remoteFile + "/" + archive_file_name));
				HCF_CC_TRACE(Execute, "Requesting to transfer to the following remote file:", archive_file.remoteFile.c_str());

				// Set local file path
				archive_file.localFile = sftp_archive_file_path;
				HCF_CC_TRACE(Execute, "Requesting to transfer the following local file:", archive_file.localFile.c_str());

				// File is given in charge to SFTP Handler
				HCF_CC_TRACE(Execute, "Adding the report file to the list of files to be transferred over SFTP protocol!", "");
				sftpHandler.add_file_to_ftq(archive_file);

				// Wait for sftp transfer end
				HCF_CC_TRACE(Execute, "Waiting the termination (or timeout expiration) of the report archive sending operation!", "");
				sftpHandler.wait_sftp_transfer_end(archiveSent, archive_sftp_msg);
			}
			else {
				const int errno_save = errno;
				archiveSent = FILE_TRANSFER_ERROR;
				archive_sftp_msg = "I/O error";
				HCF_CC_LOG(LEVEL_ERROR, "Failed to copy archive file '%s' into sftp exporting folder '%s', "
						"archive file will not be exported!", archive_file_path, sftp_folder.c_str());
				HCF_CC_TRACE(ExecuteErr, "Call 'utils::copy' failed for archive file.", errno_save);
			}
		}
	}

	// When the job execution is finished, signal to all the other jobs that one job is finished
	HCF_CC_TRACE(Execute, "Signaling that the job with the following id finished its execution!", job.id.c_str());
	job_ptr->job_execution_ongoing();

	/*
	 * To implement an housekeeping policy, check how many files are currently stored under the output folder.
	 * This mechanism must be applied only if the default output folder is used; otherwise, no housekeeping is required.
	 */
	if (job.uri.empty() || isSftpUri) {
		HCF_CC_TRACE(Execute, "Listing all the files present into the default output folder in order to check "
				"if some housekeeping operation must be performed:", job_ptr->get_local_file_store_path().c_str());
		std::set<std::string, output_file_lesser> output_files;
		if (TSL_NS_HCF_CORE::utils::list(job_ptr->get_local_file_store_path().c_str(), output_files, &is_valid_output_file) < 0) {
			const int errno_save = errno;
			HCF_CC_LOG(LEVEL_ERROR, "Call 'utils::list' failed for folder '%s'", job_ptr->get_local_file_store_path().c_str());
			job_ptr->finalize_progress_report(false, "Failed to apply the housekeeping policy");
			HCF_CC_TRACE(ExecuteErr, "Call 'utils::list' failed.", errno_save);
			return;
		}

		/*
		 * Check if the number of files currently stores if bigger than the maximum value.
		 * This threshold value is given by the value of maxNoOfReportFiles attribute in
		 * HealthCheckM MOC multiplied by the number of output files produces for each job.
		 * In case the threshold is exceeded, delete the oldest files (a number equal to the
		 * number of produced output files for each job).
		 */
		const int num_of_files = output_files.size();
		const int num_of_files_threshold = hcm_root->get_max_no_of_report_files() * HCF_NUMBER_OF_OUTPUT_FILES_FOR_JOB;

		HCF_CC_TRACE(Execute, "Currently the default output folder contains the following number of files:", std::to_string(num_of_files).c_str());
		HCF_CC_TRACE(Execute, "The current threshold value for the number of report files is:", std::to_string(num_of_files_threshold).c_str());
		if (num_of_files > num_of_files_threshold) {
			HCF_CC_LOG(LEVEL_NOTICE, "Currently there are %d files in the output folder (MAX == %d)!", num_of_files, num_of_files_threshold);
			int num_of_files_to_delete = (num_of_files - num_of_files_threshold);
			HCF_CC_TRACE(Execute, "There are too many files in the output folder, deleting the following number of files:", std::to_string(num_of_files_to_delete).c_str());

			for (std::set<std::string, output_file_lesser>::const_iterator file = output_files.begin();
					(file != output_files.end()) && (num_of_files_to_delete > 0);
					++file, --num_of_files_to_delete) {

				// Create the complete path of the file to be removed and remove it
				std::string file_to_delete_path = job_ptr->get_local_file_store_path() + "/" + (*file);
				HCF_CC_LOG(LEVEL_NOTICE, "Removing the oldest file having path '%s'", file_to_delete_path.c_str());
				HCF_CC_TRACE(Execute, "Removing the current oldest file:", file_to_delete_path.c_str());

				if (TSL_NS_HCF_CORE::utils::remove(file_to_delete_path.c_str()) < 0) {
					const int errno_save = errno;
					// In case of error, update the progressReport attribute accordingly
					HCF_CC_LOG(LEVEL_ERROR, "Call 'utils::remove' failed for file '%s'", file_to_delete_path.c_str());
					job_ptr->finalize_progress_report(false, "Failed to apply the housekeeping policy");
					HCF_CC_TRACE(ExecuteErr, "Call 'utils::remove' failed.", errno_save);
					return;
				}
			}
		}
	}
	else
		HCF_CC_LOG(LEVEL_NOTICE, "A value for the exportUri attribute has been provided and it is not a SFTP URI, no housekeeping policy will be applied!");

	// After each operation has been executed, update the progressReport attribute to show the result of all operations linked to the job execution
	HCF_CC_TRACE(Execute, "The job execution is completed: check how it's needed to update the progressReport structure!", job.id.c_str());
	if (!remote_nodes_not_reachable.empty()) {	// One or more rule targets were not reachable
		std::string msg = "Job execution completed, but the following targets cannot be reached: ";

		bool first_time = true;
		for (std::set<std::string>::const_iterator t = remote_nodes_not_reachable.begin(); t != remote_nodes_not_reachable.end(); ++t) {
			if (first_time)	first_time = false;
			else	msg += ", ";
			msg += *t;
		}

		HCF_CC_TRACE(Execute, "During job execution one or more targets were not reachable: use the following warning message:", msg.c_str());
		job_ptr->finalize_progress_report(true, msg);
	}
	else if ((reportSent == FILE_TRANSFER_OK) && (archiveSent == FILE_TRANSFER_OK)) { // Both report and archive correctly sent
		HCF_CC_TRACE(Execute, "The job execution was successful!", "");
		job_ptr->finalize_progress_report(true, "Job correctly executed");
	}
	else if (reportSent != FILE_TRANSFER_OK) { // Error on report file transfer (archive file transfer skipped)
		HCF_CC_TRACE(Execute, "An error occurred while trying to export the report file and also the archive was skipped: using the following error message:", report_sftp_msg.c_str());
		job_ptr->finalize_progress_report(false, "Error exporting data: " + report_sftp_msg);
	}
	else { // Report sent. Error on archive file transfer
		HCF_CC_TRACE(Execute, "An error occurred while trying to export the archive file: using the following error message:", archive_sftp_msg.c_str());
		job_ptr->finalize_progress_report(false, "Report file exported. Error exporting archive file: " + archive_sftp_msg);
	}

	/*
	 * Check if another job execution must be triggered after the current one is completed.
	 * This can happen if the following conditions are both met:
	 * 	1. The ID of another job has been provided.
	 * 	2. The healthiness status isn't healthy.
	 */
	if ((status != HEALTH_STATUS_HEALTHY) && !(job.job_to_trigger.empty())) {
		HCF_CC_LOG(LEVEL_NOTICE, "The node isn't healthy, there is another job to be triggered with id == '%s'!", job.job_to_trigger.c_str());
		HCF_CC_TRACE(Execute, "The node isn't healthy and the operator provided the ID of the following job to be triggered:", job.job_to_trigger.c_str());

		// Retrieve the HcJob instance having the provided ID
		imm_object_hcjob * job_to_trigger_ptr = hcjob_class_handler.get_instance(job.job_to_trigger);
		if (!job_to_trigger_ptr) {
			HCF_CC_LOG(LEVEL_ERROR, "Call 'hcjob_class_handler.get_instance' failed for HcJob having ID == '%s', "
					"the state is '%s' but it's not possible to trigger any other job!",
					job.job_to_trigger.c_str(), utils::map_health_status_to_string(status));
			HCF_CC_TRACE(ExecuteErr, "Call 'hcjob_class_handler.get_instance' failed.", ERR_NULL_INSTANCE_RETRIEVED);
			return;
		}

		HCF_CC_TRACE(Execute, "Executing the following job (as automatic execution):", job.job_to_trigger.c_str());
		if ((call_result = job_to_trigger_ptr->execute(job.uri, job.pwd, true))) {
			HCF_CC_LOG(LEVEL_ERROR, "Call 'job_to_trigger_ptr->execute' failed with error_code == %d", call_result);
			HCF_CC_TRACE(ExecuteErr, "Call 'job_to_trigger_ptr->execute' failed.", call_result);
			return;
		}
	}
}

int __TSL_CLASS_NAME__::check_exec_env (imm_object_hcrule * rule_ptr) {
	HCF_CC_TRACE_FUNCTION;

	for(std::list<SUFFIX_CONCAT(execEnv,SUFFIX_C)>::const_iterator it_execEnv = rule_ptr->get_parameter_container()->execEnv_list.begin();
			it_execEnv != rule_ptr->get_parameter_container()->execEnv_list.end(); ++it_execEnv)
	{
		HCF_CC_TRACE(CheckExecEnv, "A precondition for the following rule has been found, evaluating it!", rule_ptr->get_id().c_str());
		char complete_command[2048];
		// The rule must be executed on the current node, don't use SSH!
		::snprintf(complete_command, TSL_ARRAY_SIZE(complete_command),
#ifdef HCF_CC_RUNS_WITH_NO_ROOT_PERMISSIONS
				"sudo "
#endif
				"%s", it_execEnv->_execEnvCheck.c_str());

		// Remove from the command string all the unwanted characters (for example new lines)
		remove_new_line_characters(complete_command, ::strlen(complete_command));

		if (rule_ptr->get_skip_log() != SKIP_COMMAND && rule_ptr->get_skip_log() != SKIP_ALL) {
			HCF_CC_LOG(LEVEL_NOTICE, "Executing the execution environment check command == %s)", complete_command);
			HCF_CC_TRACE(CheckExecEnv, "In order to evaluate the precondition, executing the following command:", complete_command);
		}

		char buffer[1024 * 1024] = {0}; // 1 MB buffer to store command printout

		// Execute the execution environment command
		if (execute_command(true, complete_command, buffer, TSL_ARRAY_SIZE(buffer), rule_ptr->get_env_variables()) < 0) {
			HCF_CC_LOG(LEVEL_ERROR, "Call 'execute_command' failed, skipping this command!");
			HCF_CC_TRACE(CheckExecEnvErr, "Call 'execute_command' failed.", -1);
			continue;
		}

		HCF_CC_TRACE(CheckExecEnv, "Command execution completed, checking if its printout matches the provided list with success on match:", it_execEnv->_execOnCondMatch.c_str());
		bool printout_matched = false;
		remove_new_line_characters(buffer, ::strlen(buffer));
		for(std::list<std::string>::const_iterator it_execEnvCond = it_execEnv->_execEnvCondition_list.begin();
				it_execEnvCond != it_execEnv->_execEnvCondition_list.end(); ++it_execEnvCond)
		{
			char tmpExecEnvCond[1024 * 1024] = {0};
			snprintf(tmpExecEnvCond, ((*it_execEnvCond).size()+1),"%s",(*it_execEnvCond).c_str());
			remove_new_line_characters(tmpExecEnvCond, ::strlen(tmpExecEnvCond));

			if (strcmp(buffer,tmpExecEnvCond))
			{
				HCF_CC_TRACE(CheckExecEnv, "No match has been found!", "");
				if (rule_ptr->get_skip_log() != SKIP_COMMAND && rule_ptr->get_skip_log() != SKIP_ALL) {
					HCF_CC_LOG(LEVEL_WARNING, "Exec env command '%s' has printout '%s' that does not match condition printout '%s'",
							complete_command, buffer, (*it_execEnvCond).c_str());
					HCF_CC_TRACE(CheckExecEnv, "No match has been found for the following printout:", buffer);
				}
			}
			else
			{
				HCF_CC_TRACE(CheckExecEnv, "A match has been found, breaking!", "");
				if (rule_ptr->get_skip_log() != SKIP_COMMAND && rule_ptr->get_skip_log() != SKIP_ALL) {
					HCF_CC_LOG(LEVEL_NOTICE, "Exec env command '%s' with printout '%s' matches condition printout '%s'",
							complete_command, buffer, (*it_execEnvCond).c_str());
					HCF_CC_TRACE(CheckExecEnv, "A match has been found for the following printout:", buffer);
				}
				printout_matched = true;
				break;
			}
		}

		bool execOnCondMatch = (it_execEnv->_execOnCondMatch == "no") ? false : true;
		if ((printout_matched && !execOnCondMatch) || (!printout_matched && execOnCondMatch))
		{
			HCF_CC_LOG(LEVEL_WARNING, "Evaluation of ExecEnvironment tags stopped: printout_matched = %s, execOnCondMatch = %s", printout_matched ? "true" : "false",
					execOnCondMatch ? "true" : "false");
			HCF_CC_TRACE(CheckExecEnv, "Stopping to evaluate the rule preconditions, the rule must NOT be executed!", "");
			return -1;
		}
	}

	HCF_CC_TRACE(CheckExecEnv, "All the rule preconditions are fulfilled, the rule must be executed!", "");
	return 0;
}

int __TSL_CLASS_NAME__::execute_command (bool is_local_node, const char * cmd, char * cmd_printout, size_t cmd_printout_size, const std::vector<std::pair<std::string,std::string> > & env_variables) {
	HCF_CC_TRACE_FUNCTION;
	// Create the pipe to make communicate the parent and the child processes
	int pipe_fd[2];
	HCF_CC_TRACE(ExecuteCommand, "Received a request to execute a command, creating a pipe object!");
	if (::pipe2(pipe_fd, O_NONBLOCK) < 0) {
		const int errno_save = errno;
		HCF_CC_LOG(LEVEL_ERROR, "Call 'pipe' failed, errno == %d", errno_save);
		HCF_CC_TRACE(ExecuteCommandErr, "Call 'pipe' failed.", errno_save);
		return -1;
	}

	// Create a child process to execute the command
	HCF_CC_TRACE(ExecuteCommand, "Creating a child process to execute the command!");
	if ((_child_process_pid = ::fork()) < 0) {	// ERROR: The child creation failed, close the pipe and return
		const int errno_save = errno;
		HCF_CC_LOG(LEVEL_ERROR, "Call 'fork' failed, errno == %d", errno_save);
		::close(pipe_fd[0]);
		::close(pipe_fd[1]);
		HCF_CC_TRACE(ExecuteCommandErr, "Call 'fork' failed.", errno_save);
		return -1;
	}
	else if (_child_process_pid) {
		/********************/
		/*  FATHER PROCESS  */
		/********************/
		// Close the write-end of the pipe (the father must only read)
		::close(pipe_fd[1]);

		int child_return_code = 0;
		int call_result = 0;

		for (;;) {
			// Wait the child process termination
			HCF_CC_TRACE(ExecuteCommand, "That's the father process, let's wait the child process termination!");
			if ((call_result = ::waitpid(_child_process_pid, &child_return_code, 0)) <= 0) {
				const int errno_save = errno;
				HCF_CC_TRACE(ExecuteCommandErr, "Call 'waitpid' failed.", errno_save);

				// waitpid has been interrupted by a signal, recall it!
				if (errno_save == EINTR)	continue;

				// ERROR: waitpid call was failed, close the read-end of the pipe and return
				HCF_CC_LOG(LEVEL_ERROR, "Call 'waitpid' failed with errno == %d'", errno_save);
				::close(pipe_fd[0]);
				return -1;
			}
			// Child correctly terminated, let's read the command printout!
			_child_process_pid = -1;
			HCF_CC_TRACE(ExecuteCommand, "The child process was correctly terminated!");
			break;
		}

		// The child process termination could be due to a stop request: in that case, return immediately!
		if (root_worker_thread::is_hcf_server_stopping()) {
			HCF_CC_LOG(LEVEL_WARNING, "The child process is terminated, but the stop event has been signaled: exiting immediately!");
			HCF_CC_TRACE(ExecuteCommand, "The stop condition has been signaled, exiting!");
			::close(pipe_fd[0]);
			return 0;
		}

		ssize_t bytes_read = 0;
		ssize_t total_bytes_read = 0;

		// Use the pipe previously created to read the child process output printout
		for (;;) {
			// Read the command printout
			HCF_CC_TRACE(ExecuteCommand, "Reading the command printout provided by the child process from the pipe!");
			if ((bytes_read = ::read(pipe_fd[0], (cmd_printout + bytes_read), (cmd_printout_size - bytes_read - 1))) < 0) {
				const int errno_save = errno;
				HCF_CC_TRACE(ExecuteCommandErr, "Call 'read' failed.", errno_save);

				// Read was interrupted by a signal or no data is available, try again to read
				if ((errno_save == EINTR) || (errno_save == EWOULDBLOCK))	continue;

				// ERROR: read call was failed, close the read-end of the pipe and return
				HCF_CC_LOG(LEVEL_ERROR, "Call 'read' failed with errno == %d'", errno_save);
				::close(pipe_fd[0]);
				return -1;
			}
			else if (!bytes_read) {	// There is no more data to be read on the pipe!
				HCF_CC_TRACE(ExecuteCommand, "No more data is available to be read on the pipe!");
				break;
			}
			else {	// Some bytes have been read: try to read again!
				HCF_CC_TRACE(ExecuteCommand, "Some bytes have been correctly read: try to read again!");
				total_bytes_read += bytes_read;
			}
		}

		// Put into the read printout a terminator character and close the read-end of the pipe since it is no more needed
		cmd_printout[total_bytes_read] = 0;
		::close(pipe_fd[0]);
	}
	else {
		/*******************/
		/*  CHILD PROCESS  */
		/*******************/
		// Close the read-end of the pipe (the child must only write)
		::close(pipe_fd[0]);

		// Before executing the command, duplicate the child process stdout flow towards the write-end of the pipe
		HCF_CC_TRACE(ExecuteCommand, "That's the child process, let's redirect the standard-output stream to the pipe!");
		if (::dup3(pipe_fd[1], STDOUT_FILENO, 0) < 0) {
			const int errno_save = errno;
			HCF_CC_TRACE(ExecuteCommandErr, "Call 'dup3' failed.", errno_save);

			// ERROR: dup3 call was failed, close the write-end of the pipe and return
			HCF_CC_LOG(LEVEL_ERROR, "Call 'dup3' (for STDOUT) failed with errno == %d'", errno_save);
			::close(pipe_fd[1]);
			::exit(1);
		}

		// The same must be done also for the stderr flow: in this way the parent process can read the command printout
		HCF_CC_TRACE(ExecuteCommand, "That's the child process, let's redirect the standard-error stream to the pipe!");
		if (::dup3(pipe_fd[1], STDERR_FILENO, 0) < 0) {
			const int errno_save = errno;
			HCF_CC_TRACE(ExecuteCommandErr, "Call 'dup3' failed.", errno_save);

			// ERROR: dup3 call was failed, close the write-end of the pipe and return
			HCF_CC_LOG(LEVEL_ERROR, "Call 'dup3' (for STDOUT) failed with errno == %d'", errno_save);
			::close(pipe_fd[1]);
			::exit(2);
		}

		if (!is_local_node) {
			// When the command is executed on a remote node, no environment variable will be passed (since SSH cannot send them)
			if (env_variables.size() > 0) {
				HCF_CC_LOG(LEVEL_WARNING, "The command associated to the rule must be executed on a remote node and "
						"there is a request to export some environment variables... They will be not exported!");
				HCF_CC_TRACE(ExecuteCommand, "WARNING: The command must be executed on a remote node, but one or "
						"more environment variables have been provided: SSH didn't support them, so they will be ignored!");
			}

			// Replace the current process image with the provided command, passing it to the default system shell
			HCF_CC_TRACE(ExecuteCommand, "Executing the 'execlp' command to execute the requested command!");
			if(::execlp("/bin/sh", "/bin/sh", "-c", cmd, (char *)NULL) < 0) {
				const int errno_save = errno;
				HCF_CC_TRACE(ExecuteCommandErr, "Call 'execlp' failed.", errno_save);

				// ERROR: execlp call was failed, close the write-end of the pipe and return
				HCF_CC_LOG(LEVEL_ERROR, "Call 'execlp' failed with errno == %d'", errno_save);
				::close(pipe_fd[1]);
				::exit(3);
			}
		}
		else {
			// Before executing the command, prepare the environment variables to be passed to the exec* function
			size_t num_of_default_env_variables = 0;
			const size_t num_of_user_env_variables = env_variables.size();

			for (size_t i = 0; environ[i]; ++i, ++num_of_default_env_variables) ;
			char * env[num_of_default_env_variables + num_of_user_env_variables + 1];

			// FIRST: add the current process environment variables
			HCF_CC_TRACE(ExecuteCommand, "Adding the process environment variables to the list of environment variables to be used!");
			for (size_t i = 0; i < num_of_default_env_variables; ++i)	env[i] = environ[i];

			// SECOND: add the user defined environment variables (if present)
			HCF_CC_TRACE(ExecuteCommand, "Adding the user-defined variables to the list of environment variables to be used!");
			size_t j = 0;
			for (size_t i = num_of_default_env_variables; i < (num_of_default_env_variables + num_of_user_env_variables); ++i, ++j) {
				HCF_CC_LOG(LEVEL_NOTICE, "Adding environment variable '%s=%s'!", env_variables[j].first.c_str(), env_variables[j].second.c_str());
				const size_t env_variable_len = env_variables[j].first.length() + env_variables[j].second.length() + 2;

				if (!(env[i] = new (std::nothrow) char[env_variable_len])) {
					// ERROR: Memory allocation failed!
					HCF_CC_LOG(LEVEL_ERROR, "Failed to allocate memory for the environment variable '%s=%s',"
							" it will be skipped!", env_variables[j].first.c_str(), env_variables[j].second.c_str());
					HCF_CC_TRACE(ExecuteCommandErr, "Memory allocation failed for environment variable string!", -1);
					continue;
				}

				::snprintf(env[i], env_variable_len, "%s=%s", env_variables[j].first.c_str(), env_variables[j].second.c_str());
			}
			env[num_of_default_env_variables + num_of_user_env_variables] = 0;

			// Replace the current process image with the provided command, passing it to the default system shell
			HCF_CC_TRACE(ExecuteCommand, "Executing the 'execle' command to execute the requested command!");
			if(::execle("/bin/sh", "/bin/sh", "-c", cmd, (char *)NULL, env) < 0) {
				const int errno_save = errno;
				HCF_CC_TRACE(ExecuteCommandErr, "Call 'execle' failed.", errno_save);

				// ERROR: execle call was failed, close the write-end of the pipe and return
				HCF_CC_LOG(LEVEL_ERROR, "Call 'execle' failed with errno == %d'", errno_save);
				::close(pipe_fd[1]);
				::exit(4);
			}
		}
	}

	return ERR_NO_ERROR;
}

void __TSL_CLASS_NAME__::remove_new_line_characters (char * string, const size_t string_size) {
	HCF_CC_TRACE_FUNCTION;
	const size_t str_len = ::strlen(string);
	char new_string[str_len];

	// For each character contained into the input string, if it'a a new line (or a carriage return) skip it!
	size_t i = 0, j = 0;
	for (; i < str_len; ++i) {
		if ((string[i] != '\n') && (string[i] != '\r')) {
			HCF_CC_TRACE(RemoveNewLineCharacters, "New line character found, skipping it!");
			new_string[j++] = string[i];
		}
	}
	new_string[j] = 0;

	// After preprocessing the input string, copy the content of the temporary buffer into the input string
	::strncpy(string, new_string, string_size);
}

bool __TSL_CLASS_NAME__::is_node_reachable (const char * node_id) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(IsNodeReachable, "Received a request to check if the following remote node is reachable, retrieving its IP!", node_id);

	// Retrieve the IP address associated to the provided node
	sockaddr_in node_addr;
	if (configurator::get_addr_by_hostname(node_id, reinterpret_cast<sockaddr *>(&node_addr))) {
		HCF_CC_LOG(LEVEL_ERROR, "Call 'configurator::get_addr_by_hostname' failed for node having ID == '%s': assuming node not reachable!", node_id);
		HCF_CC_TRACE(IsNodeReachableErr, "Call 'configurator::get_addr_by_hostname' failed.", -1);
		return false;
	}

	// Set the port number related to the SSH service on the internal network (not standard 22 port)
	node_addr.sin_port = htons(HCF_INTERNAL_NETWORK_SSH_PORT_NUMBER);
	HCF_CC_TRACE(IsNodeReachable, "The remote node should be reached at the following port:", std::to_string(HCF_INTERNAL_NETWORK_SSH_PORT_NUMBER).c_str());

	// Create a socket in non-blocking mode to connect to the remote node
	int socket_fd = -1;
	HCF_CC_TRACE(IsNodeReachable, "Creating the socket to contact the remote node!", node_id);
	if ((socket_fd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0)) < 0) {
		const int errno_save = errno;
		HCF_CC_LOG(LEVEL_ERROR, "Call 'socket' failed with errno == %d", errno_save);
		HCF_CC_TRACE(IsNodeReachableErr, "Call 'socket' failed.", errno_save);
		return false;
	}

	// Connect to the remote node in non-blocking mode to check if the node is reachable or not
	HCF_CC_TRACE(IsNodeReachable, "Connecting to the remote node!", node_id);
	if (const int call_result = ::connect(socket_fd, reinterpret_cast<sockaddr *>(&node_addr), sizeof(node_addr))) {
		// Call to 'connect' was failed: it can be normal, since the socket is in non-blocking mode
		const int errno_save = errno;

		if (errno_save != EINPROGRESS) {	// Call was failed and connection is not in progress... the node is not reachable!
			HCF_CC_LOG(LEVEL_ERROR, "Call 'connect' failed with error == %d: The node is NOT REACHABLE!", errno_save);
			HCF_CC_TRACE(IsNodeReachableErr, "Call 'connect' failed.", errno_save);
			::close(socket_fd);
			return false;
		}

		// Initialize the needed data structures for 'poll' system call
		struct pollfd fds;
		fds.fd = socket_fd;
		fds.events = POLLIN | POLLPRI | POLLRDHUP | POLLERR | POLLHUP | POLLNVAL;
		fds.revents = 0;

		// The connection is in progress... let's wait for some time the connection to be established!
		HCF_CC_TRACE(IsNodeReachable, "The connection is still in progress, let's wait for the following number of seconds:", std::to_string(HCF_INTERNAL_NETWORK_CONNECTION_TIMEOUT).c_str());
		if (const int poll_result = ::poll(&fds, 1, HCF_INTERNAL_NETWORK_CONNECTION_TIMEOUT) <= 0) {
			// Connection was failed: it can be failed for an error or for a timeout expiration
			const int errno_save = errno;

			if (!poll_result) {	// Timeout occurred while waiting connection: the node is not reachable!
				HCF_CC_LOG(LEVEL_ERROR, "Timeout occurred while waiting connection response: the node is NOT REACHABLE!");
				HCF_CC_TRACE(IsNodeReachable, "Timeout expired while trying to connect to the remote node!", node_id);
			}
			else {
				HCF_CC_LOG(LEVEL_ERROR, "Call 'poll' failed with error == %d", errno_save);
				HCF_CC_TRACE(IsNodeReachableErr, "Call 'poll' failed.", errno_save);
			}

			::close(socket_fd);
			return false;
		}

		// Remote server replied to the connection request: check the response!
		int socket_error_value = 0;
		socklen_t socket_error_len = sizeof(socket_error_value);
		HCF_CC_TRACE(IsNodeReachable, "The connection operation was completed, checking the return code value!", node_id);
		if (::getsockopt(socket_fd, SOL_SOCKET, SO_ERROR, &socket_error_value, &socket_error_len)) {
			const int errno_save = errno;
			HCF_CC_LOG(LEVEL_ERROR, "Call 'getsockopt' failed with error == %d", errno_save);
			HCF_CC_TRACE(IsNodeReachableErr, "Call 'getsockopt' failed.", errno_save);
			::close(socket_fd);
			return false;
		}

		if (socket_error_value) {
			// The connection towards remote node failed: log the retrieved error and return false
			HCF_CC_LOG(LEVEL_ERROR, "The remote node is NOT REACHABLE: the connection returned %d!", socket_error_value);
			HCF_CC_TRACE(IsNodeReachable, "The remote node is NOT REACHABLE!", node_id);
			::close(socket_fd);
			return false;
		}
	}

	HCF_CC_TRACE(IsNodeReachable, "The remote node is REACHABLE!", node_id);
	::close(socket_fd);
	return true;	// The node is reachable!!!
}

const std::string __TSL_CLASS_NAME__::extract_timestamp_from_filename (const std::string & filename) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(ExtractTimestampFromFilename, "Received a request to extract the timestamp information from the following filename:", filename.c_str());

	int x, y;	// Temporary variables to be not used, only for sscanf purposes :-)
	size_t pos;

	// Extract the first underscore character (that should be always present in a valid file name)
	if ((pos = filename.find_first_of('_')) == std::string::npos) {
		HCF_CC_TRACE(ExtractTimestampFromFilename, "No underscore character found in the filename:", filename.c_str());
		return "";
	}
	std::string name = filename.substr(pos + 1);

	do {
		// Extract the position of the underscore characters
		if ((pos = name.find_first_of('_')) == std::string::npos) {
			HCF_CC_TRACE(ExtractTimestampFromFilename, "No underscore character found in the filename:", filename.c_str());
			return "";
		}
		name = name.substr(pos + 1);
	}
	while (::sscanf(name.c_str(), "%8dT%6d", &x, &y) != 2);

	HCF_CC_TRACE(ExtractTimestampFromFilename, "The found timestamp value is:", name.c_str());
	return name;
}

HCF_NS_HCF_SRV_END
