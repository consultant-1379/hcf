#include <unistd.h>

#include "class1.h"

#define TRACEPOINT_DEFINE
#define TRACEPOINT_PROBE_DYNAMIC_LINKAGE
#include "com_ericsson_test_common.h"
#include "hcf/trace/tracing_macros.h"

HCF_CC_FUNCTION_TRACER_CLASS_DEFINE

int main () {
	HCF_CC_TRACE_FUNCTION;

	class1 c1;
	c1.activate();

	HCF_CC_TRACE(Common1, "Before loop!");

	for (int i = 0; i < 100; ++i) {
		HCF_CC_TRACE(Common2, i);
		::sleep(1);
	}

	HCF_CC_TRACE(Common1, "After loop!");

	char foo[] = { "ABCDEF" };
	const size_t foo_len = ::strlen(foo);
	HCF_CC_DUMP(Common1, foo, foo_len);

	return 0;
}
