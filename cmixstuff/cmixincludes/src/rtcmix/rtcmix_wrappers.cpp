/* This file contains C-scope wrappers for RTcmix static methods which at this
   point still need to be called from C.  -DS, 8/2004
 */
 
#include "prototypes.h"
#include <RTcmix.h>

int rtsetparams_was_called()
{
	return RTcmix::rtsetparams_was_called();
}

float SR()
{
	return RTcmix::sr();
}

void set_SR(float srate)
{
	RTcmix::SR = srate;
}
