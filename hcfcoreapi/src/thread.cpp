#include <stddef.h>
#include <unistd.h>
#include <sys/syscall.h>

#include "hcf/core/utils.h"

#include "hcf/trace/com_ericsson_common_hcf_core_thread.h"
#include "hcf/trace/tracing_macros.h"

#include "hcf/core/error_constants.h"
#include "hcf/core/thread.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_CORE_THREAD__

TSL_NS_HCF_CORE_BEGIN

HCF_CC_FUNCTION_TRACER_CLASS_DEFINE

int __TSL_CLASS_NAME__::activate (uint32_t flags, void * arg) {
	HCF_CC_TRACE_FUNCTION;

	if (_id) {	// Already running
		HCF_CC_TRACE(ActivateErr, "The thread is already running!", ERR_INVALID_OPERATION);
		TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_INVALID_OPERATION, "Thread already activated");
	}

	pthread_attr_t attr;
	pthread_attr_t * attr_ptr = 0;
	int call_result = 0;

	if (flags & TSL_THREAD_DETACHED) {
		HCF_CC_TRACE(Activate, "Requested to activate a detached thread: initializing attributes!", flags);
		if ((call_result = ::pthread_attr_init((attr_ptr = &attr)))) {
			HCF_CC_TRACE(ActivateErr, "Call 'pthread_attr_init' failed.", ERR_INTERNAL_ERROR);
			TSL_SET_ERROR_INFO_AND_RETURN(-1, call_result, ERR_INTERNAL_ERROR, "Cannot initialize thread attributes");
		}

		HCF_CC_TRACE(Activate, "Requested to activate a detached thread: setting the detached state!", flags);
		if ((call_result = ::pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED))) {
			HCF_CC_TRACE(ActivateErr, "Call 'pthread_attr_setdetachstate' failed.", ERR_INTERNAL_ERROR);
			TSL_SET_ERROR_INFO_AND_RETURN(-1, call_result, ERR_INTERNAL_ERROR, "Cannot set the thread detachable attribute");
		}
	}

	_arg = arg;

	HCF_CC_TRACE(Activate, "Received a request to start a new thread!", flags);
	if ((call_result = ::pthread_create(&_id, attr_ptr, start_routine, this))) {	// ERROR: creating a new thread
		_id = 0; _arg = 0;
		HCF_CC_TRACE(ActivateErr, "Call 'pthread_create' failed.", call_result);
		TSL_SET_ERROR_INFO_AND_RETURN(-1, call_result, ERR_INTERNAL_ERROR, "Cannot create and start the thread");
	}

	if (flags & TSL_THREAD_DETACHED) {
		_attr |= TSL_THREAD_DETACHED;
		::pthread_attr_destroy(&attr);
	}

	return ERR_NO_ERROR;
}

int __TSL_CLASS_NAME__::detach () {
	HCF_CC_TRACE_FUNCTION;

	if (!_id) {
		HCF_CC_TRACE(DetachErr, "Cannot detach thread, since it is not activated!", ERR_INVALID_OPERATION);
		TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_INVALID_OPERATION, "Thread not activated");
	}

	HCF_CC_TRACE(Detach, "Received a request to detach the thread!", _id);
	if (const int call_result = ::pthread_detach(_id)) {
		HCF_CC_TRACE(DetachErr, "Call 'pthread_detach' failed.", call_result);
		TSL_SET_ERROR_INFO_AND_RETURN(-1, call_result, ERR_INTERNAL_ERROR, "Cannot detach the thread");
	}

	_attr |= TSL_THREAD_DETACHED;

	return ERR_NO_ERROR;
}

int __TSL_CLASS_NAME__::join (int & ret_val) {
	HCF_CC_TRACE_FUNCTION;

	if (!_id) {
		HCF_CC_TRACE(JoinErr, "Cannot join thread, since it is not activated!", ERR_INVALID_OPERATION);
		TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_INVALID_OPERATION, "Thread not activated");
	}

	if (_attr & TSL_THREAD_DETACHED) {
		HCF_CC_TRACE(JoinErr, "Cannot join thread, since it is detached!", ERR_INVALID_OPERATION);
		TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_INVALID_OPERATION, "Thread detached");
	}

	void * ret_val_ = 0;

	HCF_CC_TRACE(Join, "Received a request to join this thread!", _id, -1, "");
	if (const int call_result = ::pthread_join(_id, &ret_val_)) {
		HCF_CC_TRACE(JoinErr, "Call 'pthread_join' failed.", call_result);
		TSL_SET_ERROR_INFO_AND_RETURN(-1, call_result, ERR_INTERNAL_ERROR, "Error on joining the thread");
	}

	ret_val = reinterpret_cast<ptrdiff_t>(ret_val_);
	HCF_CC_TRACE(Join, "Thread correctly joined and its return value was correctly retrieved!", _id, ret_val, "");

	return ERR_NO_ERROR;
}

int __TSL_CLASS_NAME__::join (int & ret_val, const ::timespec & abstime) {
	HCF_CC_TRACE_FUNCTION;

	if (!_id) {
		HCF_CC_TRACE(JoinErr, "Cannot join thread, since it is not activated!", ERR_INVALID_OPERATION);
		TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_INVALID_OPERATION, "Thread not activated");
	}

	if (_attr & TSL_THREAD_DETACHED) {
		HCF_CC_TRACE(JoinErr, "Cannot join thread, since it is detached!", ERR_INVALID_OPERATION);
		TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_INVALID_OPERATION, "Thread detached");
	}

	void * ret_val_ = 0;

	HCF_CC_TRACE(Join, "Received a request to join this thread in a fixed time interval!", _id, -1, utils::calculate_time_string(&abstime).c_str());
	if (const int call_result = ::pthread_timedjoin_np(_id, &ret_val_, &abstime)) {
		HCF_CC_TRACE(JoinErr, "Call 'pthread_timedjoin_np' failed.", call_result);
		TSL_SET_ERROR_INFO_AND_RETURN(-1, call_result, ERR_INTERNAL_ERROR, "Error on joining the thread");
	}

	ret_val = reinterpret_cast<ptrdiff_t>(ret_val_);
	HCF_CC_TRACE(Join, "Thread correctly joined and its return value was correctly retrieved!", _id, ret_val, utils::calculate_time_string(&abstime).c_str());

	return ERR_NO_ERROR;
}

int __TSL_CLASS_NAME__::try_join (int & ret_val) {
	HCF_CC_TRACE_FUNCTION;

	if (!_id) {
		HCF_CC_TRACE(TryJoinErr, "Cannot try to join thread, since it is not activated!", ERR_INVALID_OPERATION);
		TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_INVALID_OPERATION, "Thread not activated");
	}

	if (_attr & TSL_THREAD_DETACHED) {
		HCF_CC_TRACE(TryJoinErr, "Cannot try to join thread, since it is detached!", ERR_INVALID_OPERATION);
		TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_INVALID_OPERATION, "Thread detached");
	}

	void * ret_val_ = 0;

	HCF_CC_TRACE(TryJoin, "Received a request to try to join this thread!", _id, -1);
	if (const int call_result = ::pthread_tryjoin_np(_id, &ret_val_)) {
		HCF_CC_TRACE(TryJoinErr, "Call 'pthread_tryjoin_np' failed.", call_result);
		TSL_SET_ERROR_INFO_AND_RETURN(-1, call_result, ERR_INTERNAL_ERROR, "Error on trying to join the thread");
	}

	ret_val = reinterpret_cast<ptrdiff_t>(ret_val_);
	HCF_CC_TRACE(TryJoin, "Thread correctly joined and its return value was correctly retrieved!", _id, ret_val);

	return ERR_NO_ERROR;
}

void * __TSL_CLASS_NAME__::start_routine (void * arg) {
	HCF_CC_TRACE_FUNCTION;

	__TSL_CLASS_NAME__ * my_instance = reinterpret_cast<__TSL_CLASS_NAME__ *>(arg);

	my_instance->_tid = ::syscall(SYS_gettid);

	HCF_CC_TRACE(StartRoutine, "Starting the thread: executing its main function!", my_instance->_id, -1);
	const int main_return = my_instance->main(my_instance->_arg);

	my_instance->_id = 0; my_instance->_tid = 0; my_instance->_arg = 0;

	HCF_CC_TRACE(StartRoutine, "The thread main function returned!", my_instance->_id, main_return);
	return reinterpret_cast<void *>(main_return);
}

TSL_NS_HCF_CORE_END
