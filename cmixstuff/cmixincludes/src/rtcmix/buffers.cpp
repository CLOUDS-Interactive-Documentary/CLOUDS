/* RTcmix  - Copyright (C) 2000  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/
/* Functions to handle allocation of audio buffers and copying between them.
                                                             -JGG, 17-Feb-00
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <RTcmix.h>
#include "buffers.h"
#include <bus.h>
#include <assert.h>

/* #define NDEBUG */     /* define to disable asserts */


/* -------------------------------------- copy_interleaved_buf_to_one_buf --- */
/* Copy the specified channel of an interleaved buffer to a one-channel
   buffer. Buffers must be of same type (e.g., float).
*/
void
copy_interleaved_buf_to_one_buf(
      BufPtr         dest,            /* buffer containing one chan */
      const BufPtr   src,             /* interleaved buffer */
      int            src_chans,       /* chans in interleaved buffer */
      int            src_chan,        /* channel to copy from */
      int            dest_frames)     /* frames in destination buffer */
{
   int   i, j;

   for (i = 0, j = src_chan; i < dest_frames; i++, j += src_chans)
      dest[i] = src[j];
}


/* -------------------------------------- copy_one_buf_to_interleaved_buf --- */
/* Copy a one-channel buffer into the specified channel of an interleaved
   buffer. Buffers must be of same type (e.g., float).
*/
void
copy_one_buf_to_interleaved_buf(
      BufPtr         dest,            /* interleaved buffer */
      const BufPtr   src,             /* buffer containing one chan */
      int            dest_chans,      /* chans in interleaved buffer */
      int            dest_chan,       /* channel to copy into */
      int            dest_frames)     /* frames in destination buffer */
{
   int   i, j;

   for (i = 0, j = dest_chan; i < dest_frames; i++, j += dest_chans)
      dest[j] = src[i];
}


/* ------------------------------------------ copy_interleaved_buf_to_buf --- */
/* Copy one channel between two interleaved buffers. Buffers must be
   of same type (e.g., float).
*/
void
copy_interleaved_buf_to_buf(
      BufPtr         dest,            /* interleaved buffer */
      const BufPtr   src,             /* interleaved buffer */
      int            dest_chans,      /* number of chans interleaved */
      int            dest_chan,       /* chan to copy into */
      int            dest_frames,     /* frames in dest buffer */
      int            src_chans,       /* number of chans interleaved */
      int            src_chan)        /* chan to copy from */
{
   int j = src_chan;
   int k = dest_chan;
   for (int i = 0; i < dest_frames; i++) {
      dest[k] = src[j];
      j += src_chans;
      k += dest_chans;
   }
}


/* -------------------------------------------------------- init_buf_ptrs --- */
void
RTcmix::init_buf_ptrs()
{
   int   i;

   for (i = 0; i < MAXBUS; i++) {
      audioin_buffer[i] = NULL;
      aux_buffer[i] = NULL;
      out_buffer[i] = NULL;
   }
}

/* ------------------------------------------------ clear_audioin_buffers --- */
// FIXME: not sure we need to do this ever  -JGG
/* Called from inTraverse. */
#ifdef NO_LONGER_USED
void
RTcmix::clear_audioin_buffers()
{
   int   i, j;

   for (i = 0; i < MAXBUS; i++) {
      BufPtr buf = audioin_buffer[i];
      if (buf != NULL)
         for (j = 0; j < RTBUFSAMPS; j++)
            buf[j] = 0.0;
   }
}
#endif

/* ---------------------------------------------------- clear_aux_buffers --- */
/* Called from inTraverse. */
void
RTcmix::clear_aux_buffers()
{
   int   i, j;

   for (i = 0; i < MAXBUS; i++) {
      BufPtr buf = aux_buffer[i];
      if (buf != NULL)
         for (j = 0; j < RTBUFSAMPS; j++)
            buf[j] = 0.0;
   }
}

/* ------------------------------------------------- clear_output_buffers --- */
/* Called from inTraverse. */
void
RTcmix::clear_output_buffers()
{
   int   i, j;

   for (i = 0; i < NCHANS; i++) {          /* zero just the ones in use */
      BufPtr buf = out_buffer[i];
      for (j = 0; j < RTBUFSAMPS; j++)
         buf[j] = 0.0;
   }
}


/* ----------------------------------------------------- allocate_buf_ptr --- */
static BufPtr
allocate_buf_ptr(int nsamps)       /* samples, not frames */
{
   BufPtr buf_ptr;

   buf_ptr = (BUFTYPE *) calloc(nsamps, sizeof(BUFTYPE));

   return buf_ptr;
}


/* ---------------------------------------------- allocate_audioin_buffer --- */
/* Allocate one of the global audio input bus buffers.
   Called from rtinput.
*/
int
RTcmix::allocate_audioin_buffer(short chan, int nsamps)
{
   BufPtr buf_ptr;

   assert(chan >= 0 && chan < MAXBUS);

   if (audioin_buffer[chan] == NULL) {
      buf_ptr = ::allocate_buf_ptr(nsamps);
      assert(buf_ptr != NULL);
      audioin_buffer[chan] = buf_ptr;
   }

   return 0;
}


/* -------------------------------------------------- allocate_aux_buffer --- */
/* Allocate one of the global aux bus buffers.
   Called from bus_config.
*/
int
RTcmix::allocate_aux_buffer(short chan, int nsamps)
{
   BufPtr buf_ptr;

   assert(chan >= 0 && chan < MAXBUS);

   if (aux_buffer[chan] == NULL) {
      buf_ptr = ::allocate_buf_ptr(nsamps);
      assert(buf_ptr != NULL);
      aux_buffer[chan] = buf_ptr;
   }

   return 0;
}


/* -------------------------------------------------- allocate_out_buffer --- */
/* Allocate one of the global out bus buffers.
   Called from rtsetparams.
*/
int
RTcmix::allocate_out_buffer(short chan, int nsamps)
{
   BufPtr buf_ptr;

   assert(chan >= 0 && chan < MAXBUS);

   if (out_buffer[chan] == NULL) {
      buf_ptr = ::allocate_buf_ptr(nsamps);
      assert(buf_ptr != NULL);
      out_buffer[chan] = buf_ptr;
   }

   return 0;
}

// Cleanup routine

void
RTcmix::free_buffers()
{
	for (int chan = 0; chan < MAXBUS; ++chan)
	{
		if (audioin_buffer[chan]) {
			free(audioin_buffer[chan]);
			audioin_buffer[chan] = NULL;
		}
		if (aux_buffer[chan]) {
			free(aux_buffer[chan]);
			aux_buffer[chan] = NULL;
		}
		if (out_buffer[chan]) {
			free(out_buffer[chan]);
			out_buffer[chan] = NULL;
		}
	}
}
