#ifndef SNDLIB_H
#define SNDLIB_H

#include <math.h>	// JGG added to avoid all the M_PI redefinitions

/* taken from libtool's demo/foo.h to try to protect us from C++ and ancient C's */
#ifdef __CYGWIN__
#  ifndef __CYGWIN32__
#    define __CYGWIN32__
#  endif
#endif

#undef BEGIN_DECLS
#undef END_DECLS
#ifdef __cplusplus
# define BEGIN_DECLS extern "C" {
# define END_DECLS }
#else
# define BEGIN_DECLS /* empty */
# define END_DECLS /* empty */
#endif

#undef PROTO
#if defined (__STDC__) || defined (_AIX) || (defined (__mips) && defined (_SYSTYPE_SVR4)) || defined(__CYGWIN32__) || defined(__cplusplus)
# define PROTO(protos) protos
#else
# define PROTO(protos) ()
#endif


#define SNDLIB_VERSION 13
#define SNDLIB_REVISION 1
#define SNDLIB_DATE "13-Jul-01"

/* try to figure out what type of machine (and in worst case, what OS) we're running on */

#if defined(HAVE_CONFIG_H)
  #include "config.h"
  #if (!defined(WORDS_BIGENDIAN))
     #define MUS_LITTLE_ENDIAN 1
  #endif
  #if (SIZEOF_INT_P != SIZEOF_INT)
     #define LONG_INT_P 1
  #else 
     #define LONG_INT_P 0
  #endif
#else
  #if defined(ALPHA) || defined(__alpha__)
     #define LONG_INT_P 1
  #else 
     #define LONG_INT_P 0
  #endif
  #define RETSIGTYPE void
  #ifdef __LITTLE_ENDIAN__
    /* NeXTStep on Intel */
    #define MUS_LITTLE_ENDIAN 1
  #else
    #ifdef BYTE_ORDER
      #if (BYTE_ORDER == LITTLE_ENDIAN)
        #define MUS_LITTLE_ENDIAN 1
      #else
        #if __INTEL__
          #define MUS_LITTLE_ENDIAN 1
        #endif
      #endif
    #endif
  #endif
#endif

#if defined(ALPHA) || defined(WINDOZE) || defined(__alpha__)
  #define MUS_LITTLE_ENDIAN 1
#endif

#if (!(defined(MACOS))) && (defined(MPW_C) || defined(macintosh) || defined(__MRC__))
  #define MACOS 1
  #include <MacMemory.h>
  #include <TextUtils.h>
  #include <Gestalt.h>
  #ifndef TRUE
    #define TRUE 1
  #endif
  #ifndef FALSE
    #define FALSE 0
  #endif
#endif

#if defined(__APPLE__)
  #define MAC_OSX 1
#endif

/* due to project builder stupidity, we can't always depend on -D flags here (maybe we need a SNDLIB_OS macro?) */
/* these wouldn't work with autoconf anyway, so we'll do it by hand */

#if (!defined(SGI)) && (!defined(NEXT)) && (!defined(LINUX)) && (!defined(MACOS)) && (!defined(SUN)) && (!defined(UW2)) && (!defined(SCO5)) && (!defined(ALPHA)) && (!defined(WINDOZE)) && (!defined(MAC_OSX))
  #if defined(__dest_os)
    /* we're in Metrowerks Land */
    #if (__dest_os == __mac_os)
      #define MACOS 1
    #endif
  #else
    #if macintosh
      #define MACOS 1
    #else
      #if (__WINDOWS__) || (__NT__) || (_WIN32) || (__CYGWIN__)
        #define WINDOZE 1
        #define MUS_LITTLE_ENDIAN 1
      #else
        #ifdef __alpha__
          #define ALPHA 1
          #define MUS_LITTLE_ENDIAN 1
        #endif
      #endif
    #endif
  #endif
#endif  

#ifdef MAC_OSX
  #ifdef __ppc__
    #define MUS_LITTLE_ENDIAN 0
  #else
    #define MUS_LITTLE_ENDIAN 1
  #endif
#endif

#ifndef MUS_LITTLE_ENDIAN
  #define MUS_LITTLE_ENDIAN 0
#endif

#ifndef __GNUC__
  #ifndef __FUNCTION__
    #define __FUNCTION__ ""
  #endif
#endif

/* this block needed because not everyone uses configure, and those who don't often have no clue what audio system they're using */
/*   so, if nothing is set but we're on a system that looks linux-like and we can find the OSS headers, use OSS */

#ifndef ESD
#ifndef HAVE_OSS
#ifndef HAVE_ALSA
  #if defined(LINUX) || defined(SCO5) || defined(UW2) || defined(HAVE_SOUNDCARD_H) || defined(HAVE_SYS_SOUNDCARD_H) || defined(HAVE_MACHINE_SOUNDCARD_H) || defined(USR_LIB_OSS) || defined(USR_LOCAL_LIB_OSS) || defined(OPT_OSS) || defined(VAR_LIB_OSS) || defined(__FreeBSD__) || defined(__bsdi__)
    #define HAVE_OSS 1
  #else
    #define HAVE_OSS 0
  #endif
#else
  /* this branch may be obsolete with Fernando's new OSS+Alsa code -- need to test it */
  #define HAVE_OSS 0
#endif
#endif
#endif


#if (!defined(M_PI))
  #define M_PI 3.14159265358979323846264338327
  #define M_PI_2 (M_PI / 2.0)
#endif

#if (!defined(TWO_PI))  /* JGG: wrapped this */
  #define TWO_PI (2.0 * M_PI)
#endif

#define POWER_OF_2_P(x)	((((x) - 1) & (x)) == 0)
/* from sys/param.h */

#ifndef SEEK_SET
  #define SEEK_SET 0
#endif

#ifndef SEEK_CUR
  #define SEEK_CUR 1
#endif

#ifndef SEEK_END
  #define SEEK_END 2
#endif

#if (!SNDLIB_USE_FLOATS)
  #define MUS_SAMPLE_TYPE int
  #ifndef MUS_SAMPLE_BITS
    #define MUS_SAMPLE_BITS 24
  #endif
  #define MUS_SAMPLE_0 0
  #define MUS_BYTE_TO_SAMPLE(n) ((n) << (MUS_SAMPLE_BITS - 8))
  #define MUS_SAMPLE_TO_BYTE(n) ((n) >> (MUS_SAMPLE_BITS - 8))
  #define MUS_SHORT_TO_SAMPLE(n) ((n) << (MUS_SAMPLE_BITS - 16))
  #define MUS_SAMPLE_TO_SHORT(n) ((n) >> (MUS_SAMPLE_BITS - 16))
  #if (MUS_SAMPLE_BITS < 24)
    #define MUS_INT24_TO_SAMPLE(n) ((n) >> (24 - MUS_SAMPLE_BITS))
    #define MUS_SAMPLE_TO_INT24(n) ((n) << (24 - MUS_SAMPLE_BITS))
  #else
    #define MUS_INT24_TO_SAMPLE(n) ((n) << (MUS_SAMPLE_BITS - 24))
    #define MUS_SAMPLE_TO_INT24(n) ((n) >> (MUS_SAMPLE_BITS - 24))
  #endif
  #define MUS_INT_TO_SAMPLE(n) (n)
  #define MUS_SAMPLE_TO_INT(n) (n)
  #define MUS_FLOAT_TO_FIX ((MUS_SAMPLE_BITS < 32) ? (1 << (MUS_SAMPLE_BITS - 1)) : 0x7fffffff)
  #define MUS_FIX_TO_FLOAT (1.0 / (float)(MUS_FLOAT_TO_FIX))
  #define MUS_FLOAT_TO_SAMPLE(n) ((int)((n) * MUS_FLOAT_TO_FIX))
  #define MUS_SAMPLE_TO_FLOAT(n) ((float)((n) * MUS_FIX_TO_FLOAT))
  #define MUS_DOUBLE_TO_SAMPLE(n) ((int)((n) * MUS_FLOAT_TO_FIX))
  #define MUS_SAMPLE_TO_DOUBLE(n) ((double)((n) * MUS_FIX_TO_FLOAT))
  #define MUS_SAMPLE_MAX ((MUS_SAMPLE_BITS < 32) ? (MUS_FLOAT_TO_FIX - 1) : 0x7fffffff)
  #define MUS_SAMPLE_MIN ((MUS_SAMPLE_BITS < 32) ? (-(MUS_FLOAT_TO_FIX)) : -0x7fffffff)
#else
  /* this could use MusFloat throughout and reflect the Float = double choice elsewhere */
  #define MUS_SAMPLE_TYPE float
  #ifndef MUS_SAMPLE_BITS
    #define MUS_SAMPLE_BITS 24
  #endif
  #define MUS_SAMPLE_0 0.0
  #define MUS_BYTE_TO_SAMPLE(n) ((float)(n) / (float)(1 << 7))
  #define MUS_SHORT_TO_SAMPLE(n) ((float)(n) / (float)(1 << 15))
  #define MUS_INT_TO_SAMPLE(n) ((float)(n) / (float)(1 << (MUS_SAMPLE_BITS-1)))
  #define MUS_INT24_TO_SAMPLE(n) ((float)(n) / (float)(1 << 23))
  #define MUS_FLOAT_TO_FIX 1.0
  #define MUS_FIX_TO_FLOAT 1.0
  #define MUS_FLOAT_TO_SAMPLE(n) (n)
  #define MUS_DOUBLE_TO_SAMPLE(n) (n)
  #define MUS_SAMPLE_TO_FLOAT(n) (n)
  #define MUS_SAMPLE_TO_DOUBLE(n) (n)
  #define MUS_SAMPLE_TO_INT(n) ((int)((n) * (1 << (MUS_SAMPLE_BITS-1))))
  #define MUS_SAMPLE_TO_INT24(n) ((int)((n) * (1 << 23)))
  #define MUS_SAMPLE_TO_SHORT(n) ((short)((n) * (1 << 15)))
  #define MUS_SAMPLE_TO_BYTE(n) ((char)((n) * (1 << 7)))
  #define MUS_SAMPLE_MAX 0.99999
  #define MUS_SAMPLE_MIN (-1.0)
#endif

#define MUS_DAC_CHANNEL 252525
#define MUS_DAC_REVERB 252520

#define MUS_UNSUPPORTED -1
enum {MUS_NEXT, MUS_AIFC, MUS_RIFF, MUS_BICSF, MUS_NIST, MUS_INRS, MUS_ESPS, MUS_SVX, MUS_VOC, MUS_SNDT, MUS_RAW,
      MUS_SMP, MUS_SD2, MUS_AVR, MUS_IRCAM, MUS_SD1, MUS_SPPACK, MUS_MUS10, MUS_HCOM, MUS_PSION, MUS_MAUD,
      MUS_IEEE, MUS_DESKMATE, MUS_DESKMATE_2500, MUS_MATLAB, MUS_ADC, MUS_SOUND_EDIT, MUS_SOUND_EDIT_16,
      MUS_DVSM, MUS_MIDI, MUS_ESIGNAL, MUS_SOUNDFONT, MUS_GRAVIS, MUS_COMDISCO, MUS_GOLDWAVE, MUS_SRFS,
      MUS_MIDI_SAMPLE_DUMP, MUS_DIAMONDWARE, MUS_REALAUDIO, MUS_ADF, MUS_SBSTUDIOII, MUS_DELUSION,
      MUS_FARANDOLE, MUS_SAMPLE_DUMP, MUS_ULTRATRACKER, MUS_YAMAHA_SY85, MUS_YAMAHA_TX16, MUS_DIGIPLAYER,
      MUS_COVOX, MUS_SPL, MUS_AVI, MUS_OMF, MUS_QUICKTIME, MUS_ASF, MUS_YAMAHA_SY99, MUS_KURZWEIL_2000,
      MUS_AIFF, MUS_PAF, MUS_CSL, MUS_FILE_SAMP, MUS_PVF};

#define MUS_HEADER_TYPE_OK(n) (((n) > MUS_UNSUPPORTED) && ((n) <= MUS_PVF))

enum {MUS_UNKNOWN, MUS_BSHORT, MUS_MULAW, MUS_BYTE, MUS_BFLOAT, MUS_BINT, MUS_ALAW, MUS_UBYTE, MUS_B24INT,
      MUS_BDOUBLE, MUS_LSHORT, MUS_LINT, MUS_LFLOAT, MUS_LDOUBLE, MUS_UBSHORT, MUS_ULSHORT, MUS_L24INT,
      MUS_BINTN, MUS_LINTN, MUS_L12INT};

/* MUS_LINTN and MUS_BINTN refer to 32 bit ints with 31 bits of "fraction" -- the data is "left justified" */

#define MUS_DATA_FORMAT_OK(n) (((n) > MUS_UNKNOWN) && ((n) <= MUS_L12INT))

#if MUS_LITTLE_ENDIAN
  #define MUS_COMPATIBLE_FORMAT MUS_LSHORT
#else
  #define MUS_COMPATIBLE_FORMAT MUS_BSHORT
#endif

#if MUS_LITTLE_ENDIAN
  #if SNDLIB_USE_FLOATS
    #define MUS_OUT_FORMAT MUS_LFLOAT
  #else
    #define MUS_OUT_FORMAT MUS_LINT
  #endif
#else
  #if SNDLIB_USE_FLOATS
    #define MUS_OUT_FORMAT MUS_BFLOAT
  #else
    #define MUS_OUT_FORMAT MUS_BINT
  #endif
#endif


#define MUS_NIST_SHORTPACK 2
#define MUS_AIFF_IMA_ADPCM 99

#define MUS_AUDIO_PACK_SYSTEM(n) ((n) << 16)
#define MUS_AUDIO_SYSTEM(n) (((n) >> 16) & 0xffff)
#define MUS_AUDIO_DEVICE(n) ((n) & 0xffff)

enum {MUS_AUDIO_DEFAULT, MUS_AUDIO_DUPLEX_DEFAULT, MUS_AUDIO_ADAT_IN, MUS_AUDIO_AES_IN, MUS_AUDIO_LINE_OUT,
      MUS_AUDIO_LINE_IN, MUS_AUDIO_MICROPHONE, MUS_AUDIO_SPEAKERS, MUS_AUDIO_DIGITAL_IN, MUS_AUDIO_DIGITAL_OUT,
      MUS_AUDIO_DAC_OUT, MUS_AUDIO_ADAT_OUT, MUS_AUDIO_AES_OUT, MUS_AUDIO_DAC_FILTER, MUS_AUDIO_MIXER,
      MUS_AUDIO_LINE1, MUS_AUDIO_LINE2, MUS_AUDIO_LINE3, MUS_AUDIO_AUX_INPUT, MUS_AUDIO_CD,
      MUS_AUDIO_AUX_OUTPUT, MUS_AUDIO_SPDIF_IN, MUS_AUDIO_SPDIF_OUT, MUS_AUDIO_AMP, MUS_AUDIO_SRATE,
      MUS_AUDIO_CHANNEL, MUS_AUDIO_FORMAT, MUS_AUDIO_IMIX, MUS_AUDIO_IGAIN, MUS_AUDIO_RECLEV,
      MUS_AUDIO_PCM, MUS_AUDIO_PCM2, MUS_AUDIO_OGAIN, MUS_AUDIO_LINE, MUS_AUDIO_SYNTH,
      MUS_AUDIO_BASS, MUS_AUDIO_TREBLE, MUS_AUDIO_PORT, MUS_AUDIO_SAMPLES_PER_CHANNEL,
      MUS_AUDIO_DIRECTION
};
/* Snd's recorder uses MUS_AUDIO_DIRECTION to find the size of this list */

#define MUS_AUDIO_DEVICE_OK(a) (((a) >= MUS_AUDIO_DEFAULT) && ((a) <= MUS_AUDIO_DIRECTION))

#define MUS_ERROR_TYPE int
#define MUS_ERROR -1

enum {MUS_NO_ERROR, MUS_NO_FREQUENCY, MUS_NO_PHASE, MUS_NO_GEN, MUS_NO_LENGTH,
      MUS_NO_FREE, MUS_NO_DESCRIBE, MUS_NO_DATA, MUS_NO_SCALER,
      MUS_MEMORY_ALLOCATION_FAILED, MUS_UNSTABLE_TWO_POLE_ERROR,
      MUS_CANT_OPEN_FILE, MUS_NO_SAMPLE_INPUT, MUS_NO_SAMPLE_OUTPUT,
      MUS_NO_SUCH_CHANNEL, MUS_NO_FILE_NAME_PROVIDED, MUS_NO_LOCATION, MUS_NO_CHANNEL,
      MUS_NO_SUCH_FFT_WINDOW, MUS_UNSUPPORTED_DATA_FORMAT, MUS_HEADER_READ_FAILED,
      MUS_HEADER_TOO_MANY_AUXILIARY_COMMENTS, MUS_UNSUPPORTED_HEADER_TYPE,
      MUS_FILE_DESCRIPTORS_NOT_INITIALIZED, MUS_NOT_A_SOUND_FILE, MUS_FILE_CLOSED, MUS_WRITE_ERROR,
      MUS_BOGUS_FREE, MUS_BUFFER_OVERFLOW, MUS_BUFFER_UNDERFLOW, MUS_FILE_OVERFLOW, MUS_EXPONENT_OVERFLOW,
      MUS_HEADER_WRITE_FAILED, MUS_CANT_OPEN_TEMP_FILE, MUS_INTERRUPTED,

      MUS_AUDIO_CHANNELS_NOT_AVAILABLE, MUS_AUDIO_SRATE_NOT_AVAILABLE, MUS_AUDIO_FORMAT_NOT_AVAILABLE,
      MUS_AUDIO_NO_INPUT_AVAILABLE, MUS_AUDIO_NO_OUTPUT_AVAILABLE, MUS_AUDIO_INPUT_BUSY, MUS_AUDIO_OUTPUT_BUSY,
      MUS_AUDIO_CONFIGURATION_NOT_AVAILABLE, MUS_AUDIO_INPUT_CLOSED, MUS_AUDIO_OUTPUT_CLOSED, MUS_AUDIO_IO_INTERRUPTED,
      MUS_AUDIO_NO_LINES_AVAILABLE, MUS_AUDIO_WRITE_ERROR, MUS_AUDIO_SIZE_NOT_AVAILABLE, MUS_AUDIO_DEVICE_NOT_AVAILABLE,
      MUS_AUDIO_CANT_CLOSE, MUS_AUDIO_CANT_OPEN, MUS_AUDIO_READ_ERROR, MUS_AUDIO_AMP_NOT_AVAILABLE, MUS_AUDIO_NO_OP,
      MUS_AUDIO_CANT_WRITE, MUS_AUDIO_CANT_READ, MUS_AUDIO_NO_READ_PERMISSION,

      MUS_CANT_CLOSE_FILE, MUS_ARG_OUT_OF_RANGE,

      MUS_INITIAL_ERROR_TAG};

/* keep this list in sync with error_names in sound.c */

#ifdef MACOS
  /* C's calloc/free are incompatible with Mac's SndDisposeChannel (which we can't avoid using) */
  /* realloc is enough of a mess that I'll handle each case individually */
  /*   FREE is used only when we call either CALLOC or MALLOC ourselves -- other cases use free, g_free, XtFree, etc */
  #define CALLOC(a, b)  NewPtrClear((a) * (b))
  #define MALLOC(a)    NewPtr((a))
  #define FREE(a)      DisposePtr((Ptr)(a))
#else
  #ifdef DEBUG_MEMORY
    #define CALLOC(a, b)  mem_calloc((size_t)(a), (size_t)(b), __FUNCTION__, __FILE__, __LINE__)
    #define MALLOC(a)     mem_malloc((size_t)(a), __FUNCTION__, __FILE__, __LINE__)
    #define FREE(a)       mem_free(a, __FUNCTION__, __FILE__, __LINE__)
    #define REALLOC(a, b) mem_realloc(a, (size_t)(b), __FUNCTION__, __FILE__, __LINE__)
  #else
    #define CALLOC(a, b)  calloc((size_t)(a), (size_t)(b))
    #define MALLOC(a)     malloc((size_t)(a))
    #define FREE(a)       free(a)
    #define REALLOC(a, b) realloc(a, (size_t)(b))
  #endif
#endif 

#define MUS_MAX_FILE_NAME 256

// JGG: was Float, but that conflicts with our own Float in rtcmix_types.h
#ifndef MusFloat
  #define MusFloat float
#endif

BEGIN_DECLS

/* -------- sound.c -------- */

#ifdef __GNUC__
  void mus_error(int error, const char *format, ...) __attribute__ ((format (printf, 2, 3)));
  void mus_print(const char *format, ...) __attribute__ ((format (printf, 1, 2)));
  char *mus_format(const char *format, ...) __attribute__ ((format (printf, 1, 2)));
  void mus_snprintf(char *buffer, int buffer_len, const char *format, ...)  __attribute__ ((format (printf, 3, 4)));
#else
  void mus_error              PROTO((int error, const char *format, ...));
  void mus_print              PROTO((const char *format, ...));
  char *mus_format            PROTO((const char *format, ...));
  void mus_snprintf           PROTO((char *buffer, int buffer_len, const char *format, ...));
#endif

typedef void mus_error_handler_t(int type, char *msg);
mus_error_handler_t *mus_error_set_handler PROTO((mus_error_handler_t *new_error_handler));
int mus_make_error            PROTO((char *error_name));
const char *mus_error_to_string     PROTO((int err));

typedef void mus_print_handler_t(char *msg);
mus_print_handler_t *mus_print_set_handler PROTO((mus_print_handler_t *new_print_handler));

int mus_sound_samples         PROTO((const char *arg));
int mus_sound_frames          PROTO((const char *arg));
int mus_sound_datum_size      PROTO((const char *arg));
int mus_sound_data_location   PROTO((const char *arg));
int mus_sound_chans           PROTO((const char *arg));
int mus_sound_srate           PROTO((const char *arg));
int mus_sound_header_type     PROTO((const char *arg));
int mus_sound_data_format     PROTO((const char *arg));
int mus_sound_original_format PROTO((const char *arg));
int mus_sound_comment_start   PROTO((const char *arg));
int mus_sound_comment_end     PROTO((const char *arg));
int mus_sound_length          PROTO((const char *arg));
int mus_sound_fact_samples    PROTO((const char *arg));
int mus_sound_distributed     PROTO((const char *arg));
int mus_sound_write_date      PROTO((const char *arg));
int mus_sound_type_specifier  PROTO((const char *arg));
int mus_sound_align           PROTO((const char *arg));
int mus_sound_bits_per_sample PROTO((const char *arg));

int mus_sound_set_chans       PROTO((const char *arg, int val));
int mus_sound_set_srate       PROTO((const char *arg, int val));
int mus_sound_set_header_type PROTO((const char *arg, int val));
int mus_sound_set_data_format PROTO((const char *arg, int val));
int mus_sound_set_data_location PROTO((const char *arg, int val));
int mus_sound_set_samples     PROTO((const char *arg, int val));

const char *mus_header_type_name PROTO((int type));
const char *mus_data_format_name PROTO((int format));
char *mus_sound_comment       PROTO((const char *name));
int mus_data_format_to_bytes_per_sample PROTO((int format));
float mus_sound_duration      PROTO((const char *arg));
int mus_sound_initialize      PROTO((void));
void mus_sound_finalize       PROTO((void));
int mus_sample_bits           PROTO((void));
int mus_sound_override_header PROTO((const char *arg, int srate, int chans, int format, int type, int location, int size));
int mus_sound_forget          PROTO((const char *name));
void mus_sound_print_cache    PROTO((void));
void mus_sound_report_cache   PROTO((FILE *fp));
int mus_sound_aiff_p          PROTO((const char *arg));
int *mus_sound_loop_info      PROTO((const char *arg));
void mus_sound_set_loop_info  PROTO((const char *arg, int *loop));

int mus_sound_open_input      PROTO((const char *arg));
int mus_sound_open_output     PROTO((const char *arg, int srate, int chans, int data_format, int header_type, const char *comment));
int mus_sound_reopen_output   PROTO((const char *arg, int chans, int format, int type, int data_loc));
int mus_sound_close_input     PROTO((int fd));
int mus_sound_close_output    PROTO((int fd, int bytes_of_data));
int mus_sound_read            PROTO((int fd, int beg, int end, int chans, MUS_SAMPLE_TYPE **bufs));
int mus_sound_write           PROTO((int tfd, int beg, int end, int chans, MUS_SAMPLE_TYPE **bufs));
int mus_sound_seek            PROTO((int tfd, long offset, int origin));
int mus_sound_seek_frame      PROTO((int tfd, int frame));
int mus_sound_max_amp         PROTO((const char *ifile, MUS_SAMPLE_TYPE *vals));
int mus_sound_set_max_amp     PROTO((const char *ifile, MUS_SAMPLE_TYPE *vals));
int mus_sound_max_amp_exists  PROTO((const char *ifile));
int mus_file_to_array         PROTO((const char *filename, int chan, int start, int samples, MUS_SAMPLE_TYPE *array));
int mus_array_to_file         PROTO((const char *filename, MUS_SAMPLE_TYPE *ddata, int len, int srate, int channels));
char *mus_array_to_file_with_error PROTO((const char *filename, MUS_SAMPLE_TYPE *ddata, int len, int srate, int channels));


/* -------- audio.c -------- */

#if (HAVE_OSS || HAVE_ALSA)
  #define ALSA_API 0
  #define OSS_API 1
#endif

void mus_audio_describe       PROTO((void));
char *mus_audio_report        PROTO((void));
int mus_audio_open_output     PROTO((int dev, int srate, int chans, int format, int size));
int mus_audio_open_input      PROTO((int dev, int srate, int chans, int format, int size));
int mus_audio_write           PROTO((int line, char *buf, int bytes));
int mus_audio_close           PROTO((int line));
int mus_audio_read            PROTO((int line, char *buf, int bytes));
int mus_audio_mixer_read      PROTO((int dev, int field, int chan, float *val));
int mus_audio_mixer_write     PROTO((int dev, int field, int chan, float *val));
void mus_audio_save           PROTO((void));
void mus_audio_restore        PROTO((void));
int mus_audio_initialize      PROTO((void));
int mus_audio_systems         PROTO((void));
char *mus_audio_system_name   PROTO((int sys));
char *mus_audio_moniker       PROTO((void));

#if HAVE_OSS
  void mus_audio_clear_soundcard_inputs PROTO((void));
#endif
#if (HAVE_OSS || HAVE_ALSA)
  void mus_audio_set_oss_buffers    PROTO((int num, int size));
  int mus_audio_api                 PROTO((void));
#endif

void mus_audio_mixer_save           PROTO((const char *file));
void mus_audio_mixer_restore        PROTO((const char *file));

#ifdef SUN
  void mus_audio_sun_outputs        PROTO((int speakers, int headphones, int line_out));
#endif

#if (defined(HAVE_CONFIG_H)) && (!defined(HAVE_STRERROR))
  char *strerror                    PROTO((int errnum));
#endif



/* -------- io.c -------- */

int mus_file_set_descriptors        PROTO((int tfd, const char *arg, int df, int ds, int dl, int dc, int dt));
#define mus_file_open_descriptors(Tfd, Arg, Df, Ds, Dl, Dc, Dt) mus_file_set_descriptors(Tfd, Arg, Df, Ds, Dl, Dc, Dt)
int mus_file_close_descriptors      PROTO((int tfd));
int mus_file_open_read              PROTO((const char *arg));
int mus_file_probe                  PROTO((const char *arg));
int mus_file_open_write             PROTO((const char *arg));
int mus_file_create                 PROTO((const char *arg));
int mus_file_reopen_write           PROTO((const char *arg));
int mus_file_close                  PROTO((int fd));
long mus_file_seek                  PROTO((int tfd, long offset, int origin));
int mus_file_seek_frame             PROTO((int tfd, int frame));
int mus_file_read                   PROTO((int fd, int beg, int end, int chans, MUS_SAMPLE_TYPE **bufs));
int mus_file_read_chans             PROTO((int fd, int beg, int end, int chans, MUS_SAMPLE_TYPE **bufs, MUS_SAMPLE_TYPE *cm));
int mus_file_write_zeros            PROTO((int tfd, int num));
int mus_file_write                  PROTO((int tfd, int beg, int end, int chans, MUS_SAMPLE_TYPE **bufs));
int mus_file_read_any               PROTO((int tfd, int beg, int chans, int nints, MUS_SAMPLE_TYPE **bufs, MUS_SAMPLE_TYPE *cm));
int mus_file_read_file              PROTO((int tfd, int beg, int chans, int nints, MUS_SAMPLE_TYPE **bufs));
int mus_file_read_buffer            PROTO((int charbuf_data_format, int beg, int chans, int nints, MUS_SAMPLE_TYPE **bufs, char *charbuf));
int mus_file_write_file             PROTO((int tfd, int beg, int end, int chans, MUS_SAMPLE_TYPE **bufs));
int mus_file_write_buffer           PROTO((int charbuf_data_format, int beg, int end, int chans, MUS_SAMPLE_TYPE **bufs, char *charbuf, int clipped));
char *mus_expand_filename           PROTO((char *name));
#define mus_file_full_name(File)    mus_expand_filename(File)

int mus_file_set_data_clipped       PROTO((int tfd, int clipped));
int mus_file_set_header_type        PROTO((int tfd, int type));
int mus_file_header_type            PROTO((int tfd));
char *mus_file_fd_name              PROTO((int tfd));
int mus_file_set_chans              PROTO((int tfd, int chans));
float mus_file_prescaler            PROTO((int tfd));
float mus_file_set_prescaler        PROTO((int tfd, float val));

void mus_bint_to_char               PROTO((unsigned char *j, int x));
int mus_char_to_bint                PROTO((const unsigned char *inp));
void mus_lint_to_char               PROTO((unsigned char *j, int x));
int mus_char_to_lint                PROTO((const unsigned char *inp));
int mus_char_to_uninterpreted_int   PROTO((const unsigned char *inp));
void mus_bfloat_to_char             PROTO((unsigned char *j, float x));
float mus_char_to_bfloat            PROTO((const unsigned char *inp));
void mus_lfloat_to_char             PROTO((unsigned char *j, float x));
float mus_char_to_lfloat            PROTO((const unsigned char *inp));
void mus_bshort_to_char             PROTO((unsigned char *j, short x));
short mus_char_to_bshort            PROTO((const unsigned char *inp));
void mus_lshort_to_char             PROTO((unsigned char *j, short x));
short mus_char_to_lshort            PROTO((const unsigned char *inp));
void mus_ubshort_to_char            PROTO((unsigned char *j, unsigned short x));
unsigned short mus_char_to_ubshort  PROTO((const unsigned char *inp));
void mus_ulshort_to_char            PROTO((unsigned char *j, unsigned short x));
unsigned short mus_char_to_ulshort  PROTO((const unsigned char *inp));
double mus_char_to_ldouble          PROTO((const unsigned char *inp));
double mus_char_to_bdouble          PROTO((const unsigned char *inp));
void mus_bdouble_to_char            PROTO((unsigned char *j, double x));
void mus_ldouble_to_char            PROTO((unsigned char *j, double x));
unsigned int mus_char_to_ubint      PROTO((const unsigned char *inp));
unsigned int mus_char_to_ulint      PROTO((const unsigned char *inp));

int mus_iclamp                      PROTO((int lo, int val, int hi));
MusFloat mus_fclamp                 PROTO((MusFloat lo, MusFloat val, MusFloat hi));

#if LONG_INT_P
  MUS_SAMPLE_TYPE *mus_table2ptr    PROTO((int arr));
  int mus_ptr2table                 PROTO((MUS_SAMPLE_TYPE *arr));
  void mus_untableptr               PROTO((int ip_1));
  #define MUS_SAMPLE_ARRAY(n) mus_table2ptr((int)(n))
  #define MUS_MAKE_SAMPLE_ARRAY(size) mus_ptr2table((MUS_SAMPLE_TYPE *)CALLOC((size), sizeof(MUS_SAMPLE_TYPE)))
  #define MUS_FREE_SAMPLE_ARRAY(p) mus_untableptr((int)(p))
#else
  #define MUS_SAMPLE_ARRAY(n) ((MUS_SAMPLE_TYPE *)(n))
  #define MUS_MAKE_SAMPLE_ARRAY(size) ((MUS_SAMPLE_TYPE *)CALLOC((size), sizeof(MUS_SAMPLE_TYPE)))
  #define MUS_FREE_SAMPLE_ARRAY(p) FREE((void *)(p))
#endif

#ifdef CLM
  /* these are needed to clear a saved lisp image to the just-initialized state */
  void reset_io_c                   PROTO((void));
  void reset_headers_c              PROTO((void));
  void reset_audio_c                PROTO((void));
  void set_rt_audio_p               PROTO((int rt));
#endif



/* -------- headers.c -------- */

int mus_header_samples              PROTO((void));
int mus_header_data_location        PROTO((void));
int mus_header_chans                PROTO((void));
int mus_header_srate                PROTO((void));
int mus_header_type                 PROTO((void));
int mus_header_format               PROTO((void));
int mus_header_distributed          PROTO((void));
int mus_header_comment_start        PROTO((void));
int mus_header_comment_end          PROTO((void));
int mus_header_type_specifier       PROTO((void));
int mus_header_bits_per_sample      PROTO((void));
int mus_header_fact_samples         PROTO((void));
int mus_header_block_align          PROTO((void));
int mus_header_loop_mode            PROTO((int which));
int mus_header_loop_start           PROTO((int which));
int mus_header_loop_end             PROTO((int which));
int mus_header_mark_position        PROTO((int id));
int mus_header_base_note            PROTO((void));
int mus_header_base_detune          PROTO((void));
void mus_header_set_raw_defaults    PROTO((int sr, int chn, int frm));
void mus_header_raw_defaults        PROTO((int *sr, int *chn, int *frm));
int mus_header_true_length          PROTO((void));
int mus_header_original_format      PROTO((void));
int mus_header_data_format_to_bytes_per_sample PROTO((void));
int mus_samples_to_bytes            PROTO((int format, int size));
int mus_bytes_to_samples            PROTO((int format, int size));
int mus_header_write_next_header    PROTO((int chan, int srate, int chans, int loc, int siz, int format, const char *comment, int len));
int mus_header_read_with_fd         PROTO((int chan));
int mus_header_read                 PROTO((const char *name));
int mus_header_write                PROTO((const char *name, int type, int srate, int chans, int loc, int size, int format, const char *comment, int len));
int mus_header_write_with_fd        PROTO((int chan, int type, int in_srate, int in_chans, int loc, int size, int format, const char *comment, int len));
int mus_header_update_with_fd       PROTO((int chan, int type, int siz));
int mus_header_update               PROTO((const char *name, int type, int size, int srate, int format, int chans, int loc));
int mus_header_aux_comment_start    PROTO((int n));
int mus_header_aux_comment_end      PROTO((int n));
int mus_header_update_comment       PROTO((const char *name, int loc, const char *comment, int len, int typ));
int mus_header_initialize           PROTO((void));
void mus_header_snd_set_header      PROTO((int in_srate, int in_chans, int in_format));
int mus_header_aiff_p               PROTO((void));
int mus_header_writable             PROTO((int type, int format));
void mus_header_set_aiff_loop_info  PROTO((int *data));
int mus_header_sf2_entries          PROTO((void));
char *mus_header_sf2_name           PROTO((int n));
int mus_header_sf2_start            PROTO((int n));
int mus_header_sf2_end              PROTO((int n));
int mus_header_sf2_loop_start       PROTO((int n));
int mus_header_sf2_loop_end         PROTO((int n));
const char *mus_header_original_format_name PROTO((int format, int type));

void mus_header_set_aifc            PROTO((int val)); /* backwards compatibility, sort of */
char *mus_header_riff_aux_comment   PROTO((const char *name, int *starts, int *ends));
char *mus_header_aiff_aux_comment   PROTO((const char *name, int *starts, int *ends));


#ifdef DEBUG_MEMORY
  /* snd-utils.c (only used in conjunction with Snd's memory tracking functions) */
  void *mem_calloc                  PROTO((size_t len, size_t size, const char *func, const char *file, int line));
  void *mem_malloc                  PROTO((size_t len, const char *func, const char *file, int line));
  void mem_free                     PROTO((void *ptr, const char *func, const char *file, int line));
  void *mem_realloc                 PROTO((void *ptr, size_t size, const char *func, const char *file, int line));
#endif

END_DECLS

#endif
