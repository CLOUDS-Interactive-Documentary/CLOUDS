// handle.h
// Declarations for opaque handle manipulators

#include <minc_internal.h>

#ifdef __cplusplus
extern "C" {
#endif
void ref_handle(MincHandle);
void unref_handle(MincHandle);
#ifdef __cplusplus
}
#endif

