#include <sys/timerfd.h>
#include <poll.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "clock_alarm_notifier.h"
#include "hcf/core/calendar_periodic_alarm_timeplan.h"
#include "hcf/core/periodic_alarm_timeplan.h"

#include "clock_alarm_notifier_test.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_CLOCK_ALARM_NOTIFIER_TEST__

namespace {
	class test_handler : public TSL_NS_HCF_CORE::clock_alarm_handler {
	public:
		inline test_handler () {}
		inline virtual ~test_handler () {}

		inline virtual int handle_alarm (bool event_in_the_past) {
			::timeval tv_now;
			::tm tm_now;
			::gettimeofday(&tv_now, 0);
			::localtime_r(&tv_now.tv_sec, &tm_now);
			::printf("FROM DATETIME: %02u/%02u/%04u %02u:%02u:%02u\n",
					tm_now.tm_mday, tm_now.tm_mon + 1, tm_now.tm_year + 1900, tm_now.tm_hour, tm_now.tm_min, tm_now.tm_sec);
			return 0;
		}
	};
}

int __TSL_CLASS_NAME__::operator() () {
	HCF_NS_HCF_SRV::clock_alarm_notifier can;
	TSL_NS_HCF_CORE::calendar_periodic_alarm_timeplan cpat;
	test_handler th;
	int call_result = 0;

	if ((call_result = can.open())) {
		::printf("ERROR: failed to open! error == %d\n", can.error());
	}

	if ((call_result = cpat.set_datetime_end(2022, 12, 31, 23, 59, 59))) {
		::printf("ERROR: failed to set END! error == %d\n", call_result);
	}

	if ((call_result = cpat.set_datetime_start(2015, 4, 25, 0, 0, 0))) {
		::printf("ERROR: failed to set START! error == %d\n", call_result);
	}

	cpat.t_time(12, 0, 0);
	cpat.t_month(1);


	if ((call_result = can.add_timeplan("1", cpat, &th))) {
		::printf("ERROR: failed to add! error == %d\n", can.error());
	}


//	TSL_NS_HCF_CORE::periodic_alarm_timeplan pat;
//	if ((call_result = pat.set_datetime_end(2022, 12, 31, 23, 59, 59))) {
//		::printf("ERROR: failed to set END! error == %d\n", call_result);
//	}
//	if ((call_result = pat.set_datetime_start(2015, 4, 25, 0, 0, 0))) {
//		::printf("ERROR: failed to set START! error == %d\n", call_result);
//	}
//
//
//	pat.t_minutes(1);
//
//	if ((call_result = can.add_timeplan("2", pat, &th))) {
//		::printf("ERROR: failed to add! error == %d\n", can.error());
//	}


	struct pollfd poll_fd;
	poll_fd.fd = can.get_handle();
	poll_fd.events = POLLIN | POLLPRI | POLLRDHUP | POLLHUP | POLLNVAL;
	poll_fd.revents = 0;

	while (true) {
		::timeval tv_now;
		::tm tm_now;
		::gettimeofday(&tv_now, 0);
		::localtime_r(&tv_now.tv_sec, &tm_now);
		::printf("Polling at: %02u/%02u/%04u %02u:%02u:%02u\n",
				tm_now.tm_mday, tm_now.tm_mon + 1, tm_now.tm_year + 1900, tm_now.tm_hour, tm_now.tm_min, tm_now.tm_sec);

		call_result = ::poll(&poll_fd, 1, 5000);
		const int errno_save = errno;
		::printf("Expired!\n");

		if ((call_result < 0) && (errno_save != EINTR)) {
			::printf("Call poll failed, errno == %d\n", errno_save);
			return 1;
		}

		if (call_result == 0) {
			::itimerspec ts;
			::tm tm_now;
			::timerfd_gettime(poll_fd.fd, &ts);
			::localtime_r(&ts.it_value.tv_sec, &tm_now);
			::printf("Timer will be invoked at: %02u/%02u/%04u %02u:%02u:%02u\n",
					tm_now.tm_mday, tm_now.tm_mon + 1, tm_now.tm_year + 1900, tm_now.tm_hour, tm_now.tm_min, tm_now.tm_sec);
			continue;
		}

		::printf("Calling handle_input...\n");
		can.handle_input();
	}

	can.close();

	return 0;
}

