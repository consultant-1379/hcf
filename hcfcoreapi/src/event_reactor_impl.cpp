#include "hcf/trace/com_ericsson_common_hcf_core_eventreactor.h"
#include "hcf/trace/tracing_macros.h"

#include "hcf/core/event_reactor_impl.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_CORE_EVENT_REACTOR_IMPL__

TSL_NS_HCF_CORE_BEGIN

HCF_CC_FUNCTION_TRACER_CLASS_DEFINE

int __TSL_CLASS_NAME__::is_wakeup_handler_signalled () {
	HCF_CC_TRACE_FUNCTION;

	_wakeup_handler.revents = 0;
	if (const int poll_result = ::poll(&_wakeup_handler, 1, 0)) {
		if (poll_result < 0) {	//ERROR: calling poll
			const int errno_save = errno;
			HCF_CC_TRACE(IsWakeupSignaledErr, "Call 'poll' failed.", errno_save);
			return -1;
		}
		HCF_CC_TRACE(IsWakeupSignaled, "The wakeup handler was signaled: checking the type of events!", _wakeup_handler.fd, _wakeup_handler.revents);

		if (_wakeup_handler.revents & (POLLIN | POLLPRI)) {
			HCF_CC_TRACE(IsWakeupSignaled, "The wakeup handler was signaled for input: consuming the event and signaling the stop!", _wakeup_handler.fd, _wakeup_handler.revents);
			uint64_t wakeup_handle_value = 0;
			const int tmp = ::read(_wakeup_handler.fd, &wakeup_handle_value, sizeof(wakeup_handle_value)); (void)tmp;
			return 1;
		}
		return 0;
	}
	return 0; // NOT SIGNALLED
}

TSL_NS_HCF_CORE_END
