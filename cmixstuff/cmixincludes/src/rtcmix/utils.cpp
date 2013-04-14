/* RTcmix  - Copyright (C) 2004  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/
// utils.cpp
// Functions for creating Handles and tables
//

#include "utils.h"
#include <PField.h>
#include <stdlib.h>
#include <assert.h>
#include <ugens.h>

RtcHandle
createPFieldHandle(PField *pfield)
{
	RtcHandle handle = (RtcHandle) malloc(sizeof(struct _handle));
	if (handle) {
		handle->type = PFieldType;
		handle->ptr = (void *) pfield;
#ifdef DEBUG
		printf("\tcreated PField RtcHandle %p\n", handle);
    	printf("\t\trefing PField %p\n", pfield);
#endif
		pfield->ref();
		handle->refcount = 0;
	}
	return handle;
}

RtcHandle
createInstHandle(Instrument *inst)
{
	RtcHandle handle = (RtcHandle) malloc(sizeof(struct _handle));
	if (handle) {
		handle->type = InstrumentPtrType;
		handle->ptr = (void *) inst;
#ifdef DEBUG
		printf("\tcreated Instrument RtcHandle %p\n", handle);
    	printf("\t\t (not refing) inst = %p\n", inst);
#endif
		handle->refcount = 0;
	}
	return handle;
}

void
unrefHandle(RtcHandle h)
{
#ifdef DEBUG
    printf("unrefHandle(%p): %d -> ", h, h->refcount);
#endif
	assert(h->refcount >= 0);
	--h->refcount;
#ifdef DEBUG
    printf("%d\n", h->refcount);
#endif
    if (h->refcount == 0)
    {
#ifdef DEBUG
        printf("\tfreeing RtcHandle %p\n", h);
		h->refcount = 0xdeaddead;	// for debugging
#endif
		if (h->type == PFieldType) {
#ifdef DEBUG
			printf("\tunrefing PField %p\n", h->ptr);
#endif
			RefCounted::unref((RefCounted *)h->ptr);
		}
		else if (h->type == InstrumentPtrType) {
#ifdef DEBUG
			printf("\tnot yet unrefing inst %p\n", h->ptr);
#endif
		}
		else {
			rtcmix_warn("unrefHandle", "unrefHandle: unknown handle type!");
			return;
		}
        free(h);
    }
}	


