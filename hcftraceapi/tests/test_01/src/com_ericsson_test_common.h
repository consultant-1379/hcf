#undef TRACEPOINT_PROVIDER
#define TRACEPOINT_PROVIDER com_ericsson_test_common

#undef TRACEPOINT_INCLUDE_FILE
#define TRACEPOINT_INCLUDE_FILE ./com_ericsson_test_common.h

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(_COM_ERICSSON_TEST_COMMON_H) || defined(TRACEPOINT_HEADER_MULTI_READ)
#define _COM_ERICSSON_TEST_COMMON_H

#include <lttng/tracepoint.h>

TRACEPOINT_EVENT(com_ericsson_test_common, Common1,
		TP_ARGS(const char *, msg),
		TP_FIELDS(ctf_string(string, msg)))
TRACEPOINT_LOGLEVEL(com_ericsson_test_common, Common1, TRACE_INFO)

TRACEPOINT_EVENT(com_ericsson_test_common, Common2,
		TP_ARGS(int, my_int),
		TP_FIELDS(ctf_integer(int, my_int, my_int)))
TRACEPOINT_LOGLEVEL(com_ericsson_test_common, Common2, TRACE_INFO)

TRACEPOINT_EVENT(com_ericsson_test_common, FunctionTracing,
		TP_ARGS(const char *, msg,
				const char *, function),
				TP_FIELDS(ctf_string(MSG, msg)
						ctf_string(FUNC, function)))
TRACEPOINT_LOGLEVEL(com_ericsson_test_common, FunctionTracing, TRACE_INFO)

#endif /* _COM_ERICSSON_TEST_COMMON_H */

/* This part must be outside protection */
#include <lttng/tracepoint-event.h>

#ifdef __cplusplus
}
#endif
