/* RTcmix  - Copyright (C) 2000  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/

/* rev for v2.3 by JGG */

#include <RTcmix.h>
#include "prototypes.h"
#include <ugens.h>
#include <stdio.h>
#include <assert.h>
#include "BusSlot.h"
#include <sndlibsupport.h>
#include "Instrument.h"
#include "rtdefs.h"
#include "InputFile.h"


#define INCHANS_DISCREPANCY_WARNING "\
The bus config for this instrument specifies %d input channels, \n\
but its input source has %d channels. Setting input channels to %d..."



/* ----------------------------------------------------------- rtsetinput --- */
/* Instruments call this to set their input file pointer (like setnote in cmix).
   Returns 0 if ok, -1 if error.
*/
int
Instrument::rtsetinput(float start_time, Instrument *inst)
{
   int   auxin_count = inst->getBusSlot()->auxin_count;
   int   in_count = inst->getBusSlot()->in_count;
   const char  *inst_name = inst->name();

   if (auxin_count == 0 && in_count == 0) {
      die(inst_name, "This instrument requires input from either an in bus "
                      "or an aux bus.\nChange this with bus_config().");
		return -1;
	}

   if (auxin_count > 0) {
      if (start_time != 0.0) {
         die(inst_name, "Input start must be 0 when reading from an aux bus.");
			return -1;
		}
   }

   if (in_count > 0) {
      int src_chans;
	  
	  int status = RTcmix::attachInput(start_time, &inst->_input);
	  
	  switch (status) {
	  case RT_NO_INPUT_SRC:
         die(inst_name, "No input source open for this instrument!");
         return -1;
      case RT_ILLEGAL_DEV_OFFSET:
         die(inst_name, "Input start must be 0 when reading from the "
                           "real-time audio device.");
         return -1;
      case RT_INPUT_EOF:
         rtcmix_warn(inst_name, "Attempt to read past end of input file '%s'\n",
              RTcmix::getInputPath(inst->_input.fdIndex));
		 break;
	  case RT_INPUT_CHANS_MISMATCH:
#ifdef NOMORE // pointless ifdef IGNORE_BUS_COUNT_FOR_FILE_INPUT in InputFile.cpp
         rtcmix_advise(inst_name, INCHANS_DISCREPANCY_WARNING, inst->_input.inputchans,
                                                        src_chans, src_chans);
#endif
	  default:
	     break;
	  }
      inst->sfile_on = 1;
   }
   return 0;
}

#ifdef MAXMSP
// BGG mm -- from rtcmixmain() [main.cpp] -- the buffer vars
extern int mm_buf_input; // are we using [buffer~]?, which one?
extern mm_buf mm_bufs[];
extern int n_mm_bufs;
#endif

int
RTcmix::attachInput(float start_time, InputState *input)
{
      int index = get_last_input_index();
#ifdef SGI
      if (index < 0) {
         return RT_NO_INPUT_SRC;
      }
      if ((!inputFileTable[index].isOpen())
                         && (inputFileTable[index].getFD() != AUDIO_DEVICE_FD)) {
         return RT_NO_INPUT_SRC;
      }
#else
      if (index < 0 || !inputFileTable[index].isOpen()) {
         return RT_NO_INPUT_SRC;
      }
#endif
      int src_chans;
	  int status = RT_NO_ERROR;

      /* File or audio device was opened in rtinput(). Here we store the
         index into the inputFileTable for the file or device.
      */
      input->fdIndex = index;

      /* Fill in relevant data members of instrument class. */
      input->inputsr = inputFileTable[index].sampleRate();

      src_chans = inputFileTable[index].channels();

#ifdef MAXMSP
//	 use max/msp [buffer~] for samples
		if (mm_buf_input >= 0) { // set in rtinput when [buffer~] is being used
			int stframe;
			int mmb;

			mmb = mm_buf_input; // which [buffer~] object we're using
			stframe = (int) (start_time * sr());
			if (stframe > mm_bufs[mmb].mm_buf_nframes) return RT_INPUT_EOF;

			input->inputchans = mm_bufs[mmb].mm_buf_chans;
			input->my_mm_buf = &(mm_bufs[mmb]); // address of active [buffer~] buf
			input->fileOffset = stframe; // buf frame index
			input->fdIndex = USE_MM_BUF; // from rtdefs.h, checked in rtgetin()

      } else
#endif

      if (inputFileTable[index].isAudioDevice()) {
         if (start_time != 0.0) {
		    return RT_ILLEGAL_DEV_OFFSET;
		 }
      }
      else {
         int datum_size;

         if (input->inputchans != src_chans) {
		    status = RT_INPUT_CHANS_MISMATCH;	// nonfatal
            input->inputchans = src_chans;
         }


		 // This is now rounded to the nearest frame -- DS
         int inskip_frames = (int) (0.5 + start_time * input->inputsr);

		  datum_size = ::mus_data_format_to_bytes_per_sample(inputFileTable[index].dataFormat());

         /* Offset is measured from the header size determined in rtinput(). */
         input->fileOffset = inputFileTable[index].dataLocation()
                            + (inskip_frames * input->inputchans * datum_size);

         if (start_time >= inputFileTable[index].duration())
		    status = RT_INPUT_EOF;	// not fatal -- just produces warning
      }

   /* Increment the reference count for this file. */
   inputFileTable[index].reference();

   return status;
}

