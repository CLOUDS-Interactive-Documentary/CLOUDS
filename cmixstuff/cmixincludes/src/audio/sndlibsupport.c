/* RTcmix  - Copyright (C) 2001  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/

/* sndlibsupport.c

   This file defines functions that help RTcmix use Bill Schottstaedt's
   sndlib sound file library. The library lets us read and write sound
   files with various types of header, and it hides most of the endian
   issues. The functions in this file access the lower-level interfaces
   to his library, not the higher-level ones defined in sndlib/sound.c.

   The code serves three main purposes:

     - Provide (as much as possible) backwards compatibility for
       legacy cmix code.

     - Provide wrapper functions for some sndlib calls.

     - RtcHandle my scheme for encoding peak amplitude stats in header
       comments, so that they're available for all the header types
       that sndlib can write. The jury's out on how well this works.

                                                      -- J. Gibson, 6/1/99

   Updated for sndlib v. 13.1   -JGG, 7/19/01
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>
#include <errno.h>
#include <assert.h>

#include <sndlibsupport.h>
#include <byte_routines.h>

#ifndef SGI
 #define HAVE_SNPRINTF
#endif

/* #define NDEBUG */     /* define to disable asserts */

/* Not NULL, and not space, to make sndlib believe this starts a comment. */
#define COMMENT_START_CHAR 0x01

#define COMMENT_ALLOC_WARNING "\
WARNING: Not enough room in header for peak stats and comment.\n"

#undef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))

static int open_rd_or_rdwr(const char *, int);
static int format_raw_comment(SFComment *, int, char **);
static int get_current_header_comment_alloc(void);
static int get_current_header_raw_comment(int, char **);



/* ========================================================================== */
/* functions for opening and closing sound files */

/* sndlib_create, sndlib_open_read, sndlib_open_write and sndlib_close
   are wrappers for various sndlib functions. They're meant to be called
   only when you plan to use sndlib to do sound data I/O. Don't call them
   if you're just reading and writing headers (as we do for legacy cmix
   code) -- call close and open directly.

   None of these functions check whether the file name exists before opening
   the file. If it's important, the caller should handle this first.

   (These wrapper functions do the right things to handle sndlib's internal
   file descriptor scheme, so callers don't need to worry about this.)
*/

/* -------------------------------------------------------- sndlib_create --- */
/* Creates a new file and writes a header with the given characteristics.
   <type> is a sndlib constant for header type (e.g. MUS_AIFC).
   <format> is a sndlib constant for sound data format (e.g. MUS_BSHORT).
   (These constants are defined in sndlib.h.) Caller is responsible for
   checking that the header type and data format are compatible, and that
   the header type is one that sndlib can write (is WRITEABLE_HEADER_TYPE()).
   Writes no comment, but allocates enough space in the header for one.

   NOTE: This will truncate an existing file with <sfname>, so check first!

   On success, returns a standard file descriptor, and leaves the file
   position pointer at the end of the header.
   On failure, returns -1. Caller can check errno then.
*/
int
sndlib_create(const char *sfname, int type, int format, int srate, int chans)
{
   int  fd, loc;

   assert(sfname != NULL && strlen(sfname) <= FILENAME_MAX);

   /* make sure relevant parts of sndlib are initialized */
   mus_header_initialize();

   fd = open(sfname, O_RDWR | O_CREAT | O_TRUNC, 0666);
   if (fd == -1)
      return -1;

   if (sndlib_write_header(fd, 0, type, format, srate, chans, NULL, &loc) == -1)
      return -1;

   mus_file_set_descriptors(fd, sfname, format,
                            mus_data_format_to_bytes_per_sample(format),
                            loc, chans, type);

   return fd;
}


/* ------------------------------------------------------ open_rd_or_rdwr --- */
/* Does the dirty work for sndlib_open_read and sndlib_open_write (below).
*/
static int
open_rd_or_rdwr(const char *sfname, int accesstype)
{
   int fd, format, loc, chans, type;

   assert(sfname != NULL && strlen(sfname) <= FILENAME_MAX);

   fd = open(sfname, accesstype);
   if (fd == -1)
      return -1;

   if (sndlib_read_header(fd) == -1)
      return -1;

   type = mus_header_type();
   format = mus_header_format();
   chans = mus_header_chans();
   loc = mus_header_data_location();

   mus_file_set_descriptors(fd, sfname, format,
                            mus_data_format_to_bytes_per_sample(format),
                            loc, chans, type);

   if (lseek(fd, loc, SEEK_SET) == -1) {
      perror("open_rd_or_rdwr: lseek");
      return -1;
   }
   return fd;
}


/* ----------------------------------------------------- sndlib_open_read --- */
/* Returns a new file descriptor for <sfname>, opened read-only, and reads
   its header into the sndlib header buffer. After this, the caller can
   use sndlib functions (like mus_header_type, mus_header_chans, etc.)
   to gather information from the header.

   Leaves file position pointer at start of sound data.

   The caller should check that this is actually a sound file by using
   the NOT_A_SOUND_FILE macro (sndlibsupport.h).
   The caller might want to check first that this file exists.
*/
int
sndlib_open_read(const char *sfname)
{
   return open_rd_or_rdwr(sfname, O_RDONLY);
}


/* ---------------------------------------------------- sndlib_open_write --- */
/* Just like sndlib_open_read, except that it opens the file for read/write
   access. See the comments at sndlib_open_read for more.
*/
int
sndlib_open_write(const char *sfname)
{
   return open_rd_or_rdwr(sfname, O_RDWR);
}


/* --------------------------------------------------------- sndlib_close --- */
/* Closes the file descriptor <fd> and, if <update> is true, updates its
   header with the number of samples (not frames) contained in the file.
   <type> is the type of header; <format> is the data format of the samples.
   If <update> is false, caller can pass any value for the last 3 args.

   Returns 0 on success; -1 on failure (check errno for reason).
*/
int
sndlib_close(int fd, int update, int type, int format, int nsamps)
{
   assert(fd >= 0);

   if (update) {
      int sound_bytes;

      assert(format > 0 && WRITEABLE_HEADER_TYPE(type));

      sound_bytes = nsamps * mus_data_format_to_bytes_per_sample(format);

      /* keep going even if this fails */
      sndlib_set_header_data_size(fd, type, sound_bytes);
   }

   mus_file_close_descriptors(fd);

   return close(fd);
}



/* ========================================================================== */
/* functions for reading and writing headers */


/* --------------------------------------------------- sndlib_read_header --- */
/* Reads the header of <fd> into the sndlib header buffer, so that we then
   can call sndlib functions (like mus_header_type, mus_header_chans, etc.)
   to gather information from the header.

   When a header has been read into the header buffer, we call it the
   "current header". Any function in this file with "current_header" as
   part of its name depends on a previous call to sndlib_read_header.
*/
int
sndlib_read_header(int fd)
{
   assert(fd >= 0);

   /* Init the part of sndlib we need -- only inits once. */
   mus_header_initialize();

   if (lseek(fd, 0, SEEK_SET) == -1)
      return -1;

   return mus_header_read_with_fd(fd);
}


/* -------------------------------------------------- sndlib_write_header --- */
/* (a wrapper for mus_header_write_with_fd in sndlib/headers.c)

   Writes a header to the sound file opened as <fd>. If <data_location> is
   not NULL, sets the value it references to the offset of the start of sound
   data in the file. (This is computed by sndlib during the header write.)

   Assumes that <header_type> is one writeable by sndlib. (Find out in advance
   by using the WRITEABLE_HEADER_TYPE macro.) Caller is responsible for knowing
   if <header_type> supports <data_format> (including endianness concerns).

   The tricky issue is how to deal with <rawcomment>. We call it a "raw"
   comment, because it's the actual comment we write to the header, including
   any encoded peak stats. If the file already has a header (indicated by
   the <header_exists> flag), and <rawcomment> is not NULL, we write this
   comment to the file, while preserving the amount of space reserved in the
   file header for its comment. If there isn't enough space in the file for
   <rawcomment>, we print a warning and write as much of the comment as will
   fit. If <rawcomment> is NULL (whether or not <header_exists> is true), we
   allocate DEFAULT_COMMENT_LENGTH bytes for comment space in the header.

   The idea is to balance two desirable, but sometimes incompatible, aims:
   keeping enough space in headers for our structured comment, and preserving
   the sound data start location in existing files, so that a header write
   doesn't cause sound data loss or corruption (e.g., channel swapping or bad
   sample word alignment).

   So the caller has to know what it's getting into. It's best for functions
   outside of this file to pass NULL for <rawcomment>, and later use
   sndlib_put_header_comment to write peak stats or a text comment.

   CAUTION: Be careful not to change the header type of an existing file!
   This likely will change the sound data start location, with the same
   consequences (listed just above) as changing the comment allocation.

   IMPORTANT: Before closing this sound file, you must update the header's
   idea of how much sound data is in the file, by calling
   sndlib_set_header_data_size.

   On success, leaves file position pointer at end of header and returns 0;
   Otherwise, returns -1.
*/
int
sndlib_write_header(int  fd,
                    int  header_exists,
                    int  header_type,
                    int  data_format,
                    int  srate,
                    int  chans,
                    char *rawcomment,
                    int  *data_location)
{
   int  result, comment_len = 0;
   char *comment;

   assert(fd >= 0 && header_type > -1 && data_format > 0);

   if (data_location)
      *data_location = 0;

   if (header_exists & (rawcomment != NULL)) {
      if (sndlib_read_header(fd) == -1) {
         fprintf(stderr, "sndlib_write_header: Error reading header\n");
         return -1;
      }
      comment_len = get_current_header_comment_alloc();

      if (strlen(rawcomment) > comment_len)
         fprintf(stderr, COMMENT_ALLOC_WARNING);
   }
   else
      comment_len = DEFAULT_COMMENT_LENGTH;

   comment = (char *)calloc(comment_len + 1, sizeof(char));
   if (comment == NULL) {
      perror("sndlib_write_header: calloc");
      return -1;
   }

   /* sndlib now ignores comments that start with nulls or spaces, so
      we trick it into accepting our comment by storing a different
      character into the beginning of the string.  -JGG, 7/20/01
   */
   comment[0] = COMMENT_START_CHAR;

   if (rawcomment)
      strncpy(comment, rawcomment, comment_len);

   /* Now here's some awful stuff ... the sndlib function
      mus_header_write_next_header fiddles around with the data location
      field of the header (to handle a word-alignment problem? -- not sure).
      The workaround for now is to decrement the comment length passed to
      mus_header_write_with_fd for NeXT headers. A side-effect is that the
      last char of the comment won't get written. But our comments are zero-
      padded so much that there's not likely to be any important data loss.
   */
   if (header_type == MUS_NEXT)
      if (!(comment_len % 4))
         comment_len--;

   if (lseek(fd, 0, SEEK_SET) == -1) {
      perror("sndlib_write_header: lseek");
      return -1;
   }

   mus_header_initialize();            /* in case this hasn't been done yet */

   result = mus_header_write_with_fd(fd, header_type, srate, chans, 0, 0,
                                          data_format, comment, comment_len);
   if (result != MUS_NO_ERROR) {
      fprintf(stderr, "sndlib_write_header: Can't write header\n");
      return -1;
   }

   /* Data location computed during header write. */
   if (data_location)
      *data_location = mus_header_data_location();

   free(comment);

   return 0;
}


/* ------------------------------------------ sndlib_set_header_data_size --- */
/* Update the header of file opened as <fd>, of type <header_type>, with
   the number of bytes of sound data written to the file, <sound_bytes).
*/
int
sndlib_set_header_data_size(int fd, int header_type, int sound_bytes)
{
   assert(fd >= 0 && header_type >= 0 && sound_bytes >= 0);

   /* Note: as of sndlib-5.5, this doesn't require us to make sure
      file's header is in sndlib header buffer. Nor do we have to
      seek to the beginning of the file first.
   */
   return mus_header_update_with_fd(fd, header_type, sound_bytes);
}


/* ------------------------------------ sndlib_print_current_header_stats --- */
/* Prints info about the current header into a static string, and returns a
   pointer to this. Caller should copy this if necessary. If <sfc> is NULL,
   calls sndlib_get_current_header_comment to retrieve peak stats, otherwise
   expects that <sfc> holds them. Assumes that <fd> is the file descriptor
   of the current header. <verbosity> indicates how much info to print:
      0 = header type, data format, srate, chans, duration
      1 = adds peak stats, if any
      2 = adds number of frames and header size
   Doesn't print the file name or any plain text comment it might have.
*/
char *
sndlib_print_current_header_stats(int fd, SFComment *sfc, int verbosity)
{
   int         type, format, data_loc, srate, nchans;
   int         n, nchars;
   long        nsamps;
   float       duration;
   SFComment   *sfcp, nsfc;
   static char str[1024];

   assert(fd >= 0);

   if (sfc == NULL && verbosity > 0) {
      if (sndlib_get_current_header_comment(fd, &nsfc) == -1) {
         fprintf(stderr, "Can't read header comment!\n");
         nsfc.offset = -1;          /* make sure it's ignored below */
         nsfc.comment[0] = '\0';
         /* but keep going */
      }
      sfcp = &nsfc;
   }
   else
      sfcp = sfc;

   type = mus_header_type();
   format = mus_header_format();
   data_loc = mus_header_data_location();
   srate = mus_header_srate();
   nchans = mus_header_chans();
   nsamps = mus_header_samples();           /* samples, not frames */
   duration = (float)(nsamps / nchans) / (float)srate;

#ifdef HAVE_SNPRINTF
   nchars = snprintf(str, 256,
                    "%s, %s\nsrate: %d  chans: %d\nduration: %g seconds\n",
                     mus_header_type_name(type),
                     mus_data_format_name(format), srate, nchans, duration);
#else
   nchars = sprintf(str, 
                    "%s, %s\nsrate: %d  chans: %d\nduration: %g seconds\n",
                     mus_header_type_name(type),
                     mus_data_format_name(format), srate, nchans, duration);
#endif

   if (verbosity == 2) {
      char tmp[128];

#ifdef HAVE_SNPRINTF
      snprintf(tmp, 128, "frames: %ld\nheader size: %d bytes\n",
                                                 nsamps / nchans, data_loc);
#else
      sprintf(tmp, "frames: %ld\nheader size: %d bytes\n",
                                                 nsamps / nchans, data_loc);
#endif
      strcat(str, tmp);
   }

   if (verbosity > 0) {                 /* want this after verbosity=2 info */
      char tmp[80];

      if (SFCOMMENT_PEAKSTATS_VALID(sfcp)) {
         strcat(str, "peak amplitudes:\n");
         for (n = 0; n < nchans; n++) {
#ifdef HAVE_SNPRINTF
            snprintf(tmp, 80, "  chan %d:  peak: %g  loc: %ld\n",
                                         n, sfcp->peak[n], sfcp->peakloc[n]);
#else
            sprintf(tmp, "  chan %d:  peak: %g  loc: %ld\n",
                                         n, sfcp->peak[n], sfcp->peakloc[n]);
#endif
            strcat(str, tmp);
         }
         strftime(tmp, 80, "  peak stats updated: %a %b %d %H:%M:%S %Z %Y\n",
                           localtime(&sfcp->timetag));
         strcat(str, tmp);
      }
      else
         strcat(str, "(no peak amp stats)\n");
   }

   return str;
}


/* ========================================================================== */
/* functions for reading the header comment with encoded peak stats */


/* === PEAK STATS ===
   Peak amps are encoded at the end of the sound file comment as follows:

      "[peakamp: <peak> at <loc>, <peak> at <loc>; time=<time>]"

   where...

      <peak> is a (possibly floating point) peak amplitude,

      <loc> is an integer frame number, and

      <time> is the time of last peak stats update in seconds since 1970.

   There will be one <peak>, <loc> pair for each channel.

   The text must appear exactly in this format (e.g., with brackets and
   punctuation) to be parsed by RTcmix. (The function that parses this is
   sndlib_get_current_header_comment; the function that writes this is
   format_raw_comment; both in this file.)

   For example:
      "[peakamp: 32767 at 1037829, 27498.58 at 1048292; time=9283478561]"

   Note that the <time> tag is not year 2038 compliant.  ;-)
*/

#define PEAK_TAG      "[maxamp:"


/* ------------------------------------ sndlib_get_current_header_comment --- */
/* Fills in an SFComment struct for the given sound file descriptor <fd>.
   Returns -1 if there's an error; otherwise returns 0.
   Does NOT close the file.

   Reads the comment from the file, and checks to see if it encodes peak
   stats in the format given just above. If it does, then parses the stats
   and stores them into the <peak> and <peakloc> fields of the SFComment
   struct. Stores the time of the last update to the file's peak stats
   into the <timetag> field of the SFComment struct. Stores the comment
   text preceding -- but not including -- the peak stat text into the
   <comment> field of the SFComment struct.

   NOTE: Text following the peak stats will be ignored.

   If there is no comment in the file, zeros the <comment> field and
   returns 0. If there are no parseable peak stats (and there can't be
   if there's no comment!), sets the <offset> field to -1 and returns 0.
   (Use the SFCOMMENT_PEAKSTATS_VALID macro to see if offset not -1.)

   If the comment appears to have peak stats, but we fail to parse them,
   the <comment> field will contain as much of the file's comment text as
   it will hold; we print a warning and set <offset> to -1.

   Note that there can be peak stats AND an empty <comment> field, in the
   (very common) case where the file's comment contains only peak stats.

   Assumes the header has already been read into sndlib's header buffer.
*/
int
sndlib_get_current_header_comment(int fd, SFComment *sfc)
{
   int       n, len, maxchars, found;
   char      *buf, *pstr;

   assert(fd >= 0 && sfc != NULL);

   sfc->offset = -1;
   sfc->comment[0] = '\0';
   sfc->timetag = 0L;

   len = get_current_header_raw_comment(fd, &buf);
   if (len == -1)
      return -1;
   if (len == 0)
      return 0;                     /* no comment text at all (not an error) */

   if (buf[0] == COMMENT_START_CHAR) { /* empty comment (not an error) */
      free(buf);
      return 0;
   }

   maxchars = MIN(len, MAX_COMMENT_CHARS - 1);   /* not including term. NULL */

   /* See if comment contains peak info. */

   found = 0;
   pstr = buf;
   while (!found) {
      pstr = strrchr(pstr, '[');      /* search backwards from end of string */
      if (pstr == NULL)
         break;
      if (strncmp(pstr, PEAK_TAG, strlen(PEAK_TAG)) == 0) {
         found = 1;
         break;
      }
      if (pstr == buf)        /* '[' at beginning of string, but no PEAK_TAG */
         break;
   }

   /* Copy buf into sfc->comment. This will contain as much of the real
      file comment as will fit, including any encoded peak stats. If we
      successfully parse the peak stats below, we'll chop them off
      the end of sfc->comment. (We don't alter the file, of course!)
   */
   strncpy(sfc->comment, buf, maxchars);
   sfc->comment[maxchars] = '\0';                      /* ensure termination */

   /* If we found the peak info tag, parse the statement it introduces, and
      store the peak values into our SFComment struct.  The error checking
      below seems paranoid, but that's because a user could mangle the peak
      info while editing a comment.
   */
   if (found) {
      int   nchans;
      long  timetag;
      char  *p, *pos;

      sfc->offset = pstr - buf;

      p = strchr(pstr, ':');
      if (p == NULL)
         goto parse_err;
      p++;

      nchans = mus_header_chans();

      for (n = 0; n < nchans; n++) {
         double  peak;
         long    peakloc;

         /* read peak amp value */
         pos = NULL;
         errno = 0;
         peak = strtod(p, &pos);
         if (peak == 0.0 && pos == p)      /* no conversion performed */
            goto parse_err;
         if (errno == ERANGE)              /* overflow or underflow */
            goto parse_err;
         sfc->peak[n] = (float)peak;

         /* find peak location value */
         p = strchr(p, 'a');               /* find "at" */
         if (p == NULL)
            goto parse_err;
         if (*(p+1) != 't')
            goto parse_err;
         p += 2;                           /* skip over "at" */

         /* read peak location value */
         pos = NULL;
         errno = 0;
         peakloc = strtol(p, &pos, 10);
         if (pos == p)                     /* no digits to convert */
            goto parse_err;
         if (errno == ERANGE)              /* overflow or underflow */
            goto parse_err;
         sfc->peakloc[n] = peakloc;

         /* skip to info for next channel, or stop */

         if (*pos == ';')                  /* reached end of peak info */
            break;                         /* even if file has more chans */

         p = pos + 1;                      /* skip over ',' separating chans */
      }
      if (*pos != ';')
         goto parse_err;                   /* no time tag separator */

      p = strchr(pos, '=');                /* find end of "time=" */
      if (p == NULL)
         goto parse_err;

      p++;                                 /* skip over '=' */

      /* read time tag value */
      pos = NULL;
      errno = 0;
      timetag = strtol(p, &pos, 10);
      if (pos == p)                        /* no digits to convert */
         goto parse_err;
      if (errno == ERANGE)                 /* overflow or underflow */
         goto parse_err;
      sfc->timetag = timetag;

      /* truncate comment to omit peak info (and anything after!) */
      if (sfc->offset < maxchars) {
         int ind = sfc->offset;
         sfc->comment[ind] = '\0';
         if (sfc->comment[ind - 1] == '\n')
            sfc->comment[ind - 1] = '\0';  /* chop newline */
      }
   }

   free(buf);
   return 0;

parse_err:
   sfc->offset = -1;
   fprintf(stderr, "WARNING: Can't parse peak amp stats!\n");
   free(buf);
   return 0;       /* still not really an error! */
}


/* -------------------------------------------- sndlib_get_header_comment --- */
/* A wrapper for sndlib_get_current_header_comment() that ensures sndlib
   has the header for <fd> in its buffer. Returns 0 on success, -1 on failure.
*/
int
sndlib_get_header_comment(int fd, SFComment *sfc)
{
   if (sndlib_read_header(fd) == -1) {
      fprintf(stderr, "sndlib_get_header_comment: Error reading header\n");
      return -1;
   }

   return sndlib_get_current_header_comment(fd, sfc);
}



/* ========================================================================== */
/* functions for writing the header comment with encoded peak stats */


/* --------------------------------------------------- format_raw_comment --- */
/* Translate the data in the SFComment struct <sfc> into the string
   <rawcomment>, for writing into the file header comment. The struct
   contains peak stats and a plain text comment. <rawcomment> encodes
   these according to the syntax described above (see "PEAK STATS").
   <nchans> is the number of channels for the sound file. If sfc->offset
   is -1, doesn't encode the peak stats.

   <rawcomment> is malloc'd in this function; caller should free it.
   NOTE: no sound file i/o done in this function.

   Returns 0 if OK, -1 if error.
*/
static int
format_raw_comment(SFComment *sfc, int nchans, char **rawcomment)
{
   int   n;
   char  tmp[128], peakstr[DEFAULT_COMMENT_LENGTH];

   /* allocate comment string to pass back */
   *rawcomment = (char *)calloc(DEFAULT_COMMENT_LENGTH, sizeof(char));
   if (*rawcomment == NULL) {
      perror("format_raw_comment: calloc");
      return -1;
   }

   /* prepare peak stats string */
   peakstr[0] = 0;
   if (SFCOMMENT_PEAKSTATS_VALID(sfc)) {        /* use supplied peak stats */
      strcat(peakstr, PEAK_TAG);
      for (n = 0; n < nchans; n++) {
         sprintf(tmp, " %.9g at %ld,", sfc->peak[n], sfc->peakloc[n]);
         strcat(peakstr, tmp);
      }
      peakstr[strlen(peakstr) - 1] = ';';         /* replace last ',' */
      sprintf(tmp, " time=%ld]", sfc->timetag);
      strcat(peakstr, tmp);

      if (strlen(peakstr) >= MAX_PEAK_CHARS) {
         fprintf(stderr,
              "format_raw_comment: Not enough comment space for peak stats\n");
         return -1;
      }
   }
   /* else peak stats empty */

   /* prepare comment string */
   strncpy(*rawcomment, sfc->comment, MAX_COMMENT_CHARS - 1);

   /* append peak stats string to comment string */
   if (peakstr[0] && (*rawcomment)[0])
      strcat(*rawcomment, "\n");    /* separate real comment from peak stats */
   strcat(*rawcomment, peakstr);

   return 0;
}


/* ------------------------------------ sndlib_put_current_header_comment --- */
/* Updates the header of the existing sound file descriptor <fd> using the
   supplied values for peak amplitudes and their locations (in frames from
   beginning of file), and the <comment> string. The <peak> and <peakloc>
   arrays have an element for each channel in the sound file. If <peak> or
   <peakloc> is NULL, doesn't update the file's peak amp stats. If <comment>
   is NULL, doesn't update the comment. If it updates the peak stats, it
   also updates their timetag ("calendar time") when stats written.

   Note that the peak amp stats are encoded as part of the file comment.
   (See above for an explanation of this scheme.)

   Returns 0 on success, and leaves file position pointer at end of header.
   Otherwise, returns -1. Does NOT close the file.

   NOTE: This resets the header's data size field to zero, so you have to
   call sndlib_set_header_data_size later for this file!

   Assumes the header has already been read into sndlib's header buffer.
*/
int
sndlib_put_current_header_comment(int    fd,
                                  float  peak[],
                                  long   peakloc[],
                                  char   *comment)
{
   int        result, header_type, chans;
   char       *rawcomment;
   SFComment  sfc;

   assert(fd >= 0);

   header_type = mus_header_type();

   if (!WRITEABLE_HEADER_TYPE(header_type)) {
      fprintf(stderr, "sndlib_put_current_header_comment:\n");
      fprintf(stderr, "  can't write this type of header.\n");
      return -1;
   }
   if (header_type == MUS_RAW)     /* We can't write a comment for these. */
      return 0;

   /* Initialize SFComment struct. */
   memset(&sfc, 0, sizeof(SFComment));
   sfc.offset = -1;

   /* If caller isn't supplying all the data, we have to get some of it
      from the existing file header.
   */
   if (comment == NULL || peak == NULL || peakloc == NULL) {
      result = sndlib_get_current_header_comment(fd, &sfc);
      if (result == -1)
         return -1;
   }

   chans = mus_header_chans();

   if (peak && peakloc) {                         /* use supplied peak stats */
      struct timeval tp;
      int n;

      for (n = 0; n < chans; n++) {
         sfc.peak[n] = peak[n];
         sfc.peakloc[n] = peakloc[n];
      }
      gettimeofday(&tp, NULL);
      sfc.timetag = tp.tv_sec;

      /* Though we don't know what the real offset (bytes from header
         comment start to peak stats text) will be, we set this to
         something other than -1, so that format_raw_comment() won't
         ignore the peak stats passed to it.
      */
      sfc.offset = 0;
   }
   if (comment) {                                 /* use supplied comment */
      strncpy(sfc.comment, comment, MAX_COMMENT_CHARS - 1);
      sfc.comment[MAX_COMMENT_CHARS - 1] = '\0';  /* ensure termination */
   }

   /* Formatted comment text passed back in malloc'd <rawcomment>. */
   result = format_raw_comment(&sfc, chans, &rawcomment);
   if (result == -1)
      return -1;

   /* Check that file header has enough space allocated for comment.
      Not likely to be a problem, since we pre-allocate enough space
      in rtoutput and sfcreate. But DEFAULT_COMMENT_LENGTH could change,
      or the user could save the sound file with Snd, etc.
   */
   if (!sndlib_current_header_comment_alloc_good(rawcomment)) {
      fprintf(stderr, "sndlib_put_current_header_comment:");
      fprintf(stderr, "  not enough space for comment in file header\n");
      result = -1;
      goto err;
   }

   result = sndlib_write_header(fd, 1, header_type, mus_header_format(),
                                mus_header_srate(), chans, rawcomment, NULL);
   if (result == -1)
      goto err;

   result = 0;
err:
   if (rawcomment)
      free(rawcomment);
   return result;
}


/* -------------------------------------------- sndlib_put_header_comment --- */
/* A wrapper for sndlib_put_current_header_comment() that ensures sndlib
   has the header for <fd> in its buffer. Returns 0 on success, -1 on failure.
*/
int
sndlib_put_header_comment(int fd, float peak[], long peakloc[], char *comment)
{
   if (sndlib_read_header(fd) == -1) {
      fprintf(stderr, "sndlib_put_header_comment: Error reading header\n");
      return -1;
   }

   return sndlib_put_current_header_comment(fd, peak, peakloc, comment);
}



/* ========================================================================== */
/* miscellaneous helper functions */


/* ------------------------------------- get_current_header_comment_alloc --- */
static int
get_current_header_comment_alloc()
{
   int  comment_start, comment_end, len;

   /* offset of first comment byte (from start of header) */
   comment_start = mus_header_comment_start();
   // Not a valid assumption, since Peak writes comments after sound data. -JGG
   //assert(comment_start >= 0 && comment_start < 3000);  /* see if plausible */

   /* offset of last comment byte (from start of header) */
   comment_end = mus_header_comment_end();
   // Not a valid assumption, since Peak writes comments after sound data. -JGG
   //assert(comment_end >= 0 && comment_end < 3000);

   /* total bytes available for comment in header */
   len = comment_end - comment_start;
   assert(len >= 0);

   return (len > 0 ? len + 1 : len);
}


/* ----------------------------- sndlib_current_header_comment_alloc_good --- */
/* We write headers with more than enough comment space to hold peak stats
   and some brief plain text comments. But some programs (like Snd) can
   save files in a way that removes unused comment space. So we have to
   make sure there's enough to store our comments.

   If <rawcomment> is NULL, tests whether file header comment space is
   exactly DEFAULT_COMMENT_LENGTH bytes long.

   If <rawcomment> is not NULL, tests whether header has enough space for
   the <rawcomment> string. Only functions in this file, which know about
   encoded peak stats, should pass a raw comment string to this function.

   Returns 1 if the test passes, 0 if it doesn't.

   Assumes the file we care about is in sndlib's header buffer.
*/
int
sndlib_current_header_comment_alloc_good(char *rawcomment)
{
   int  len;

   len = get_current_header_comment_alloc();

   /* Note: DEFAULT_COMMENT_LENGTH includes terminating NULL; strlen doesn't */
   if (rawcomment)
      return (len > strlen(rawcomment));
   else
      return (len == DEFAULT_COMMENT_LENGTH);
}


/* --------------------------------------- get_current_header_raw_comment --- */
/* Reads the raw header comment string from the file opened as <fd>.
   Passes back the malloc'd comment in <rawcomment>. Caller should free.
   The "raw" comment includes all of the comment text, including any
   peak stats encoded using the scheme described above (at "PEAK STATS").

   Returns the length (in bytes) of the comment space allocated in the
   header, or -1 if error. Note that the allocated space may be larger
   than the strlen length of the comment string. <rawcomment> is
   allocated len + 1 bytes;

   Assumes the header we want has already been read into the sndlib
   header buffer (sndlib_read_header).

   Leaves the file position pointer at the end of the comment.
*/
static int
get_current_header_raw_comment(int fd, char **rawcomment)
{
   int   start, end, len, bytes;

   assert(fd >= 0 && rawcomment != NULL);

   *rawcomment = NULL;

   start = mus_header_comment_start();
   // Not a valid assumption, since Peak writes comments after sound data. -JGG
   //assert(start >= 0 && start < 3000);

   end = mus_header_comment_end();
   assert(end >= start);

   len = end - start;

   if (len > 0)
      len++;
   else
      len = 0;

   if (len > 0) {
      *rawcomment = (char *)calloc(len + 1, sizeof(char)); /* incl term NULL */
      if (*rawcomment == NULL) {
         perror("get_current_header_raw_comment: calloc");
         return -1;
      }

      if (lseek(fd, start, SEEK_SET) == -1) {
         perror("get_current_header_raw_comment: lseek");
         return -1;
      }

      bytes = read(fd, *rawcomment, len);

      /* we should've read the amount sndlib said is there */
      if (bytes < len) {
         fprintf(stderr, "get_current_header_raw_comment: Read failed\n");
         return -1;
      }
   }
   /* else no comment text at all (not an error) */

   return len;
}


/* ------------------------------------------ sfcomment_peakstats_current --- */
/* Return 1 if the peak stats in <sfc> are not more than MAX_PEAK_STATS_AGE
   seconds older than the modification date of the open file represented
   by the file descriptor <fd>.  This constant is defined in H/sfheader.h.
   Otherwise, return 0.
*/
int
sfcomment_peakstats_current(const SFComment *sfc, const int fd)
{
   struct stat statbuf;

   if (fstat(fd, &statbuf) == -1) {
      perror("sfcomment_peakstats_current");
      return 0;
   }
   return (statbuf.st_mtime <= sfc->timetag + MAX_PEAK_STATS_AGE);
}


/* ------------------------------------------------------ sndlib_findpeak --- */
/* Finds the peak absolute value per channel, and the location (in frames)
   of this peak, in the soundfile window specified by <startframe> and
   <nframes>. Also finds the average (absolute value) amplitude, rms amplitude
   and average DC offset for this time span. If <outfd> is not -1, writes each
   input buffer to <outfd> with no change. (So the files must have same byte
   order.) Stores peak stats into <peak> and <peakloc> arrays, which are assumed
   to have at least <nchans> locations. Same goes for <ampavg>, <rms> and
   <dcavg> arrays. Leaves file position pointer of input file unchanged.
   Input files can be either short integer, floating point or 24-bit integer.

   Returns 0 if successful, -1 if error.

   The ability to copy input to output files is probably only useful for
   the sndpeak program.
*/

#define BUF_FRAMES  (1024 * 16)
#define ABS(x) ((x) < 0 ? -(x) : (x))
#define NORMFACTOR  1.0 / 32768.0
#define DENORMFACTOR 32768.0

int
sndlib_findpeak(int    infd,
                int    outfd,               /* ignored if -1 */
                int    indataloc,           /* header size (bytes) */
                int    outdataloc,          /* ignored if outfd is -1 */
                int    informat,            /* sndlib constant */
                int    nchans,              /* of input file */
                long   startframe,
                long   nframes,
                float  peak[],
                long   peakloc[],
                double ampavg[],
                double dcavg[],
                double rms[])
{
   int   i, n, bytespersamp, isfloat, byteswap;
   long  frames, bufframes, startbyte, bufbytes, bufcount = 0;
   off_t oldloc;
   char  *buffer;

   assert(infd >= 0 && indataloc >= 0);
   assert(!INVALID_DATA_FORMAT(informat));
   assert(nchans > 0 && nchans <= MAXCHANS);
   assert(startframe >= 0 && nframes > 0);
   assert(peak != NULL && peakloc != NULL);

   bytespersamp = mus_data_format_to_bytes_per_sample(informat);
   isfloat = IS_FLOAT_FORMAT(informat);

   assert(isfloat || bytespersamp == 2 || bytespersamp == 3);

#if MUS_LITTLE_ENDIAN
   byteswap = IS_BIG_ENDIAN_FORMAT(informat);
#else
   byteswap = IS_LITTLE_ENDIAN_FORMAT(informat);
#endif

   for (n = 0; n < nchans; n++) {
      peak[n] = 0.0;
      peakloc[n] = 0;
      ampavg[n] = 0.0;
      dcavg[n] = 0.0;
      rms[n] = 0.0;
   }

   bufbytes = BUF_FRAMES * bytespersamp * nchans;
   buffer = (char *)malloc(bufbytes);
   if (buffer == NULL) {
      perror("sndlib_findpeak: malloc");
      return -1;
   }

   if (outfd > -1) {
      assert(outdataloc >= 0);
      if (lseek(outfd, outdataloc, SEEK_SET) == -1) {
         perror("sndlib_findpeak: lseek");
         return -1;
      }
   }
   oldloc = lseek(infd, 0, SEEK_CUR);
   startbyte = (startframe * nchans * bytespersamp) + indataloc;
   if (lseek(infd, startbyte, SEEK_SET) == -1) {
      perror("sndlib_findpeak: lseek");
      return -1;
   }

   for (frames = 0; frames < nframes; frames += bufframes) {
      long inbytes = read(infd, buffer, bufbytes);
      if (inbytes == -1) {
         perror("sndlib_findpeak: read");
         goto err;
      }
      if (inbytes == 0)
         break;                           /* already reached EOF */

      bufframes = inbytes / bytespersamp / nchans;

      if (isfloat) {
         float fsamp, fabsamp, normsamp;
         float *fbuf = (float *)buffer;

         for (i = 0; i < bufframes; i++) {
            for (n = 0; n < nchans; n++) {
               fsamp = *fbuf++;
               if (byteswap)
                  byte_reverse4(&fsamp);
               fabsamp = (float)fabs((double)fsamp);
               if (fabsamp > peak[n]) {
                  peak[n] = fabsamp;
                  peakloc[n] = startframe + frames + i;
               }
               ampavg[n] += fabsamp * NORMFACTOR;
               normsamp = fsamp * NORMFACTOR;
               dcavg[n] += normsamp;
               rms[n] += normsamp * normsamp;
            }
         }
      }
      else if (bytespersamp == 3) {
         float fsamp, fabsamp, normsamp, scalefactor = 1.0 / (float) (1 << 8);
         unsigned char *bufp = (unsigned char *)buffer;
         if (informat == MUS_L24INT) {
            for (i = 0; i < bufframes; i++) {
               for (n = 0; n < nchans; n++) {
                  int samp = (int) (((bufp[2] << 24)
                                   + (bufp[1] << 16)
                                   + (bufp[0] << 8)) >> 8);
                  fsamp = (float) samp * scalefactor;
                  fabsamp = fabs(fsamp);
                  if (fabsamp > peak[n]) {
                     peak[n] = fabsamp;
                     peakloc[n] = startframe + frames + i;
                  }
                  ampavg[n] += fabsamp * NORMFACTOR;
                  normsamp = fsamp * NORMFACTOR;
                  dcavg[n] += normsamp;
                  rms[n] += normsamp * normsamp;
                  bufp += 3;
               }
            }
         }
         else {   // informat == MUS_B24INT
            for (i = 0; i < bufframes; i++) {
               for (n = 0; n < nchans; n++) {
                  int samp = (int) (((bufp[0] << 24)
                                   + (bufp[1] << 16)
                                   + (bufp[2] << 8)) >> 8);
                  fsamp = (float) samp * scalefactor;
                  fabsamp = fabs(fsamp);
                  if (fabsamp > peak[n]) {
                     peak[n] = fabsamp;
                     peakloc[n] = startframe + frames + i;
                  }
                  ampavg[n] += fabsamp * NORMFACTOR;
                  normsamp = fsamp * NORMFACTOR;
                  dcavg[n] += normsamp;
                  rms[n] += normsamp * normsamp;
                  bufp += 3;
               }
            }
         }
      }
      else if (bytespersamp == 2) {      /* short ints */
         short samp, absamp;
         float normsamp;
         short *sbuf = (short *)buffer;

         for (i = 0; i < bufframes; i++) {
            for (n = 0; n < nchans; n++) {
               samp = *sbuf++;
               if (byteswap)
                  byte_reverse2(&samp);
               absamp = ABS(samp);
               if (absamp > peak[n]) {
                  peak[n] = absamp;
                  peakloc[n] = startframe + frames + i;
               }
               ampavg[n] += absamp * NORMFACTOR;
               normsamp = samp * NORMFACTOR;
               dcavg[n] += normsamp;
               rms[n] += normsamp * normsamp;
            }
         }
      }
      if (outfd > -1) {   /* copy to output file (with same byte order) */
         long outbytes = write(outfd, buffer, inbytes);
         if (outbytes != inbytes) {
            perror("sndlib_findpeak: write");
            goto err;
         }
      }
   }

   /* denormalize averages so that they correspond to input range;
      we do the normalization to avoid overflow during accumulation.
   */
   for (n = 0; n < nchans; n++) {
      ampavg[n] = (ampavg[n] / nframes) * DENORMFACTOR;
      dcavg[n] = (dcavg[n] / nframes) * DENORMFACTOR;
      rms[n] = pow(rms[n] / nframes, 0.5) * DENORMFACTOR;
   }

   free(buffer);
   lseek(infd, oldloc, SEEK_SET);
   return 0;
err:
   free(buffer);
   lseek(infd, oldloc, SEEK_SET);
   return -1;
}



/* ========================================================================== */
/* compatibility functions for disk-based cmix to read and write headers */

#ifdef CMIX_LEGACY

static int peak_stats_to_sfmaxamp(SFMAXAMP *, SFComment *, int);
static int sfmaxamp_to_peak_stats(SFComment *, SFMAXAMP *, int);
static int sf_class_to_data_format(int, int);


/* ------------------------------------------------------- sndlib_rheader --- */
/* Invoked by the rheader macro in H/sfheader.h. This is meant to be called
   only by legacy disk-based cmix software, like sound.c and the utility
   programs. New programs and the RTcmix core should use sndlib_read_header
   instead.

   Fills in the fields of <sfh> for the file already opened as <fd>. Checks
   that sound data format (e.g., 16-bit linear) is of a type that cmix can
   use, and that the file doesn't have too many channels. Parses peak stats 
   and text comment from the file header comment, and stores them into the
   sf_maxamp and sf_comment fields of <sfh>.

   Seeks to beginning of sound data.

   Returns 0 if OK, -1 if error.
*/
int
sndlib_rheader(int fd, SFHEADER *sfh)
{
   SFComment  sfc;

   assert(fd >= 0 && sfh != NULL);

   if (sndlib_read_header(fd) == -1) {
      fprintf(stderr, "sndlib_rheader: Error reading header\n");
      return -1;
   }

   /* Now we can grab data from sndlib's current header buffer. */

   sfheadertype(sfh) = mus_header_type();
   if (NOT_A_SOUND_FILE(sfheadertype(sfh))) {
      fprintf(stderr, "File is probably not a sound file.\n");
      return -1;
   }

   sfdataformat(sfh) = mus_header_format();
   if (!SUPPORTED_DATA_FORMAT_NO24BIT(sfdataformat(sfh))) {
      fprintf(stderr, "Sound file not in a supported data format. (Can be \n");
      fprintf(stderr, "16-bit linear or 32-bit float, either byte order.)\n");
      return -1;
   }

   sfclass(sfh) = mus_header_data_format_to_bytes_per_sample();

   sfdatalocation(sfh) = mus_header_data_location();

   sfchans(sfh) = mus_header_chans();
   if (sfchans(sfh) > SF_MAXCHAN) {
      fprintf(stderr, "Soundfile has %d channels (only %d supported)\n",
                       sfchans(sfh), SF_MAXCHAN);
      return -1;
   }

   sfsrate(sfh) = (float)mus_header_srate();

   /* mus_header_samples() gives number of samples. */
   sfdatasize(sfh) = mus_header_samples() * sfclass(sfh);

   /* Read peak stats, if any, from file, and copy them from the SFComment
      struct into our SFHEADER's sf_maxamp struct. We don't really care
      if there's an error while reading these ... the sf_maxamp struct
      will be zero'd out in that case.
    */
   sndlib_get_current_header_comment(fd, &sfc);
   peak_stats_to_sfmaxamp(&sfmaxampstruct(sfh), &sfc, sfchans(sfh));

   /* Copy comment string, if any, into SFHEADER. */
   strncpy(sfcommentstr(sfh), sfc.comment, MAXCOMM);
   sfcommentstr(sfh)[MAXCOMM - 1] = '\0';             /* ensure termination */

   /* Seek to beginning of snd data. */
   if (lseek(fd, sfdatalocation(sfh), SEEK_SET) == -1) {
      perror("sndlib_rheader: lseek");
      return -1;
   }

   return 0;
}


/* ------------------------------------------------------- sndlib_wheader --- */
/* Invoked by the wheader macro in H/sfheader.h. This is meant to be called
   only by legacy disk-based cmix software, like sound.c and the utility
   programs. New programs and the RTcmix core should use sndlib_write_header
   instead.

   Writes header according to the fields of <sfh> for the file already opened
   as <fd>. Updates file peak stats, using the ones in <sfh>. Ignores the peak
   stat timestamp (timetag) it finds there, because sndlib_put_header_comment
   updates this.

   It's possible to change the following header fields with this call:
      header type (must be a WRITEABLE_HEADER_TYPE),
      class or packmode (internally converted to sndlib data format),
      sampling rate,
      number of channels.
   Be careful changing the header type! This will almost certainly cause
   some dropped or added samples at beginning of file, and may even cause
   channel swapping or bad sample word alignment.

   Before closing an output file, you should update the header's idea of
   how much sound data there is by calling putlength.

   Normally the header has already been created (by sfcreate), and this
   function is called to update it for new peak stats and so forth. But
   some utility programs (rescale, sndreverse) use this function to write
   a header on a newly created file. If the file opened as <fd> has zero
   length, we write a new header on it (with zero'd peak stats and comment).
   Otherwise, we assume the file has a valid header that just needs updating.

   In all cases the data format stored in the SFHEADER struct is ignored;
   it's filled in here by converting from the sf_packmode (sfclass).

   On success, leaves file position pointer at start of sound data, and
   returns 0; Otherwise, returns -1.
*/
int
sndlib_wheader(int fd, SFHEADER *sfh)
{
   int    result, header_type, data_format;
   off_t  file_length;
   char   *rawcomment = NULL;

   assert(fd >= 0 && sfh != NULL);

   header_type = sfheadertype(sfh);

   if (!WRITEABLE_HEADER_TYPE(header_type)) {
      fprintf(stderr, "sndlib_wheader: Can't write this type of header\n");
      return -1;
   }

   data_format = sf_class_to_data_format(sfclass(sfh), header_type);
   sfdataformat(sfh) = data_format;

   /* Use AIFF, not AIFC, unless it's a float file. */
   if (header_type == MUS_AIFF && data_format == MUS_BFLOAT)
      header_type = MUS_AIFC;

   file_length = lseek(fd, 0, SEEK_END);
   if (file_length == -1) {
      perror("sndlib_wheader: lseek");
      return -1;
   }

   if (file_length > 0) {
      SFComment sfc;

      /* Copy peak stats and comment into SFComment struct. */
      sfmaxamp_to_peak_stats(&sfc, &(sfmaxampstruct(sfh)), sfchans(sfh));

      strncpy(sfc.comment, sfcommentstr(sfh), MAX_COMMENT_CHARS);
      sfc.comment[MAX_COMMENT_CHARS - 1] = '\0';      /* ensure termination */

      result = format_raw_comment(&sfc, sfchans(sfh), &rawcomment);
      if (result == -1)
         rawcomment = NULL;          /* just don't write comment */
   }

   result = sndlib_write_header(fd, (file_length > 0), header_type,
                                data_format, sfsrate(sfh), sfchans(sfh),
                                rawcomment, &(sfdatalocation(sfh)));
   if (rawcomment)
      free(rawcomment);

   return result;
}


/* -------------------------------------------------------- getheadersize --- */
/* Replacement for old getheadersize in wheader.c.
*/
int
getheadersize(SFHEADER *sfh)
{
   assert(sfh != NULL);
   return sfdatalocation(sfh);
}


/* ------------------------------------------------------------ putlength --- */
/* Replacement for old putlength in wheader.c.
   Using header info in <sfh>, updates data_size field of <sfh> and writes
   this to file header. Ignores <sfname>, which is there only for 
   compatibility with the old function.

   Should call this just before closing the file.

   Returns 0 if OK, -1 if error.
*/
int
putlength(char *sfname, int fd, SFHEADER *sfh)
{
   int   result;
   long  header_size;
   off_t file_length;

   assert(fd >= 0 && sfh != NULL);

   if (!WRITEABLE_HEADER_TYPE(sfheadertype(sfh))) {
      fprintf(stderr, "putlength: Can't write this type of header.\n");
      return -1;
   }

   header_size = sfdatalocation(sfh);

   assert(header_size >= 24);                  /* feeble sanity check */

   file_length = lseek(fd, 0, SEEK_END);
   if (file_length == -1) {
      perror("putlength: lseek");
      return -1;
   }

   if (file_length >= header_size) {
      sfdatasize(sfh) = file_length - header_size;

      result = sndlib_set_header_data_size(fd, sfheadertype(sfh),
                                                            sfdatasize(sfh));
      if (result == -1) {
         fprintf(stderr, "putlength: Can't update header data size\n");
         return -1;
      }
   }
   else {
      fprintf(stderr, "putlength: Invalid data_location\n");
      return -1;
   }

   return 0;
}


/* Below are versions of getsfcode and putsfcode that work with sndlib.
   Header codes other than SF_MAXAMP and SF_COMMENT will trigger an error
   message. The goal here is to keep the old API where possible, so that
   most callers won't have to be changed.

   NOTES:

     (1) The SFHEADER passed to these functions is the header in memory,
         already read from disk by the rheader macro (in H/sfheader.h).
         These functions don't do any file i/o.

     (2) No byte-swapping of maxamp data is ever necessary with sndlib.
         So any byte-swapping done by code that calls these functions 
         should be commented out for the sndlib version.

     (3) The way getsfcode works is pretty funky. If you aren't using it
         in exactly the way cited below, you should use the new function
         getsfmaxamp instead.
*/

/* ------------------------------------------------------------ getsfcode --- */
/* This is kind of nasty -- it returns a pointer with the expectation that
   the caller will increment it by sizeof(SFCODE), in order to copy the
   SFMAXAMP struct or comment string from the header into local storage.
   This is to handle the normal use of this function in cmix:

        char *cp = getsfcode(&sfdesc[fno], SF_MAXAMP);
        bcopy(cp + sizeof(SFCODE), (char *)&sfm[fno], sizeof(SFMAXAMP));

   If you aren't following this usage exactly, you should use the new
   getsfmaxamp function below, or for a comment, just copy up to MAXCOMM
   chars from sfcommentstr(sfh) into local storage.

   Returns NULL if requested code is not of type SF_MAXAMP or SF_COMMENT.
*/
char *
getsfcode(SFHEADER *sfh, int code)
{
   assert(sfh != NULL);

   switch (code) {
      case SF_MAXAMP:
         return ((char *)(&(sfmaxampstruct(sfh))) - sizeof(SFCODE));
         break;
      case SF_COMMENT:
         return (sfcommentstr(sfh) - sizeof(SFCODE));
         break;
      default:
         fprintf(stderr, "getsfcode: Requested code type invalid\n");
   }
   return NULL;
}


/* ------------------------------------------------------------ putsfcode --- */
/* If <codeptr> represents an old IRCAM header code of type SF_MAXAMP,
   putsfcode blindly copies sizeof(SFMAXAMP) bytes from <ptr> into the 
   <sf_maxamp> field of the given SFHEADER. If <codeptr> represents a
   header code of type SF_COMMENT, copies up to MAXCOMM chars from <ptr>
   into the <sf_comment> field of SFHEADER.

   (Note that we ignore the <bsize> field of the SFCODE struct.)

   Returns 0 if the requested code is of type SF_MAXAMP or SF_COMMENT;
   otherwise, returns -1.
*/
int
putsfcode(SFHEADER *sfh, char *ptr, SFCODE *codeptr)
{
   assert(sfh != NULL && ptr != NULL && codeptr != NULL);

   switch (codeptr->code) {
      case SF_MAXAMP:
         memcpy(&(sfmaxampstruct(sfh)), ptr, sizeof(SFMAXAMP));
         break;
      case SF_COMMENT:
         strncpy(sfcommentstr(sfh), ptr, MAXCOMM - 1);
         sfcommentstr(sfh)[MAXCOMM - 1] = '\0';
         break;
      default:
         fprintf(stderr, "putsfcode: Requested code type invalid\n");
         return -1;
   }

   return 0;
}


/* ---------------------------------------------------------- getsfmaxamp --- */
/* The old getsfcode has an undesirable API for sndlib purposes. So here's
   a new function that retrieves an SFMAXAMP struct from an SFHEADER in
   memory. Copies the sf_maxamp struct of <sfh> into <sfm> (storage
   allocated by caller).
   Returns -1 if either argument is NULL; otherwise returns 0.
 */
int
getsfmaxamp(SFHEADER *sfh, SFMAXAMP *sfm)
{
   assert(sfh != NULL && sfm != NULL);

   memcpy(sfm, &(sfmaxampstruct(sfh)), sizeof(SFMAXAMP));

   return 0;
}


/* ----------------------------------------------- peak_stats_to_sfmaxamp --- */
static int
peak_stats_to_sfmaxamp(SFMAXAMP *sfm, SFComment *sfc, int nchans)
{
   int   n;

   assert(sfm != NULL && sfc != NULL);
   /* awkward: two constants for max, in sfheader.h and rtstuff/rtdefs.h */
   assert(nchans > 0 && nchans <= SF_MAXCHAN);

   memset(sfm, 0, sizeof(SFMAXAMP));        /* zero out the SFMAXAMP struct */

   if (SFCOMMENT_PEAKSTATS_VALID(sfc)) {    /* sfc has valid peak stats */
      for (n = 0; n < nchans; n++) {
         sfm->value[n] = sfc->peak[n];
         sfm->samploc[n] = sfc->peakloc[n];
      }
      sfm->timetag = sfc->timetag;
   }

   return 0;
}


/* ----------------------------------------------- sfmaxamp_to_peak_stats --- */
/* NOTE: This zeros out the SFComment struct, so don't store comment string
   there before calling this function!
*/
static int
sfmaxamp_to_peak_stats(SFComment *sfc, SFMAXAMP *sfm, int nchans)
{
   int   n;

   assert(sfc != NULL && sfm != NULL);
   /* awkward: two constants for max, in sfheader.h and rtstuff/rtdefs.h */
   assert(nchans > 0 && nchans <= MAXCHANS);

   memset(sfc, 0, sizeof(SFComment));       /* zero out the SFComment struct */

   if (sfm->timetag != 0) {                 /* sfm has valid peak stats */
      for (n = 0; n < nchans; n++) {
         sfc->peak[n] = sfm->value[n];
         sfc->peakloc[n] = sfm->samploc[n];
      }
      sfc->timetag = sfm->timetag;
   }

   return 0;
}


/* ---------------------------------------------- sf_class_to_data_format --- */
/* Return the sndlib data format for the given class and header type.
   Assumes that 2-byte words are 16-bit linear and 4-byte words are
   32-bit float (not 32-bit linear). Also assumes that the data format
   is big-endian, unless the header type is RIFF (aka WAV). These
   are safe assumptions for disk-based cmix.
*/
static int
sf_class_to_data_format(int class, int header_type)
{
   int   data_format;

   assert(class == SF_SHORT || class == SF_FLOAT);

   if (class == SF_SHORT)
      data_format = MUS_BSHORT;
   else           /* class == SF_FLOAT */
      data_format = MUS_BFLOAT;

   if (header_type == MUS_RIFF)
      data_format = (data_format == MUS_BSHORT) ? MUS_LSHORT : MUS_LFLOAT;
   return data_format;
}


#endif /* CMIX_LEGACY */

