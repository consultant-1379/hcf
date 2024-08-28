#include "hcf/core/utils.h"

#include "hcf/trace/com_ericsson_common_hcf_core_alarmtimeplan.h"
#include "hcf/trace/tracing_macros.h"

#include "hcf/core/alarm_timeplan.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_CORE_ALARM_TIMEPLAN__

TSL_NS_HCF_CORE_BEGIN

HCF_CC_FUNCTION_TRACER_CLASS_DEFINE

const unsigned __TSL_CLASS_NAME__::days_per_month [12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

int __TSL_CLASS_NAME__::get_next_datetime (::tm * tm_ptr, const ::timeval * tv_from) {
	HCF_CC_TRACE_FUNCTION;
	::timeval tv_next_relative;

	HCF_CC_TRACE(GetNextDateTime, "Calculating the next timer point starting from the following one:", utils::calculate_time_string(tv_from).c_str());
	if (const int call_result = get_next_timer_point(&tv_next_relative, tv_from)) {
		HCF_CC_TRACE(GetNextDateTimeErr, "Call 'get_next_timer_point' failed.", call_result);
		return call_result;
	}

	::timeval tv_next;

	timeradd(tv_from, &tv_next_relative, &tv_next);
	HCF_CC_TRACE(GetNextDateTime, "The next timer point is:", utils::calculate_time_string(&tv_next).c_str());

	time_t time_next = tv_next.tv_sec + ((tv_next.tv_usec >= 500000) ? 1 : 0);

	::localtime_r(&time_next, tm_ptr);
	HCF_CC_TRACE(GetNextDateTime, "The next available time is:", utils::calculate_time_string(tm_ptr).c_str());
	return ERR_NO_ERROR;
}

TSL_NS_HCF_CORE_END
