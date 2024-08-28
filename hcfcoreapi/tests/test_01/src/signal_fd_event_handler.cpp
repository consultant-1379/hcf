#include "signal_fd_event_handler.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_TEST_01_SIGNAL_FD_EVENT_HANDLER__

int __TSL_CLASS_NAME__::add (int signum, bool block_sig) {
	switch (::sigismember(&_sigset, signum)) {
	case -1: return -1; break; // ERROR: not a valid signum
	case 1: return 0; break; // Signal signum already handled
	default: break;
	}

	if (::sigaddset(&_sigset, signum) == -1) return -1;

	if (block_sig && (::sigprocmask(SIG_BLOCK, &_sigset, 0) == -1)) { // ERROR: Trying to block the signum signal
		::sigdelset(&_sigset, signum);
		return -1;
	}

	const int sigfd = ::signalfd(_sigfd, &_sigset, 0);
	if (sigfd == -1) { // ERROR:
		::sigdelset(&_sigset, signum);
		return -1;
	}

	_sigfd = sigfd;

	return 0;
}
