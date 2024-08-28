#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "hcf/core/calendar_periodic_alarm_timeplan.h"

#include "calendar_periodic_alarm_timeplan_test.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_CALENDAR_PERIODIC_ALARM_TIMEPLAN_TEST__

int __TSL_CLASS_NAME__::operator() () {
	TSL_NS_HCF_CORE::calendar_periodic_alarm_timeplan cpatp;
	int call_result = 0;

	call_result = cpatp.set_datetime_end(2022, 8, 31, 20, 30, 0);
	::printf("CALL [cpatp.set_datetime_end]: call_result == 0x%X\n", call_result);

	call_result = cpatp.set_datetime_start(2015, 4, 22, 13, 0, 0);
	::printf("CALL [cpatp.set_datetime_start]: call_result == 0x%X\n", call_result);

	cpatp.t_time(13, 5, 7);

	cpatp.t_month(1);
//	cpatp.t_dayofmonth(5);
	cpatp.t_dayofweek(7);
	cpatp.t_dayofweek_occurrence(5);

	::timeval tv_now;
	::tm tm_now;
	::timeval tv_next_relative_value;

	::gettimeofday(&tv_now, 0);
	::localtime_r(&tv_now.tv_sec, &tm_now);

	for (int i = 0; i < 1000; i++) {
		tv_now.tv_sec += 98435 + (random() % 60125) * 3;
		::localtime_r(&tv_now.tv_sec, &tm_now);
		::printf("FROM DATETIME: %02u/%02u/%04u %02u:%02u:%02u\n",
				tm_now.tm_mday, tm_now.tm_mon + 1, tm_now.tm_year + 1900, tm_now.tm_hour, tm_now.tm_min, tm_now.tm_sec);

		if ((call_result = cpatp.get_next_timer_point(tv_next_relative_value, tv_now))) {
			::printf("CALL [cpatp.get_next_timer_point]: call_result == 0x%X\n", call_result);
		} else {
			::tm next_tm;
			cpatp.get_next_datetime(next_tm, tv_now);

			::printf("NEXT TIMER POINT RELATIVE: sec. %ld.%ld --> %04d/%02d/%02d %02d:%02d:%02d\n",
					tv_next_relative_value.tv_sec, tv_next_relative_value.tv_usec,
					next_tm.tm_year + 1900, next_tm.tm_mon + 1, next_tm.tm_mday, next_tm.tm_hour, next_tm.tm_min, next_tm.tm_sec);
		}

		::sleep(1);
	}

	return 0;
}
