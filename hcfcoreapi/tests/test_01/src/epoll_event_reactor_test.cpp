#include <unistd.h>
#include <pthread.h>

#include "hcf/core/event_reactor.h"
#include "hcf/core/thread_set.h"

#include "fifo_reader.h"

#include "epoll_event_reactor_test.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_EPOLL_EVENT_REACTOR_TEST__

namespace {
/*
	void * thread_procedure (void * arg) {
		TSL_TRACE_ON_SCREEN;
		TSL_NS_HCF_CORE::event_reactor * ev_reactor = reinterpret_cast<TSL_NS_HCF_CORE::event_reactor *>(arg);

		ev_reactor->run_event_loop();

		return 0;
	}
*/

/*
	class worker_thread : public TSL_NS_HCF_CORE::thread {
	public:
		inline worker_thread () : thread() {
			TSL_TRACE_ON_SCREEN;
		}

		inline virtual ~worker_thread () {}

		virtual int main (void * arg);
	};

	int worker_thread::main (void * arg) {
		TSL_NS_HCF_CORE::event_reactor * ev_reactor = reinterpret_cast<TSL_NS_HCF_CORE::event_reactor *>(arg);

		ev_reactor->run_event_loop();

		::printf("[THREAD %d]: terminated\n", tid());

		return 0;
	}
*/

	class worker_thread_set : public TSL_NS_HCF_CORE::thread_set {
	public:
		inline worker_thread_set () : thread_set() { TSL_TRACE_ON_SCREEN; }

		virtual inline ~ worker_thread_set () {}

		virtual int main (void * arg);
	};

	int worker_thread_set::main (void * arg) {
		TSL_NS_HCF_CORE::event_reactor * ev_reactor = reinterpret_cast<TSL_NS_HCF_CORE::event_reactor *>(arg);

		pid_t tid = ::syscall(SYS_gettid);

		::printf("[TID: %d]: Entering event loop...\n", tid);

		ev_reactor->run_event_loop();

		unsigned sleep_seconds = (tid % 10) + 10;

		::printf("[TID: %d]: My event loop terminated. Sleeping for %u seconds\n", tid, sleep_seconds);

		::sleep(sleep_seconds);

		::printf("[THREAD %d]: terminated\n", tid);

		return 0;
	}
}

int __TSL_CLASS_NAME__::operator() () {
	/*TSL_TRACE_ON_SCREEN;*/
	fifo_reader fr_01;
	fifo_reader fr_02;

//	TSL_NS_HCF_CORE::epoll_event_reactor epoll_ev_reactor;
//	TSL_NS_HCF_CORE::event_reactor ev_reactor(&epoll_ev_reactor);
	TSL_NS_HCF_CORE::event_reactor ev_reactor;

	::printf("ev_reactor.state == 0x%016X\n", ev_reactor.state());

	int call_result = 0;

	// FIFO 01
	call_result = fr_01.open("/home/nicmut/tmp/fifos/fifo_01");
	if (call_result < 0) { // ERROR: opening the fifo
		::printf("[ERROR] fr_01.open ERROR: errno == %d\n", errno);
	} else {

		::printf("ev_reactor.state == 0x%016X\n", ev_reactor.state());

		call_result = ev_reactor.register_handler(&fr_01, TSL_NS_HCF_CORE::TSL_EVENT_RWE_MASK);
		if (call_result < 0) {
			::printf("[ERROR]: ev_reactor.register_handler: ev_reactor.error == %d, ev_reactor.error_text == '%s'",
					ev_reactor.error(), ev_reactor.error_text());
		} else {
			::printf("fr_01 handler registered correctly\n");
		}
	}

	// FIFO 01
	call_result = fr_02.open("/home/nicmut/tmp/fifos/fifo_02");
	if (call_result < 0) { // ERROR: opening the fifo
		::printf("[ERROR] fr_02.open ERROR: errno == %d\n", errno);
	} else {

		::printf("ev_reactor.state == 0x%016X\n", ev_reactor.state());

		call_result = ev_reactor.register_handler(&fr_02, TSL_NS_HCF_CORE::TSL_EVENT_RWE_MASK);
		if (call_result < 0) {
			::printf("[ERROR]: ev_reactor.register_handler: ev_reactor.error == %d, ev_reactor.error_text == '%s'",
					ev_reactor.error(), ev_reactor.error_text());
		} else {
			::printf("fr_02 handler registered correctly\n");
		}
	}

	::printf("ev_reactor.state == 0x%016X\n", ev_reactor.state());

	call_result = ev_reactor.open();
	::printf("call_result == %d: ev_reactor.error == %d, ev_reactor.error_text == '%s'\n",
			call_result, ev_reactor.error(), ev_reactor.error_text());

	::printf("ev_reactor.state == 0x%016X\n", ev_reactor.state());

	worker_thread_set wts;

	::sleep(20);

	call_result = wts.activate(0, 10, &ev_reactor);
	::printf("AFTER ACTIVATING 10 THREAD SET: call_result == %d\n", call_result);




/*
	worker_thread my_threads [threads_counter];

	for (int i = 0; i < threads_counter; ++i) {
		call_result = my_threads[i].activate(&ev_reactor);
		::printf("call_result == %d: my_threads[%02d].id == %lu, my_threads[%02d].tid == %d\n",
				call_result, i, my_threads[i].id(), i, my_threads[i].tid());
	}
*/

/*
	for (int i = 0; i < 100; ++i) {
		pthread_t tid = 0;

		call_result = ::pthread_create(&tid, 0, thread_procedure, &ev_reactor);
		if (call_result) { // ERROR
			::printf("[ERROR] pthread_create ERROR: call_result == %d\n", call_result);
		}
	}

	call_result = ev_reactor.run_event_loop();
*/

	::sleep(30);

	call_result = ev_reactor.end_event_loop();
	::printf("[END EVENT LOOP] call_result == %d: ev_reactor.error == %d, ev_reactor.error_text == '%s'\n",
			call_result, ev_reactor.error(), ev_reactor.error_text());

	::printf("JOINING on thread set...\n");
	call_result = wts.join();
	::printf("[JOIN] call_result == %d: wts.error == %d, wts.sys_error == %d, wts.error_text == '%s'\n",
			call_result, wts.error(), wts.sys_error(), wts.error_text());

	::sleep(5);

	return 0;
}
