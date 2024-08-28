#ifndef __INCLUDE_GUARD_CLASS_CALENDAR_PERIODIC_ALARM_TIMEPLAN_TEST__
#define __INCLUDE_GUARD_CLASS_CALENDAR_PERIODIC_ALARM_TIMEPLAN_TEST__ calendar_periodic_alarm_timeplan_test

#include "hcf/core/macros.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_CALENDAR_PERIODIC_ALARM_TIMEPLAN_TEST__

class __TSL_CLASS_NAME__ {
	//==============//
	// Constructors //
	//==============//
public:
	inline TSL_CTOR () {}

private:
	TSL_CTOR (const __TSL_CLASS_NAME__ &);

	//============//
	// Destructor //
	//============//
public:
	inline TSL_DTOR () {}

	//===========//
	// Operators //
	//===========//
public:
	int operator() ();

private:
	__TSL_CLASS_NAME__ & operator= (const __TSL_CLASS_NAME__ &);
};

#endif /* __INCLUDE_GUARD_CLASS_CALENDAR_PERIODIC_ALARM_TIMEPLAN_TEST__ */
