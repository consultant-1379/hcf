#ifndef __INCLUDE_GUARD_CLASS_CORE_LOGGER_IMPL__
#define __INCLUDE_GUARD_CLASS_CORE_LOGGER_IMPL__ logger_impl

/*
 *	@file logger_impl
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

#include <stdint.h>

#include "logger_macros.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_CORE_LOGGER_IMPL__

TSL_NS_HCF_CORE_BEGIN

class __TSL_CLASS_NAME__ {
	//===========//
	// Constants //
	//===========//
protected:
	enum {
		SYSLOG_DISABLED_MASK	=	99999,
		ERRNO_DISABLED_MASK		=	-99999
	};

	//==============//
	// Constructors //
	//==============//
public:
	inline TSL_CTOR (logging_level_t logging_level)
		TSL_CTOR_INIT_LIST(_logging_level(logging_level)) {}

private:
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
	 * Methods to be used to get/set the default logging level.
	 * This value will be used in case no logging level is provided.
	 */
	inline logging_level_t logging_level () { return _logging_level; }
	inline void logging_level (logging_level_t level) { _logging_level = level; }

	/**
	 * Methods to be used to send log messages that will be also logged into Syslog log file (with errno logging).
	 * Those methods accept a format string and a variable argument list, as done by vprintf() function of C standard library.
	 * The caller can provide the following information:
	 * 		1) System error (errno) (MANDATORY)
	 * 		2) Syslog priority      (MANDATORY)
	 * 		3) Logging message      (MANDATORY)
	 * 		4) Logging level        (OPTIONAL)
	 * 		5) Source function name (OPTIONAL)
	 * 		6) Source line          (OPTIONAL)
	 */
	inline int syslogf_errno (int sys_errno, int syslog_priority,	logging_level_t level, const char * format, va_list argp) {
		return vlogf(sys_errno, syslog_priority, level, format, argp);
	}
	inline int syslogf_errno (int sys_errno, int syslog_priority, const char * format, va_list argp) {
		return vlogf(sys_errno, syslog_priority, _logging_level, format, argp);
	}
	inline int syslogf_errno_source (int sys_errno, int syslog_priority, logging_level_t level, const char * source_func_name, int source_line, const char * format, va_list argp) {
		return vlogf_source(sys_errno, syslog_priority, level, source_func_name, source_line, format, argp);
	}
	inline int syslogf_errno_source (int sys_errno, int syslog_priority, const char * source_func_name, int source_line, const char * format, va_list argp) {
		return vlogf_source(sys_errno, syslog_priority, _logging_level, source_func_name, source_line, format, argp);
	}

	/**
	 * Methods to be used to send log messages that will be also logged into Syslog log file.
	 * Those methods accept a format string and a variable argument list, as done by vprintf() function of C standard library.
	 * The caller can provide the following information:
	 * 		1) Syslog priority      (MANDATORY)
	 * 		2) Logging message      (MANDATORY)
	 * 		3) Logging level        (OPTIONAL)
	 * 		4) Source function name (OPTIONAL)
	 * 		5) Source line          (OPTIONAL)
	 */
	inline int syslogf (int syslog_priority, logging_level_t level, const char * format, va_list argp) {
		return vlogf(ERRNO_DISABLED_MASK, syslog_priority, level, format, argp);
	}
	inline int syslogf (int syslog_priority, const char * format, va_list argp) {
		return vlogf(ERRNO_DISABLED_MASK, syslog_priority, _logging_level, format, argp);
	}
	inline int syslogf_source (int syslog_priority, logging_level_t level,	const char * source_func_name, int source_line, const char * format, va_list argp) {
		return vlogf_source(ERRNO_DISABLED_MASK, syslog_priority, level, source_func_name, source_line, format, argp);
	}
	inline int syslogf_source (int syslog_priority, const char * source_func_name, int source_line, const char * format, va_list argp) {
		return vlogf_source(ERRNO_DISABLED_MASK, syslog_priority, _logging_level, source_func_name, source_line, format, argp);
	}

	/**
	 * Methods to be used to send log messages.
	 * Those methods accept a format string and a variable argument list, as done by vprintf() function of C standard library.
	 * The caller can provide the following information:
	 * 		1) Logging message      (MANDATORY)
	 * 		2) Logging level        (OPTIONAL)
	 * 		3) Source function name (OPTIONAL)
	 * 		4) Source line          (OPTIONAL)
	 */
	inline int logf (logging_level_t level, const char * format, va_list argp) {
		return vlogf(ERRNO_DISABLED_MASK, SYSLOG_DISABLED_MASK, level, format, argp);
	}
	inline int logf (const char * format, va_list argp) {
		return vlogf(ERRNO_DISABLED_MASK, SYSLOG_DISABLED_MASK, _logging_level, format, argp);
	}
	inline int logf_source (logging_level_t level, const char * source_func_name, int source_line, const char * format, va_list argp) {
		return vlogf_source(ERRNO_DISABLED_MASK, SYSLOG_DISABLED_MASK, level, source_func_name, source_line, format, argp);
	}
	inline int logf_source (const char * source_func_name, int source_line, const char * format, va_list argp) {
		return vlogf_source(ERRNO_DISABLED_MASK, SYSLOG_DISABLED_MASK, _logging_level, source_func_name, source_line, format, argp);
	}

	/**
	 * Methods to be used to send log messages (with errno logging).
	 * Those methods accept a format string and a variable argument list, as done by vprintf() function of C standard library.
	 * The caller can provide the following information:
	 * 		1) System error (errno) (MANDATORY)
	 * 		2) Logging message      (MANDATORY)
	 * 		3) Logging level        (OPTIONAL)
	 * 		4) Source function name (OPTIONAL)
	 * 		5) Source line          (OPTIONAL)
	 */
	inline int logf_errno (int sys_errno, logging_level_t level, const char * format, va_list argp) {
		return vlogf(sys_errno, SYSLOG_DISABLED_MASK, level, format, argp);
	}
	inline int logf_errno (int sys_errno, const char * format, va_list argp) {
		return vlogf(sys_errno, SYSLOG_DISABLED_MASK, _logging_level, format, argp);
	}
	inline int logf_errno_source (int sys_errno, logging_level_t level, const char * source_func_name, int source_line, const char * format, va_list argp) {
		return vlogf_source(sys_errno, SYSLOG_DISABLED_MASK, level, source_func_name, source_line, format, argp);
	}
	inline int logf_errno_source (int sys_errno, const char * source_func_name,	int source_line, const char * format, va_list argp) {
		return vlogf_source(sys_errno, SYSLOG_DISABLED_MASK, _logging_level, source_func_name, source_line, format, argp);
	}

	//==============================//
	// BEGIN: logger_impl interface //
	//==============================//
public:
	/**
	 * Methods to be used to initialize and to finalize the particular implementation of the logging framework.
	 * During initialization, the caller must provide an application name to be used to identity the logging user.
	 */
	virtual int initialize (const char * application_name) = 0;
	virtual int finalize () = 0;

	/**
	 * Method to be used to log an hexadecimal buffer, dumping it into a predefined number of lines and characters.
	 * The caller must provide the buffer to be dumped, its size and the total number of characters to be dumped.
	 * Optionally, it is possible to provide also the number of characters to be dumped into a single line.
	 * If not specified, the default value will be used.
	 */
	virtual void dump (uint8_t * buffer, size_t size, size_t dumping_size, size_t dumping_line_length = HCF_CC_DEFAULT_DUMP_LINE_LENGTH) = 0;

protected:
	/**
	 * Methods to be used to produce the logging buffers and to send them towards the final destination.
	 */
	virtual int vlogf (int sys_errno, int syslog_priority, logging_level_t level, const char * format, va_list argp) = 0;
	virtual int vlogf_source (int sys_errno, int syslog_priority, logging_level_t level, const char * source_func_name, int source_line, const char * format, va_list argp) = 0;
	//==============================//
	// END:   logger_impl interface //
	//==============================//

	//===========//
	// Operators //
	//===========//
private:
	__TSL_CLASS_NAME__ & operator=(const __TSL_CLASS_NAME__ & rhs);

	//========//
	// Fields //
	//========//
protected:
	logging_level_t _logging_level;
};

TSL_NS_HCF_CORE_END

#endif /* __INCLUDE_GUARD_CLASS_CORE_LOGGER_IMPL__ */
