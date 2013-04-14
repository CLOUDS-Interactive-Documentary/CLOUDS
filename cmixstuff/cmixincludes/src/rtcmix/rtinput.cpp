/* RTcmix  - Copyright (C) 2000  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/

/* Originally by Brad Garton, Doug Scott, and Dave Topper.
   Reworked for v2.3 by John Gibson.
   Reworked again for 3.7 by Douglas Scott.
*/
#include <RTcmix.h>
#include "prototypes.h"
#include <ugens.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <sndlibsupport.h>
#include <rtdefs.h>
#include <Option.h>
#include <MMPrint.h>
#include "audio_devices.h"
#include "InputFile.h"
#ifdef LINUX
   #include <fcntl.h>
#endif /* LINUX */

/* code that lets user specify buses for input sources */
//#define INPUT_BUS_SUPPORT

typedef enum {
   MIC,
   LINE,
   DIGITAL
} AudioPortType;

static long last_input_index = -1;


/* ------------------------------------------------- get_last_input_index --- */
/* Called by rtsetinput to find out which file to set for the inst.
*/
int
get_last_input_index()
{
   return (int) last_input_index;   // FIX ME -- should return long
}


/* ------------------------------------------------------ open_sound_file --- */
int
open_sound_file(
      const char     *funcname,       // for error messages
      const char     *sfname,         // name of sound file to open
      int     		 *header_type,    // Remaining args are pointers to storage for
      int     		 *data_format,    //    various bits of file header info.  If
      int     		 *data_location,  //    pointer is NULL, it will be ignored.
      double   		 *srate,          //    Info is undefined on error return (-1).
      int     		 *nchans,
      long     		 *nsamps)
{
   // See if file exists and is a regular file or link.
   struct stat sfst;
   if (stat(sfname, &sfst) == -1) {
      rterror(funcname, "\"%s\": %s", sfname, strerror(errno));
      return -1;
   }
   if (!S_ISREG(sfst.st_mode) && !S_ISLNK(sfst.st_mode)) {
      rterror(funcname, "\"%s\" is not a regular file or a link.\n", sfname);
      return -1;
   }

   // Open the file and read its header.
   int fd = sndlib_open_read(sfname);
   if (fd == -1) {
      rterror(funcname, "Can't read header from \"%s\" (%s)\n",
		   sfname, strerror(errno));
      return -1;
   }

   // Now info is available from sndlib query functions.

   int type = mus_header_type();

   if (NOT_A_SOUND_FILE(type)) {
      rterror(funcname, "\"%s\" is probably not a sound file\n", sfname);
      sndlib_close(fd, 0, 0, 0, 0);
      return -1;
   }

   int format = mus_header_format();

   if (INVALID_DATA_FORMAT(format)) {
      rterror(funcname, "\"%s\" has invalid sound data format\n", sfname);
      sndlib_close(fd, 0, 0, 0, 0);
      return -1;
   }

   if (!SUPPORTED_DATA_FORMAT(format)) {
      rterror(funcname, "Can't open \"%s\": can read only 16-bit integer, "
                        "24-bit integer and 32-bit float files.", sfname);
      sndlib_close(fd, 0, 0, 0, 0);
	  return -1;
   }

   if (header_type)
      *header_type = type;
   if (data_format)
      *data_format = format;
   if (data_location)
      *data_location = mus_header_data_location();
   if (srate)
      *srate = (double) mus_header_srate();
   if (nchans)
      *nchans = mus_header_chans();
   if (nsamps)
      *nsamps = mus_header_samples();

   return fd;
}

/* -------------------------------------------------------------- rtinput --- */
/* This function is used in the score to open a file or an audio device for
   subsequent reads by RT Instruments. "rtsetinput" is used in
   the init member functions of the Instruments to access the file.

   p[0] is either a sound file name or "AUDIO" (for an audio device).

   If it's "AUDIO", then p[1] (optional) can be "MIC", "LINE" or
   "DIGITAL". (This option not available yet under Linux.)
   This sets up the input device to do real-time reading of sound.

FIXME: this stuff not implemented yet  -JGG
   pfields after these are bus specification strings in the format
   accepted by the bus_config Minc function. They *must* be "in"
   buses, not "auxin". The bus spec. is optional - if absent, the
   default is "in0-X", where `X' is the number of channels in the
   sound file; "AUDIO" input defaults to stereo ("in0-1").

   Note that the only reason to bother with the bus specification
   is to make it possible to have more than one input source for
   an instrument.
*/

#ifdef MAXMSP
// BGG mm -- from rtcmixmain() [main.cpp] -- the buffer vars
extern mm_buf mm_bufs[]; // mm_buf defined in rtdefs.h
extern int n_mm_bufs;
extern int mm_buf_input;
#endif

double
RTcmix::rtinput(float p[], int n_args, double pp[])
{
	int            audio_in, p1_is_audioport, start_pfield, fd;
	int            is_open, header_type, data_format, data_location, nchans;
#ifdef INPUT_BUS_SUPPORT
	int            startchan, endchan;
	short          busindex, buslist[MAXBUS];
	BusType        type;
#endif /* INPUT_BUS_SUPPORT */
	double         srate, dur;
	char           *sfname, *str;
	AudioPortType  port_type = MIC;

	header_type = MUS_UNSUPPORTED;
	data_format = MUS_UNSUPPORTED;
	data_location = 0;
	dur = 0.0;

	audio_in = 0;
	p1_is_audioport = 0;
	is_open = 0;

	sfname = DOUBLE_TO_STRING(pp[0]);

	/* Catch stoopid NULL filenames */
	if (sfname == NULL) {
		rterror("rtinput", "NULL filename!");
		return -1;
	}

	if (strcmp(sfname, "AUDIO") == 0) {
		audio_in = 1;

		if (n_args > 1 && pp[1] != 0.0) {
			p1_is_audioport = 1;
			str = DOUBLE_TO_STRING(pp[1]);
			if (strcmp(str, "MIC") == 0)
				port_type = MIC;
			else if (strcmp(str, "LINE") == 0)
				port_type = LINE;
			else if (strcmp(str, "DIGITAL") == 0)
				port_type = DIGITAL;
			else
				p1_is_audioport = 0;		/* p[1] might be a bus spec. */
		}

		/* This signals inTraverse() to grab buffers from the audio device. */
		rtrecord = 1;

// FIXME: need to replace this with the bus spec scheme below... -JGG
		audioNCHANS = (n_args > 2) ? (int) p[2] : NCHANS;
		nchans = audioNCHANS;
		srate = SR;
	}

#ifdef MAXMSP
// this segment is to allow rtcmix to access sample data from the
// max/msp [buffer~] object.
	if (strcmp(sfname, "MMBUF") == 0) {
			int i;

			str = DOUBLE_TO_STRING(pp[1]);
			for (i = 0; i < n_mm_bufs; i++) {
				if (strcmp(str, mm_bufs[i].name) == 0) {
					mm_buf_input = i; // used in rtsetinput() for buf ID
					break;
				}
			}
			if (i == n_mm_bufs) {
				die("rtinput", "no max/msp buffer named %s is set", str);
				mm_buf_input = -1; // we are NOT using [buffer~] input, even if set
				return -1;
			}
			audio_in = 1; // I think I need this to initialize some vars...

// FIXME: need to replace this with the bus spec scheme below... -JGG
		audioNCHANS =  NCHANS;
		nchans = audioNCHANS;
		srate = SR;
		} else {
			mm_buf_input = -1; // we are NOT using [buffer~] input, even if set
		}
#endif // MAXMSP

#ifdef INPUT_BUS_SUPPORT
	/* Parse bus specification. */

	busindex = 0;
	for (i = 0; i < MAXBUS; i++)
		buslist[i] = -1;

	type = BUS_IN;
	startchan = endchan = -1;

	start_pfield = p1_is_audioport ? 2 : 1;

	for (i = start_pfield; i < n_args; i++) {
		ErrCode  err;

		str = DOUBLE_TO_STRING(pp[i]);
		if (str == NULL) {
			rterror("rtinput", "NULL bus name!");
			rtrecord = 0;
			return -1;
		}

		err = parse_bus_name(str, &type, &startchan, &endchan);
		if (err) {
			rterror("rtinput", "Invalid bus name specification.");
			rtrecord = 0;
			return -1;
		}
		if (type != BUS_IN) {
			rterror("rtinput", "You have to use an \"in\" bus with rtinput.");
			rtrecord = 0;
			return -1;
		}

		for (j = startchan; j <= endchan; j++)
			buslist[busindex++] = j;
	}

	if (startchan == -1) {           /* no bus specified */
	}
#endif /* INPUT_BUS_SUPPORT */
    long i;
    
	/* See if this audio device or file has already been opened. */
	for (i = 0; i < max_input_fds; i++) {
		if (inputFileTable[i].isOpen()) {
			if (inputFileTable[i].hasFile(sfname)) {
				last_input_index = i;
				is_open = 1;
				break;
			}
		}
	}
	if (!is_open) {			/* if not, open input audio device or file. */
		long nsamps = 0;
		if (audio_in) {
			if (rtsetparams_was_called()) {
				// If audio *playback* was disabled, but there is a request for
				// input audio, create the audio input device here.
				if (!audioDevice && !Option::play()) {
					int nframes = RTBUFSAMPS;
					if ((audioDevice = create_audio_devices(true, false,
											 NCHANS, SR, &nframes,
											 Option::bufferCount())) == NULL)
					{
						rtrecord = 0;	/* because we failed */
						return -1;
					}
					Option::record(true);
					RTBUFSAMPS = nframes;
					rtcmix_advise("Input audio set",  "%g sampling rate, %d channels\n", SR, NCHANS);
				}
				// If record disabled during rtsetparams(), we cannot force it on here.
				else if (!Option::record()) {
					die("rtinput", "Audio already configured for playback only via rtsetparams()");
					rtrecord = 0;	/* because we failed */
					return -1;
				}
			}
			else {
				// This allows rtinput("AUDIO") to turn on record
				Option::record(1);
				rtrecord = 1;
			}
			fd = 1;  /* we don't use this; set to 1 so rtsetinput() will work */
			for (i = 0; i < nchans; i++) {
				allocate_audioin_buffer(i, RTBUFSAMPS);
			}
#ifdef INPUT_BUS_SUPPORT
#endif /* INPUT_BUS_SUPPORT */
		}
		else {
			rtrecord = 0;
			fd = ::open_sound_file("rtinput", sfname, &header_type, &data_format,
							&data_location, &srate, &nchans, &nsamps);
			if (fd == -1)
				return -1;

#ifdef INPUT_BUS_SUPPORT
			if (startchan == -1) {     /* no bus specified above */
				startchan = 0;
				endchan = nchans - 1;
			}
			else {
				if (endchan - startchan >= nchans) {
					rtcmix_warn("rtinput", "You specifed more input buses than "
							"input file '%s' has channels.", sfname);
					rtcmix_warn("rtinput", "Using in buses %d", foo);
					endchan = (startchan + nchans) - 1;
				}
			}
#endif /* INPUT_BUS_SUPPORT */

			dur = (double) (nsamps / nchans) / srate;
			rtcmix_advise(NULL, "Input file set for reading:\n");
			rtcmix_advise(NULL, "      name:  %s\n", sfname);
			rtcmix_advise(NULL, "      type:  %s\n", mus_header_type_name(header_type));
			rtcmix_advise(NULL, "    format:  %s\n", mus_data_format_name(data_format));
			rtcmix_advise(NULL, "     srate:  %g\n", srate);
			rtcmix_advise(NULL, "     chans:  %d\n", nchans);
			rtcmix_advise(NULL, "  duration:  %g\n", dur);
#ifdef INPUT_BUS_SUPPORT
#endif /* INPUT_BUS_SUPPORT */

			if (srate != SR) {
				rtcmix_warn("rtinput", "The input file sampling rate is %g, but "
							"the output rate is currently %g.", srate, SR);
			}
		}

		/* Put new file descriptor into the first available slot in the
			inputFileTable.  Also copy the filename for later checking, and
			fill in the other fields in the InputFile struct (see InputFile.h).

			last_input_index is the value that will be used by any instrument
			created after this call to rtinput().
		*/
		for (i = 0; i < max_input_fds; i++) {
			if (!inputFileTable[i].isOpen()) {
                inputFileTable[i].init(fd, sfname, audio_in, header_type, data_format, data_location, nsamps, (float)srate, nchans, dur);
				last_input_index = i;
				break;
			}
		}

		/* If this is true, we've used up all input descriptors in our array. */
		if (i == max_input_fds)
			die("rtinput", "You have exceeded the maximum number of input "
												"files (%ld)!", max_input_fds);
	}

	/* Return this to Minc, so user can pass it to functions. */
	return (double) last_input_index;
}

// This is called by Instrument::gone() to decrement references to open
// input files.

void
RTcmix::releaseInput(int fdIndex)
{
   // BGG -- added this to prevent file closings in interactive mode
   // we don't know if a file will be referenced again in the future
   if (!interactive()) {
#ifdef DEBUG
      printf("RTcmix::releaseInput: fdIndex %d\n", fdIndex);
#endif
	   inputFileTable[fdIndex].unreference();
   }
}


