#ifndef __INCLUDE_GUARD_FILE_CORE_LOGGER_MACROS_H__
#define __INCLUDE_GUARD_FILE_CORE_LOGGER_MACROS_H__

/*
 *	@file logger_macros.h
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

typedef enum {
	LEVEL_EMERGENCY  = 0
	, LEVEL_ALERT    = 1
	, LEVEL_CRITICAL = 2
	, LEVEL_ERROR    = 3
	, LEVEL_WARNING  = 4
	, LEVEL_NOTICE   = 5
	, LEVEL_INFO     = 6
} logging_level_t;

TSL_NS_HCF_CORE_BEGIN

#if !defined (HCF_CC_LOGGER_CLASS)
#	define HCF_CC_LOGGER_CLASS TSL_NS_HCF_CORE::logger

#	define HCF_CC_LOGGER_METHOD logf
#	define HCF_CC_LOGGER_ERRNO_METHOD logf_errno
#	define HCF_CC_SYSLOGGER_METHOD syslogf
#	define HCF_CC_SYSLOGGER_ERRNO_METHOD syslogf_errno

#	define HCF_CC_LOGGER_METHOD_SOURCE logf_source
#	define HCF_CC_LOGGER_ERRNO_METHOD_SOURCE logf_errno_source
#	define HCF_CC_SYSLOGGER_METHOD_SOURCE syslogf_source
#	define HCF_CC_SYSLOGGER_ERRNO_METHOD_SOURCE syslogf_errno_source

# define HCF_CC_DUMPER_METHOD dump
# define HCF_CC_DEFAULT_DUMP_LINE_LENGTH 16
# define HCF_CC_DEFAULT_DUMP_SIZE 128
#endif

#if !defined (HCF_CC_LOG_IMPL)
#	define HCF_CC_LOG_IMPL(level, ...) HCF_CC_LOGGER_CLASS::HCF_CC_LOGGER_METHOD(level, __VA_ARGS__)
#	define HCF_CC_LOG_ERRNO_IMPL(sys_errno, level, ...) \
		HCF_CC_LOGGER_CLASS::HCF_CC_LOGGER_ERRNO_METHOD(sys_errno, level, __VA_ARGS__)
#	define HCF_CC_SYSLOG_IMPL(syslog_priority, level, ...) \
		HCF_CC_LOGGER_CLASS::HCF_CC_SYSLOGGER_METHOD(syslog_priority, level, __VA_ARGS__)
#	define HCF_CC_SYSLOG_ERRNO_IMPL(sys_errno, syslog_priority, level, ...) \
		HCF_CC_LOGGER_CLASS::HCF_CC_SYSLOGGER_ERRNO_METHOD(sys_errno, syslog_priority, level, __VA_ARGS__)

#	define HCF_CC_LOG_SOURCE_IMPL(level, source, line, ...) \
		HCF_CC_LOGGER_CLASS::HCF_CC_LOGGER_METHOD_SOURCE(level, source, line, __VA_ARGS__)
#	define HCF_CC_LOG_SOURCE_ERRNO_IMPL(sys_errno, level, source, line, ...) \
		HCF_CC_LOGGER_CLASS::HCF_CC_LOGGER_ERRNO_METHOD_SOURCE(sys_errno, level, source, line, __VA_ARGS__)
#	define HCF_CC_SYSLOG_SOURCE_IMPL(syslog_priority, level, source, line, ...) \
		HCF_CC_LOGGER_CLASS::HCF_CC_SYSLOGGER_METHOD_SOURCE(syslog_priority, level, source, line, __VA_ARGS__)
#	define HCF_CC_SYSLOG_SOURCE_ERRNO_IMPL(sys_errno, syslog_priority, level, source, line, ...) \
		HCF_CC_LOGGER_CLASS::HCF_CC_SYSLOGGER_ERRNO_METHOD_SOURCE(sys_errno, syslog_priority, level, source, line, __VA_ARGS__)
#endif

inline int HCF_CC_LOG_NO_LOGGING () { return 0; }

#ifdef HCF_CC_HAS_LOGGING
#	ifdef HCF_CC_LOGGING_HAS_SOURCE_INFO
#		ifdef HCF_CC_LOGGING_USE_PRETTY_FUNCTION
#			define HCF_CC_LOG(level, ...) HCF_CC_LOG_SOURCE_IMPL(level, __PRETTY_FUNCTION__, __LINE__, __VA_ARGS__)
#			define HCF_CC_LOG_ERRNO(sys_errno, level, ...) HCF_CC_LOG_SOURCE_ERRNO_IMPL(sys_errno, level, __PRETTY_FUNCTION__, __LINE__, __VA_ARGS__)
#			define HCF_CC_SYSLOG(syslog_priority, level, ...) HCF_CC_SYSLOG_SOURCE_IMPL(syslog_priority, level, __PRETTY_FUNCTION__, __LINE__, __VA_ARGS__)
#			define HCF_CC_SYSLOG_ERRNO(sys_errno, syslog_priority, level, ...) HCF_CC_SYSLOG_SOURCE_ERRNO_IMPL(sys_errno, syslog_priority, level, __PRETTY_FUNCTION__, __LINE__, __VA_ARGS__)
#		else
#			define HCF_CC_LOG(level, ...) HCF_CC_LOG_SOURCE_IMPL(level, __func__, __LINE__, __VA_ARGS__)
#			define HCF_CC_LOG_ERRNO(sys_errno, level, ...) HCF_CC_LOG_SOURCE_ERRNO_IMPL(sys_errno, level, __func__, __LINE__, __VA_ARGS__)
#			define HCF_CC_SYSLOG(syslog_priority, level, ...) HCF_CC_SYSLOG_SOURCE_IMPL(syslog_priority, level, __func__, __LINE__, __VA_ARGS__)
#			define HCF_CC_SYSLOG_ERRNO(sys_errno, syslog_priority, level, ...) HCF_CC_SYSLOG_SOURCE_ERRNO_IMPL(sys_errno, syslog_priority, level, __func__, __LINE__, __VA_ARGS__)
#		endif // HCF_CC_LOGGING_USE_PRETTY_FUNCTION
#	else
#		define HCF_CC_LOG(level, ...) HCF_CC_LOG_IMPL(level, __VA_ARGS__)
#		define HCF_CC_LOG_ERRNO(sys_errno, level, ...) HCF_CC_LOG_ERRNO_IMPL(sys_errno, level, __VA_ARGS__)
#		define HCF_CC_SYSLOG(syslog_priority, level, ...) HCF_CC_SYSLOG_IMPL(syslog_priority, level, __VA_ARGS__)
#		define HCF_CC_SYSLOG_ERRNO(sys_errno, syslog_priority, level, ...) HCF_CC_SYSLOG_ERRNO_IMPL(sys_errno, syslog_priority, level, __VA_ARGS__)
#	endif //HCF_CC_LOGGING_HAS_SOURCE_INFO
#else
#	define HCF_CC_LOG(level, ...) HCF_CC_LOG_NO_LOGGING()
#	define HCF_CC_LOG_ERRNO(sys_errno, level, ...) HCF_CC_LOG_NO_LOGGING()
#	define HCF_CC_SYSLOG(syslog_priority, level, ...) HCF_CC_LOG_NO_LOGGING()
#	define HCF_CC_SYSLOG_ERRNO(sys_errno, syslog_priority, level, ...) HCF_CC_LOG_NO_LOGGING()
#endif //HCF_CC_HAS_LOGGING

#if defined (HCF_CC_HAS_DUMPING)
#	define HCF_CC_LOG_DUMP(buffer, size, ...) HCF_CC_LOGGER_CLASS::HCF_CC_DUMPER_METHOD(buffer, size, __VA_ARGS__)
#else
#	define HCF_CC_LOG_DUMP(buffer, size, ...) HCF_CC_LOG_NO_LOGGING()
#endif

TSL_NS_HCF_CORE_END

#endif /* __INCLUDE_GUARD_FILE_CORE_LOGGER_MACROS_H__ */
