#ifdef __cplusplus
extern "C" {
#endif

__attribute__ ((constructor))
void tsl_trace_init () {}

__attribute__ ((destructor))
void tsl_trace_fini () {}

#ifdef __cplusplus
}
#endif
