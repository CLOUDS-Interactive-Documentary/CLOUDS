#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <ugens.h>
#include "prototypes.h"
#include <sndlibsupport.h>
#include "byte_routines.h"

#define BUFSIZE   1024 * 64


/* ------------------------------------------------- dump_gen_to_raw_file --- */
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
static void
dump_gen_to_raw_file(double *buf, int nsamps)
{
   int         result, nbytes;
   static int  fd = -1;

   if (fd == -1) {
      fd = open("dumpaudio.raw", O_RDWR | O_CREAT | O_TRUNC, 0666);
      assert(fd > 0);
      fprintf(stderr, "Dumping audio (as double samps) to \"dumpaudio.raw\".\n");
   }
   nbytes = nsamps * sizeof(double);
   result = write(fd, buf, nbytes);
   assert(result != -1);
}


/* ----------------------------------------------------------------- gen1 --- */
/* Let user load a function table with samples from a sound file.  The file
   can be 16-bit integer or 32-bit floating point in either byte order, and
   in any of the header formats recognized by sndlib.  The makegen syntax
   used in Minc is:

      frames_read = makegen(slot, 1, size, filename, inskip [, inchan, dump])

   <size> is the duration (in seconds) of the sound file segment, or if it's
   negative, the number of sample frames to read.  If <size> is zero, the
   whole file is slurpped in.  (Beware with large files -- there is no
   check on memory consumption here!)

   <filename> is a double-quoted pathname (as in rtinput).

   <inskip> is the time (in seconds) to skip before reading, or if it's 
   negative, the number of sample frames to skip before reading.

   If <inchan> is missing, reads all channels from the file; otherwise,
   reads just the channel specified by <inchan> (with zero as first chan).

   If <dump> is 1, then dumps the gen table to a header-less sound file,
   called "dumpaudio.raw," in the current directory.  The file is 32-bit
   float, using the host byte order, <inchan> chans, and the sampling
   rate in the source file's header.

   As usual, if the slot number is positive, the table will be rescaled
   to fit in the range [-1,1]; if it's negative, it will not be rescaled.

   Returns to Minc the number of sample frames read.

   JGG, 07 Feb 2001
*/
double
gen1(struct gen *gen, char *sfname)
{
   int      i, fd, header_type, data_format, data_location, inchan;
   int      gen_chans, gen_frames, gen_samps, file_chans, file_frames;
   int      start_frame, bytes_per_samp, byteswap, is_float, dump;
   int      buf_start_frame, end_frame, frames_read, buf_frames;
   long     file_samps;
   off_t    seek_to;
   float    request_dur, filedur, inskip;
   double   srate, *block, *blockp;
   char     *buf;

   request_dur = gen->pvals[0];
   inskip = gen->pvals[2];
   if (gen->nargs > 3)
      inchan = (int) gen->pvals[3];
   else
      inchan = -1;   /* read all chans */
   dump = gen->nargs > 4? (int) gen->pvals[4] : 0;

   fd = open_sound_file("gen1", sfname, &header_type, &data_format,
                        &data_location, &srate, &file_chans, &file_samps);
   if (fd == -1)
      return die("gen1", "Can't open input file: \"%s\"!", sfname);

   if (srate != SR()) {
      rtcmix_warn("gen1", "The input file sampling rate is %g, but "
                      "the output rate is currently %g.", srate, SR());
   }

   file_frames = file_samps / file_chans;

   if (inchan == -1)
      gen_chans = file_chans;
   else
      gen_chans = 1;

   if (file_chans == 1)
      inchan = -1;                       /* more efficient copy below */

   if (inchan >= file_chans)
      return die("gen1", "You asked for channel %d of a %d-channel file. (\"%s\")",
                                                   inchan, file_chans, sfname);

   if (request_dur < 0.0)
      gen_frames = (int) -request_dur;
   else if (request_dur > 0.0)
      gen_frames = (int) (request_dur * srate);
   else
      gen_frames = file_frames;

   if (inskip < 0.0)
      start_frame = (int) -inskip;
   else
      start_frame = (int) (inskip * srate); 

   if (start_frame + gen_frames > file_frames)
      gen_frames = file_frames - start_frame;

   gen_samps = gen_frames * gen_chans;
 
   block = (double *) malloc((size_t) (gen_samps * sizeof(double)));
   if (block == NULL)
      return die("gen1", "Not enough memory for function table %d.", gen->slot);

   buf = (char *) malloc((size_t) BUFSIZE);
   if (buf == NULL)
      return die("gen1", "Not enough memory for temporary buffer.");

   bytes_per_samp = mus_data_format_to_bytes_per_sample(data_format);

   seek_to = data_location + (start_frame * file_chans * bytes_per_samp);
   if (lseek(fd, seek_to, SEEK_SET) == -1) {
#ifdef MAXMSP
		return die("gen1", "lseek problem");
#else
      perror("gen1 (lseek)");
      exit(1);
#endif
   }

#if MUS_LITTLE_ENDIAN
   byteswap = IS_BIG_ENDIAN_FORMAT(data_format);
#else
   byteswap = IS_LITTLE_ENDIAN_FORMAT(data_format);
#endif
   is_float = IS_FLOAT_FORMAT(data_format);

   buf_frames = (BUFSIZE / bytes_per_samp) / file_chans;

   end_frame = start_frame + gen_frames;

   blockp = block;
   frames_read = 0;
   buf_start_frame = start_frame;
   for ( ; buf_start_frame < end_frame; buf_start_frame += frames_read) {
      int  samps_read;
      long bytes_read;

      if (buf_start_frame + buf_frames > end_frame) {      /* last buffer */
         int samps = (end_frame - buf_start_frame) * file_chans;
         bytes_read = read(fd, buf, samps * bytes_per_samp);
      }
      else
         bytes_read = read(fd, buf, BUFSIZE);
      if (bytes_read == -1) {
#ifdef MAXMSP
			return die("gen1", "read problem");
#else
         perror("read");
         exit(1);
#endif
      }
      if (bytes_read == 0)          /* EOF, somehow */
         break;

      samps_read = bytes_read / bytes_per_samp;
      frames_read = samps_read / file_chans;

      if (is_float) {
         float *bufp = (float *) buf;

         if (inchan == -1) {                 /* store all chans */
            if (byteswap) {
               for (i = 0; i < samps_read; i++) {
                  byte_reverse4(bufp);       /* modify *bufp in place */
                  *blockp++ = (double) *bufp++;
               }
            }
            else {
               for (i = 0; i < samps_read; i++)
                  *blockp++ = (double) *bufp++;
            }
         }
         else {                              /* store only inchan */
            bufp += inchan;
            if (byteswap) {
               for (i = 0; i < samps_read; i += file_chans) {
                  byte_reverse4(bufp);       /* modify *bufp in place */
                  *blockp++ = (double) *bufp;
                  bufp += file_chans;
               }
            }
            else {
               for (i = 0; i < samps_read; i += file_chans) {
                  *blockp++ = (double) *bufp;
                  bufp += file_chans;
               }
            }
         }
      }
      else {                                 /* is shortint file */
         short *bufp = (short *) buf;

         if (inchan == -1) {                 /* store all chans */
            if (byteswap) {
               for (i = 0; i < samps_read; i++, bufp++) {
                  short samp = reverse_int2(bufp);
                  *blockp++ = (double) samp;
               }
            }
            else {
               for (i = 0; i < samps_read; i++)
                  *blockp++ = (double) *bufp++;
            }
         }
         else {                              /* store only inchan */
            bufp += inchan;
            if (byteswap) {
               for (i = 0; i < samps_read; i += file_chans) {
                  short samp = reverse_int2(bufp);
                  *blockp++ = (double) samp;
                  bufp += file_chans;
               }
            }
            else {
               for (i = 0; i < samps_read; i += file_chans) {
                  *blockp++ = (double) *bufp;
                  bufp += file_chans;
               }
            }
         }
      }
   }

   free(buf);
   sndlib_close(fd, 0, 0, 0, 0);

   gen->array = block;
   gen->size = gen_samps;

   fnscl(gen);

   if (dump)
      dump_gen_to_raw_file(block, gen_samps);

   return (double) gen_frames;
}


