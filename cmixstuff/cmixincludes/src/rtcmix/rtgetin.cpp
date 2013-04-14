/* RTcmix  - Copyright (C) 2000  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/

/* Functions to fill instrument input buffers from file, aux bus or audio dev.
                                                             -JGG, 17-Feb-00
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <RTcmix.h>
#include "prototypes.h"
#include <sndlibsupport.h>
#include "byte_routines.h"
#include <Instrument.h>
#include "BusSlot.h"
#include "InputFile.h"
#include <ugens.h>
#include <rtdefs.h>
#include <assert.h>

/* ------------------------------------------------------------ rtinrepos --- */
/* Change rt input sound file position pointer, in preparation for a
   subsequent call to rtgetin.  Designed for use by instruments that 
   want to reposition the input file arbitrarily (like REVMIX, which
   reads a file backwards).

   <whence> works just as in lseek: it specifies the point from which
   to move <frames> number of frames.  <frames> can be negative.
   Values of <whence> are SEEK_SET, SEEK_CUR and SEEK_END, all defined
   in <unistd.h>.

   This function doesn't actually do an lseek on the file yet -- that
   happens inside of rtgetin -- this merely updates the instrument's
   <fileOffset> for the next read.
*/
int
Instrument::rtinrepos(Instrument *inst, int frames, int whence)
{
   int   fdindex;
   off_t offset;

   fdindex = inst->_input.fdIndex;

   if (fdindex == NO_DEVICE_FDINDEX || RTcmix::isInputAudioDevice(fdindex)) {
      rtcmix_warn("rtinrepos", "request to reposition input, but input is not a file.");
	  return -1;
   }

   offset = RTcmix::seekInputFile(fdindex, frames, inst->_input.inputchans, whence);

   switch (whence) {
      case SEEK_SET:
         assert(offset >= 0);
         inst->_input.fileOffset = offset;
         break;
      case SEEK_CUR:
         inst->_input.fileOffset += offset;
         break;
      case SEEK_END:
         rtcmix_warn("rtinrepos", "SEEK_END unimplemented\n");
#ifndef MAXMSP
         exit(1);
#endif
         break;
      default:
         rtcmix_warn("rtinrepos", "invalid <whence>\n");
#ifndef MAXMSP
         exit(1);
#endif
         break;
   }

   return 0;
}

off_t 
RTcmix::seekInputFile(int fdIndex, int frames, int chans, int whence)
{
    const int format = inputFileTable[fdIndex].dataFormat();
    const int bytes_per_samp = ::mus_data_format_to_bytes_per_sample(format);

   off_t bytes = frames * chans * bytes_per_samp;

   switch (whence) {
      case SEEK_SET:
         assert(bytes >= 0);
         bytes += inputFileTable[fdIndex].dataLocation();
         break;
      case SEEK_CUR:
         break;
	  default:
         break;
   }
   return bytes;
}


/* -------------------- RTcmix::readFromAuxBus -------- [was get_aux_in] --- */
void
RTcmix::readFromAuxBus(
      BufPtr      dest,             /* interleaved buffer from inst */
      int         dest_chans,       /* number of chans interleaved */
      int         dest_frames,      /* frames in interleaved buffer */
      const short src_chan_list[],  /* list of auxin-bus chan nums from inst */
      short       src_chans,        /* number of auxin-bus chans to copy */
      int  	      output_offset)
{
   assert(dest_chans >= src_chans);

   for (int n = 0; n < src_chans; n++) {
      int chan = src_chan_list[n];

      BufPtr src = aux_buffer[chan];
      assert(src != NULL);

      /* The inst might be playing only the last part of a buffer. If so,
         we want it to read the corresponding segment of the aux buffer.
      */
      src += output_offset;

      ::copy_one_buf_to_interleaved_buf(dest, src, dest_chans, n, dest_frames);
   }
}


/* ------------------------RTcmix::readFromAudioDevice [was get_audio_in] --- */
void
RTcmix::readFromAudioDevice(
      BufPtr      dest,             /* interleaved buffer from inst */
      int         dest_chans,       /* number of chans interleaved */
      int         dest_frames,      /* frames in interleaved buffer */
      const short src_chan_list[],  /* list of in-bus chan numbers from inst */
      short       src_chans,        /* number of in-bus chans to copy */
      int  		  output_offset)
{
#ifdef NOMORE  // FIXME: not sure these are right
   assert(dest_chans >= src_chans);
   assert(audioNCHANS >= src_chans);
#endif

   for (int n = 0; n < src_chans; n++) {
      int chan = src_chan_list[n];

      BufPtr src = audioin_buffer[chan];
      assert(src != NULL);

      /* The inst might be playing only the last part of a buffer. If so,
         we want it to read the corresponding segment of the audioin buffer.
      */
      src += output_offset;

      copy_one_buf_to_interleaved_buf(dest, src, dest_chans, n, dest_frames);
   }
}


/* ---------------------------RTcmix::readFromInputFile [was get_file_in ]--- */
void
RTcmix::readFromInputFile(
      BufPtr      dest,             /* interleaved buffer from inst */
      int         dest_chans,       /* number of chans interleaved */
      int         dest_frames,      /* frames in interleaved buffer */
      const short src_chan_list[],  /* list of in-bus chan numbers from inst */
      short       src_chans,        /* number of in-bus chans to copy */
      int		  fdIndex,			/* index into input file desc. array */
	  off_t		  *pFileOffset)		/* ptr to inst's file offset (updated) */
{
    /* File opened by earlier call to rtinput. */
    InputFile &inputFile = inputFileTable[fdIndex];
    
    off_t amountRead = inputFile.readSamps(*pFileOffset,
                                           dest,
                                           dest_chans,
                                           dest_frames,
                                           src_chan_list,
                                           src_chans);

   /* Advance saved offset by the number of bytes read.
      Note that this includes samples in channels that were read but
      not copied into the dest buffer!
   */
   *pFileOffset += amountRead;

}

#ifdef MAXMSP
// BGG -- support for reading from the [buffer~] object buffer
#define MM_IN_GAIN_FACTOR 32768.0 // goose it up for RTcmix

/* ------------------------RTcmix::readFromMMbuf ------------------------ */
void
RTcmix::readFromMMbuf(
		BufPtr		dest,				 /* interleaved buffer from inst */
		int			dest_frames,		/* frames in interleaved buffer */
		int			mmchans,			 /* [buffer~] chans */
		float			*mmbufstart,		/* start of [buffer~] data */
		int			mmbufframes,		/* length of [buffer~] data */
		int			mmbufdex)			/* frame index from mmbufstart */
{
	int i, j, k;

	k = mmbufdex * mmchans;
	for (i = 0, j = 0; i < dest_frames; i++, j += mmchans, k += mmchans) {
			dest[j] = mmbufstart[k] * MM_IN_GAIN_FACTOR;
			if (mmchans == 2) dest[j+1] = mmbufstart[k+1] * MM_IN_GAIN_FACTOR;
			if (++mmbufdex >= mmbufframes) {
				mmbufdex--;
				dest[j] = 0.0;
				if (mmchans == 2) dest[j+1] = 0.0;
			}
	}
//	return 0;
}
#endif // MAXMSP


/* -------------------------------------------------------------- rtgetin --- */
/* This static version switches between the old bus-based mode and the new chained buffer mode */

int
Instrument::rtgetin(float		*inarr,  /* interleaved array of <inputchans> */
        			Instrument	*inst,
        			int			nsamps)         /* samps, not frames */
{
	if (inst->hasChainedInput()) {
		const int chans = inst->inputChannels();
		const int frames = nsamps / chans;
#ifdef DEBUG
		printf("%s::rtgetin(): copying from inputChainBuf %p to inarr %p\n", inst->name(), inst->inputChainBuf, inarr);
#endif
		// Copy all channels of audio from inputChainBuf, which is a pointer to the output buffer of
		// the previous instrument in the chain.
		for (int ch = 0; ch < chans; ++ch) {
			copy_interleaved_buf_to_buf(
										inarr,            		/* interleaved dest buffer */
										inst->inputChainBuf,    /* interleaved source buffer */
										chans,                /* number of dest chans interleaved */
										ch,                      /* output chan to copy into */
										frames,                 /* frames in dest buffer */
										chans,                /* number of input chans interleaved */
										ch);                     /* input chan to copy from */
		}
		return nsamps;
	}
	else
		return inst->rtgetin(inarr, nsamps);
}

/* -------------------------------------------------------------- rtgetin --- */
/* For use by instruments that take input either from an in buffer or from
 an aux buffer, but not from both at once. Also, input from files or from
 the audio in device can come only through an in buffer, not from an aux
 buffer. Note that file input ignores the bus_config entirely, simply
 reading all the file channels into the insts buffer, which we assume is
 large enough to hold them. (This should be a safe assumption, since
 rtsetinput makes sure that inst->inputchans matches the file chans.)
 */

int	Instrument::rtgetin(float *inarr, int nsamps)
{
	const int inchans = inputChannels();    /* total in chans inst expects */
	const BusSlot *busSlot = getBusSlot();
	const short in_count = busSlot->in_count;
	const int fdindex = _input.fdIndex;
	const int frames = nsamps / inchans;
	
#ifdef DEBUG
	printf("%s::rtgetin(): doing normal read from input\n", name());
#endif

	assert(inarr != NULL);
	
	if (frames > RTcmix::bufsamps()) {
		die(name(), "Internal Error: rtgetin: nsamps out of range!");
		return -1;
	}
	
	if (fdindex == NO_DEVICE_FDINDEX) {               /* input from aux buses */
		const short *auxin = busSlot->auxin;        /* auxin channel list */
		const short auxin_count = busSlot->auxin_count;
		
		assert(auxin_count > 0);
		
		RTcmix::readFromAuxBus(inarr, inchans, frames, auxin, auxin_count, output_offset);
	}

#ifdef MAXMSP
// BGG -- read from [buffer~] buffer
// in the older rtgetin(), this has to come before the isInputAudioDevice()
// check (was never sure why).  This may no longer be true
   else if (fdindex == USE_MM_BUF) {
      RTcmix::readFromMMbuf(inarr, frames,
            _input.inputchans,
            _input.my_mm_buf->mm_bufstart,
            _input.my_mm_buf->mm_buf_nframes,
            _input.fileOffset);

      _input.fileOffset += frames;
      if (_input.fileOffset >= _input.my_mm_buf->mm_buf_nframes)
         _input.fileOffset = _input.my_mm_buf->mm_buf_nframes - 1;
   }
#endif // MAXMSP

	else if (RTcmix::isInputAudioDevice(fdindex)) {  /* input from mic/line */
		const short *in = busSlot->in;              /* in channel list */
		
		assert(in_count > 0);
		
		RTcmix::readFromAudioDevice(inarr, inchans, frames, in, in_count, output_offset);
	}
	else {                                            /* input from file */
		const short *in = busSlot->in;              /* in channel list */
		
		assert(in_count > 0);
		
		RTcmix::readFromInputFile(inarr, inchans, frames, in, in_count,
								  fdindex, &_input.fileOffset);
	}
	
	return nsamps;   // this seems pointless, but no insts pay attention anyway
}

