#ifndef __INCLUDE_GUARD_CLASS_EPOLL_EVENT_REACTOR_TEST__
#define __INCLUDE_GUARD_CLASS_EPOLL_EVENT_REACTOR_TEST__ epoll_event_reactor_test

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_EPOLL_EVENT_REACTOR_TEST__

class __TSL_CLASS_NAME__ {
	//==============//
	// Constructors //
	//==============//
public:
	inline TSL_CTOR () { /*TSL_TRACE_ON_SCREEN;*/ }

private:
	TSL_CTOR (const __TSL_CLASS_NAME__ &);

	//============//
	// Destructor //
	//============//
public:
	inline TSL_DTOR () { /*TSL_TRACE_ON_SCREEN;*/ }

	//===========//
	// Operators //
	//===========//
public:
	int operator() ();

private:
	__TSL_CLASS_NAME__ & operator= (const __TSL_CLASS_NAME__ &);
};

#endif /* __INCLUDE_GUARD_CLASS_EPOLL_EVENT_REACTOR_TEST__ */
