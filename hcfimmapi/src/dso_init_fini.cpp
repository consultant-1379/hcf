#ifdef __cplusplus
extern "C" {
#endif

__attribute__ ((constructor))
void tsl_imm_init () {}

__attribute__ ((destructor))
void tsl_imm_fini () {}

#ifdef __cplusplus
}
#endif
