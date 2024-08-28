#include <sys/time.h>
#include <stdlib.h>
#include <syslog.h>

#include "hcf/core/logger.h"
#include "hcf/core/thread_set.h"

using namespace TSL_NS_HCF_CORE;

void test_001 ();
void test_002 ();

int main () {
	test_001();
//	test_002();
	return 0;
}

void test_001 () {
	logger::initialize("HCF");

	struct timeval t1, t2;
	::gettimeofday(&t1, 0);
	HCF_CC_LOG(LEVEL_EMERGENCY, "Hello, this is a log line: EMERGENCY, only log.");
	::gettimeofday(&t2, 0);
	::printf("The call took %ld seconds and %ld microseconds!\n", labs((t2.tv_sec - t1.tv_sec)), labs((t2.tv_usec - t1.tv_usec)));

	::gettimeofday(&t1, 0);
	HCF_CC_LOG_ERRNO(11, LEVEL_ALERT, "Hello, this is a log line: ALERT, with errno == 11.");
	::gettimeofday(&t2, 0);
	::printf("The call took %ld seconds and %ld microseconds!\n", labs((t2.tv_sec - t1.tv_sec)), labs((t2.tv_usec - t1.tv_usec)));

	::gettimeofday(&t1, 0);
	HCF_CC_SYSLOG(LOG_DEBUG, LEVEL_CRITICAL, "Hello, this is a log line, CRITICAL, with LOG_DEBUG for syslog.");
	::gettimeofday(&t2, 0);
	::printf("The call took %ld seconds and %ld microseconds!\n", labs((t2.tv_sec - t1.tv_sec)), labs((t2.tv_usec - t1.tv_usec)));

	::gettimeofday(&t1, 0);
	HCF_CC_SYSLOG_ERRNO(11, LOG_DEBUG, LEVEL_ERROR, "Hello, this is a log line: ERROR, with LOG_DEBUF for syslog, with errno == 11.");
	::gettimeofday(&t2, 0);
	::printf("The call took %ld seconds and %ld microseconds!\n", labs((t2.tv_sec - t1.tv_sec)), labs((t2.tv_usec - t1.tv_usec)));

	::gettimeofday(&t1, 0);
	unsigned char buffer[1024];
	for (size_t i = 0; i < 1024; ++i)	buffer[i] = 'A';
	HCF_CC_LOG_DUMP(buffer, 1024, HCF_CC_DEFAULT_DUMP_LINE_LENGTH);
	::gettimeofday(&t2, 0);
	::printf("The call took %ld seconds and %ld microseconds!\n", labs((t2.tv_sec - t1.tv_sec)), labs((t2.tv_usec - t1.tv_usec)));

	logger::finalize();
}

class logger_thread_set : public thread_set {
public:
	virtual int main (void * arg) {
		for (int i = 0; i < 1000; ++i) {
			int x = rand() % 10;
			HCF_CC_LOG(LEVEL_NOTICE, "[%02d] Logging line...", i);
			::usleep(x);
		}
		return 0;
	}
};

void test_002 () {
	logger::initialize("HCF");

	logger_thread_set l;
	l.activate(TSL_THREAD_NULL_MASK, 10);
	l.join();

	logger::finalize();
}
