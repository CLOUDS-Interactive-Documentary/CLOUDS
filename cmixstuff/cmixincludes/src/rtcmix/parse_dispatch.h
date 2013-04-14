#ifndef _PARSE_DISPATCH_H_
#define _PARSE_DISPATCH_H_ 1

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "rtcmix_types.h"

int parse_dispatch(const char *funcname, const Arg arglist[], const int n_args,
   Arg *return_val);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _PARSE_DISPATCH_H_ */
