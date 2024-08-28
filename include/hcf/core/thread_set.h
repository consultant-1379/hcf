#ifndef __INCLUDE_GUARD_CLASS_CORE_THREAD_SET__
#define __INCLUDE_GUARD_CLASS_CORE_THREAD_SET__ thread_set

/*
 *	@file thread_set.h
 *	@brief
 *	@author xnicmut (Nicola Muto)
 *	@date 2014-11-19
 *
 *	COPYRIGHT Ericsson AB, 2014
 *	All rights reserved.
 *
 *	The information in this document is the property of Ericsson.
 *	Except as specifically authorized in writing by Ericsson, the receiver of
 *	this document shall keep the information contained herein confidential and
 *	shall protect the same in whole or in part from disclosure and dissemination
 *	to third parties. Disclosure and disseminations to the receivers employees
 *	shall only be made on a strict need to know basis.
 *
 *	REVISION INFO
 *	+==========+============+==============+=====================================+
 *	| REV      | DATE       | AUTHOR       | DESCRIPTION                         |
 *	+==========+============+==============+=====================================+
 *	| 2014-001 | 2014-11-19 | xnicmut      | Creation and first revision.        |
 *	+==========+============+==============+=====================================+
 */

#include <stdint.h>
#include <sys/types.h>
#include <pthread.h>

//#include <vector>
//#include <map>

#include "macros.h"
#include "error_info_provider.h"
#include "thread_mutex.h"
#include "thread_condition.h"
#include "thread.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_CORE_THREAD_SET__

TSL_NS_HCF_CORE_BEGIN

class __TSL_CLASS_NAME__ : public error_info_provider {
	//==============//
	// Constructors //
	//==============//
public:
	inline TSL_CTOR ()
	TSL_CTOR_INIT_LIST(error_info_provider(), _sync(), _activate_end_condition(), _activate_ongoing_counter(0),
			_join_ongoing(0), _arg_to_main(0), _attr_of_thread(TSL_THREAD_NULL_MASK), _worker_threads(),
			_running_index(0), _join_wait_index(TSL_ARRAY_SIZE(_worker_threads)))
	{}

private:
	TSL_CTOR (const __TSL_CLASS_NAME__ &);

	//============//
	// Destructor //
	//============//
public:
	inline virtual TSL_DTOR () {}

	//===========//
	// Functions //
	//===========//
public:
	inline int activate () { return activate(TSL_THREAD_NULL_MASK, 1, 0); }
	inline int activate (uint32_t flags) { return activate(flags, 1, 0); }
	inline int activate (uint32_t flags, int n_threads) { return activate(flags, n_threads, 0); }
	inline int activate (uint32_t flags, void * arg_to_main) { return activate(flags, 1, arg_to_main); }
	inline int activate (void * arg_to_main) { return activate(TSL_THREAD_NULL_MASK, 1, arg_to_main); }
	int activate (uint32_t flags, int n_threads, void * arg_to_main);

	int join ();

	inline int threads_running () const { return _running_index; }
	inline int threads_join_waiting () const { return (TSL_ARRAY_SIZE(_worker_threads) - _join_wait_index); }

protected:
	virtual int main (void * arg) = 0;

private:
	int join_thread (pthread_t id);

	int thread_detach_state (pthread_t id, int & detach_state);

	static void * start_routine (void * ts_instance);

	//===========//
	// Operators //
	//===========//
private:
	__TSL_CLASS_NAME__ & operator= (const __TSL_CLASS_NAME__ &);


	//=============//
	// Sub classes //
	//=============//
private:
	class worker_thread : public thread {
	public:
		inline worker_thread () : thread() {}

		inline worker_thread (pthread_t id, pid_t tid, void * arg, uint32_t attr) : thread(id, tid, arg, attr) {}

		inline worker_thread (const worker_thread & rhs) : thread(rhs) {}

		inline virtual ~worker_thread () {}

		inline virtual int main (void * /*arg*/) { return 0; }

		inline worker_thread & operator= (const worker_thread & rhs) {
			thread::operator=(rhs);
			return *this;
		}
	};

	//========//
	// Fields //
	//========//
private:
	typedef thread_mutex_recursive _sync_t;
	_sync_t _sync;
	thread_condition _activate_end_condition;

	int _activate_ongoing_counter;
	int _join_ongoing;

	void * _arg_to_main;
	uint32_t _attr_of_thread;

//	std::vector<worker_thread> _worker_threads;
//	std::map<pid_t, worker_thread> _worker_threads_map;

	worker_thread _worker_threads[32];
	int _running_index;
	int _join_wait_index;
};

TSL_NS_HCF_CORE_END

#endif /* __INCLUDE_GUARD_CLASS_CORE_THREAD_SET__ */
