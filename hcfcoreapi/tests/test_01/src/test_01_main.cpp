#include <stdlib.h>
#include <stdio.h>

//#include "clock_alarm_test.h"
//#include "epoll_event_monitor_test.h"
//#include "epoll_event_reactor_test.h"
//#include "hcf/core/oneshot_alarm_timeplan.h"
//#include "periodic_alarm_timeplan_test.h"
#include "calendar_periodic_alarm_timeplan_test.h"

int main (int /*argc*/, char * /*argv*/ []) {
	/*TSL_TRACE_ON_SCREEN;*/
//	epoll_event_monitor_test test_001;
//	epoll_event_reactor_test reactor_test;
//	clock_alarm_test cat;
//	periodic_alarm_timeplan_test patp_test;
	calendar_periodic_alarm_timeplan_test cpatp_test;

//	test_001();
//	reactor_test();
//	cat();
	cpatp_test();

/*
	unsigned values[6] = {0};

	int call_result = 0;

	for (int i = 1; i <= 6; ++i) {
		if (argc > i) values[i - 1] = ::strtoul(argv[i], 0, 0);
	}

	call_result = TSL_NS_HCF_CORE::alarm_timeplan::is_leap_year(values[0]);
	::printf("is_leap_year(%u) == %d\n", values[0], call_result);

	call_result = TSL_NS_HCF_CORE::alarm_timeplan::check_date(values[0], values[1], values[2]);
	::printf("check_date(%u, %u, %u) == %d\n", values[0], values[1], values[2], call_result);

	call_result = TSL_NS_HCF_CORE::alarm_timeplan::check_time(values[3], values[4], values[5]);
	::printf("check_time(%u, %u, %u) == %d\n", values[3], values[4], values[5], call_result);
*/

	return 0;
}




#if 0

//#include <sys/signalfd.h>
//#include <signal.h>
//#include <errno.h>
#include <unistd.h>
#include <stdio.h>
//#include <errno.h>

//#include "hcf/core/event_processor.h"
//#include "hcf/core/epoll_event_info.h"
//#include "hcf/core/epoll_event_monitor.h"
//#include "hcf/core/epoll_event_processor.h"
#include "hcf/core/thread_mutex.h"
#include "hcf/core/thread_condition.h"
#include "hcf/core/guard.h"

//#include "signal_fd_event_handler.h"
//#include "fifo_reader.h"

namespace {
	void test_01 (TSL_NS_HCF_CORE::thread_mutex_base & m) {
		::printf("DOING: lock()...\n");
		int call_result = m.lock();
		::printf("DONE: lock(): call_result == %d\n", call_result);

		::sleep(5);

		::printf("DOING: lock()...\n");
		call_result = m.lock();
		::printf("DONE: lock(): call_result == %d\n", call_result);
	}
}

int main (int /*argc*/, char * /*argv*/ []) {
	TSL_NS_HCF_CORE::thread_mutex_fast m_fast;
	TSL_NS_HCF_CORE::thread_mutex_recursive m_recursive;
	TSL_NS_HCF_CORE::thread_mutex_errorcheck m_errorcheck;
	TSL_NS_HCF_CORE::thread_mutex_adaptive m_adaptive;
	TSL_NS_HCF_CORE::thread_mutex mutex_1;
	TSL_NS_HCF_CORE::thread_mutex mutex_2(TSL_NS_HCF_CORE::TSL_THREAD_MUTEX_ERRORCHECK);

	::printf("m_fast.mutex_type = %d\n", m_fast.mutex_type());
	::printf("m_recursive.mutex_type = %d\n", m_recursive.mutex_type());
	::printf("m_errorcheck.mutex_type = %d\n", m_errorcheck.mutex_type());
	::printf("m_adaptive.mutex_type = %d\n", m_adaptive.mutex_type());
	::printf("mutex_1.mutex_type = %d\n", mutex_1.mutex_type());
	::printf("mutex_2.mutex_type = %d\n", mutex_2.mutex_type());

//	mutex_2.init(TSL_NS_HCF_CORE::TSL_THREAD_MUTEX_ADAPTIVE);

	TSL_CRITICAL_SECTION_THREAD_MUTEX_BEGIN(m_recursive);
		::printf("mutex_2.mutex_type = %d\n", mutex_2.mutex_type());
	TSL_CRITICAL_SECTION_THREAD_MUTEX_END;


//	test_01(m_fast);
//	test_01(m_recursive);
//	test_01(m_errorcheck);
//	test_01(m_adaptive);
//	test_01(mutex_1);
	test_01(mutex_2);

	mutex_2.destroy();

	int call_result = mutex_2.init(TSL_NS_HCF_CORE::TSL_THREAD_MUTEX_RECURSIVE);
	::printf("init call_result == %d\n", call_result);

	test_01(mutex_2);


/*
	TSL_NS_HCF_CORE::thread_condition tcond;

	tcond.wait(m_fast);
	tcond.wait(m_recursive);
	tcond.wait(m_errorcheck);
	tcond.wait(m_adaptive);
	tcond.wait(mutex_1);
	tcond.wait(mutex_2);
*/





//	nome per la classe signalfd --> signal_event_monitor


/*
	fifo_reader fr;
	int call_result = 0;

	errno = 0;
	call_result = fr.open();
	if (call_result < 0) { // ERROR
		::printf("{%s::%d} [ERROR]: call_result == %d, errno == %d\n", __PRETTY_FUNCTION__, __LINE__, call_result, errno);
		return 1;
	}

	TSL_NS_HCF_CORE::epoll_event_processor event_processor;
*/



/*
	signal_fd_event_handler sig_fd_handler;

	int call_result = sig_fd_handler.add(SIGUSR1);
	::printf("%s::%d: [DEBUG]: call_result == %d\n", __PRETTY_FUNCTION__, __LINE__, call_result);

	call_result = sig_fd_handler.add(SIGUSR2);
	::printf("%s::%d: [DEBUG]: call_result == %d\n", __PRETTY_FUNCTION__, __LINE__, call_result);
*/





/*
	TSL_NS_HCF_CORE::event_processor_epoll epep;
	TSL_NS_HCF_CORE::event_processor ep(&epep);

	int call_result = ep.run_processor_event_loop();

	::printf("call_result == %d\n", call_result);
*/


/*
	::printf("START\n");

	::epoll_event event;

	TSL_NS_HCF_CORE::epoll_event_info ei_1;
	TSL_NS_HCF_CORE::epoll_event_info ei_2(ei_1);
	TSL_NS_HCF_CORE::epoll_event_info ei_3 = event;

	ei_1 = ei_3;
	ei_2 = event;

	event = ei_1;

	::printf("sizeof(event) == %zu, sizeof(TSL_NS_HCF_CORE::epoll_event_info) == %zu, sizeof(ei_1) == %zu\n",
			sizeof(event), sizeof(TSL_NS_HCF_CORE::epoll_event_info), sizeof(ei_1));

	TSL_NS_HCF_CORE::epoll_event_monitor event_monitor(0);

	int call_result = 0;

//	call_result = event_monitor.init();
//	::printf("call_result == %d\n", call_result);

	call_result = event_monitor.add(0, TSL_NS_HCF_CORE::epoll_event_info());
	::printf("%s::%d: call_result = %d\n", __PRETTY_FUNCTION__, __LINE__, call_result);

	TSL_NS_HCF_CORE::epoll_event_info event_infos[3];

	call_result = event_monitor.wait(event_infos, 5000);
	::printf("%s::%d: call_result = %d\n", __PRETTY_FUNCTION__, __LINE__, call_result);
*/


	return 0;
}

#endif
