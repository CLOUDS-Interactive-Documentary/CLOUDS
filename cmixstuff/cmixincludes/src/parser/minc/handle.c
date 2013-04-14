// handle.c 
// Special interface code to allow reference counting on opaque handles

#include <minc_internal.h>
#include "../../rtcmix/utils.h"

void
ref_handle(MincHandle h)
{
	RtcHandle handle = (RtcHandle) h;
	refHandle(handle);
}

void
unref_handle(MincHandle h)
{
	RtcHandle handle = (RtcHandle) h;
	if (handle)
		unrefHandle(handle);
}
