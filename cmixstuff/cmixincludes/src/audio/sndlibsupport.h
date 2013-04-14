/* RTcmix  - Copyright (C) 2000  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/

#ifndef _SNDLIB_SUPPORT_H_
#define _SNDLIB_SUPPORT_H_

#if defined(LINUX)
#include <endian.h>      /* so that sndlib.h will get host byte-order right */
#elif defined(MACOSX)
#include <machine/endian.h>
#elif defined(SGI)
#include <sys/endian.h>
#elif defined(FREEBSD)
#include <machine/endian.h>
#else
#error "This platform is not supported by sndlibsupport.h"
#endif
#include <stdio.h>          /* for FILE, needed by sndlib.h */
#include <sndlib.h>
#include <sfheader.h>       /* for SFHEADER */
#include <rtdefs.h>         /* just for MAXCHANS */

/* This value must be OR'd into our sample format when it is used to set the
 * format of an input or output audio device
 */
enum { MUS_NON_INTERLEAVED = 0x1000, MUS_INTERLEAVED = 0x2000 };
#define MUS_INTERLEAVE_MASK 0x3000
#define MUS_GET_INTERLEAVE(fmt) ((fmt) & MUS_INTERLEAVE_MASK)
#define IS_INTERLEAVED_FORMAT(fmt) (MUS_GET_INTERLEAVE(fmt) == MUS_INTERLEAVED)

/* This value may be OR'd into float sample formats to indicate the data is
 * scaled between -1.0 and 1.0.  It should be ignored for any other format.
 */
enum { MUS_NORMALIZED = 0x10000 };
#define IS_NORMALIZED_FORMAT(fmt) (((fmt) & MUS_NORMALIZED) != 0)

/* This value may be OR'd into float sample formats to indicate the data is
 * to be clipped between -32768 and 32767.  It should be ignored for any 
 * other format.
 */
enum { MUS_CLIPPED = 0x100000 };
#define IS_CLIPPED_FORMAT(fmt) (((fmt) & MUS_CLIPPED) != 0)

#define MUS_FORMAT_MASK 0x0fff
#define MUS_GET_FORMAT(fmt) ((fmt) & MUS_FORMAT_MASK)

/* Use these to describe "native" endian for formats */
#if MUS_LITTLE_ENDIAN
#define NATIVE_SHORT_FMT MUS_LSHORT
#define NATIVE_24BIT_FMT MUS_L24INT
#define NATIVE_32BIT_FMT MUS_LINT
#define NATIVE_FLOAT_FMT MUS_LFLOAT
#else
#define NATIVE_SHORT_FMT MUS_BSHORT
#define NATIVE_24BIT_FMT MUS_B24INT
#define NATIVE_32BIT_FMT MUS_BINT
#define NATIVE_FLOAT_FMT MUS_BFLOAT
#endif

/* used to handle encoding peak stats in sound file comment */

/* all 3 constants are meant to include terminating NULL */
#define MAX_COMMENT_CHARS       512        /* not including peak stat text */
#define MAX_PEAK_CHARS          256
#define DEFAULT_COMMENT_LENGTH  (MAX_COMMENT_CHARS + MAX_PEAK_CHARS)

typedef struct {
   int     offset;          /* of peak info in comment; -1 if no peak info */
   float   peak[MAXCHANS];
   long    peakloc[MAXCHANS];
   long    timetag;         /* "calendar time" of last peak stats file update */
   char    comment[MAX_COMMENT_CHARS];
} SFComment;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* functions in sys/sndlibsupport.c */

/* for creating, opening and closing files for sndlib I/O */
int sndlib_create(const char *, int, int, int, int);
int sndlib_open_read(const char *);
int sndlib_open_write(const char *);
int sndlib_close(int, int, int, int, int);

/* for reading and writing headers */
int sndlib_read_header(int);
int sndlib_write_header(int, int, int, int, int, int, char *, int *);
int sndlib_set_header_data_size(int, int, int);
char *sndlib_print_current_header_stats(int, SFComment *, int);

/* for reading and writing header comments with encoded peak stats */
int sndlib_get_current_header_comment(int, SFComment *);
int sndlib_get_header_comment(int, SFComment *);
int sndlib_put_current_header_comment(int, float [], long [], char *);
int sndlib_put_header_comment(int, float [], long [], char *);

/* helper functions */
int sndlib_current_header_comment_alloc_good(char *);
int sfcomment_peakstats_current(const SFComment *, const int);
int sndlib_findpeak(int, int, int, int, int, int, long, long,
                    float [], long [], double [], double [], double []);

/* for cmix legacy support */
int sndlib_rheader(int, SFHEADER *);
int sndlib_wheader(int, SFHEADER *);
int getheadersize(SFHEADER *);
int putlength(char *, int, SFHEADER *);
char *getsfcode(SFHEADER *, int);
int putsfcode(SFHEADER *, char *, SFCODE *);
int getsfmaxamp(SFHEADER *, SFMAXAMP *);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

/* some handy macros */

#define IS_FLOAT_FORMAT(format) (                        \
              MUS_GET_FORMAT(format) == MUS_BFLOAT       \
           || MUS_GET_FORMAT(format) == MUS_LFLOAT       )

#define IS_32BIT_FORMAT(format) (                        \
              MUS_GET_FORMAT(format) == MUS_BINT         \
           || MUS_GET_FORMAT(format) == MUS_LINT         )

#define IS_24BIT_FORMAT(format) (                        \
              MUS_GET_FORMAT(format) == MUS_B24INT       \
           || MUS_GET_FORMAT(format) == MUS_L24INT       )

#define IS_SHORT_FORMAT(format) (                        \
              MUS_GET_FORMAT(format) == MUS_BSHORT       \
           || MUS_GET_FORMAT(format) == MUS_LSHORT       )

#define NOT_A_SOUND_FILE(header_type) (                  \
              (header_type) == MUS_UNSUPPORTED           \
           || (header_type) == MUS_RAW                   )

/* The header types that sndlib can write, as of sndlib-13.1. */
#define WRITEABLE_HEADER_TYPE(type) (                    \
               (type) == MUS_AIFC                        \
           ||  (type) == MUS_AIFF                        \
           ||  (type) == MUS_NEXT                        \
           ||  (type) == MUS_RIFF                        \
           ||  (type) == MUS_IRCAM                       \
           ||  (type) == MUS_RAW                         )

#define INVALID_DATA_FORMAT(format) ((format) < 1)

/* The data formats that RTcmix can read and write. */
#define SUPPORTED_DATA_FORMAT(format) (                  \
               (format) == MUS_BSHORT                    \
           ||  (format) == MUS_LSHORT                    \
           ||  (format) == MUS_BFLOAT                    \
           ||  (format) == MUS_LFLOAT                    \
           ||  (format) == MUS_B24INT                    \
           ||  (format) == MUS_L24INT                    \
           ||  (format) == MUS_BINT                      \
           ||  (format) == MUS_LINT                      )

/* The data formats that disk-based cmix can read and write. */
#define SUPPORTED_DATA_FORMAT_NO24BIT(format) (          \
               (format) == MUS_BSHORT                    \
           ||  (format) == MUS_LSHORT                    \
           ||  (format) == MUS_BFLOAT                    \
           ||  (format) == MUS_LFLOAT                    )

/* Usually endianness is handled for us by sndlib, but there are some
   cases where it's helpful to know (e.g., sfcreate). These are the
   more common formats sndlib supports -- not all of them.
*/
#define IS_BIG_ENDIAN_FORMAT(format) (                   \
              (format) == MUS_BSHORT                     \
           || (format) == MUS_BFLOAT                     \
           || (format) == MUS_B24INT                     \
           || (format) == MUS_BINT                       \
           || (format) == MUS_UBSHORT                    )

#define IS_LITTLE_ENDIAN_FORMAT(format) (                \
              (format) == MUS_LSHORT                     \
           || (format) == MUS_LFLOAT                     \
           || (format) == MUS_L24INT                     \
           || (format) == MUS_LINT                       \
           || (format) == MUS_ULSHORT                    )

#define FORMATS_SAME_BYTE_ORDER(format1, format2) (      \
             ( IS_BIG_ENDIAN_FORMAT(format1)             \
            && IS_BIG_ENDIAN_FORMAT(format2) )           \
         ||  ( IS_LITTLE_ENDIAN_FORMAT(format1)          \
            && IS_LITTLE_ENDIAN_FORMAT(format2) )        )

#define SFCOMMENT_PEAKSTATS_VALID(sfcptr)  ((sfcptr)->offset != -1)


/* These macros nabbed from sndlib/io.c, since they aren't exposed to us,
   and they're useful for doing our own i/o.
*/

#if MUS_LITTLE_ENDIAN

  #define m_big_endian_short(n)                   (mus_char_to_bshort(n))
  #define m_big_endian_int(n)                     (mus_char_to_bint(n))
  #define m_big_endian_float(n)                   (mus_char_to_bfloat(n))
  #define m_big_endian_double(n)                  (mus_char_to_bdouble(n))
  #define m_big_endian_unsigned_short(n)          (mus_char_to_ubshort(n))

  #define m_little_endian_short(n)                (*((short *)(n)))
  #define m_little_endian_int(n)                  (*((int *)(n)))
  #define m_little_endian_float(n)                (*((float *)(n)))
  #define m_little_endian_double(n)               (*((double *)(n)))
  #define m_little_endian_unsigned_short(n)       (*((unsigned short *)(n)))

  #define m_set_big_endian_short(n, x)             mus_bshort_to_char(n, (short)(x))
  #define m_set_big_endian_int(n, x)               mus_bint_to_char(n, (int)(x))
  #define m_set_big_endian_float(n, x)             mus_bfloat_to_char(n, (float)(x))
  #define m_set_big_endian_double(n, x)            mus_bdouble_to_char(n, (double)(x))
  #define m_set_big_endian_unsigned_short(n, x)    mus_ubshort_to_char(n, (unsigned short)(x))

  #define m_set_little_endian_short(n, x)          (*((short *)(n))) = (short)(x)
  #define m_set_little_endian_int(n, x)            (*((int *)(n))) = (int)(x)
  #define m_set_little_endian_float(n, x)          (*((float *)(n))) = (float)(x)
  #define m_set_little_endian_double(n, x)         (*((double *)(n))) = (double)(x)
  #define m_set_little_endian_unsigned_short(n, x) (*((unsigned short *)(n))) = (unsigned short)(x)

#else

  #ifndef SUN
    #define m_big_endian_short(n)                   (*((short *)(n)))
    #define m_big_endian_int(n)                     (*((int *)(n)))
    #define m_big_endian_float(n)                   (*((float *)(n)))
    #define m_big_endian_double(n)                  (*((double *)(n)))
    #define m_big_endian_unsigned_short(n)          (*((unsigned short *)(n)))

    #define m_set_big_endian_short(n, x)             (*((short *)(n))) = (short)(x)
    #define m_set_big_endian_int(n, x)               (*((int *)(n))) = (int)(x)
    #define m_set_big_endian_float(n, x)             (*((float *)(n))) = (float)(x)
    #define m_set_big_endian_double(n, x)            (*((double *)(n))) = (double)(x)
    #define m_set_big_endian_unsigned_short(n, x)    (*((unsigned short *)(n))) = (unsigned short)(x)
  #else
    #define m_big_endian_short(n)                   (mus_char_to_bshort(n))
    #define m_big_endian_int(n)                     (mus_char_to_bint(n))
    #define m_big_endian_float(n)                   (mus_char_to_bfloat(n))
    #define m_big_endian_double(n)                  (mus_char_to_bdouble(n))
    #define m_big_endian_unsigned_short(n)          (mus_char_to_ubshort(n))

    #define m_set_big_endian_short(n, x)             mus_bshort_to_char(n, (short)(x))
    #define m_set_big_endian_int(n, x)               mus_bint_to_char(n, (int)(x))
    #define m_set_big_endian_float(n, x)             mus_bfloat_to_char(n, (float)(x))
    #define m_set_big_endian_double(n, x)            mus_bdouble_to_char(n, (double)(x))
    #define m_set_big_endian_unsigned_short(n, x)    mus_ubshort_to_char(n, (unsigned short)(x))
  #endif

  #define m_little_endian_short(n)                  (mus_char_to_lshort(n))
  #define m_little_endian_int(n)                    (mus_char_to_lint(n))
  #define m_little_endian_float(n)                  (mus_char_to_lfloat(n))
  #define m_little_endian_double(n)                 (mus_char_to_ldouble(n))
  #define m_little_endian_unsigned_short(n)         (mus_char_to_ulshort(n))

  #define m_set_little_endian_short(n, x)            mus_lshort_to_char(n, (short)(x))
  #define m_set_little_endian_int(n, x)              mus_lint_to_char(n, (int)(x))
  #define m_set_little_endian_float(n, x)            mus_lfloat_to_char(n, (float)(x))
  #define m_set_little_endian_double(n, x)           mus_ldouble_to_char(n, (double)(x))
  #define m_set_little_endian_unsigned_short(n, x)   mus_ulshort_to_char(n, (unsigned short)(x))

#endif	// MUS_LITTLE_ENDIAN

#endif	// _SNDLIB_SUPPORT_H_

