// AudioFileDevice.cpp
//
// Allows applications to write audio to disk just as if they were writing
// to an audio device.  Performs sample fmt conversion between float and 
// other formats.  Constructor specifies audio file type, the file's sample 
// format, and options concerning format of data.

#include <math.h>       /* for fabs */
#include "AudioFileDevice.h"
#include <byte_routines.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#if defined(linux) || defined(MACOSX)
#include <unistd.h>
#endif

#define DEBUG 0

#if DEBUG > 1
#define PRINT0 if (1) printf
#define PRINT1 if (1) printf
#elif DEBUG > 0
#define PRINT0 if (1) printf
#define PRINT1 if (0) printf
#else
#define PRINT0 if (0) printf
#define PRINT1 if (0) printf
#endif

struct AudioFileDevice::Impl {
	char						*path;
	int							fileType;		// wave, aiff, etc.
};

AudioFileDevice::AudioFileDevice(const char *path,
								 int fileType)
	: _impl(new Impl)
{
	PRINT1("AudioFileDevice::AudioFileDevice\n");
	_impl->path = (char *)path;
	_impl->fileType = fileType;
}

AudioFileDevice::~AudioFileDevice()
{
	PRINT1("AudioFileDevice::~AudioFileDevice\n");
	close();
	delete _impl;
}

// We override open() in order to call setDeviceParams() BEFORE doOpen().

int AudioFileDevice::open(int mode, int fileSampFmt, int fileChans, double srate)
{
	PRINT1("AudioFileDevice::open\n");

	if ((mode & Record) != 0)
		return error("Record from file device not supported");
	
	int status = 0;
	if (!isOpen()) {
		// Audio file formats are always interleaved.
		setDeviceParams((fileSampFmt & ~MUS_INTERLEAVE_MASK) | MUS_INTERLEAVED,
						fileChans,
						srate);
		if ((status = doOpen(mode)) == 0) {
			setState(Open);
			if ((status = doSetFormat(fileSampFmt, fileChans, srate)) != 0)
				close();
			else
				setState(Configured);
		}
		else {
			setState(Closed);
		}
	}
	return status;
}

int AudioFileDevice::doOpen(int mode)
{
	assert(!(mode & Record));
	setMode(mode);
	
	int fd = sndlib_create((char *)_impl->path, _impl->fileType,
						   getDeviceFormat(), (int) getSamplingRate(), 
						   getDeviceChannels());
	// Catch and store any error
	if (fd < 0) {
		error(::strerror(errno));
	}
	setDevice(fd);
	closing(false);
	resetFrameCount();
	return (fd > 0) ? 0 : -1;
}

int AudioFileDevice::doClose()
{
	PRINT1("AudioFileDevice::doClose\n");
	int status = 0;
	if (!closing()) {
		closing(true);
		if (checkPeaks()) {
			// Normalize peaks if file was normalized.
			if (isDeviceFmtNormalized())
				for (int chan = 0; chan < getDeviceChannels(); ++chan)
					_peaks[chan] /= 32768.0;
			(void) sndlib_put_header_comment(device(),
											 _peaks,
											 _peakLocs,
											 NULL);
		}
		// Last arg is samples not frames, for some archaic reason.
		status = sndlib_close(device(),
							  true,
							  _impl->fileType,
							  getDeviceFormat(),
							  getFrameCount() * getDeviceChannels());
		setDevice(-1);
	}
	return status;
}

int AudioFileDevice::doStart()
{
//	printf("AudioFileDevice::doStart: starting thread\n");
	return ThreadedAudioDevice::startThread();
}

int AudioFileDevice::doPause(bool paused)
{
	this->paused(paused);
	return 0;
}

int AudioFileDevice::doSetFormat(int sampfmt, int chans, double srate)
{
	return 0;
}

int AudioFileDevice::doSetQueueSize(int *pWriteSize, int *pCount)
{
	return 0;
}

int AudioFileDevice::doGetFrameCount() const
{
	return frameCount();
}

int AudioFileDevice::doGetFrames(void *frameBuffer, int frameCount)
{
	return error("Reading from file device not yet supported");
}

int	AudioFileDevice::doSendFrames(void *frameBuffer, int frames)
{
	long bytesToWrite = frames * getDeviceBytesPerFrame();
	long bytesWritten = ::write(device(), frameBuffer, bytesToWrite);
	if (bytesWritten < 0) {
		return error("Error writing to file.");
	}
	else if (bytesWritten < bytesToWrite) {
		return error("Incomplete write to file (disk full?)");
	}
	incrementFrameCount(frames);
	return frames;
}

void AudioFileDevice::run()
{
	PRINT1("AudioFileDevice::run entered\n");
	assert(!isPassive());	// Cannot call this method when passive!
	
	while (!stopping()) {
		while (paused()) {
#ifdef linux
			::usleep(1000);
#endif
		}
		if (runCallback() != true) {
//			printf("AudioFileDevice::run: callback returned false\n");
			break;
		}
	}
	// If we stopped due to callback being done, set the state so that the
	// call to close() does not attempt to call stop, which we cannot do in
	// this thread.  Then, check to see if we are being closed by the main
	// thread before calling close() here, to avoid a reentrant call.
	if (!stopping()) {
		setState(Configured);
		if (!closing()) {
			PRINT1("AudioFileDevice::run: calling close()\n");
			close();
		}
	}
//		printf("AudioFileDevice::run: calling stop callback\n");
	stopCallback();
	PRINT1("AudioFileDevice::run: thread exiting\n");
}

