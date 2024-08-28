#include <stdlib.h>

#include "hcf/core/alarm_timeplan.h"
#include "hcf/core/logger.h"

#include "hcf/trace/com_ericsson_common_hcf_srv_jobschedulingpolicy.h"
#include "hcf/trace/tracing_macros.h"

#include "constants.h"
#include "job_scheduling_policy.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_SRV_JOB_SCHEDULING_POLICY__

HCF_NS_HCF_SRV_BEGIN

HCF_CC_FUNCTION_TRACER_CLASS_DEFINE

int __TSL_CLASS_NAME__::convert_datetime_into_iso8601_format (const date_time & d, std::string & date) {
	HCF_CC_TRACE_FUNCTION;

	// Create the ISO 8601 date string in a temporary buffer
	char tmp[64];
	::snprintf(tmp, TSL_ARRAY_SIZE(tmp), "%04u-%02u-%02uT%02u:%02u:%02u", d.year(), d.month(), d.day(), d.hour(), d.min(), d.sec());

	// Assign the built string to the method parameter
	date = tmp;
	HCF_CC_TRACE(ConvertDatetimeIntoIso8601Format, "The converted date value is:", date.c_str());
	return ERR_NO_ERROR;
}

int __TSL_CLASS_NAME__::convert_iso8601_format_into_datetime (const std::string & date, date_time & dt) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(ConvertIso8601IntoDatetimeFormat, "Received a request to convert into ISO 8601 format the following date:", date.c_str());
	int call_result = 0;
	const int num_of_arguments = 6;

	unsigned y, mo, d, h, mi, s;
	if ((call_result = ::sscanf(date.c_str(), "%04d-%02d-%02dT%02d:%02d:%02d", &y, &mo, &d, &h, &mi, &s)) != num_of_arguments) {
		HCF_CC_TRACE(ConvertIso8601IntoDatetimeFormatErr, "Call 'scanf' failed.", call_result, num_of_arguments);
		HCF_CC_LOG(LEVEL_ERROR, "A date value (%s) different from ISO 8601 format has been provided, call_result == %d", date.c_str(), call_result);
		return ERR_BAD_VALUE_PROVIDED;
	}

	HCF_CC_TRACE(ConvertIso8601IntoDatetimeFormat, "The following date has been correctly converted into ISO 8601 format!", date.c_str());
	return dt.set_date_time(y, mo, d, h, mi, s);
}

int __TSL_CLASS_NAME__::convert_time_string_into_datetime_format (const std::string & time, date_time & dt) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(ConvertTimeStringIntoDatetimeFormat, "Received a request to convert a date time format the following time:", time.c_str());
	int call_result = 0;
	const int num_of_arguments = 3;

	unsigned h, m, s;
	if ((call_result = ::sscanf(time.c_str(), "%02d:%02d:%02d", &h, &m, &s)) != num_of_arguments) {
		HCF_CC_TRACE(ConvertTimeStringIntoDatetimeFormatErr, "Call 'scanf' failed.", call_result, num_of_arguments);
		HCF_CC_LOG(LEVEL_ERROR, "Failed to convert the time string (%s), call_result == %d", time.c_str(), call_result);
		return ERR_BAD_VALUE_PROVIDED;
	}

	HCF_CC_TRACE(ConvertIso8601IntoDatetimeFormat, "The following time has been correctly converted into datetime object!", time.c_str());
	return dt.set_time(h, m ,s);
}

HCF_NS_HCF_SRV_END
