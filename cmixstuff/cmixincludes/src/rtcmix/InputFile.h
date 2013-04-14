//
//  InputFile.h
//
//  Created by Douglas Scott on 9/24/11.
//

#ifndef INPUTDESC_H
#define INPUTDESC_H

#include "Lockable.h"
#include "rt_types.h"
#include <sys/types.h>
#include <string.h>

typedef int (*ReadFun)(int,int,int,off_t,long,BufPtr,int,int,const short[],short,void*);

/* definition of input file struct used by rtinput */
struct InputFile : public Lockable {
public:
#ifdef MULTI_THREAD
	static void createConversionBuffers(int inBufSamps);
	static void destroyConversionBuffers();
#endif
	InputFile();
    void init(int inFd, const char *inFileName, bool isAudioDev, int inHeaderType,
              int inDataFormat, int inDataLocation, long nFrames, float inSampleRate,
              int inChannels, double inDuration);
	
    void reference();
    void unreference();
	
	int	getFD() const { return fd; }
	const char *fileName() const { return filename; }
	bool hasFile(const char *inPath) const { return strcmp(inPath, filename) == 0; }
	float sampleRate() const { return srate; }
	short channels() const { return chans; }
	bool isAudioDevice() const { return is_audio_dev; }
	short dataFormat() const { return data_format; }
	int dataLocation() const { return data_location; }
	double duration() const { return dur; }
	
    off_t readSamps(off_t     cur_offset,       /* current file position before read */
                  BufPtr      dest,             /* interleaved buffer from inst */
                  int         dest_chans,       /* number of chans interleaved */
                  int         dest_frames,      /* frames in interleaved buffer */
                  const short src_chan_list[],  /* list of in-bus chan numbers from inst */
                  /* (or NULL to fill all chans) */
                  short       src_chans         /* number of in-bus chans to copy */
    );
	bool isOpen() const { return fd > 0; }
    
protected:
	void close();

private:
	char     *filename;         /* allocated by rtinput() */
	int      fd;                /* file descriptor, or NO_FD, or AUDIO_DEVICE */
	short    is_audio_dev;      /* true if input from audio dev, not from file */
	short    header_type;       /* e.g., AIFF_sound_file (in sndlib.h) */
	short    data_format;       /* e.g., snd_16_linear (in sndlib.h) */
	short    is_float_format;   /* true if data format is 32-bit float */
	int      data_location;     /* offset of sound data start in file */
	long     endbyte;           /* index of byte following last sample */
	float    srate;
	short    chans;
	double   dur;
    void*      readBuffer;
	int      refcount;
    ReadFun  readFunction;
#ifdef MULTI_THREAD
	static char *		sConversionBuffers[];
#endif
};

#endif
