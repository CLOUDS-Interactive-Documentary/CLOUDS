/* RTcmix  - Copyright (C) 2000  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/
#define DBUG
//#define DENORMAL_CHECK
#include <pthread.h>
#include <sys/resource.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>

#include "RTcmixMain.h"
#include "prototypes.h"
#include <ugens.h>
#include <ug_intro.h>
#include "version.h"
#include "rt.h"
#include "heap.h"
#include "sockdefs.h"
#include "notetags.h"           // contains defs for note-tagging
#include "dbug.h"
#include <MMPrint.h>

#ifdef MAXMSP
// BGG -- this is how you print to the console.app now in max/msp
extern "C" {
extern
void cpost(char *fmt, ...);
}
#endif

extern "C" {
#ifdef SGI
  void flush_all_underflows_to_zero();
#endif
#ifdef LINUX
  void sigfpe_handler(int sig);
#endif
}


/* ----------------------------------------------------- detect_denormals --- */
/* Unmask "denormalized operand" bit of the x86 FPU control word, so that
   any operations with denormalized numbers will raise a SIGFPE signal,
   and our handler will be called.  NOTE: This is for debugging only!
   This will not tell you how many denormal ops there are, so just because
   the exception is thrown doesn't mean there's a serious problem.  For
   more info, see: http://www.musicdsp.org/files/other001.txt.
*/
#ifdef LINUX
#ifdef DENORMAL_CHECK
static void
detect_denormals()
{
   #include <fpu_control.h>
   int cw = 0;
   _FPU_GETCW(cw);
   cw &= ~_FPU_MASK_DM;
   _FPU_SETCW(cw);
}
#endif /* DENORMAL_CHECK */
#endif /* LINUX */


#ifndef MAXMSP
/* ----------------------------------------------------------------- main --- */
int
main(int argc, char *argv[], char **env)
{
#ifdef LINUX
 #ifdef DENORMAL_CHECK
   detect_denormals();
 #endif
   signal(SIGFPE, sigfpe_handler);          /* Install signal handler */
#endif /* LINUX */
#ifdef SGI
   flush_all_underflows_to_zero();
#endif

	bzero(MMPrint::mm_print_buf, SIZEOF_MMPRINTBUF);
	MMPrint::mm_print_ptr = MMPrint::mm_print_buf;

   RTcmixMain app(argc, argv, env);
   app.run();

   return 0;
}

#else // MAXMSP
/* ----------------------------------------------------------- rtcmixmain --- */
RTcmixMain *app;

extern "C" {
int
rtcmixmain()	// BGG mm -- now called this for max/msp
{
	bzero(MMPrint::mm_print_buf, SIZEOF_MMPRINTBUF);
	MMPrint::mm_print_ptr = MMPrint::mm_print_buf;

// BGG no argc and argv in max/msp version mm
   app = new RTcmixMain();
   app->run(); // in max/msp this just sets it all up...

   return 0;
}


// BGG -- these will be called from max/msp, an instrument will set
// the *_ready value to signal to return something other than NULL
// max/msp will then respond accordingly

int bang_ready = 0;

int check_bang()
{  
   if (bang_ready == 1) {
      bang_ready = 0;
      return(1);
   } else {
      return(0);
   }
}


int vals_ready = 0;
float maxmsp_vals[MAXDISPARGS];

int check_vals(float thevals[])
{  
   int i, tvals;
   
   if (vals_ready > 0) { // vals_ready will contain how many vals to return
      for (i = 0; i < vals_ready; i++) thevals[i] = maxmsp_vals[i];
      tvals = vals_ready;
      vals_ready = 0;
      return(tvals);
   } else {
      return(0);
   }
}

char *get_print()
{
	return(MMPrint::mm_print_buf);
}

void reset_print()
{
	bzero(MMPrint::mm_print_buf, SIZEOF_MMPRINTBUF);
	MMPrint::mm_print_ptr = MMPrint::mm_print_buf;
}

#ifdef IOS
// BGG ii -- we can fill these directly in iOS, so we pass them in below
extern short *maxmsp_outbuf; // defined in mm_rtsetparams.cpp
extern short *maxmsp_inbuf; // defined in mm_rtsetparams.cpp

void pullTraverse(short *inbuf, short *outbuf)
{
	maxmsp_inbuf = inbuf;
	maxmsp_outbuf = outbuf;

	app->inTraverse(NULL, NULL);
}
#else // not IOS
void pullTraverse()
{
	app->inTraverse(NULL, NULL);
}
#endif // IOS


// BGG ii -- mm_inbuf/mm_outbuf come as NULL, they are passed in pulltraverse()
int maxmsp_rtsetparams(float sr, int nchans, int vecsize, float *mm_inbuf, float *mm_outbuf)
{
	int status;

	status = (int)app->mm_rtsetparams(sr, nchans, vecsize, mm_inbuf, mm_outbuf);

	return(status);
}


// these are set from inlets on the rtcmix~ object, using PFields to
// control the Instruments
// rtcmix~ is set to constrain up to a max of 19 inlets for PFields
float inletvals[20];

void pfield_set(int inlet, float pval)
{
	inletvals[inlet-1] = pval;
}


// use this to pass in info from the max/msp [buffer~] object.  All
// we need to know is the number of frames, nchans, and starting address
// flags and globals to support this stuff, arg!
#define MAX_MM_BUFS 50
mm_buf mm_bufs[MAX_MM_BUFS]; // mm_buf defined in rtdefs.h
int mm_buf_input = -1; // used in rtgetin() and then rtsetinput()
int n_mm_bufs = 0;

void buffer_set(char *bufname, float *bufstart, int nframes, int nchans, int modtime)
{
	int i;
	int foundit;


	if (strlen(bufname) >= MAX_MM_BUFNAME) { // defined in rtdefs.h
		rtcmix_warn("bufset", "[buffer~] name has to be < %d chars", MAX_MM_BUFNAME);
		return;
	}

	// check to see if this is already set
	foundit = 0;
	for (i = 0; i < n_mm_bufs; i++) {
			if (strcmp(bufname, mm_bufs[i].name) == 0) {
				foundit = 1;
				if (modtime > mm_bufs[i].mm_modtime) { // buffer was modified
					mm_bufs[i].mm_bufstart = bufstart;
					mm_bufs[i].mm_buf_nframes = nframes;
					mm_bufs[i].mm_buf_chans = nchans;
					mm_bufs[i].mm_modtime = modtime;
				}
				break;
			}
	}

	// it's a new one
	if (foundit == 0) {
		if (n_mm_bufs >= MAX_MM_BUFS) {
			rtcmix_warn("bufset", "we can only do %d [buffer~] buffers at present, sorry!", MAX_MM_BUFS);
			return;
		}
		strcpy(mm_bufs[n_mm_bufs].name, bufname);
		mm_bufs[n_mm_bufs].mm_bufstart = bufstart;
		mm_bufs[n_mm_bufs].mm_buf_nframes = nframes;
		mm_bufs[n_mm_bufs].mm_buf_chans = nchans;
		mm_bufs[n_mm_bufs].mm_modtime = modtime;
		n_mm_bufs++;
	}
}

// called for the [flush] message; deletes and reinstantiates the rtQueue
// and rtHeap, thus flushing all scheduled events in the future
void flush_sched()
{
	app->resetQueueHeap(); // in RTcmixMain.cpp
}


// this is for dynamic loading of RTcmix instruments (for development)
// only one rtcmix~ may be instantiated for this to work
// the scorefile load() system is disabled in rtcmix~
void loadinst(char *dsoname)
{
	// the dsoname should be a fully-qualified pathname to the dynlib
	app->doload(dsoname);
}

void unloadinst()
{
	// it is necessary to unload the dso directly, otherwise the dlopen()
	// system keeps it in memory
	app->unload();
}

} // end of "extern C"  BGG mm
#endif // MAXMSP

