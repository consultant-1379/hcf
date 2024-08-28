#include "hcf/core/utils.h"

#include "hcf/trace/com_ericsson_common_hcf_core_periodicalarmtimeplan.h"
#include "hcf/trace/tracing_macros.h"

#include "hcf/core/periodic_alarm_timeplan.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_CORE_PERIODIC_ALARM_TIMEPLAN__

TSL_NS_HCF_CORE_BEGIN

HCF_CC_FUNCTION_TRACER_CLASS_DEFINE

int __TSL_CLASS_NAME__::get_next_timer_point (::timeval * relative_value, const ::timeval * tv_from) {
	HCF_CC_TRACE_FUNCTION;
	time_t time_now = ::time(0);
	::tm tm_current_point;
	::tm tm_end_point;

	::localtime_r(&time_now, &tm_current_point);
	tm_end_point = tm_current_point;

	tm_current_point.tm_year = _start_tm.tm_year;
	tm_current_point.tm_mon = _start_tm.tm_mon;
	tm_current_point.tm_mday = _start_tm.tm_mday;
	tm_current_point.tm_hour = _start_tm.tm_hour;
	tm_current_point.tm_min = _start_tm.tm_min;
	tm_current_point.tm_sec = _start_tm.tm_sec;

	tm_end_point.tm_year = _end_tm.tm_year;
	tm_end_point.tm_mon = _end_tm.tm_mon;
	tm_end_point.tm_mday = _end_tm.tm_mday;
	tm_end_point.tm_hour = _end_tm.tm_hour;
	tm_end_point.tm_min = _end_tm.tm_min;
	tm_end_point.tm_sec = _end_tm.tm_sec;

	::timeval tv_end_point;
	tv_end_point.tv_sec = ::mktime(&tm_end_point); tv_end_point.tv_usec = 0; tm_end_point.tm_isdst = -1;

	::timeval tv_current_point;
	tv_current_point.tv_sec = ::mktime(&tm_current_point); tv_current_point.tv_usec = 0; tm_current_point.tm_isdst = -1;

	HCF_CC_TRACE(GetNextTimerPoint, "Starting to calculate the next timer point starting from the following time:",
			utils::calculate_time_string(tv_from).c_str(), _t_months, _t_weeks, _t_days, _t_hours, _t_minutes);

	if (_t_months || _t_weeks || _t_days || _t_hours || _t_minutes)
		for (
				;
				timercmp(&tv_current_point, tv_from, <) && timercmp(&tv_current_point, &tv_end_point, <);
				tm_current_point.tm_mon += _t_months, tm_current_point.tm_mday += 7 * _t_weeks + _t_days,
						tm_current_point.tm_hour += _t_hours, tm_current_point.tm_min += _t_minutes,
						tv_current_point.tv_sec = ::mktime(&tm_current_point), tv_current_point.tv_usec = 0, tm_current_point.tm_isdst = -1
		) ;

	if (timercmp(&tv_current_point, tv_from, <) || timercmp(&tv_current_point, &tv_end_point, >)) {
		HCF_CC_TRACE(GetNextTimerPointErr, "No timer point was found!", ERR_NOT_FOUND);
		return ERR_NOT_FOUND;
	}

	timersub(&tv_current_point, tv_from, relative_value);

	return ERR_NO_ERROR;
}

TSL_NS_HCF_CORE_END
