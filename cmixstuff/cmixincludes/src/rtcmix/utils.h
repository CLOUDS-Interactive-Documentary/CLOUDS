/* RTcmix  - Copyright (C) 2004  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/

// utils.h
// Decls of RtcHandle and other utilities.

#include "rtcmix_types.h"
#include <assert.h>
#include <stdio.h>

#ifdef __cplusplus
RtcHandle createPFieldHandle(class PField *);
RtcHandle createInstHandle(class Instrument *);
inline void refHandle(RtcHandle h)
{
	assert(h->refcount >= 0);
#ifdef DEBUG
    printf("refHandle(%p): %d -> ", h, h->refcount);
#endif
	++h->refcount;
#ifdef DEBUG
    printf("%d\n", h->refcount);
#endif
}
#endif	// __cplusplus

#ifdef __cplusplus
extern "C" {
#endif	// __cplusplus

void unrefHandle(RtcHandle);

#ifdef __cplusplus
}
#endif	// __cplusplus

#define ALLOW_INLINE_C

#ifndef __cplusplus
#ifdef ALLOW_INLINE_C
inline void refHandle(RtcHandle h)
{
	assert(h->refcount >= 0);
#ifdef DEBUG
    printf("refHandle(%p): %d -> ", h, h->refcount);
#endif
	++h->refcount;
#ifdef DEBUG
    printf("%d\n", h->refcount);
#endif
}
#else	// !ALLOW_INLINE_C
// This is macro-ized to allow inlining in C
#define refHandle(h) \
	{ \
	/* printf("refHandle(%p): %d -> ", h, h->refcount); */ \
	assert(h->refcount >= 0); \
	++h->refcount; \
	/* printf("%d\n", h->refcount); */ \
	} 

#endif	// ALLOW_INLINE_C

#endif	// !__cplusplus
