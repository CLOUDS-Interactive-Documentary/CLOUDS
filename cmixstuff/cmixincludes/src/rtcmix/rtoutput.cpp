/* RTcmix  - Copyright (C) 2000  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/
#include <RTcmix.h>
#include <ugens.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <sndlibsupport.h>
#include "AudioDevice.h"
#include "audio_devices.h"
#include "rtdefs.h"
#include <Option.h>


/* The syntax of rtoutput is expanded when using sndlib:

   rtoutput("filename" [, "header_type"] [, "data_format"])

   - "header_type" is one of:
        "aiff", "aifc", "wav", "next", "sun", or "ircam"

      The default is "aiff", since this is what most other unix
      programs can use (Notam software, Cecilia, etc.).

      "next" and "sun" (the familiar ".au" format) are synonyms.
      "ircam" is the older, non-hybrid BICSF format.
      (Note that sndlib can't write the hybrid EBICSF headers.)

      All formats are bigendian, except for "wav".

   - "data_format" is one of:
        "short"      16-bit linear
        "float"      32-bit floating point
        "normfloat"  32-bit floating point in range (mostly) from -1.0 to +1.0
        "16"         synonym for "short"
        "24"         24-bit linear, not yet supported in RTcmix

        The default is "short".


   NOTES:

     (1) The sampling rate and number of channels are specified in
         the call to rtsetparams.

     (2) If you ask for "aiff" and "float" (or "normfloat"),
         you'll get "aifc" format instead. This is because sndlib
         doesn't handle floats properly with AIFF format.

     (3) The case of the "header_type" and "data_format" arguments
         is not significant, nor is their order.

     (4) If you want to use floating point files in Snd, choose
         "normfloat" format. If you want to use them in Mxv, choose
         the "next" header type. Many programs don't read AIFC files,
         maybe because they assume these are always compressed.

   There are other possibilities with sndlib, but limiting the choices
   to those listed above makes for an easier Minc interface to rtoutput.
*/


/* CAUTION: Don't change these without thinking about constraints
            imposed by the various formats, and by sndlib and any
            programs (Mxv, Mix, Cecilia) that should be compatible.
*/
#define DEFAULT_HEADER_TYPE    MUS_AIFF
#define DEFAULT_DATA_FORMAT    MUS_BSHORT

#define CLOBBER_WARNING       \
"Specified output file already exists! \n\n\
Turn on \"clobber mode\" in your score to overwrite it.\n\
(Put \"set_option(\"clobber_on\")\" before call to rtoutput).\n"


typedef enum {
   INVALID_PARAM = 0,
   HEADER_TYPE,
   DATA_FORMAT,
   ENDIANNESS
} ParamType;

typedef struct {
   ParamType  type;
   int        value;
   char       arg[16];
} Param;

/* See description of these strings above.
   (Supporting endian request would be really confusing, because there
   are many constraints in the formats. The only thing it would buy
   us is the ability to specify little-endian AIFC linear formats.
   Not worth the trouble.)
*/
static Param param_list[] = {
   { HEADER_TYPE,  MUS_NEXT,     "next"      },
   { HEADER_TYPE,  MUS_NEXT,     "sun"       },
   { HEADER_TYPE,  MUS_AIFF,     "aiff"      },
   { HEADER_TYPE,  MUS_AIFC,     "aifc"      },
   { HEADER_TYPE,  MUS_RIFF,     "wav"       },
   { HEADER_TYPE,  MUS_IRCAM,    "ircam"     },
   { HEADER_TYPE,  MUS_RAW,      "raw"       },
   { DATA_FORMAT,  MUS_BSHORT,   "short"     },
   { DATA_FORMAT,  MUS_BFLOAT,   "float"     },
   { DATA_FORMAT,  MUS_BFLOAT,   "normfloat" },
   { DATA_FORMAT,  MUS_BSHORT,   "16"        },
   { DATA_FORMAT,  MUS_B24INT,   "24"        },
   { ENDIANNESS,   0,            "big"       },    /* not implemented */
   { ENDIANNESS,   1,            "little"    }
};
static int num_params = sizeof(param_list) / sizeof(Param);

/* for use in header_type_from_filename */
typedef struct {
   int   format;
   char  arg[8];
} Extension;

static Extension format_extension_list[] = {
   { MUS_NEXT,    "au"     },
   { MUS_NEXT,    "snd"    },
   { MUS_AIFF,    "aif"    },
   { MUS_AIFF,    "aiff"   },
   { MUS_AIFC,    "aifc"   },
   { MUS_RIFF,    "wav"    },
   { MUS_IRCAM,   "sf"     },
   { MUS_RAW,     "raw"    }
};
static int num_format_extensions = sizeof(format_extension_list)
                                                      / sizeof(Extension);

static int header_type_from_filename(char *);
static int parse_rtoutput_args(int, double []);


/* -------------------------------------------- header_type_from_filename --- */
static int
header_type_from_filename(char *fname)
{
   int   i, format = -2;
   char  *p;

   p = strrchr(fname, '.');
   if (p != NULL) {
      p++;     /* skip over '.' */
      for (i = 0; i < num_format_extensions; i++) {
         if (strcasecmp(format_extension_list[i].arg, p) == 0) {
            format = format_extension_list[i].format;
            break;
         }
      }
		if (format == -2)
         return die("rtoutput", "Unrecognized sound file extension: \".%s\"", p);
   }
   return format;
}


/* -------------------------------------------------- parse_rtoutput_args --- */
int
RTcmix::parse_rtoutput_args(int nargs, double pp[])
{
   int   i, j, matched;
   int   normfloat_requested;
   char  *arg;

   if (nargs == 0) {
      rterror("rtoutput", "you didn't specify a file name!");
      return -1;
   }

   rtoutsfname = DOUBLE_TO_STRING(pp[0]);
   if (rtoutsfname == NULL)
   {
      rterror("rtoutput", "NULL file name!");
      return -1;
   }

   output_header_type = header_type_from_filename(rtoutsfname);
   if (output_header_type == -1)
      return -1;
   if (output_header_type == -2)
      output_header_type = DEFAULT_HEADER_TYPE;
   output_data_format = DEFAULT_DATA_FORMAT;

   normfloat_requested = 0;

   for (i = 1; i < nargs; i++) {
      arg = DOUBLE_TO_STRING(pp[i]);

      matched = 0;
      for (j = 0; j < num_params; j++) {
         if (strcasecmp(param_list[j].arg, arg) == 0) {
            matched = 1;
            break;
         }
      }
      if (!matched) {
         rterror("rtoutput", "unrecognized argument \"%s\"", arg);
         return -1;
      }

      switch (param_list[j].type) {
         case HEADER_TYPE:
            output_header_type = param_list[j].value;
            break;
         case DATA_FORMAT:
            output_data_format = param_list[j].value;
            if (output_data_format == MUS_BFLOAT
                           && strcasecmp(param_list[j].arg, "normfloat") == 0)
               normfloat_requested = 1;
            break;
         case ENDIANNESS:  /* currently unused */
            break;
         default:
            break;
      }
   }

   /* RtcHandle some special cases. */

   /* If "wav", make data format little-endian. */
   if (output_header_type == MUS_RIFF) {
      switch (output_data_format) {
         case MUS_BSHORT:
            output_data_format = MUS_LSHORT;
            break;
         case MUS_B24INT:
            output_data_format = MUS_L24INT;
            break;
         case MUS_BFLOAT:
            output_data_format = MUS_LFLOAT;
            break;
      }
   }

   /* If AIFF, use AIFC only if explicitly requested, or if
      the data format is float.
   */
   if (output_header_type == MUS_AIFF && output_data_format == MUS_BFLOAT)
      output_header_type = MUS_AIFC;

   /* If writing to a float file, decide whether to normalize the
      samples, i.e., to divide them all by 32768 so as to make the
      normal range fall between -1.0 and +1.0. This is what Snd
      and sndlib like to see, but it's not the old cmix way.
   */
   if (normfloat_requested)
      normalize_output_floats = 1;

   is_float_format = IS_FLOAT_FORMAT(output_data_format);

#ifdef ALLBUG
   fprintf(stderr, "name: %s, head: %d, data: %d, norm: %d\n",
                   rtoutsfname, output_header_type, output_data_format,
                   normalize_output_floats);
#endif

   return 0;
}


/* ------------------------------------------------------------- rtoutput --- */
/* This routine is used in the Minc score to open up a file for
   writing by RT instruments.  pp[0] is a pointer to the soundfile
   name, disguised as a double by the crafty Minc.  (p[] is passed in
   just for fun.)  Optional string arguments follow the filename,
   and parse_rtoutput_args processes these. See the comment at the
   top of this file for the meaning of these arguments.

   If "clobber" mode is on, we delete an existing file with the
   specified file name, creating a header according to what
   parse_rtoutput_args determines.

   Returns -1.0 if a file is already open for writing. Dies if there
   is any other error.
*/
double
RTcmix::rtoutput(float p[], int n_args, double pp[])
{
   int         error;
   struct stat statbuf;

   if (rtfileit == 1) {
      rterror("rtoutput", "A soundfile is already open for writing...");
      return -1;
   }

   /* flag set to -1 until we reach end of function.  This way, if anything
      fails along the way, we leave this set as we want it.
   */
   rtfileit = -1;

   if (SR == 0) {
      die("rtoutput", "You must call rtsetparams before rtoutput.");
      return -1;
   }

   error = parse_rtoutput_args(n_args, pp);
   if (error)
      return -1;          /* already reported in parse_rtoutput_args */

   error = stat(rtoutsfname, &statbuf);

   if (error) {
      if (errno == ENOENT) { 
         ;              /* File doesn't exist -- no problem */
      }
      else {
         rterror("rtoutput", "Error accessing file \"%s\": %s",
                                                rtoutsfname, strerror(errno));
         return -1;  /* was exit() */
      }
   }
   else {               /* File exists; find out whether we can clobber it */
      if (!get_bool_option(kOptionClobber)) {
         rterror("rtoutput", "\n%s", CLOBBER_WARNING);
         return -1;
      }
      else {
         /* make sure it's a regular file */
         if (!S_ISREG(statbuf.st_mode)) {
            rterror("rtoutput", "\"%s\" isn't a regular file; won't clobber it",
                                                                 rtoutsfname);
            return -1;
         }
      }
   }

   // If user has chosen to turn off audio playback, we delete
   // the device that might have been created during rtsetparams().
   if (!Option::record() && !Option::play()) {
       delete audioDevice;
	   audioDevice = NULL;
   }
   AudioDevice *dev;
   if ((dev = create_audio_file_device(audioDevice,
				   				rtoutsfname, output_header_type,
                                output_data_format, NCHANS, SR,
                                normalize_output_floats,
                                get_bool_option(kOptionCheckPeaks))) == NULL)
      return -1;  /* failed! */

   audioDevice = dev;

   rtfileit = 1;  /* here we finally set this to 1 */

   return 1;
}


