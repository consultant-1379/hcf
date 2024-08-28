#ifndef __INCLUDE_GUARD_CLASS_TRACE_TEST_CLASS1__
#define __INCLUDE_GUARD_CLASS_TRACE_TEST_CLASS1__ class1

#include "hcf/core/thread.h"

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_TRACE_TEST_CLASS1__

class __TSL_CLASS_NAME__ : public TSL_NS_HCF_CORE::thread {
	//==============//
	// Constructors //
	//==============//
public:
	TSL_CTOR ();

private:
	TSL_CTOR (const __TSL_CLASS_NAME__ &);

	//============//
	// Destructor //
	//============//
public:
	virtual TSL_DTOR ();

	//===========//
	// Functions //
	//===========//
protected:
	virtual int main (void * arg);

private:
	void do_something ();

	//===========//
	// Operators //
	//===========//
private:
	__TSL_CLASS_NAME__ & operator= (const __TSL_CLASS_NAME__ &);

	//========//
	// Fields //
	//========//
private:
};

#endif /* __INCLUDE_GUARD_CLASS_TRACE_TEST_CLASS1__ */
