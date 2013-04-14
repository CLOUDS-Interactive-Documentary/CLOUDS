/* RTcmix  - Copyright (C) 2000  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/
/* rev'd for v2.3, JGG, 20-Feb-00 */
/* major revision for 3.7 (converted to C++ source), DS, April-04 
   (see deleted .c file for earlier history)
*/

#include <stdio.h>
#include <unistd.h>
#include <math.h>       /* for fabs */
#include <assert.h>
#include <RTcmix.h>
#include "prototypes.h"
#include "buffers.h"
#include <ugens.h>
#include <sndlibsupport.h>
#include "rtdefs.h"
#include <AudioDevice.h>
#include <AudioFileDevice.h>
#include "Option.h"
#include <bus.h>

/* #define DUMP_AUDIO_TO_RAW_FILE */
/* #define USE_REAL2INT */

static int printing_dots = 0;


/* local prototypes */
static int write_to_audio_device(BufPtr out_buffer[], int samps, AudioDevice *);

#ifdef DUMP_AUDIO_TO_RAW_FILE
/* ----------------------------------------------- dump_audio_to_raw_file --- */
/* For debugging audio device writes.
*/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
static void
dump_audio_to_raw_file(OBufPtr dest, int nbytes)
{
   int         result;
   static int  fd = -1;

   if (fd == -1) {
      fd = open("dumpaudio.raw", O_RDWR | O_CREAT | O_TRUNC, 0666);
      assert(fd > 0);
      fprintf(stderr, "Dumping audio to \"dumpaudio.raw\".\n");
   }
   result = write(fd, dest, nbytes);
   assert(result != -1);
}
#endif /* DUMP_AUDIO_TO_RAW_FILE */

#ifdef USE_REAL2INT
/* ------------------------------------------------------------- real2int --- */
/* This is supposed to be more efficient than casting from float to int,
   but I didn't notice much difference.  This came from info off of the
   music-dsp list.  -JGG
*/
typedef long int32;
static double _double2fixmagic = 68719476736.0 * 1.5;
static int32 _shiftamt = 16;
#if MUS_LITTLE_ENDIAN    /* processor is little-endian */
   #define iman_   0
#else
   #define iman_   1
#endif

static INLINE int
real2int(float val)
{
   double x = (double) val;
   x = x + _double2fixmagic;
   return ((int32 *) &x)[iman_] >> _shiftamt;
}
#endif /* USE_REAL2INT */


#ifdef MAXMSP

#ifndef IOS
extern float *maxmsp_outbuf; // set in mm_rtsetparams()
#define OUT_GAIN_FACTOR .000030517578125 // BGG mm -- normalizing (1.0/32768.0)

#else // IOS
extern short *maxmsp_outbuf; // set in mm_rtsetparams()
#endif // IOS

#endif // MAXMSP


/* ------------------------------------------------ write_to_audio_device --- */
static int
write_to_audio_device(BufPtr out_buffer[], int samps, AudioDevice *device)
{
#ifndef MAXMSP
	return device->sendFrames(out_buffer, samps) == samps ? 0 : -1;

#else // MAXMSP
// BGG mm
// FIXME:  If max/msp was set up with buffers filled as an AudioDevice,
// then this would be a much better world
#ifdef IOS
	short *out;
#else
   float *out;
#endif
   int i,j;
   int bsamps, nchans;

// BGG mm
// maxmsp_outbuf is a pointer to a max/msp buffer, passed in via
// maxmsp_rtsetparams

	bsamps = RTcmix::bufsamps();
	nchans = RTcmix::chans();
	out = maxmsp_outbuf;
	for(i = 0; i < bsamps; i++)
		for (j = 0; j < nchans; j++) {
#ifdef IOS
			*out++ = out_buffer[j][i];
#else
			*out++ = out_buffer[j][i] * OUT_GAIN_FACTOR;
#endif
			if (Option::reportClipping()) {
				if (ABS(out_buffer[j][i]) > 32768.0)
					rtcmix_warn("CLIPPING","val: %f	channel: %d",out_buffer[j][i], j);
			}
		}

	return 0;
#endif // MAXMSP
}

/* ---------------------------------------------------------- rtsendzeros --- */
/* Send a buffer of zeros to the audio output device, and to the output sound
   file if <also_write_to_file> is true.

   NOTE NOTE NOTE: This clears all the global out_buffers!
*/
int
RTcmix::rtsendzeros(AudioDevice *device, int also_write_to_file)
{
   int   err = 0;

   clear_output_buffers();

   if (Option::play()) {
      err = ::write_to_audio_device(out_buffer, bufsamps(), device);
      if (err) {
         rtcmix_warn("rtsendzeros error", "%s\n", device->getLastError());
		 return err;
	  }
   }
   return err;
}


/* ---------------------------------------------------------- rtsendsamps --- */
/* Called by the scheduler to write the output buffer to the audio device
   and/or a sound file.   All format conversion and limting happens inside
   the AudioDevice.
*/

int
RTcmix::rtsendsamps(AudioDevice *device)
{
   int   err = 0;
   const bool playing = Option::play();
   
   /* If we're writing to a file, and not playing, print a dot to show
      we've output one buffer.
   */
   if (!playing && rtfileit && Option::print()) {
      printing_dots = 1;
      printf(".");    /* no '\n' */
   }
   err = ::write_to_audio_device(out_buffer, bufsamps(), device);
   if (err != 0) {
      rtcmix_warn("rtsendsamps error", "%s\n", device->getLastError());
   }
   return err;
}


/* -------------------------------------------------------- rtreportstats --- */
// BGG -- this isn't used in maxmsp
void
RTcmix::rtreportstats(AudioDevice *device)
{
   static const double dbref = ::dbamp(32768.0);
	
   if (Option::checkPeaks()) {
      BUFTYPE peaks[MAXBUS];
      long peaklocs[MAXBUS];
      printf("\nPeak amplitudes of output:\n");
      for (int n = 0; n < NCHANS; n++) {
         peaks[n] = device->getPeak(n, &peaklocs[n]);
         double peak_dbfs = ::dbamp(peaks[n]) - dbref;
         printf("  channel %d: %12.6f (%6.2f dBFS) at frame %ld (%g seconds)\n",
                n, peaks[n], peak_dbfs, peaklocs[n], (float) peaklocs[n] / SR);
      }
   }
}


