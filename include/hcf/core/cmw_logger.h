#ifndef __INCLUDE_GUARD_CLASS_CORE_CMW_LOGGER__
#define __INCLUDE_GUARD_CLASS_CORE_CMW_LOGGER__ cmw_logger

/*
 *	@file cmw_logger.h
 *	@brief
 *	@author xcasale (Alessio Cascone)
 *	@date 2015-03-12
 *
 *	COPYRIGHT Ericsson AB, 2015
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
 *	| 2015-001 | 2015-03-12 | xcasale      | Creation and first revision.        |
 *	+==========+============+==============+=====================================+
 */

#include <saLog.h>

#include "logger_impl.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_CORE_CMW_LOGGER__

TSL_NS_HCF_CORE_BEGIN

class __TSL_CLASS_NAME__ : public logger_impl {
	//==============//
	// Constructors //
	//==============//
public:
	inline TSL_CTOR (logging_level_t logging_level)
		TSL_CTOR_INIT_LIST(logger_impl(logging_level), _initialized(false),
				_application_name(), _log_handle(0), _version(), _log_stream_handle(0)) {}

private:
	TSL_CTOR (const __TSL_CLASS_NAME__ & rhs);

	//============//
	// Destructor //
	//============//
public:
	virtual inline TSL_DTOR () {}

	//======================================//
	// logger_impl interface implementation //
	//======================================//
public:
	/**
	 * This method initializes the implementation of the logging framework based on the Log Service provided by SAF.
	 *
	 * @param[in] application_name - The identified to be used to open the SAF logging
	 * @return 0 on success, a not zero value otherwise.
	 */
	virtual int initialize (const char * application_name);

	/**
	 * This method finalizes the implementation of the logging framework based on the Log Service provided by SAF.
	 *
	 * @return 0 on success, a not zero value otherwise.
	 */
	virtual int finalize ();

	/**
	 * Method to be used to log an hexadecimal buffer, dumping it into a predefined number of lines and characters.
	 * Once the buffer has been dumped, it is sent into SAF log file.
	 *
	 * @param[in] buffer - A pointer to the buffer to be dumped.
	 * @param[in] size - The size of the input buffer.
	 * @param[in] dumping_size - The maximum number of characters to be dumped.
	 * @param[in] dumping_line_length - The number of characters into each line.
	 */
	virtual void dump (uint8_t * buffer, size_t size, size_t dumping_size, size_t dumping_line_length = HCF_CC_DEFAULT_DUMP_LINE_LENGTH);

protected:
	/**
	 * Method to be used to log build and send towards the SAF log file a log line.
	 * It is used by all the high level methods that don't need to log information about the source code.
	 *
	 * @param[in] sys_errno - The value of errno variable.
	 * @param[in] syslog_priority - The priority level to be used to send the log message to Syslog.
	 * @param[in] level - The logging level to be used to send the log message to SAF log file.
	 * @param[in] format - The format string to be filled with the variable list of parameters.
	 * @param[in] argp - The variable list of arguments to be used to complete the format string.
	 * @return 0 on success, a not zero value otherwise.
	 */
	virtual int vlogf (int sys_errno, int syslog_priority, logging_level_t level, const char * format, va_list argp);

	/**
	 * Method to be used to log build and send towards the SAF log file a log line.
	 * It is used by all the high level methods that need to log information about the source code.
	 *
	 * @param[in] sys_errno - The value of errno variable.
	 * @param[in] syslog_priority - The priority level to be used to send the log message to Syslog.
	 * @param[in] level - The logging level to be used to send the log message to SAF log file.
	 * @param[in] source_func_name - The name of the function where the logging method has been called.
	 * @param[in] source_line - The line of the function where the logging method has been called.
	 * @param[in] format - The format string to be filled with the variable list of parameters.
	 * @param[in] argp - The variable list of arguments to be used to complete the format string.
	 * @return 0 on success, a not zero value otherwise.
	 */
	virtual int vlogf_source (int sys_errno, int syslog_priority, logging_level_t level, const char * source_func_name, int source_line, const char * format, va_list argp);

private:
	/**
	 * Method to be used to send a pre-formatted log buffer towards the SAF log service.
	 *
	 * @param[in] level - The logging level to be used to send the log message to SAF log file.
	 * @param[in] buffer - The pre-formatted log buffer.
	 * @return 0 on success, a not zero value otherwise.
	 */
	int send_log_buffer (logging_level_t level, const char * buffer);

	//===========//
	// Operators //
	//===========//
private:
	__TSL_CLASS_NAME__ & operator=(const __TSL_CLASS_NAME__ & rhs);

	//========//
	// Fields //
	//========//
private:
	static SaTimeT CMW_LOGGER_TIMEOUT_DEFAULT_VALUE;

	bool _initialized;
	SaNameT _application_name;
	SaLogHandleT _log_handle;
	SaVersionT _version;
	SaLogStreamHandleT _log_stream_handle;
};

TSL_NS_HCF_CORE_END

#endif /* __INCLUDE_GUARD_CLASS_CORE_CMW_LOGGER__ */
