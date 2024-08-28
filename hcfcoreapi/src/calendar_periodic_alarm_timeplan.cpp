#include "hcf/core/utils.h"

#include "hcf/trace/com_ericsson_common_hcf_core_calendaralarmtimeplan.h"
#include "hcf/trace/tracing_macros.h"

#include "hcf/core/calendar_periodic_alarm_timeplan.h"

namespace {
	inline bool less_than (const ::tm & lhs, const ::tm & rhs) {
		if (lhs.tm_year ^ rhs.tm_year) return (lhs.tm_year < rhs.tm_year);
		if (lhs.tm_mon ^ rhs.tm_mon) return (lhs.tm_mon < rhs.tm_mon);
		if (lhs.tm_mday ^ rhs.tm_mday) return (lhs.tm_mday < rhs.tm_mday);
		if (lhs.tm_hour ^ rhs.tm_hour) return (lhs.tm_hour < rhs.tm_hour);
		if (lhs.tm_min ^ rhs.tm_min) return (lhs.tm_min < rhs.tm_min);
		return (lhs.tm_sec < rhs.tm_sec);
	}

	inline bool equal_to (const ::tm & lhs, const ::tm & rhs) {
		return (
				(lhs.tm_year == rhs.tm_year) && (lhs.tm_mon == rhs.tm_mon) && (lhs.tm_mday == rhs.tm_mday) &&
				(lhs.tm_hour == rhs.tm_hour) && (lhs.tm_min == rhs.tm_min) && (lhs.tm_sec == rhs.tm_sec)
		);
	}

	inline bool operator< (const ::tm & lhs, const ::tm & rhs) { return less_than(lhs, rhs); }
	inline bool operator> (const ::tm & lhs, const ::tm & rhs) { return less_than(rhs, lhs); }
	inline bool operator== (const ::tm & lhs, const ::tm & rhs) { return equal_to(lhs, rhs); }
	inline bool operator!= (const ::tm & lhs, const ::tm & rhs) { return !(lhs == rhs); }
	inline bool operator<= (const ::tm & lhs, const ::tm & rhs) { return !(rhs < lhs); }
	inline bool operator>= (const ::tm & lhs, const ::tm & rhs) { return !(lhs < rhs); }

	inline void dx_increment_adjust_by_month (::tm & tm_) {
		while (TSL_NS_HCF_CORE::alarm_timeplan::check_date(tm_.tm_year, tm_.tm_mon + 1, tm_.tm_mday)) {
			if (tm_.tm_mon++ >= 11) { tm_.tm_year++; tm_.tm_mon = 0; }
		}
	}
}

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_CORE_CALENDAR_PERIODIC_ALARM_TIMEPLAN__

TSL_NS_HCF_CORE_BEGIN

HCF_CC_FUNCTION_TRACER_CLASS_DEFINE

int __TSL_CLASS_NAME__::get_next_timer_point (::timeval * relative_value, const ::timeval * tv_from) {
	HCF_CC_TRACE_FUNCTION;
	_start_tm.tm_isdst = _end_tm.tm_isdst = -1;

	time_t time_start = ::mktime(&_start_tm); _start_tm.tm_isdst = -1;
	time_t time_from = tv_from->tv_sec + (tv_from->tv_usec >= 500000 ? 1 : 0);

	::timeval tv_now; ::gettimeofday(&tv_now, 0);

	time_t time_now = tv_now.tv_sec + (tv_now.tv_usec >= 500000 ? 1 : 0);

	time_t time_next = (time_start <= time_from ? time_from : time_start);
	if (time_next < time_now) time_next = time_now;

	::tm tm_next;
	::localtime_r(&time_next, &tm_next); tm_next.tm_isdst = -1;
	HCF_CC_TRACE(GetNextTimerPoint, "Starting to calculate the next timer point starting from the following time:", utils::calculate_time_string(&tm_next).c_str(),
			_t_time_hour, _t_time_min, _t_time_sec, _t_month, _t_dayofmonth, _t_dayofweek, _t_dayofweek_occurrence);

	bool time_point_found = false;

	while (!time_point_found) {
		if (_t_month) {
			register const int m = _t_month - 1;
			if (tm_next.tm_mon ^ m) {
				if (tm_next.tm_mon > m) tm_next.tm_year++;
				tm_next.tm_mon = m; tm_next.tm_mday = 1;
				tm_next.tm_hour = 0; tm_next.tm_min = 0; tm_next.tm_sec = 0;
				time_next = ::mktime(&tm_next); tm_next.tm_isdst = -1;
			}
			if (_end_tm < tm_next) break;
		}

		if (_t_dayofmonth) {
			if (tm_next.tm_mday ^ _t_dayofmonth) {
				bool inc_month = (tm_next.tm_mday > static_cast<int>(_t_dayofmonth));
				tm_next.tm_mday = _t_dayofmonth;
				if (inc_month && (tm_next.tm_mon++ >= 11)) { tm_next.tm_year++; tm_next.tm_mon = 0; }
				dx_increment_adjust_by_month(tm_next);
				tm_next.tm_hour = 0; tm_next.tm_min = 0; tm_next.tm_sec = 0;
				time_next = ::mktime(&tm_next); tm_next.tm_isdst = -1;
				if (_end_tm < tm_next) break;
				continue;
			}
		}

		if (_t_dayofweek) {
			register int dow = ((_t_dayofweek ^ 7) ? _t_dayofweek : 0);
			time_next = ::mktime(&tm_next); tm_next.tm_isdst = -1;

			if (tm_next.tm_wday ^ dow) {
				if (tm_next.tm_wday > dow) dow += 7;
				tm_next.tm_mday += (dow - tm_next.tm_wday);
				tm_next.tm_hour = 0; tm_next.tm_min = 0; tm_next.tm_sec = 0;
				time_next = ::mktime(&tm_next); tm_next.tm_isdst = -1;

				if (_end_tm < tm_next) break;
				continue;
			}
		}

		if (_t_dayofweek_occurrence) {
			uint32_t dowo = ((tm_next.tm_mday - 1) / 7) + 1;
			const int leap_day = ((tm_next.tm_mon == 1) && (is_leap_year(tm_next.tm_year + 1900)) ? 1 : 0);
			const int last_day_of_month = static_cast<int>(days_per_month[tm_next.tm_mon]) + leap_day;
			bool is_last = ((tm_next.tm_mday + 7) > (static_cast<int>(days_per_month[tm_next.tm_mon]) + leap_day));

			if (dowo < _t_dayofweek_occurrence) {
				if (!is_last) {
					if (_t_dayofweek_occurrence < 5) {
						tm_next.tm_mday += 7 * (_t_dayofweek_occurrence - dowo);
					} else { // Increment to the last occurrence
						while ((tm_next.tm_mday += 7) < last_day_of_month) ;
						if (tm_next.tm_mday > last_day_of_month) tm_next.tm_mday -= 7;
					}
					tm_next.tm_hour = 0; tm_next.tm_min = 0; tm_next.tm_sec = 0;
					time_next = ::mktime(&tm_next); tm_next.tm_isdst = -1;

					if (_end_tm < tm_next) break;
					continue;
				}
			} else if (dowo > _t_dayofweek_occurrence) {
				// Increment to the next month the first
				if (tm_next.tm_mon++ >= 11) { tm_next.tm_year++; tm_next.tm_mon = 0; }
				tm_next.tm_mday = 1;
				tm_next.tm_hour = 0; tm_next.tm_min = 0; tm_next.tm_sec = 0;
				time_next = ::mktime(&tm_next); tm_next.tm_isdst = -1;

				if (_end_tm < tm_next) break;
				continue;
			}
		}

		// Set the time
		::tm tm_target = tm_next; tm_target.tm_hour = _t_time_hour; tm_target.tm_min = _t_time_min; tm_target.tm_sec = _t_time_sec;

		if (tm_next != tm_target) {
			if (tm_next < tm_target) {
				tm_next.tm_hour = _t_time_hour; tm_next.tm_min = _t_time_min; tm_next.tm_sec = _t_time_sec;
				time_next = ::mktime(&tm_next); tm_next.tm_isdst = -1;
				if (_end_tm < tm_next) break;
			} else {
				const int leap_day = ((tm_next.tm_mon == 1) && (is_leap_year(tm_next.tm_year + 1900)) ? 1 : 0);
				const int last_day_of_month = static_cast<int>(days_per_month[tm_next.tm_mon]) + leap_day;

				if (tm_next.tm_mday++ >= last_day_of_month) {
					tm_next.tm_mday = 1;
					if (tm_next.tm_mon++ >= 11) { tm_next.tm_year++; tm_next.tm_mon = 0; }
				}
				tm_next.tm_hour = 0; tm_next.tm_min = 0; tm_next.tm_sec = 0;
				time_next = ::mktime(&tm_next); tm_next.tm_isdst = -1;
				if (_end_tm < tm_next) break;
				continue;
			}
		}

		time_point_found = true;
	}

	if (!time_point_found) {
		HCF_CC_TRACE(GetNextTimerPointErr, "No timer point was found!", ERR_NOT_FOUND);
		return ERR_NOT_FOUND;
	}

	::timeval tv_next;
	tv_next.tv_sec = ::mktime(&tm_next); tm_next.tm_isdst = -1;
	tv_next.tv_usec = 0;

	timersub(&tv_next, tv_from, relative_value);

	HCF_CC_TRACE(GetNextTimerPoint, "Timer point found:", utils::calculate_time_string(&tm_next).c_str(),
			_t_time_hour, _t_time_min, _t_time_sec, _t_month, _t_dayofmonth, _t_dayofweek, _t_dayofweek_occurrence);

	return ERR_NO_ERROR;
}

TSL_NS_HCF_CORE_END
