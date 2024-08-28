#include <unistd.h>

#define TRACEPOINT_DEFINE
#define TRACEPOINT_PROBE_DYNAMIC_LINKAGE
#include "com_ericsson_test_class1.h"
#include "hcf/trace/tracing_macros.h"

#include "class1.h"

HCF_CC_FUNCTION_TRACER_CLASS_DEFINE

#undef __TSL_CLASS_NAME__
#define __TSL_CLASS_NAME__ __INCLUDE_GUARD_CLASS_TRACE_TEST_CLASS1__

__TSL_CLASS_NAME__::TSL_CTOR () : thread() {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(MyEvent1, "In class constructor...");
}

__TSL_CLASS_NAME__::TSL_DTOR () {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(MyEvent1, "In class destructor...");
}

int __TSL_CLASS_NAME__::main (void * /*arg*/) {
	HCF_CC_TRACE_FUNCTION;
	do_something();
	return 0;
}

void __TSL_CLASS_NAME__::do_something () {
	HCF_CC_TRACE_FUNCTION;
	HCF_CC_TRACE(MyEvent1, "Before loop...");

	for (int i = 0; i < 60; ++i) {
		HCF_CC_TRACE(MyEvent2, i);
		::sleep(1);
	}

	HCF_CC_TRACE(MyEvent1, "After loop...");
}

