#ifndef __INCLUDE_GUARD_CLASS_CORE_LOGGER__
#define __INCLUDE_GUARD_CLASS_CORE_LOGGER__ logger

/*
 *	@file logger.h
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

#include "macros.h"
#include "logger_macros.h"
#include "logger_impl.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_CORE_LOGGER__

TSL_NS_HCF_CORE_BEGIN

class __TSL_CLASS_NAME__ {
	//==============//
	// Constructors //
	//==============//
private:
	TSL_CTOR ();
	TSL_CTOR (const __TSL_CLASS_NAME__ & rhs);

	//============//
	// Destructor //
	//============//
public:
	virtual inline TSL_DTOR () {}

	//===========//
	// Functions //
	//===========//
public:
	/**
	 * Methods to be used to initialize and to finalize the logging framework.
	 * During initialization, the caller must provide an application name to be used to identity the logging user.
	 * During initialization, the caller can also provide an implementation to be used by the logging framework.
	 */
	static int initialize (const char * application_name, logger_impl * implementation = 0);
	static int finalize ();

	/**
	 * Methods to be used to get/set the default logging level.
	 * This value will be used in case no logging level is provided.
	 */
	static logging_level_t logging_level ();
	static void logging_level (logging_level_t level);

	/**
	 * Methods to be used to send log messages.
	 * The caller can provide the following information:
	 * 		1) Logging message      (MANDATORY)
	 * 		2) Logging level        (OPTIONAL)
	 * 		3) Source function name (OPTIONAL)
	 * 		4) Source line          (OPTIONAL)
	 */
	static inline int log (const char * message) {
		return logf(_logging_level, message);
	}
	static inline int log (logging_level_t level, const char * message) {
		return logf(level, message);
	}
	static inline int log (const char * source_func_name, int source_line, const char * message) {
		return logf_source(_logging_level, source_func_name, source_line, message);
	}
	static inline int log (logging_level_t level, const char * source_func_name, int source_line, const char * message) {
		return logf_source(level, source_func_name, source_line, message);
	}

	/**
	 * Methods to be used to send log messages (with errno logging).
	 * The caller can provide the following information:
	 * 		1) System error (errno) (MANDATORY)
	 * 		2) Logging message      (MANDATORY)
	 * 		3) Logging level        (OPTIONAL)
	 * 		4) Source function name (OPTIONAL)
	 * 		5) Source line          (OPTIONAL)
	 */
	static inline int log_errno (int sys_errno, const char * message) {
		return logf_errno(sys_errno, _logging_level, message);
	}
	static inline int log_errno (int sys_errno, logging_level_t level, const char * message) {
		return logf_errno(sys_errno, level, message);
	}
	static inline int log_errno (int sys_errno, const char * source_func_name, int source_line, const char * message) {
		return logf_errno_source(sys_errno, _logging_level, source_func_name, source_line, message);
	}
	static inline int log_errno (int sys_errno, logging_level_t level, const char * source_func_name, int source_line, const char * message) {
		return logf_errno_source(sys_errno, level, source_func_name, source_line, message);
	}

	/**
	 * Methods to be used to send log messages that will be also logged into Syslog log file.
	 * The caller can provide the following information:
	 * 		1) Syslog priority      (MANDATORY)
	 * 		2) Logging message      (MANDATORY)
	 * 		3) Logging level        (OPTIONAL)
	 * 		4) Source function name (OPTIONAL)
	 * 		5) Source line          (OPTIONAL)
	 */
	static inline int syslog (int syslog_priority, const char * message) {
		return syslogf(syslog_priority, _logging_level, message);
	}
	static inline int syslog (int syslog_priority, logging_level_t level, const char * message) {
		return syslogf(syslog_priority, level, message);
	}
	static inline int syslog (int syslog_priority, const char * source_func_name, int source_line, const char * message) {
		return syslogf_source(syslog_priority, _logging_level, source_func_name, source_line, message);
	}
	static inline int syslog (int syslog_priority, logging_level_t level, const char * source_func_name, int source_line, const char * message) {
		return syslogf_source(syslog_priority, level, source_func_name, source_line, message);
	}

	/**
	 * Methods to be used to send log messages that will be also logged into Syslog log file (with errno logging).
	 * The caller can provide the following information:
	 * 		1) System error (errno) (MANDATORY)
	 * 		2) Syslog priority      (MANDATORY)
	 * 		3) Logging message      (MANDATORY)
	 * 		4) Logging level        (OPTIONAL)
	 * 		5) Source function name (OPTIONAL)
	 * 		6) Source line          (OPTIONAL)
	 */
	static inline int syslog_errno (int sys_errno, int syslog_priority, const char * message) {
		return syslogf_errno(sys_errno, syslog_priority, _logging_level, message);
	}
	static inline int syslog_errno (int sys_errno, int syslog_priority, logging_level_t level, const char * message) {
		return syslogf_errno(sys_errno, syslog_priority, level, message);
	}
	static inline int syslog_errno (int sys_errno, int syslog_priority, const char * source_func_name, int source_line, const char * message) {
		return syslogf_errno_source(sys_errno, syslog_priority, _logging_level, source_func_name, source_line, message);
	}
	static inline int syslog_errno (int sys_errno, int syslog_priority, logging_level_t level, const char * source_func_name, int source_line, const char * message) {
		return syslogf_errno_source(sys_errno, syslog_priority, level, source_func_name, source_line, message);
	}

	/**
	 * Method to be used to log an hexadecimal buffer, dumping it into a predefined number of lines and characters.
	 * The caller must provide the buffer to be dumped, its size and the total number of characters to be dumped.
	 * Optionally, it is possible to provide also the number of characters to be dumped into a single line.
	 * If not specified, the default value will be used.
	 */
	static void dump (uint8_t * buffer, size_t size, size_t dumping_size, size_t dumping_line_length = HCF_CC_DEFAULT_DUMP_LINE_LENGTH);

	/**
	 * Methods to be used to send log messages that will be also logged into Syslog log file (with errno logging).
	 * Those methods accept a format string, as done by printf() function of C standard library.
	 * The caller can provide the following information:
	 * 		1) System error (errno) (MANDATORY)
	 * 		2) Syslog priority      (MANDATORY)
	 * 		3) Logging message      (MANDATORY)
	 * 		4) Logging level        (OPTIONAL)
	 * 		5) Source function name (OPTIONAL)
	 * 		6) Source line          (OPTIONAL)
	 */
	static int syslogf_errno (int sys_errno, int syslog_priority,	logging_level_t level, const char * format, ...) __attribute__ ((format (printf, 4, 5)));
	static int syslogf_errno (int sys_errno, int syslog_priority, const char * format, ...) __attribute__ ((format (printf, 3, 4)));
	static int syslogf_errno_source (int sys_errno, int syslog_priority, logging_level_t level, const char * source_func_name, int source_line, const char * format, ...) __attribute__ ((format (printf, 6, 7)));
	static int syslogf_errno_source (int sys_errno, int syslog_priority, const char * source_func_name, int source_line, const char * format, ...) __attribute__ ((format (printf, 5, 6)));

	/**
	 * Methods to be used to send log messages that will be also logged into Syslog log file.
	 * Those methods accept a format string, as done by printf() function of C standard library.
	 * The caller can provide the following information:
	 * 		1) Syslog priority      (MANDATORY)
	 * 		2) Logging message      (MANDATORY)
	 * 		3) Logging level        (OPTIONAL)
	 * 		4) Source function name (OPTIONAL)
	 * 		5) Source line          (OPTIONAL)
	 */
	static int syslogf (int syslog_priority, logging_level_t level, const char * format, ...) __attribute__ ((format (printf, 3, 4)));
	static int syslogf (int syslog_priority, const char * format, ...) __attribute__ ((format (printf, 2, 3)));
	static int syslogf_source (int syslog_priority, logging_level_t level,	const char * source_func_name, int source_line, const char * format, ...) __attribute__ ((format (printf, 5, 6)));
	static int syslogf_source (int syslog_priority, const char * source_func_name, int source_line, const char * format, ...) __attribute__ ((format (printf, 4, 5)));

	/**
	 * Methods to be used to send log messages.
	 * Those methods accept a format string, as done by printf() function of C standard library.
	 * The caller can provide the following information:
	 * 		1) Logging message      (MANDATORY)
	 * 		2) Logging level        (OPTIONAL)
	 * 		3) Source function name (OPTIONAL)
	 * 		4) Source line          (OPTIONAL)
	 */
	static int logf (logging_level_t level, const char * format, ...) __attribute__ ((format (printf, 2, 3)));
	static int logf (const char * format, ...) __attribute__ ((format (printf, 1, 2)));
	static int logf_source (logging_level_t level, const char * source_func_name, int source_line, const char * format, ...) __attribute__ ((format (printf, 4, 5)));
	static int logf_source (const char * source_func_name, int source_line, const char * format, ...) __attribute__ ((format (printf, 3, 4)));

	/**
	 * Methods to be used to send log messages (with errno logging).
	 * Those methods accept a format string, as done by printf() function of C standard library.
	 * The caller can provide the following information:
	 * 		1) System error (errno) (MANDATORY)
	 * 		2) Logging message      (MANDATORY)
	 * 		3) Logging level        (OPTIONAL)
	 * 		4) Source function name (OPTIONAL)
	 * 		5) Source line          (OPTIONAL)
	 */
	static int logf_errno (int sys_errno, logging_level_t level, const char * format, ...) __attribute__ ((format (printf, 3, 4)));
	static int logf_errno (int sys_errno, const char * format, ...) __attribute__ ((format (printf, 2, 3)));
	static int logf_errno_source (int sys_errno, logging_level_t level, const char * source_func_name, int source_line, const char * format, ...) __attribute__ ((format (printf, 5, 6)));
	static int logf_errno_source (int sys_errno, const char * source_func_name,	int source_line, const char * format, ...) __attribute__ ((format (printf, 4, 5)));

	//===========//
	// Operators //
	//===========//
private:
	__TSL_CLASS_NAME__ & operator=(const __TSL_CLASS_NAME__ & rhs);

	//========//
	// Fields //
	//========//
private:
	static logger_impl * _impl;
	static bool _delete_impl;
	static logging_level_t _logging_level;
};

TSL_NS_HCF_CORE_END

#endif /* __INCLUDE_GUARD_CLASS_CORE_LOGGER__ */
