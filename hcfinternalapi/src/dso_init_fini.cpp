#ifdef __cplusplus
extern "C" {
#endif

__attribute__ ((constructor))
void tsl_internal_init () {}

__attribute__ ((destructor))
void tsl_internal_fini () {}

#ifdef __cplusplus
}
#endif
