#include <signal.h>
#include <sys/syscall.h>

#include "hcf/core/error_constants.h"
#include "hcf/core/guard.h"

#include "hcf/trace/com_ericsson_common_hcf_core_threadset.h"
#include "hcf/trace/tracing_macros.h"

#include "hcf/core/thread_set.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_CORE_THREAD_SET__

TSL_NS_HCF_CORE_BEGIN

HCF_CC_FUNCTION_TRACER_CLASS_DEFINE

int __TSL_CLASS_NAME__::activate (uint32_t flags, int n_threads, void * arg_to_main) {
	HCF_CC_TRACE_FUNCTION;

	if (n_threads <= 0) {
		HCF_CC_TRACE(ActivateErr, "Invalid argument for the number of threads received!", ERR_INVALID_ARGUMENT);
		TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_INVALID_ARGUMENT);
	}
	HCF_CC_TRACE(Activate, "Received a request to activate some threads!", flags, n_threads);

	TSL_CRITICAL_SECTION_BEGIN(_sync_t, _sync);
		if (_activate_ongoing_counter > 0) {
			HCF_CC_TRACE(ActivateErr, "Another thread activation procedure is in progress!", ERR_INVALID_OPERATION);
			TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_INVALID_OPERATION, "Another threads activation in progress");
		}

		if (_join_ongoing) {
			HCF_CC_TRACE(ActivateErr, "A join operation is in progress!", ERR_INVALID_OPERATION);
			TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_INVALID_OPERATION, "Join operation in progress");
		}

		if (_running_index >= _join_wait_index) {
			HCF_CC_TRACE(ActivateErr, "There are too many threads activated: cannot activate more threads!", ERR_NO_RESOURCES_AVAILABLE);
			TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_NO_RESOURCES_AVAILABLE, "Cannot activate other threads");
		}

		_activate_ongoing_counter = n_threads + 1;
	TSL_CRITICAL_SECTION_END;

	pthread_attr_t attr;
	pthread_attr_t * attr_ptr = 0;
	int call_result = 0;

	_attr_of_thread = TSL_THREAD_NULL_MASK;

	if (flags & TSL_THREAD_DETACHED) {
		HCF_CC_TRACE(Activate, "The threads to be created must be detached: initializing attributes!", flags, n_threads);
		if ((call_result = ::pthread_attr_init((attr_ptr = &attr)))) {
			_activate_ongoing_counter = 0;
			HCF_CC_TRACE(ActivateErr, "Call 'pthread_attr_init' failed.", call_result);
			TSL_SET_ERROR_INFO_AND_RETURN(-1, call_result, ERR_INTERNAL_ERROR, "Cannot initialize thread attributes");
		}

		HCF_CC_TRACE(Activate, "The threads to be created must be detached: setting detached attribute!", flags, n_threads);
		if ((call_result = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED))) {
			_activate_ongoing_counter = 0;
			HCF_CC_TRACE(ActivateErr, "Call 'pthread_attr_setdetachstate' failed.", call_result);
			TSL_SET_ERROR_INFO_AND_RETURN(-1, call_result, ERR_INTERNAL_ERROR, "Cannot set the thread detachable attribute");
		}

		_attr_of_thread |= TSL_THREAD_DETACHED;
	}

	int first_create_error = 0;
	int threads_created = 0;

	_arg_to_main = arg_to_main;

	pthread_t id = 0;

	int running_index = _running_index;

	HCF_CC_TRACE(Activate, "Creating and starting all the requested threads!", flags, n_threads);
	while ((n_threads-- > 0) && (running_index < _join_wait_index)) {
		if ((call_result = ::pthread_create(&id, attr_ptr, start_routine, this))) { // ERROR: creating and starting the thread
			if (!first_create_error) first_create_error = call_result;

			TSL_CRITICAL_SECTION_BEGIN(_sync_t, _sync);
				_activate_ongoing_counter--;
			TSL_CRITICAL_SECTION_END;
		} else { // OK
			threads_created++;
			running_index++;
		}
	}

	if (flags & TSL_THREAD_DETACHED) ::pthread_attr_destroy(&attr);

	// Wait the thread activation process was completed
	TSL_CRITICAL_SECTION_BEGIN(_sync_t, _sync);
		while (_activate_ongoing_counter > 1) _activate_end_condition.wait(_sync);

		_activate_ongoing_counter = 0;
	TSL_CRITICAL_SECTION_END;

	if (!threads_created) {
		HCF_CC_TRACE(ActivateErr, "Call 'pthread_create' failed: no thread has been created!", first_create_error);
		TSL_SET_ERROR_INFO_AND_RETURN(-1, first_create_error, ERR_INTERNAL_ERROR, "Cannot create any thread");
	}

	return threads_created;
}

int __TSL_CLASS_NAME__::join () {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(Join, "Received a request to join all the running threads!", (_running_index + 1));

	TSL_CRITICAL_SECTION_BEGIN(_sync_t, _sync);
		if (_activate_ongoing_counter > 0) {
			HCF_CC_TRACE(JoinErr, "Thread activation procedure is in progress: cannot join!", ERR_INVALID_OPERATION);
			TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_INVALID_OPERATION, "Threads activation operation in progress");
		}

		if (_join_ongoing) {
			HCF_CC_TRACE(JoinErr, "Another join operation is in progress!", ERR_INVALID_OPERATION);
			TSL_SET_ERROR_INFO_AND_RETURN(-1, ERR_INVALID_OPERATION, "Another join operation already in progress");
		}

		_join_ongoing = 1;
	TSL_CRITICAL_SECTION_END;

	int call_result = 0;
	int threads_joined_counter = 0;
	int first_bad_call_result = 0;

////	for (std::vector<worker_thread>::iterator wt_it = _worker_threads.begin(); wt_it != _worker_threads.end(); wt_it++)
//	for (std::map<pid_t, worker_thread>::iterator wt_it = _worker_threads_map.begin(); wt_it != _worker_threads_map.end(); wt_it++)
//		if ((call_result = join_thread((wt_it->second).id()))) {
//			if (!first_bad_call_result) first_bad_call_result = call_result;
//		} else threads_joined_counter++;

	HCF_CC_TRACE(Join, "Performing join operation for all the running threads!", (_running_index + 1));
	for (int i = 0; i < _running_index; i++) {
		if ((call_result = join_thread(_worker_threads[i].id()))) {
			if (!first_bad_call_result) first_bad_call_result = call_result;
		} else threads_joined_counter++;
	}

	HCF_CC_TRACE(Join, "Performing join operation for all the threads previously stopped!", (TSL_ARRAY_SIZE(_worker_threads) - _join_wait_index));
	for (size_t i = _join_wait_index; i < TSL_ARRAY_SIZE(_worker_threads); i++) {
		if ((call_result = join_thread(_worker_threads[i].id()))) {
			if (!first_bad_call_result) first_bad_call_result = call_result;
		} else threads_joined_counter++;
	}

	TSL_CRITICAL_SECTION_BEGIN(_sync_t, _sync);
		_join_ongoing = 0;
	TSL_CRITICAL_SECTION_END;

	if (!threads_joined_counter) {
		HCF_CC_TRACE(JoinErr, "Call 'join_thread' failed: no thread has been joined!", first_bad_call_result);
		TSL_SET_ERROR_INFO_AND_RETURN(-1, first_bad_call_result, ERR_INTERNAL_ERROR, "No thread was joined successfully");
	}

	return threads_joined_counter;
}

int __TSL_CLASS_NAME__::join_thread (pthread_t id) {
	HCF_CC_TRACE_FUNCTION;

	// Check the thread lives
	if (::pthread_kill(id, 0) == ESRCH) {	// No thread found? No join ...
		HCF_CC_TRACE(JoinThread, "No thread found with the given ID!", id);
		return 0;
	}

	::pthread_attr_t thr_attr;

	HCF_CC_TRACE(JoinThread, "Received a request to join the thread: retrieving its attributes!", id);
	int call_result = ::pthread_getattr_np(id, &thr_attr);
	if (call_result) {
		HCF_CC_TRACE(JoinThreadErr, "Call 'pthread_getattr_np' failed.", call_result);
		return call_result;
	}

	int detach_state = 0;

	HCF_CC_TRACE(JoinThread, "Received a request to join the thread: checking if it's joinable!", id);
	if ((call_result = ::pthread_attr_getdetachstate(&thr_attr, &detach_state))) {
		HCF_CC_TRACE(JoinThreadErr, "Call 'pthread_attr_getdetachstate' failed.", call_result);
		::pthread_attr_destroy(&thr_attr); return call_result;
	}

	::pthread_attr_destroy(&thr_attr);

	if (detach_state == PTHREAD_CREATE_DETACHED) {
		HCF_CC_TRACE(JoinThread, "The thread is detached: no join required!", id);
		return 0;
	}

	HCF_CC_TRACE(JoinThread, "Joining the thread!", id);
	call_result = ::pthread_join(id, 0);

	return call_result;
}

int __TSL_CLASS_NAME__::thread_detach_state (pthread_t id, int & detach_state) {
	HCF_CC_TRACE_FUNCTION;

	// Check the thread lives
	int call_result;
	if ((call_result = ::pthread_kill(id, 0))) {
		HCF_CC_TRACE(ThreadDetachState, "No thread found with the given ID!", id, -1);
		return call_result;
	}

	::pthread_attr_t thr_attr;

	HCF_CC_TRACE(ThreadDetachState, "Retrieving the thread attributes!", id, -1);
	if ((call_result = ::pthread_getattr_np(id, &thr_attr))) {
		HCF_CC_TRACE(ThreadDetachStateErr, "Call 'pthread_getattr_np' failed.", call_result);
		return call_result;
	}

	call_result = ::pthread_attr_getdetachstate(&thr_attr, &detach_state);

	::pthread_attr_destroy(&thr_attr);

	HCF_CC_TRACE(ThreadDetachState, "Retrieved the thread detached state!", id, detach_state);
	return (call_result ?: 0);
}

void * __TSL_CLASS_NAME__::start_routine (void * ts_instance) {
	HCF_CC_TRACE_FUNCTION;

	__TSL_CLASS_NAME__ * ts = reinterpret_cast<__TSL_CLASS_NAME__ *>(ts_instance);

	int activate_ongoing_counter = 0;
	pthread_t id = ::pthread_self();
	pid_t tid = ::syscall(SYS_gettid);
	int my_index = 0;

	TSL_CRITICAL_SECTION_BEGIN(_sync_t, ts->_sync);
		HCF_CC_TRACE(StartRoutine, "Creating the new thread!", id);
		my_index = (ts->_running_index)++;

//		ts->_worker_threads.push_back(worker_thread(::pthread_self(), ::syscall(SYS_gettid), ts->_arg_to_main, ts->_attr_of_thread));
//		ts->_worker_threads_map[tid] = worker_thread(::pthread_self(), tid, ts->_arg_to_main, ts->_attr_of_thread);

		ts->_worker_threads[my_index] = worker_thread(id, tid, ts->_arg_to_main, ts->_attr_of_thread);

		activate_ongoing_counter = --(ts->_activate_ongoing_counter);
	TSL_CRITICAL_SECTION_END;

	// Signal that the thread activation process is completed
	if (activate_ongoing_counter <= 2) ts->_activate_end_condition.broadcast();

	// Now the main function can be called
	HCF_CC_TRACE(StartRoutine, "Executing the thread main function!", id);
	const int main_return = ts->main(ts->_arg_to_main);
	HCF_CC_TRACE(StartRoutine, "The thread main function is completed!", id);

	int detach_state = 0;

	TSL_CRITICAL_SECTION_BEGIN(_sync_t, ts->_sync);
		if (ts->_join_ongoing) {
			HCF_CC_TRACE(StartRoutine, "There is a join operation ongoing: no other operation is needed!", id);
			return reinterpret_cast<void *>(main_return);
		}

		int call_result = ts->thread_detach_state(id, detach_state);

		if ((call_result == ESRCH) || (!call_result && (detach_state == PTHREAD_CREATE_DETACHED))) {
			// This thread can be deleted from internal container
			HCF_CC_TRACE(StartRoutine, "No join operation ongoing: the thread is detached, deleting it from the internal structures!", id);
				if (my_index < --(ts->_running_index)) ts->_worker_threads[my_index] = ts->_worker_threads[ts->_running_index];
		} else {
			HCF_CC_TRACE(StartRoutine, "No join operation ongoing: the thread is joinable, adding it to the join waiting set!", id);
			// Move the thread into the join waiting set
			if (my_index < --(ts->_running_index)) {
				worker_thread wt_tmp = ts->_worker_threads[my_index];
				ts->_worker_threads[my_index] = ts->_worker_threads[ts->_running_index];
				ts->_worker_threads[--(ts->_join_wait_index)] = wt_tmp;
			} else {
				if (--(ts->_join_wait_index) > my_index) ts->_worker_threads[ts->_join_wait_index] = ts->_worker_threads[my_index];
			}
		}
	TSL_CRITICAL_SECTION_END;

	return reinterpret_cast<void *>(main_return);
}

TSL_NS_HCF_CORE_END
