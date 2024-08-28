#include <pthread.h>

#include "hcf/core/macros.h"
#include "hcf/core/thread_mutex.h"

TSL_NS_HCF_CORE_BEGIN

template <>
const pthread_mutex_t
__thread_mutex_static_initializer<TSL_THREAD_MUTEX_FAST>::MUTEX_INIT_VALUE = PTHREAD_MUTEX_INITIALIZER;

template <>
const pthread_mutex_t
__thread_mutex_static_initializer<TSL_THREAD_MUTEX_RECURSIVE>::MUTEX_INIT_VALUE = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;

template <>
const pthread_mutex_t
__thread_mutex_static_initializer<TSL_THREAD_MUTEX_ERRORCHECK>::MUTEX_INIT_VALUE = PTHREAD_ERRORCHECK_MUTEX_INITIALIZER_NP;

template <>
const pthread_mutex_t
__thread_mutex_static_initializer<TSL_THREAD_MUTEX_ADAPTIVE>::MUTEX_INIT_VALUE = PTHREAD_ADAPTIVE_MUTEX_INITIALIZER_NP;

TSL_NS_HCF_CORE_END
