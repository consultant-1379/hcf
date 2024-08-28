#include <stdarg.h>
#include <new>

#include "hcf/core/logger.h"
#include "hcf/core/cmw_logger.h"
#include "hcf/core/error_constants.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_CORE_LOGGER__

TSL_NS_HCF_CORE_BEGIN

// Static fields initialization
logger_impl * __TSL_CLASS_NAME__::_impl;
bool __TSL_CLASS_NAME__::_delete_impl = false;
logging_level_t __TSL_CLASS_NAME__::_logging_level;

int __TSL_CLASS_NAME__::initialize (const char * application_name, logger_impl * implementation) {
	if (!(_impl = implementation)) {
		_delete_impl = (_impl = new (std::nothrow) cmw_logger(_logging_level));
	}
	return _impl->initialize(application_name);
}

int __TSL_CLASS_NAME__::finalize () {
	const int call_result = _impl->finalize();
	if (_delete_impl) {
		delete _impl; _impl = 0;
	}
	return call_result;
}

logging_level_t __TSL_CLASS_NAME__::logging_level () {
	if (!_impl)	return LEVEL_EMERGENCY;
	return _impl->logging_level();
}

void __TSL_CLASS_NAME__::logging_level (logging_level_t level) {
	if (!_impl)	return;
	_logging_level = level;
	_impl->logging_level(level);
}

void __TSL_CLASS_NAME__::dump (uint8_t * buffer, size_t size, size_t dumping_size, size_t dumping_line_length) {
	if (!_impl)	return;
	_impl->dump(buffer, size, dumping_size, dumping_line_length);
}

int __TSL_CLASS_NAME__::syslogf_errno (int sys_errno, int syslog_priority,	logging_level_t level, const char * format, ...) {
	if (!_impl)	return ERR_IMPLEMENTATION_NOT_FOUND;

	va_list argp;
	va_start(argp, format);
	const int call_result = _impl->syslogf_errno(sys_errno, syslog_priority, level, format, argp);
	va_end(argp);
	return call_result;
}

int __TSL_CLASS_NAME__::syslogf_errno (int sys_errno, int syslog_priority, const char * format, ...) {
	if (!_impl)	return ERR_IMPLEMENTATION_NOT_FOUND;

	va_list argp;
	va_start(argp, format);
	const int call_result = _impl->syslogf_errno(sys_errno, syslog_priority, format, argp);
	va_end(argp);
	return call_result;
}

int __TSL_CLASS_NAME__::syslogf_errno_source (int sys_errno, int syslog_priority, logging_level_t level, const char * source_func_name, int source_line, const char * format, ...) {
	if (!_impl)	return ERR_IMPLEMENTATION_NOT_FOUND;

	va_list argp;
	va_start(argp, format);
	const int call_result = _impl->syslogf_errno_source(sys_errno, syslog_priority, level, source_func_name, source_line, format, argp);
	va_end(argp);
	return call_result;
}

int __TSL_CLASS_NAME__::syslogf_errno_source (int sys_errno, int syslog_priority, const char * source_func_name, int source_line, const char * format, ...) {
	if (!_impl)	return ERR_IMPLEMENTATION_NOT_FOUND;

	va_list argp;
	va_start(argp, format);
	const int call_result = _impl->syslogf_errno_source(sys_errno, syslog_priority, source_func_name, source_line, format, argp);
	va_end(argp);
	return call_result;
}

int __TSL_CLASS_NAME__::syslogf (int syslog_priority, logging_level_t level, const char * format, ...) {
	if (!_impl)	return ERR_IMPLEMENTATION_NOT_FOUND;

	va_list argp;
	va_start(argp, format);
	const int call_result = _impl->syslogf(syslog_priority, level, format, argp);
	va_end(argp);
	return call_result;
}

int __TSL_CLASS_NAME__::syslogf (int syslog_priority, const char * format, ...) {
	if (!_impl)	return ERR_IMPLEMENTATION_NOT_FOUND;

	va_list argp;
	va_start(argp, format);
	const int call_result = _impl->syslogf(syslog_priority, format, argp);
	va_end(argp);
	return call_result;
}

int __TSL_CLASS_NAME__::syslogf_source (int syslog_priority, logging_level_t level,	const char * source_func_name, int source_line, const char * format, ...) {
	if (!_impl)	return ERR_IMPLEMENTATION_NOT_FOUND;

	va_list argp;
	va_start(argp, format);
	const int call_result = _impl->syslogf_source(syslog_priority, level, source_func_name, source_line, format, argp);
	va_end(argp);
	return call_result;
}

int __TSL_CLASS_NAME__::syslogf_source (int syslog_priority, const char * source_func_name, int source_line, const char * format, ...) {
	if (!_impl)	return ERR_IMPLEMENTATION_NOT_FOUND;

	va_list argp;
	va_start(argp, format);
	const int call_result = _impl->syslogf_source(syslog_priority, source_func_name, source_line, format, argp);
	va_end(argp);
	return call_result;
}

int __TSL_CLASS_NAME__::logf (logging_level_t level, const char * format, ...) {
	if (!_impl)	return ERR_IMPLEMENTATION_NOT_FOUND;

	va_list argp;
	va_start(argp, format);
	const int call_result = _impl->logf(level, format, argp);
	va_end(argp);
	return call_result;
}

int __TSL_CLASS_NAME__::logf (const char * format, ...) {
	if (!_impl)	return ERR_IMPLEMENTATION_NOT_FOUND;

	va_list argp;
	va_start(argp, format);
	const int call_result = _impl->logf(format, argp);
	va_end(argp);
	return call_result;
}

int __TSL_CLASS_NAME__::logf_source (logging_level_t level, const char * source_func_name, int source_line, const char * format, ...) {
	if (!_impl)	return ERR_IMPLEMENTATION_NOT_FOUND;

	va_list argp;
	va_start(argp, format);
	const int call_result = _impl->logf_source(level, source_func_name, source_line, format, argp);
	va_end(argp);
	return call_result;
}

int __TSL_CLASS_NAME__::logf_source (const char * source_func_name, int source_line, const char * format, ...) {
	if (!_impl)	return ERR_IMPLEMENTATION_NOT_FOUND;

	va_list argp;
	va_start(argp, format);
	const int call_result = _impl->logf_source(source_func_name, source_line, format, argp);
	va_end(argp);
	return call_result;
}

int __TSL_CLASS_NAME__::logf_errno (int sys_errno, logging_level_t level, const char * format, ...) {
	if (!_impl)	return ERR_IMPLEMENTATION_NOT_FOUND;

	va_list argp;
	va_start(argp, format);
	const int call_result = _impl->logf_errno(sys_errno, level, format, argp);
	va_end(argp);
	return call_result;
}

int __TSL_CLASS_NAME__::logf_errno (int sys_errno, const char * format, ...) {
	if (!_impl)	return ERR_IMPLEMENTATION_NOT_FOUND;

	va_list argp;
	va_start(argp, format);
	const int call_result = _impl->logf_errno(sys_errno, format, argp);
	va_end(argp);
	return call_result;
}

int __TSL_CLASS_NAME__::logf_errno_source (int sys_errno, logging_level_t level, const char * source_func_name, int source_line, const char * format, ...) {
	if (!_impl)	return ERR_IMPLEMENTATION_NOT_FOUND;

	va_list argp;
	va_start(argp, format);
	const int call_result = _impl->logf_errno_source(sys_errno, level, source_func_name, source_line, format, argp);
	va_end(argp);
	return call_result;
}

int __TSL_CLASS_NAME__::logf_errno_source (int sys_errno, const char * source_func_name,	int source_line, const char * format, ...) {
	if (!_impl)	return ERR_IMPLEMENTATION_NOT_FOUND;

	va_list argp;
	va_start(argp, format);
	const int call_result = _impl->logf_errno_source(sys_errno, source_func_name, source_line, format, argp);
	va_end(argp);
	return call_result;
}

TSL_NS_HCF_CORE_END
