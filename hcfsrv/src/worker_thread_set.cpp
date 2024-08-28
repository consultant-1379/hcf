#include "hcf/core/logger.h"
#include "hcf/core/event_reactor.h"

#include "constants.h"

#include "hcf/trace/com_ericsson_common_hcf_srv_workerthreadset.h"
#include "hcf/trace/tracing_macros.h"

#include "worker_thread_set.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_SRV_CLASS_WORKER_THREAD_SET__

HCF_NS_HCF_SRV_BEGIN

HCF_CC_FUNCTION_TRACER_CLASS_DEFINE

int __TSL_CLASS_NAME__::main (void * arg) {
	HCF_CC_TRACE_FUNCTION;

	pid_t tid = ::syscall(SYS_gettid);
	HCF_CC_LOG(LEVEL_NOTICE, "[%d] Worker Thread starting to work!", tid);
	HCF_CC_TRACE(Main, "Worker Tread starting to work!", tid);

	TSL_NS_HCF_CORE::event_reactor * event_reactor = reinterpret_cast<TSL_NS_HCF_CORE::event_reactor *>(arg);
	event_reactor->run_event_loop();

	HCF_CC_LOG(LEVEL_NOTICE, "[%d] Worker Thread exiting!", tid);
	HCF_CC_TRACE(Main, "Worker Tread completed its work!", tid);
	return ERR_NO_ERROR;
}

HCF_NS_HCF_SRV_END
