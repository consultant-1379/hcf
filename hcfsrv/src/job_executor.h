#ifndef __INCLUDE_GUARD_CLASS_SRV_JOB_EXECUTOR__
#define __INCLUDE_GUARD_CLASS_SRV_JOB_EXECUTOR__ job_executor

/*
 *	@file job_executor.h
 *	@brief
 *	@author xcasale (Alessio Cascone)
 *	@date 2014-12-15
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
 *	| 2014-001 | 2014-12-15 | xcasale      | Creation and first revision.        |
 *	+==========+============+==============+=====================================+
 *	| 2015-002 | 2015-03-06 | xcasale      | Event handling adaptation.          |
 *	+==========+============+==============+=====================================+
 */

#include <dirent.h>

#include <queue>

#include "hcf/core/event_handler.h"

#include "srv_macros.h"
#include "imm_object_hcjob.h"
#include "imm_object_hcrule.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_SRV_JOB_EXECUTOR__

HCF_NS_HCF_SRV_BEGIN

class __TSL_CLASS_NAME__ : public TSL_NS_HCF_CORE::event_handler {
	//==========//
	// Typedefs //
	//==========//
private:
	typedef TSL_NS_HCF_CORE::thread_mutex_recursive sync_t;

	//===============//
	// Nested Classes//
	//==============//
public:
	class output_file_lesser {
		/**
		 * This functor class is used to check if the name of the first
		 * output file must be put before the second one in the sorting.
		 * Both input files have one of the following formats:
		 * - MEID_JOBID_timestamp_man
		 * - MEID_JOBID_timestamp
		 * The files must be ordered only according to timestamp values.
		 *
		 * @param[in]  first - The name of the first file.
		 * @param[in]  second - The name of the second file.
		 * @return true if the first file must be put before the second one, false otherwise.
		 */
	public:
		bool operator ()(const std::string & first, const std::string & second);
	};

private:
	/**
	 * This structure stores all the information needed to execute a health check job.
	 */
	struct job_to_execute {
		std::string id;
		std::set<category_t> categories;
		std::string uri;
		std::string pwd;
		std::string job_to_trigger;
		bool is_scheduled_execution;
	};

	//==============//
	// Constructors //
	//==============//
public:
	inline TSL_CTOR ()
		TSL_CTOR_INIT_LIST(TSL_NS_HCF_CORE::event_handler(), _event_fd(-1), _jobs(), _jobs_mutex(), _child_process_pid(-1)) {}

private:
	TSL_CTOR (const __TSL_CLASS_NAME__ & rhs);

	//============//
	// Destructor //
	//============//
public:
	virtual inline TSL_DTOR () {}

	//======================================================//
	// event_handler class Virtual Functions Implementation //
	//======================================================//
public:
	/**
	 * This method returns the I/O handle associated to the Event Handler object.
	 *
	 * @return the I/O handle associated to the class implementer.
	 */
	inline virtual int get_handle () { return _event_fd; }

	/**
	 * This method is called when an input operation is signaled on the I/O handle.
	 *
	 * @return 0 on success, -1 on failure (the Event Handler is removed from the reactor)
	 */
	virtual int handle_input ();

	/**
	 * This method is called when an output operation is signaled on the I/O handle.
	 *
	 * @return 0 on success, -1 on failure (the Event Handler is removed from the reactor)
	 */
	virtual int handle_output ();

	/**
	 * This method is called when an error occurs on the I/O handle.
	 *
	 * @return 0 on success, -1 on failure (the Event Handler is removed from the reactor)
	 */
	virtual int handle_exception (bool error_flag, bool hungup_flag, bool read_hungup_flag);

	/**
	 * This method is called before removing the Event Handler from the reactor.
	 *
	 * @return 0 on success, -1 on failure.
	 */
	virtual int handle_close ();

	//===========//
	// Functions //
	//===========//
public:
	/**
	 * This method initializes the Job Executor object.
	 *
	 * @return 0 on success, a non zero value otherwise.
	 */
	int initialize ();

	/**
	 * This method finalizes the Job Executor object.
	 *
	 * @return 0 on success, a non zero value otherwise.
	 */
	int finalize ();

	/**
	 * This method signals the stop event to the Job Executor object.
	 * This is useful since the Job Executor could be waiting the child process termination.
	 */
	int stop ();

	/**
	 * This method adds a job to the list of the jobs to be executed by the Job Executor.
	 *
	 * @param[in] job_id - The ID of the HcJob instance to be executed.
	 * @param[in] rules_categories - The categories of rules to be executed.
	 * @param[in] export_uri - The URI of the remote destination where the files must be sent.
	 * @param[in] export_password - The password needed to access to exportUri.
	 * @param[in] job_to_trigger - The ID of another HcJob instance to be executed if the current one fails.
	 * @param[in] is_scheduled_execution - A flag indicating if the job has been executed manually or by a scheduler.
	 * @return 0 on success, a not zero value otherwise.
	 */
	int execute_job (
			const std::string & job_id,
			const std::set<category_t> & rules_categories,
			const std::string & export_uri,
			const std::string & export_password,
			const std::string & job_to_trigger,
			bool is_scheduled_execution);

private:
	/**
	 * This method executes the health check job having the provided info.
	 *
	 * @param[in] job - The structure containing the info about the job.
	 */
	void execute (job_to_execute & job);

	/**
	 * This method checks the preconditions to be fulfilled before executing a rule.
	 *
	 * @param[in] rule_ptr - The pointer to the rule object.
	 * @return 0 when preconditions are fulfilled, a non zero value otherwise.
	 */
	int check_exec_env(imm_object_hcrule * rule_ptr);

	/**
	 * This method execute the command provided as input.
	 * The command output is redirected to the provided file descriptors.
	 *
	 * @param[in]  is_local_node - A flag indicating if the command is to be executed on the local node or not.
	 * @param[in]  cmd - The command to be executed.
	 * @param[out] cmd_printout - The command printout.
	 * @param[in]  cmd_printout_size - The maximum size for the command printout.
	 * @param[in]  env_variables - The user-defined environment variables to be used before command execution.
	 * @return 0 on success, a non zero value otherwise.
	 */
	int execute_command (bool is_local_node, const char * cmd, char * cmd_printout, size_t cmd_printout_size, const std::vector<std::pair<std::string,std::string> > & env_variables);


	/**
	 * This method executes a preprocessing operation on the provided command string.
	 * During that operation, for example, all the new line characters are removed.
	 *
	 * @param[in/out] string - The command string on which the preprocessing must be executed.
	 * @param[in]     string_size - The size of the command string.
	 */
	static void remove_new_line_characters (char * string, const size_t string_size);

	/**
	 * This method checks if the node identified by the provided name is reachable
	 * or not over an SSH connection on the node internal network.
	 *
	 * @param[in]  node_id - The identifier for the remote node.
	 * @return a boolean value indicating if the node can be reached over SSH.
	 */
	static bool is_node_reachable (const char * node_id);

	/**
	 * This method checks if the given directory entry is a regular file and
	 * if its name matches the HCF output file format.
	 *
	 * @param[in]  entry - The directory entry to be checked.
	 * @return true if the file is a regular file and if matches the pattern.
	 */
	static inline bool is_valid_output_file (struct dirent * entry) {
		if (entry->d_type != DT_REG)	return false;
		return !extract_timestamp_from_filename(entry->d_name).empty();
	}

	/**
	 * This method extracts the timestamp information from the file name.
	 *
	 * @param[in]  filename - The name of the file from which the timestamp must be extracted.
	 * @return the timestamp information, empty string otherwise.
	 */
	static const std::string extract_timestamp_from_filename (const std::string & filename);

	//===========//
	// Operators //
	//===========//
private:
	__TSL_CLASS_NAME__ & operator=(const __TSL_CLASS_NAME__ & rhs);

	//========//
	// Fields //
	//========//
private:
	static const int HCF_INTERNAL_NETWORK_SSH_PORT_NUMBER    = 1022;
	static const int HCF_INTERNAL_NETWORK_CONNECTION_TIMEOUT = 5 * 1000;

	int _event_fd;
	std::queue<job_to_execute> _jobs;
	TSL_NS_HCF_CORE::thread_mutex_recursive _jobs_mutex;
	pid_t _child_process_pid;
};

HCF_NS_HCF_SRV_END

#endif /* __INCLUDE_GUARD_CLASS_SRV_JOB_EXECUTOR__ */
