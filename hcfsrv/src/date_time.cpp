#include "hcf/core/logger.h"

#include "hcf/trace/com_ericsson_common_hcf_srv_datetime.h"
#include "hcf/trace/tracing_macros.h"

#include "constants.h"
#include "date_time.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_SRV_DATE_TIME__

HCF_NS_HCF_SRV_BEGIN

HCF_CC_FUNCTION_TRACER_CLASS_DEFINE

// Static fields initialization
__TSL_CLASS_NAME__ __TSL_CLASS_NAME__::DATE_MAX(~0U, ~0U, ~0U, ~0U, ~0U, ~0U);

int __TSL_CLASS_NAME__::set_date (unsigned y, unsigned m, unsigned d) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(SetDate, "Received a request to set a new date value", y, m, d);

	// Check if the provided triple is a valid date
	if (const int call_result = check_date(y, m, d)) {
		HCF_CC_TRACE(SetDateErr, "Call 'check_date' failed: not valid date provided!", call_result);
		HCF_CC_LOG(LEVEL_ERROR, "Invalid date (%u/%u/%u) provided!", y, m, d);
		return ERR_BAD_VALUE_PROVIDED;
	}

	// The triple is valid, set the class fields
	HCF_CC_TRACE(SetDate, "The date values are valid, setting it in the internal fields", y, m, d);
	_year = y; _month = m; _day = d;
	return ERR_NO_ERROR;
}

int __TSL_CLASS_NAME__::set_time (unsigned h, unsigned m, unsigned s) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(SetTime, "Received a request to set a new time value", h, m, s);

	// Check if the provided triple is a valid time
	if (const int call_result = check_time(h, m, s)) {
		HCF_CC_TRACE(SetTimeErr, "Call 'check_time' failed: not valid time provided!", call_result);
		HCF_CC_LOG(LEVEL_ERROR, "Invalid time (%u:%u:%u) provided!", h, m, s);
		return ERR_BAD_VALUE_PROVIDED;
	}

	// The triple is valid, set the class fields
	HCF_CC_TRACE(SetTime, "The date values are valid, setting it in the internal fields", h, m, s);
	_hour = h; _min = m; _sec = s;
	return ERR_NO_ERROR;
}

int __TSL_CLASS_NAME__::set_date_time (unsigned y, unsigned mo, unsigned d, unsigned h, unsigned mi, unsigned s) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(SetDateTime, "Received a request to set a new date & time value", h, mo, s, h, mi, s);

	// Check if the provided values are valid date and time
	if ((check_date(y, mo, d)) || (check_time(h, mi, s))) {
		HCF_CC_TRACE(SetDateTimeErr, "Call 'check_date' or 'check_time' failed: not valid date/time provided!", ERR_BAD_VALUE_PROVIDED);
		HCF_CC_LOG(LEVEL_ERROR, "Invalid date-time (%u/%u/%u %u:%u:%u) provided!", y, mo, d, h, mi, s);
		return ERR_BAD_VALUE_PROVIDED;
	}

	// The values are valid, set the class fields
	HCF_CC_TRACE(SetDateTime, "The date & time values are valid, setting it in the internal fields", y, mo, d, h, mi, s);
	_year = y; _month = mo; _day = d;
	_hour = h; _min = mi; _sec = s;
	return ERR_NO_ERROR;
}

HCF_NS_HCF_SRV_END
