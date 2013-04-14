//
//  InputFile.cpp
//
//  Created by Douglas Scott on 9/24/11.
//

#include "InputFile.h"
#include "rtdefs.h"
#include "RTcmix.h"
#include <sndlib.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sndlibsupport.h>
#include <ugens.h>
#include "byte_routines.h"
#ifdef MULTI_THREAD
#include "RTThread.h"
#endif

/* The define below is to disable some fancy bus-mapping code for file
 input that was not well thought out. As a user, when I open a file,
 I typically just want the instrument to read all its channels without
 me worrying how many there are. If the instrument can read only one,
 I'll tell it which one in its inchan pfield. But what numbers do I use
 in my call to bus_config for the "in" bus? As a user, I don't care.
 So we're ignoring them here, and making sure that inst->inputchans
 is set to match the number of chans in the file (in rtsetinput).
 
 What we lose by ignoring the bus numbers is consistency of the
 bus_config interface between file and rt input. We also lose the
 ability to select arbitrary channels from a file to send into an
 instrument. (But what instruments could make use of this?) My
 feeling is that this selection feature would be more trouble for
 the user than it's worth. What do you think?
 
 If we expand the rtinput syntax to allow opening more than one
 file for an instrument to use, then this could all change. But
 it's not easy to see how, exactly. Note that the commented out
 code below interprets the bus numbers as a selection of channels
 from the input file. However, if you could say:
 
 rtinput("foo.snd", "in 0-1"); rtinput("bar.snd", "in 2-3")
 
 Then you'd expect this to work the way an aux-in bus does now: the
 first file is read into buses 0-1, the second into buses 2-3. But
 these "buses" aren't really buses the way aux buses are. Instead,
 they're just private buffers in this file (or in an instrument,
 depending on how you look at it). This makes sense, because sound
 files aren't real-time input, and instruments often don't read from
 the same section of a file during the same timeslice -- so they'd
 hardly ever be able to share a file input bus. So with the rtinput
 syntax above, we'd just pretend that those are real buses. What
 kind of bus_config would work with those two rtinputs, and what
 would its in-bus numbers mean?
 
 JGG, 27-May-00
 */
#define IGNORE_BUS_COUNT_FOR_FILE_INPUT

static inline long lmin(long a, long b) { return a < b ? a : b; }

/* ----------------------------------------------------- read_float_samps --- */
static int
read_float_samps(
      int         fd,               /* file descriptor for open input file */
      int         data_format,      /* sndlib data format of input file */
      int         file_chans,       /* total chans in input file */
      off_t       cur_offset,       /* current file position before read */
      long        endbyte,          /* first byte following last file sample */
      BufPtr      dest,             /* interleaved buffer from inst */
      int         dest_chans,       /* number of chans interleaved */
      int         dest_frames,      /* frames in interleaved buffer */
      const short src_chan_list[],  /* list of in-bus chan numbers from inst */
                 /* (or NULL to fill all chans) */
      short       src_chans,         /* number of in-bus chans to copy */
      void *      read_buffer        /* block to read from disk into */
      )
{    
    const int bytes_per_samp = sizeof(float);
    char *bufp = (char *) read_buffer;
    
    const int bytes_requested = dest_frames * file_chans * bytes_per_samp;
    const long bytes_remaining = endbyte - cur_offset;
    const long extra_bytes = (bytes_requested > bytes_remaining)
    ? bytes_requested - bytes_remaining : 0;
    ssize_t bytes_to_read = lmin(bytes_remaining, bytes_requested);
    
    while (bytes_to_read > 0) {
        ssize_t bytes_read = read(fd, bufp, bytes_to_read);
        if (bytes_read == -1) {
#ifndef MAXMSP
            perror("read_float_samps (read)");
            exit(1);
#endif
        }
        if (bytes_read == 0)          /* EOF */
            break;
        
        bufp += bytes_read;
        bytes_to_read -= bytes_read;
    }
    
    /* If we reached EOF, zero out remaining part of buffer that we
     expected to fill.
     */
    bytes_to_read += extra_bytes;
    while (bytes_to_read > 0) {
        (* (float *) bufp) = 0.0;
        bufp += bytes_per_samp;
        bytes_to_read -= bytes_per_samp;
    }
    
    /* Copy interleaved file buffer to dest buffer, with bus mapping. */
    
    const int src_samps = dest_frames * file_chans;
    float *fbuf = (float *) read_buffer;
    
    for (int n = 0; n < dest_chans; n++) {
#ifdef IGNORE_BUS_COUNT_FOR_FILE_INPUT
        const int chan = n;
#else
        const int chan = src_chan_list ? src_chan_list[n] : n;
#endif
        int j = n;
        for (int i = chan; i < src_samps; i += file_chans, j += dest_chans)
            dest[j] = (BUFTYPE) fbuf[i];
    }
    
#if MUS_LITTLE_ENDIAN
    const bool swap = IS_BIG_ENDIAN_FORMAT(data_format);
#else
    const bool swap = IS_LITTLE_ENDIAN_FORMAT(data_format);
#endif
    if (swap) {
        for (int i = 0; i < src_samps; i++)
            byte_reverse4(&dest[i]);
    }
    
    return 0;
}


/* ----------------------------------------------------- read_24bit_samps --- */
static int
read_24bit_samps(
      int         fd,               /* file descriptor for open input file */
      int         data_format,      /* sndlib data format of input file */
      int         file_chans,       /* total chans in input file */
      off_t       cur_offset,       /* current file position before read */
      long        endbyte,          /* first byte following last file sample */
      BufPtr      dest,             /* interleaved buffer from inst */
      int         dest_chans,       /* number of chans interleaved */
      int         dest_frames,      /* frames in interleaved buffer */
      const short src_chan_list[],  /* list of in-bus chan numbers from inst */
                 /* (or NULL to fill all chans) */
      short       src_chans,         /* number of in-bus chans to copy */
      void *      read_buffer        /* block to read from disk into */
      )
{
    
    const int bytes_per_samp = 3;         /* 24-bit int */
    unsigned char *bufp = (unsigned char *) read_buffer;
    
    const int bytes_requested = dest_frames * file_chans * bytes_per_samp;
    const long bytes_remaining = endbyte - cur_offset;
    const long extra_bytes = (bytes_requested > bytes_remaining)
    ? bytes_requested - bytes_remaining : 0;
    ssize_t bytes_to_read = lmin(bytes_remaining, bytes_requested);
    
    while (bytes_to_read > 0) {
        ssize_t bytes_read = read(fd, bufp, bytes_to_read);
        if (bytes_read == -1) {
#ifndef MAXMSP
            perror("read_24bit_samps (read)");
            exit(1);
#endif
        }
        if (bytes_read == 0)          /* EOF */
            break;
        
        bufp += bytes_read;
        bytes_to_read -= bytes_read;
    }
    
    /* If we reached EOF, zero out remaining part of buffer that we
     expected to fill.
     */
    bytes_to_read += extra_bytes;
    while (bytes_to_read > 0) {
        *bufp++ = 0;
        bytes_to_read--;
    }
    
    /* Copy interleaved file buffer to dest buffer, with bus mapping. */
    
    const int src_samps = dest_frames * file_chans;
    const BUFTYPE scaleFactor = 1 / (BUFTYPE) (1 << 8);
    unsigned char *cbuf = (unsigned char *) read_buffer;
    
    for (int n = 0; n < dest_chans; n++) {
#ifdef IGNORE_BUS_COUNT_FOR_FILE_INPUT
        const int chan = n;
#else
        const int chan = src_chan_list ? src_chan_list[n] : n;
#endif
        int i = chan * bytes_per_samp;
        const int incr = file_chans * bytes_per_samp;
        const int src_bytes = src_samps * bytes_per_samp;
        if (data_format == MUS_L24INT) {
            for (int j = n; i < src_bytes; i += incr, j += dest_chans) {
                int samp = (int) (((cbuf[i + 2] << 24)
                                   + (cbuf[i + 1] << 16)
                                   + (cbuf[i] << 8)) >> 8);
                dest[j] = (BUFTYPE) samp * scaleFactor;
            }
        }
        else {   /* data_format == MUS_B24INT */
            for (int j = n; i < src_bytes; i += incr, j += dest_chans) {
                int samp = (int) (((cbuf[i] << 24)
                                   + (cbuf[i + 1] << 16)
                                   + (cbuf[i + 2] << 8)) >> 8);
                dest[j] = (BUFTYPE) samp * scaleFactor;
            }
        }
    }
    
    return 0;
}


/* ----------------------------------------------------- read_short_samps --- */
static int
read_short_samps(
      int         fd,               /* file descriptor for open input file */
      int         data_format,      /* sndlib data format of input file */
      int         file_chans,       /* total chans in input file */
      off_t       cur_offset,       /* current file position before read */
      long        endbyte,          /* first byte following last file sample */
      BufPtr      dest,             /* interleaved buffer from inst */
      int         dest_chans,       /* number of chans interleaved */
      int         dest_frames,      /* frames in interleaved buffer */
      const short src_chan_list[],  /* list of in-bus chan numbers from inst */
                 /* (or NULL to fill all chans) */
      short       src_chans,         /* number of in-bus chans to copy */
      void *      read_buffer        /* block to read from disk into */
      )
{
    const int bytes_per_samp = 2;         /* short int */
    char *bufp = (char *) read_buffer;
    
    const int bytes_requested = dest_frames * file_chans * bytes_per_samp;
    const long bytes_remaining = endbyte - cur_offset;
    const long extra_bytes = (bytes_requested > bytes_remaining)
    ? bytes_requested - bytes_remaining : 0;
    ssize_t bytes_to_read = lmin(bytes_remaining, bytes_requested);
    
    while (bytes_to_read > 0) {
        ssize_t bytes_read = read(fd, bufp, bytes_to_read);
        if (bytes_read == -1) {
#ifndef MAXMSP
            perror("read_short_samps (read)");
            exit(1);
#endif
        }
        if (bytes_read == 0)          /* EOF */
            break;
        
        bufp += bytes_read;
        bytes_to_read -= bytes_read;
    }
    
    /* If we reached EOF, zero out remaining part of buffer that we
     expected to fill.
     */
    bytes_to_read += extra_bytes;
    while (bytes_to_read > 0) {
        (* (short *) bufp) = 0;
        bufp += bytes_per_samp;
        bytes_to_read -= bytes_per_samp;
    }
    
    /* Copy interleaved file buffer to dest buffer, with bus mapping. */
    
#if MUS_LITTLE_ENDIAN
    const bool swap = IS_BIG_ENDIAN_FORMAT(data_format);
#else
    const bool swap = IS_LITTLE_ENDIAN_FORMAT(data_format);
#endif
    
    const int src_samps = dest_frames * file_chans;
    short *sbuf = (short *)read_buffer;
    
    for (int n = 0; n < dest_chans; n++) {
#ifdef IGNORE_BUS_COUNT_FOR_FILE_INPUT
        const int chan = n;
#else
        const int chan = src_chan_list ? src_chan_list[n] : n;
#endif
        if (swap) {
            int j = n;
            for (int i = chan; i < src_samps; i += file_chans, j += dest_chans) {
                sbuf[i] = reverse_int2(&sbuf[i]);
                dest[j] = (BUFTYPE) sbuf[i];
            }
        }
        else {
            int j = n;
            for (int i = chan; i < src_samps; i += file_chans, j += dest_chans)
                dest[j] = (BUFTYPE) sbuf[i];
        }
    }
    
    return 0;
}

#ifdef MULTI_THREAD

char *		InputFile::sConversionBuffers[RT_THREAD_COUNT];

void InputFile::createConversionBuffers(int inBufSamps)
{
	/* Allocate buffers needed to convert input audio files as they are read */
	for (int i = 0; i < RT_THREAD_COUNT; ++i) {
		sConversionBuffers[i] = (char *) malloc(sizeof(BUFTYPE) * MAXCHANS * inBufSamps);
	}
}

void InputFile::destroyConversionBuffers()
{
	for (int i = 0; i < RT_THREAD_COUNT; ++i) {
		free(sConversionBuffers[i]);
		sConversionBuffers[i] = NULL;
	}
}

#endif

InputFile::InputFile() : filename(NULL), fd(NO_FD), readBuffer(NULL)
{
}

void InputFile::init(int inFd, const char *inFileName, bool isAudioDev, int inHeaderType,
          int inDataFormat, int inDataLocation, long inFrames, float inSampleRate,
          int inChannels, double inDuration)
{
    filename = strdup(inFileName);
    fd = inFd;
    refcount = 0;
    is_audio_dev = isAudioDev;
    header_type = inHeaderType;
    data_format = inDataFormat;
    is_float_format = IS_FLOAT_FORMAT(data_format);
    data_location = inDataLocation;
    int bytes_per_samp = ::mus_data_format_to_bytes_per_sample(data_format);
    endbyte = data_location + (inFrames * bytes_per_samp);
    srate = inSampleRate;
    chans = inChannels;
    dur = inDuration;

    assert(chans <= MAXCHANS);
    
    if (is_float_format)
        readFunction = &read_float_samps;
    else if (IS_24BIT_FORMAT(data_format))
        readFunction = &read_24bit_samps;
    else
        readFunction = &read_short_samps;
#ifndef MULTI_THREAD
    readBuffer = (void *) malloc((size_t) RTcmix::bufsamps() * MAXCHANS * bytes_per_samp);
    if (readBuffer == NULL) {
#ifndef MAXMSP
        perror("rtinput (malloc)");
        exit(1);
#endif
    }
#endif
}

void InputFile::reference()
{
	if (++refcount == 1) {
		// In here we can do any post-initialization that only needs to be done (once) when we are
		// sure that this InputFile is being used by an instrument.
	}
}

void InputFile::unreference()
{
#ifdef DEBUG
	printf("InputFile::unreference: refcount = %d\n", refcount);
#endif
	if (--refcount <= 0) {
		close();
	}
}

void InputFile::close()
{
	if (fd > 0) {
#ifdef DEBUG
		printf("\tInputFile: closing fd %d\n", fd);
#endif
		mus_file_close(fd);
		fd = NO_FD;
	}
#ifndef MULTI_THREAD
	if (readBuffer)
		free(readBuffer);
	readBuffer = NULL;
#endif
	if (filename)
		free(filename);
	filename = NULL;
	header_type = MUS_UNSUPPORTED;
	data_format = MUS_UNSUPPORTED;
	is_float_format = 0;
	data_location = 0;
	endbyte = 0;
	srate = 0.0;
	chans = 0;
	dur = 0.0;
}

off_t InputFile::readSamps(off_t cur_offset,
                         BufPtr dest,
                         int dest_chans,
                         int dest_frames,
                         const short src_chan_list[],
                         short src_chans)
{
#ifndef IGNORE_BUS_COUNT_FOR_FILE_INPUT
    assert(dest_chans >= src_chans);
#endif
    assert(chans >= dest_chans);
#ifdef MULTI_THREAD
	readBuffer = sConversionBuffers[RTThread::GetIndexForThread()];
#endif
	{
		AutoLock fileLock(this);
		if (lseek(fd, cur_offset, SEEK_SET) == -1) {
#ifndef MAXMSP
			perror("RTcmix::readFromInputFile (lseek)");
			exit(1);
#endif
		}
		
		int status = (*this->readFunction)(fd,
									 data_format,
									 chans,
									 cur_offset,
									 endbyte,
									 dest, dest_chans, dest_frames,
									 src_chan_list, src_chans,
									 readBuffer);
	}
    int bytes_per_samp = ::mus_data_format_to_bytes_per_sample(data_format);
    return dest_frames * chans * bytes_per_samp;
}
