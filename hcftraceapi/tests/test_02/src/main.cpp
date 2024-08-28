#include <stdio.h>

#include "hcf/trace/com_ericsson_common_hcf.h"
#include "hcf/trace/tracing_macros.h"

HCF_CC_FUNCTION_TRACER_CLASS_DEFINE

int main () {
	HCF_CC_TRACE_FUNCTION;

	::printf("This is the beginning of the test.\n");
//	HCF_CC_TRACE(Event1, "This is the beginning of the test.");

	for (int i = 0; i < 60; ++i) {
		::printf("Iteration %d!\n", i);
//		HCF_CC_TRACE(Event2, i);
		::sleep(1);
	}

	::printf("This is the end of the test.\n");
//	HCF_CC_TRACE(Event1, "This is the end of the test.");
	return 0;
}
