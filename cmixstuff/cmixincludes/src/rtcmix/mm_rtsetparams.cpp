/* RTcmix  - Copyright (C) 2000  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/

/* Originally by Brad Garton and Doug Scott (SGI code) and Dave Topper
   (Linux code). Reworked for v2.3 by John Gibson.
   Reworked to use new AudioDevice code for v3.7 by Douglas Scott.

   This is the max/msp version of rtsetparams which initializes
   SR, NCHANS and RTBUFSAMPS.  I'm not using rtsetparams() directly
   because I am using it as a no-op so that standard RTcmix scores
   can be read directly by the rtcmix~ object.  This is called from
	max/msp via the maxmsp_rtsetparams() extern "C" function
	-- BGG mm 1/2005

	modified slightly for iPhone development -- BGG ii 7/2009
	added IOS ifdefs -- BGG 1/2013
*/

#include <RTcmix.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <errno.h>
#include <assert.h>
#include "audio_devices.h"
#include <ugens.h>
#include <Option.h>
#include "rtdefs.h"
#include <MMPrint.h>

/* #define DEBUG */

#ifdef IOS
/ BGG ii -- converted these to short pointers for direct buffer-passing
//		declared here even though they're not used here
short *maxmsp_outbuf;
short *maxmsp_inbuf;
#else
float *maxmsp_outbuf; // written to in rtsendsamps (write_to_audio_device)
float *maxmsp_inbuf;
#endif // IOS


/* ------------------------------------------------------- mm_rtsetparams --- */
/* Minc function that sets output sampling rate (p0), maximum number of
   output channels (p1), and (optionally) I/O buffer size (p2).

   On SGI, the optional p3 lets you specify the output port as a string
   (e.g., "LINE", "DIGITAL", etc.).

   Based on this information, rtsetparams allocates a mono output buffer
   for each of the output channels, and opens output devices.

	BGG ii -- we don't use the *mm_inbuf and *mm_outbuf params for iPhone
		work; these are passed in through the calls to pulltraverse()
*/
double
RTcmix::mm_rtsetparams(float sr, int nchans, int vecsize, float *mm_inbuf, float *mm_outbuf)
{
   int         i, status;
   int         verbose = Option::print();
   int         play_audio = Option::play();
   int         record_audio = Option::record(true);
#ifdef SGI
   static char *out_port_str = NULL;
#endif /* SGI */

// FIXME: Need better names for NCHANS and RTBUFSAMPS. -JGG

   SR = sr;
   NCHANS = nchans;
   RTBUFSAMPS = vecsize;

#ifndef IOS
	maxmsp_inbuf = mm_inbuf; // passed in from max/msp via maxmsp_rtsetparams()
	maxmsp_outbuf = mm_outbuf; // passed in from max/msp via maxmsp_rtsetparams()
#endif

   int numBuffers = Option::bufferCount();

   if (SR <= 0.0) {
	   die("rtsetparams", "Sampling rate must be greater than 0.");
	   return -1;
   }

   if (NCHANS > MAXBUS) {
      die("rtsetparams", "You can only have up to %d output channels.", MAXBUS - 1);
	  return -1;
   }

   /* play_audio is true unless user has called set_option("audio_off") before
      rtsetparams. This would let user run multiple jobs, as long as only one
      needs the audio drivers.  -JGG

	  record_audio is false unless user has called set_option("full_duplex_on")
	  or has explicity turned record on by itself via set_option("record_on"),
	  or has already called rtinput("AUDIO").  -DS
   */
   if (play_audio || record_audio) {
      int nframes = RTBUFSAMPS;
		
	  if (create_audio_devices(record_audio, play_audio, 
	  						   NCHANS, SR, &nframes, numBuffers) < 0)
	  	return -1;

      /* This may have been reset by driver. */
      RTBUFSAMPS = nframes;
   }

   /* inTraverse waits for this. Set it even if play_audio is false! */
   pthread_mutex_lock(&audio_config_lock);
   audio_config = 1;
   pthread_mutex_unlock(&audio_config_lock);

   if (verbose >= MMP_PRINTALL)
      rtcmix_advise(NULL, "Audio set:  %g sampling rate, %d channels\n", SR, NCHANS);

   /* Allocate output buffers. Do this *after* opening audio devices,
      in case OSS changes our buffer size, for example.
   */
   for (i = 0; i < NCHANS; i++) {
      allocate_out_buffer(i, RTBUFSAMPS);
   }

   rtsetparams_called = 1;	/* Put this at end to allow re-call due to error */

   return 0;
}

