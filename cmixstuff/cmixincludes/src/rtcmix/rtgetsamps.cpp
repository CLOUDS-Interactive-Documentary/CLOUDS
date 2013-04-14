/* RTcmix  - Copyright (C) 2000  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/
/* rev'd for v2.3, JGG, 20-Feb-00 */
/* rev'd for v3.7 (converted to C++ source), DS, April-04 */

#include <RTcmix.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include "prototypes.h"
#include "buffers.h"
#include "rtdefs.h"
#include <AudioDevice.h>
#include <ugens.h>
#include "Option.h"


#ifdef MAXMSP

#ifndef IOS
extern float *maxmsp_inbuf; // set in mm_rtsetparams()
#define IN_GAIN_FACTOR 32768.0 // BGG -- for normalizing

#else // IOS
extern short *maxmsp_inbuf; // set in mm_rtsetparams()
#endif // IOS

#endif // MAXMSP


/* ----------------------------------------------------------- rtgetsamps --- */
void
RTcmix::rtgetsamps(AudioDevice *inputDevice)
{
	assert(Option::record() == true);

#ifndef MAXMSP
	if (inputDevice->getFrames(audioin_buffer, RTBUFSAMPS) < 0)
	{
		rtcmix_warn("rtgetsamps", "%s\n", inputDevice->getLastError());
	}

#else // MAXMSP
// BGG mm
// maxmsp_inbuf is a pointer to a max/msp buffer, passed via maxmsp_rtsetparams
#ifdef IOS
	short *in = maxmsp_inbuf;
#else
	float *in = maxmsp_inbuf;
#endif

	for(int i = 0; i < RTBUFSAMPS; i++)
		for (int j = 0; j < chans(); j++)
#ifdef IOS
			audioin_buffer[j][i] = *in++;
#else
			audioin_buffer[j][i] = *in++ * IN_GAIN_FACTOR;
#endif

	return;
#endif // MAXMSP
}

