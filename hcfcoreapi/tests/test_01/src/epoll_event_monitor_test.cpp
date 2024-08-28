#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <stdio.h>

#include "hcf/core/epoll_event_info.h"

#include "epoll_event_monitor_test.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_EPOLL_EVENT_MONITOR_TEST__

int __TSL_CLASS_NAME__::operator() () {
	TSL_NS_HCF_CORE::epoll_event_monitor monitor;

	int call_result = monitor.init();
	::printf("%s::%d: call_result == %d\n", __PRETTY_FUNCTION__, __LINE__, call_result);

	int fifo_fd = ::open("/home/xnicmut/tmp/test_fd", O_RDWR);
	if (fifo_fd < 0) { // ERROR: Opening the fifo
		::printf("%s::%d: errno == %d\n", __PRETTY_FUNCTION__, __LINE__, errno);
		return -1;
	}

	TSL_NS_HCF_CORE::epoll_event_info ee_info(
			TSL_NS_HCF_CORE::TSL_EPOLLIN | TSL_NS_HCF_CORE::TSL_EPOLLRDHUP | TSL_NS_HCF_CORE::TSL_EPOLLPRI | TSL_NS_HCF_CORE::TSL_EPOLLONESHOT,
			fifo_fd);

	::printf("%s::%d: events == 0x%08" PRIx32 ", fd == %d, ptr == %p, u32 == %" PRIu32 ", u64 == %" PRIu64 ", this == %p\n",
			__PRETTY_FUNCTION__, __LINE__, ee_info.events, ee_info.data.fd, ee_info.data.ptr, ee_info.data.u32, ee_info.data.u64, this);

	call_result = monitor.add(fifo_fd, ee_info);
	::printf("%s::%d: call_result == %d\n", __PRETTY_FUNCTION__, __LINE__, call_result);

	::printf("%s::%d: Waiting for events on FIFO\n", __PRETTY_FUNCTION__, __LINE__);
	call_result = monitor.wait(ee_info);
	::printf("%s::%d: Events ready: events == 0x%08" PRIx32 "\n", __PRETTY_FUNCTION__, __LINE__, ee_info.events);

	char buffer[1024] = {0};
	ssize_t bytes_read = ::read(ee_info.data.fd, buffer, 2);
//	::printf("%s::%d: bytes_read == %zd" PRId64 "\n", __PRETTY_FUNCTION__, __LINE__, bytes_read);
	::printf("%s::%d: bytes_read == %zd\n", __PRETTY_FUNCTION__, __LINE__, bytes_read);

	if (bytes_read < 0) {
//		::printf("%s::%d: bytes_read == %" PRId64 "\n", __PRETTY_FUNCTION__, __LINE__, bytes_read);
		::printf("%s::%d: bytes_read == %zd\n", __PRETTY_FUNCTION__, __LINE__, bytes_read);
	} else {
		::printf("%s::%d: first char == '%c'\n", __PRETTY_FUNCTION__, __LINE__, buffer[0]);
	}

	// Rearming the epoll
	ee_info.events = TSL_NS_HCF_CORE::TSL_EPOLLIN | TSL_NS_HCF_CORE::TSL_EPOLLRDHUP
			| TSL_NS_HCF_CORE::TSL_EPOLLPRI | TSL_NS_HCF_CORE::TSL_EPOLLONESHOT;

	::printf("%s::%d: events == 0x%08" PRIx32 ", fd == %d, ptr == %p, u32 == %" PRIu32 ", u64 == %" PRIu64 ", this == %p\n",
			__PRETTY_FUNCTION__, __LINE__, ee_info.events, ee_info.data.fd, ee_info.data.ptr, ee_info.data.u32, ee_info.data.u64, this);

	call_result = monitor.change(fifo_fd, ee_info);
	::printf("%s::%d: call_result == %d\n", __PRETTY_FUNCTION__, __LINE__, call_result);

	::printf("%s::%d: Waiting for events on FIFO\n", __PRETTY_FUNCTION__, __LINE__);
	call_result = monitor.wait(ee_info);
	::printf("%s::%d: Events ready: events == 0x%08" PRIx32 "\n", __PRETTY_FUNCTION__, __LINE__, ee_info.events);

	bytes_read = ::read(ee_info.data.fd, buffer, 2);
//	::printf("%s::%d: bytes_read == %" PRId64 "\n", __PRETTY_FUNCTION__, __LINE__, bytes_read);
	::printf("%s::%d: bytes_read == %zd\n", __PRETTY_FUNCTION__, __LINE__, bytes_read);

	if (bytes_read < 0) {
//		::printf("%s::%d: bytes_read == %" PRId64 "\n", __PRETTY_FUNCTION__, __LINE__, bytes_read);
		::printf("%s::%d: bytes_read == %zd\n", __PRETTY_FUNCTION__, __LINE__, bytes_read);
	} else {
		::printf("%s::%d: first char == '%c'\n", __PRETTY_FUNCTION__, __LINE__, buffer[0]);
	}

	::printf("%s::%d: Waiting for events on FIFO\n", __PRETTY_FUNCTION__, __LINE__);
	call_result = monitor.wait(ee_info);
	::printf("%s::%d: Events ready\n", __PRETTY_FUNCTION__, __LINE__);

	call_result = monitor.remove(fifo_fd);
	::printf("%s::%d: call_result == %d\n", __PRETTY_FUNCTION__, __LINE__, call_result);

	return 0;
}

