#include <stdio.h>
#include <syslog.h>
#include <string.h>
#include <saAis.h>

#include "hcf/core/macros.h"
#include "hcf/core/error_constants.h"
#include "hcf/core/cmw_logger.h"

#include "hcf/trace/com_ericsson_common_hcf_core_cmwlogger.h"
#include "hcf/trace/tracing_macros.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_CORE_CMW_LOGGER__

TSL_NS_HCF_CORE_BEGIN

HCF_CC_FUNCTION_TRACER_CLASS_DEFINE

// Static fields initialization
SaTimeT __TSL_CLASS_NAME__::CMW_LOGGER_TIMEOUT_DEFAULT_VALUE = SA_TIME_ONE_SECOND * 3;

int __TSL_CLASS_NAME__::initialize (const char * application_name) {
	HCF_CC_TRACE_FUNCTION;

	if (_initialized) {
		HCF_CC_TRACE(Initialize, "The CMW Logger object was already initialized: nothing to do!",
				reinterpret_cast<const char *>(_application_name.value), SA_LOG_STREAM_SYSTEM);
		return ERR_NO_ERROR;
	}

	// Save the application name
	::strncpy((char *)_application_name.value, application_name, SA_MAX_NAME_LENGTH);
	_application_name.length = ::strlen((char *)_application_name.value);

	// Set the SAF Log service version to A.2.1
	_version.releaseCode  = 'A';
	_version.majorVersion = 0x02;
	_version.minorVersion = 0x01;

	// Initialize the SAF log service
	SaAisErrorT call_result = SA_AIS_OK;
	HCF_CC_TRACE(Initialize, "Initializing the CMW logger service!", application_name, SA_LOG_STREAM_SYSTEM);
	if ((call_result = saLogInitialize(&_log_handle, 0, &_version)) != SA_AIS_OK) {
		::syslog(LOG_ERR, "%s: Call 'saLogInitialize' failed: call_result == %d", __PRETTY_FUNCTION__, call_result);
		HCF_CC_TRACE(InitializeErr, "Call 'saLogInitialize' failed.", call_result);
		return ERR_LOGGER_INITIALIZE_FAILED;
	}

	// Build the name of the default system logging stream
	SaNameT logging_stream;
	::strncpy((char *)logging_stream.value, SA_LOG_STREAM_SYSTEM, SA_MAX_NAME_LENGTH);
	logging_stream.length = ::strlen((char *)logging_stream.value);

	// Open a logging stream using the provided name
	HCF_CC_TRACE(Initialize, "Opening the CMW logger stream!", application_name, SA_LOG_STREAM_SYSTEM);
	if ((call_result = saLogStreamOpen_2(_log_handle, &logging_stream, 0, 0, CMW_LOGGER_TIMEOUT_DEFAULT_VALUE, &_log_stream_handle)) != SA_AIS_OK) {
		::syslog(LOG_ERR, "%s: Call 'saLogStreamOpen_2' failed: call_result == %d", __PRETTY_FUNCTION__, call_result);
		HCF_CC_TRACE(InitializeErr, "Call 'saLogStreamOpen_2' failed: finalizing the log handle.", call_result);
		// Before returning error, finalize the log handle
		saLogFinalize(_log_handle);
		return ERR_LOGGER_INITIALIZE_FAILED;
	}

	HCF_CC_TRACE(Initialize, "The CMW logger service is now initialized!", application_name, SA_LOG_STREAM_SYSTEM);
	_initialized = true;
	return ERR_NO_ERROR;
}

int __TSL_CLASS_NAME__::finalize () {
	HCF_CC_TRACE_FUNCTION;

	if (!_initialized) {
		HCF_CC_TRACE(Finalize, "The CMW Logger object was already finalized: nothing to do!", reinterpret_cast<const char *>(_application_name.value));
		return ERR_NO_ERROR;
	}

	// FIRST: Finalize the logging stream handle
	HCF_CC_TRACE(Finalize, "Finalizing the CMW logger stream!", reinterpret_cast<const char *>(_application_name.value));
	SaAisErrorT call_result = SA_AIS_OK;
	if ((call_result = saLogStreamClose(_log_stream_handle)) != SA_AIS_OK) {
		::syslog(LOG_ERR, "%s: Call 'saLogStreamClose' failed: call_result == %d", __PRETTY_FUNCTION__, call_result);
		HCF_CC_TRACE(FinalizeErr, "Call 'saLogStreamClose' failed.", call_result);
	}

	// SECOND: Finalize the logging handle
	HCF_CC_TRACE(Finalize, "Finalizing the CMW logger service!", reinterpret_cast<const char *>(_application_name.value));
	if ((call_result = saLogFinalize(_log_handle)) != SA_AIS_OK) {
		::syslog(LOG_ERR, "%s: Call 'saLogFinalize' failed: call_result == %d", __PRETTY_FUNCTION__, call_result);
		HCF_CC_TRACE(FinalizeErr, "Call 'saLogFinalize' failed.", call_result);
	}

	return ERR_NO_ERROR;
}

void __TSL_CLASS_NAME__::dump (uint8_t * buffer, size_t size, size_t dumping_size, size_t dumping_line_length) {
	HCF_CC_TRACE_FUNCTION;

	size_t output_buffer_size = 2 + 8 + 2 + 3 * dumping_line_length + 16;
	char output_buffer[output_buffer_size];

	for (size_t i = 0; (i < size) && (i < dumping_size); ) {
		int chars = ::snprintf(output_buffer, output_buffer_size, "  %08zX:", i);
		for (size_t col = 0; (col < dumping_line_length) && (i < size) && (i < dumping_size); ++i, ++col)
			chars += ::snprintf(output_buffer + chars, output_buffer_size - chars, " %02X", buffer[i]);
		if (_initialized)	send_log_buffer(LEVEL_INFO, output_buffer);
	}
}

int __TSL_CLASS_NAME__::vlogf (int sys_errno, int syslog_priority, logging_level_t level, const char * format, va_list argp) {
	HCF_CC_TRACE_FUNCTION;

	if (!(_initialized) && (syslog_priority == SYSLOG_DISABLED_MASK))	return ERR_NO_ERROR;

	char buffer[8 * 1024];
	int char_count = 0;

	char_count = ::vsnprintf(buffer, TSL_ARRAY_SIZE(buffer), format, argp);

	if (sys_errno ^ ERRNO_DISABLED_MASK) { // Add system errno information to the log message
		char errno_buf[1024];
		::snprintf(buffer + char_count, TSL_ARRAY_SIZE(buffer) - char_count, " [errno == %d, errno_text == '%s']",
				sys_errno, ::strerror_r(sys_errno, errno_buf, TSL_ARRAY_SIZE(errno_buf)));
	}

	if (syslog_priority ^ SYSLOG_DISABLED_MASK) ::syslog(syslog_priority, buffer);

	return send_log_buffer(level, buffer);
}

int __TSL_CLASS_NAME__::vlogf_source (int sys_errno, int syslog_priority, logging_level_t level, const char * source_func_name, int source_line, const char * format, va_list argp) {
	HCF_CC_TRACE_FUNCTION;

	if (!(_initialized) && (syslog_priority == SYSLOG_DISABLED_MASK))	return ERR_NO_ERROR;

	char buffer[10 * 1024];
	int char_count = 0;

	char_count =  ::snprintf (buffer, TSL_ARRAY_SIZE(buffer), "[0x%08X] {%s@@%d} ", (pid_t) syscall (SYS_gettid), source_func_name, source_line);
	char_count += ::vsnprintf(buffer + char_count, TSL_ARRAY_SIZE(buffer) - char_count, format, argp);

	if (sys_errno ^ ERRNO_DISABLED_MASK) { // Add system errno information to the log message
		char errno_buf[1024];
		::snprintf(buffer + char_count, TSL_ARRAY_SIZE(buffer) - char_count, " [errno == %d, errno_text == '%s']",
				sys_errno, ::strerror_r(sys_errno, errno_buf, TSL_ARRAY_SIZE(errno_buf)));
	}

	if (syslog_priority ^ SYSLOG_DISABLED_MASK) ::syslog(syslog_priority, buffer);

	return send_log_buffer(level, buffer);
}

int __TSL_CLASS_NAME__::send_log_buffer (logging_level_t level, const char * buffer) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(SendLogBuffer, "Received a request to send a log buffer!", level, buffer);

	// FIRST: Prepare the log buffer to attach to the log record
	SaLogBufferT log_buffer;
	log_buffer.logBuf     = const_cast<SaUint8T *>(reinterpret_cast<const SaUint8T *>(buffer));
	log_buffer.logBufSize = ::strlen(buffer);

	// SECOND: Prepare the log record to be sent to the SAF log service
	SaLogRecordT log_record;
	log_record.logTimeStamp                             = SA_TIME_UNKNOWN;
	log_record.logHdrType                               = SA_LOG_GENERIC_HEADER;
	log_record.logHeader.genericHdr.logSeverity         = level;
	log_record.logHeader.genericHdr.logSvcUsrName       = &_application_name;
	log_record.logHeader.genericHdr.notificationClassId = 0;
	log_record.logBuffer                                = &log_buffer;

	// THIRD: Send the prepared record to the SAF log service
	SaAisErrorT call_result = SA_AIS_OK;
	HCF_CC_TRACE(SendLogBuffer, "Calling 'saLogWriteLogAsync' function to send the buffer to the CMW logger service!", level, "");
	if ((call_result = saLogWriteLogAsync(_log_stream_handle, 0, 0, &log_record)) != SA_AIS_OK) {
		::syslog(LOG_ERR, "%s: Call 'saLogWriteLogAsync' failed: call_result == %d", __PRETTY_FUNCTION__, call_result);
		HCF_CC_TRACE(SendLogBufferErr, "Call 'saLogWriteLogAsync' failed.", call_result);
		return ERR_SEND_LOG_BUFFER_FAILED;
	}

	return ERR_NO_ERROR;
}

TSL_NS_HCF_CORE_END
