// OSSAudioDevice.cpp

#if defined(LINUX)

#include <math.h>
#include "OSSAudioDevice.h"
#include "AudioIODevice.h"
#include <sys/soundcard.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>	// strerror()

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

// BGG Michael Gogins fix for ALSA
#include "../config.h"
#ifdef HAVE_LIBASOUND
#warning "Checking for ALSA version compatibility..."
#include <alsa/version.h>
// To fix: SOUND_PCM_WRITE_CHANNELS_is_obsolete_use_SNDCTL_DSP_CHANNELS_instead
#if (SND_LIB_VERSION >= 0x10018)
#undef SOUND_PCM_WRITE_CHANNELS
#define SOUND_PCM_WRITE_CHANNELS SNDCTL_DSP_CHANNELS
#endif
#endif

OSSAudioDevice::OSSAudioDevice(const char *devPath)
	: _inputDeviceName(NULL), _outputDeviceName(NULL), _bytesPerFrame(0)
{
	_inputDeviceName = new char [strlen(devPath) + 1];
	_outputDeviceName = new char [strlen(devPath) + 1];
	strcpy(_inputDeviceName, devPath);
	strcpy(_outputDeviceName, devPath);
}

OSSAudioDevice::~OSSAudioDevice()
{
	close();
	delete [] _inputDeviceName;
	delete [] _outputDeviceName;
}

int OSSAudioDevice::doClose()
{
	int status = 0;
	if (!closing()) {
		closing(true);
		resetFrameCount();
		_bufferSize = 0;	// lets us know it must be recalculated.
		PRINT0("\tOSSAudioDevice::doClose\n");
		status = closeDevice();
	}
	return status;
}

int OSSAudioDevice::doStart()
{
	// Get this now if user never set it.
	if (_bufferSize == 0) {
		if (ioctl(SNDCTL_DSP_GETBLKSIZE, &_bufferSize) == -1) {
			return error("Error while retrieving block size.");
		}
	}
	return ThreadedAudioDevice::startThread();
}

int OSSAudioDevice::doPause(bool isPaused)
{
	paused(isPaused);
	return 0;
}

int
OSSAudioDevice::setDeviceFormat(int dev, int sampleFormat,
								int *chans, int srate)
{
	int confirmedFormat = sampleFormat;
	if (::ioctl(dev, SNDCTL_DSP_SETFMT, &confirmedFormat))
		return error("OSS error while setting sample format: ", strerror(errno));
	else if (confirmedFormat != sampleFormat)
		return error("This sample format not supported by device.");

	int reqChans = *chans;
#ifndef SOUND_PCM_WRITE_CHANNELS	// OLD VERSION
	if (reqChans != 1 && reqChans != 2) {
		return error("This device supports only mono and stereo");
	}
#else
	if (::ioctl(dev, SOUND_PCM_WRITE_CHANNELS, &reqChans) == 0) {
		if (reqChans < *chans) {
			PRINT0("OSSAudioDevice::setDeviceFormat:  Max device channel count is %d\n", reqChans);
			return error("OSS device does not support this channel count");
		}
		else if (reqChans > *chans) {
			PRINT0("OSSAudioDevice::setDeviceFormat:  Device forced channel count to %d\n", reqChans);
			*chans = reqChans;
		}
	}
	else if (*chans < 8) {
		PRINT0("OSSAudioDevice::setDeviceFormat: ioctl for SOUND_PCM_WRITE_CHANNELS returned %s -- trying 8 channels\n",
		 	   strerror(errno));
		// Try opening device in 8-channel mode
		reqChans = 8;
		if (::ioctl(dev, SOUND_PCM_WRITE_CHANNELS, &reqChans) == 0) {
			PRINT0("OSSAudioDevice::setDeviceFormat:  Device forced channel count to %d\n", reqChans);
			*chans = reqChans;
		}
	}
	else if (*chans > 2) {
		PRINT0("OSSAudioDevice::setDeviceFormat: ioctl for SOUND_PCM_WRITE_CHANNELS returned %s\n",
		 	   strerror(errno));
		return error("This device does not support setting chans > 2");
	}
	else
#endif
	{
		int dsp_stereo = (reqChans == 2);
		if (::ioctl(dev, SNDCTL_DSP_STEREO, &dsp_stereo)
			|| dsp_stereo != (reqChans == 2))
		{
			return error("OSS error while setting channel count: ",
						 strerror(errno));
		}
	}
	int dsp_speed = (int) srate;
	if (::ioctl(dev, SNDCTL_DSP_SPEED, &dsp_speed))
		return error("OSS error while setting sample rate: ", strerror(errno));
	if (dsp_speed != (int) srate)
		return error("Device does not support this sample rate");
#ifdef SOUND_PCM_WRITE_CHANNELS
	PRINT0("OSSAudioDevice::setDeviceFormat: srate = %d, channels = %d\n", dsp_speed, reqChans);
#endif
	return 0;
}

int
OSSAudioDevice::ioctl(int req, void *argp) {
	return ::ioctl(device(), req, argp);
}

static char zeroBuffer[32768];

void OSSAudioDevice::run()
{
	audio_buf_info info;
	const bool playing = isPlaying();
	int ret;
	PRINT1("OSSAudioDevice::run: top of loop\n");
	while ((ret = waitForDevice(0)) == 0) {
		if (ioctl(playing ? SNDCTL_DSP_GETOSPACE : SNDCTL_DSP_GETISPACE,
				  &info))
		{
			error("OSS error: ", strerror(errno));
			break;
		}
		PRINT1("\tOSSAudioDevice::run: %d out of %d frags (%d bytes) can be %s\n",
			   info.fragments, info.fragstotal, info.bytes, playing ? "written" : "read");
		else if (info.bytes < 0) {
			error("OSS error: ", "available audio queue space < 0");
			break;
		}
		if (info.bytes < bufferSize() / 2) {
			PRINT1("\tOSSAudioDevice::run: only %d bytes avail...waiting\n", info.bytes);
			usleep(10);
			continue;
		}
		if (runCallback() != true) {
			break;
		}
		// Spin if device is paused
		if (paused()) {
			ioctl(SNDCTL_DSP_POST, 0);
			while (paused()) {
				usleep(1000);
			}
		}
	}
	if (ret >= 0 && playing) {
		// Write buffer of zeros.
		doSendFrames(zeroBuffer, sizeof(zeroBuffer)/getDeviceBytesPerFrame());
	
		PRINT1("OSSAudioDevice::run: flushing...\n");
		// Flush device.
		ioctl(SNDCTL_DSP_SYNC, 0);
	}

	// If we stopped due to callback being done, set the state so that the
	// call to close() does not attempt to call stop, which we cannot do in
	// this thread.  Then, check to see if we are being closed by the main
	// thread before calling close() here, to avoid a reentrant call.
	
	if (!stopping()) {
		setState(Configured);
		if (!closing()) {
			PRINT1("OSSAudioDevice::run: calling close()\n");
			close();
		}
	}
	PRINT1("OSSAudioDevice::run: stop callback\n");
	stopCallback();
	PRINT1("OSSAudioDevice::run: thread exiting\n");
}

#endif	// LINUX

