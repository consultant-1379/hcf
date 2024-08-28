#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>

#include "hcf/core/periodic_alarm_timeplan.h"

#include "periodic_alarm_timeplan_test.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_PERIODIC_ALARM_TIMEPLAN_TEST__

int __TSL_CLASS_NAME__::operator() () {
	TSL_NS_HCF_CORE::periodic_alarm_timeplan patp;
	int call_result = 0;

	call_result = patp.set_datetime_end(2016, 4, 20, 20, 30, 0);
	::printf("CALL [patp.set_datetime_end]: call_result == 0x%X\n", call_result);

	call_result = patp.set_datetime_start(2015, 4, 14, 13, 0, 0);
	::printf("CALL [patp.set_datetime_start]: call_result == 0x%X\n", call_result);

	patp.t_minutes(3);
	patp.t_hours(1);
	patp.t_days(2);
	patp.t_months(1);
	patp.t_weeks(3);

	::timeval tv_now;

	::timeval tv_next_relative_value;

	for (int i = 0; i < 1000; i++) {
		::gettimeofday(&tv_now, 0);
		if ((call_result = patp.get_next_timer_point(tv_next_relative_value, tv_now))) {
			::printf("CALL [patp.get_next_timer_point]: call_result == 0x%X\n", call_result);
		} else {
			::tm next_tm;
			patp.get_next_datetime(next_tm);

			::printf("NEXT TIMER POINT RELATIVE: sec. %ld.%ld --> %04d/%02d/%02d %02d:%02d:%02d\n",
					tv_next_relative_value.tv_sec, tv_next_relative_value.tv_usec,
					next_tm.tm_year + 1900, next_tm.tm_mon + 1, next_tm.tm_mday, next_tm.tm_hour, next_tm.tm_min, next_tm.tm_sec);
		}

		::sleep(1);
	}

	return 0;
}
