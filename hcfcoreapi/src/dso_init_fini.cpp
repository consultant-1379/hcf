#ifdef __cplusplus
extern "C" {
#endif

__attribute__ ((constructor))
void tsl_core_init () {}

__attribute__ ((destructor))
void tsl_core_fini () {}

#ifdef __cplusplus
}
#endif
