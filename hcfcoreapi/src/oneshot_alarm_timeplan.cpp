#include "hcf/core/utils.h"

#include "hcf/trace/com_ericsson_common_hcf_core_oneshotalarmtimeplan.h"
#include "hcf/trace/tracing_macros.h"

#include "hcf/core/oneshot_alarm_timeplan.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_CORE_ONESHOT_ALARM_TIMEPLAN__

TSL_NS_HCF_CORE_BEGIN

HCF_CC_FUNCTION_TRACER_CLASS_DEFINE

int __TSL_CLASS_NAME__::get_next_timer_point (::timeval * relative_value, const ::timeval * tv_from) {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(GetNextTimerPoint, "The next available timer point for this policy is:", utils::calculate_time_string(&_tm).c_str());
	_tm.tm_isdst = -1;

	::timeval my_tv;
	my_tv.tv_sec = ::mktime(&_tm);
	my_tv.tv_usec = 0;

	if (timercmp(&my_tv, tv_from, <)) { // WARNING: My time is over
		HCF_CC_TRACE(GetNextTimerPoint, "The provided starting point is after the policy one!", utils::calculate_time_string(tv_from).c_str());
		return ERR_NOT_FOUND;
	}

	timersub(&my_tv, tv_from, relative_value);

	return ERR_NO_ERROR;
}

TSL_NS_HCF_CORE_END
