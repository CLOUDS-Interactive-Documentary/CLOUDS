// ALSAAudioDevice.cpp

#ifdef ALSA

#include "ALSAAudioDevice.h"
#include "AudioIODevice.h"
#include <sys/time.h>
#include <sys/resource.h>	// setpriority()
#include <unistd.h>
#include <stdio.h>

#include <sndlibsupport.h>	// RTcmix header

// TO DO:  Look up default device during open()

#define DEFAULT_DEVICE "hw:0,0"

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

inline int getstatus(int x) { return (x == 0) ? 0 : -1; }

ALSAAudioDevice::ALSAAudioDevice(const char *devName)
	: _deviceName(devName), _handle(NULL), _hwParams(NULL), _bufSize(0),
	  _periodSize(0), _stopDuringPause(false)
{
}

ALSAAudioDevice::~ALSAAudioDevice()
{
	PRINT0("ALSAAudioDevice::~ALSAAudioDevice\n");
	close();
	snd_pcm_hw_params_free (_hwParams);
}

int ALSAAudioDevice::doOpen(int mode)
{
	int status;
	if ((mode & RecordPlayback) == RecordPlayback)
		return error("ALSA device does not yet support full duplex.");
	else if (mode & Record) {
		if ((status = snd_pcm_open(&_handle, 
								   _deviceName, 
								   SND_PCM_STREAM_CAPTURE, 0)) < 0)
		{
			return error("Cannot open audio input device: ", snd_strerror(status));
		}
	}
	else if (mode & Playback) {
		if ((status = snd_pcm_open(&_handle, 
								   _deviceName, 
								   SND_PCM_STREAM_PLAYBACK, 0)) < 0)
		{
			return error("Cannot open audio output device: ", snd_strerror(status));
		}
	}
	// Allocate hw params struct.
	if ((status = snd_pcm_hw_params_malloc(&_hwParams)) < 0) {
		error("Cannot allocate hardware parameter structure: ", 
			  snd_strerror (status));
	}
	if ((status = snd_pcm_hw_params_any(_handle, _hwParams)) < 0) {
		return error("Cannot initialize hardware parameter structure: ",
					 snd_strerror (status));
	}
	// HACK:  Get descriptor from pollfd's to use for select
	int count = snd_pcm_poll_descriptors_count(_handle);
	struct pollfd *pfds = new pollfd[count];
	snd_pcm_poll_descriptors(_handle, pfds, count);
	setDevice(pfds[0].fd);
	delete [] pfds;		
	return 0;
}

int ALSAAudioDevice::doClose()
{
	resetFrameCount();
	PRINT0("ALSAAudioDevice::doClose\n");
	setDevice(0);
	return snd_pcm_close(_handle);
}

int ALSAAudioDevice::doStart()
{
	int status;
	if ((status = snd_pcm_prepare(_handle)) < 0) {
		return error("Cannot prepare audio interface for use: ", 
					 snd_strerror (status));
	}
	return ThreadedAudioDevice::startThread();
}

int ALSAAudioDevice::doPause(bool isPaused)
{
	int status = 0;
//	printf("ALSAAudioDevice::doPause(%d): stopping=%d paused=%d\n", paused, stopping(), _paused);
	status = snd_pcm_pause(_handle, isPaused);
	if (status == 0) {
		paused(isPaused);
	}
	return getstatus(status);
}

int ALSAAudioDevice::doStop()
{
	if (!stopping()) {
		PRINT0("ALSAAudioDevice::doStop: waiting for thread to finish...\n");
		stopping(true);		// signals play thread
		if (paused()) {
			_stopDuringPause = true;	// we handle this case as special
			waitForThread();
			snd_pcm_drop(_handle);
		}
		else if (isRecording() || isPlaying()) {
			waitForThread();
		}
		PRINT0("ALSAAudioDevice::doStop: done\n");
	}
	return 0;
}

static snd_pcm_format_t convertSampleFormat(int sampfmt)
{
	snd_pcm_format_t sampleFormat = SND_PCM_FORMAT_UNKNOWN;
	switch (sampfmt) {
		case MUS_UBYTE:
			sampleFormat = SND_PCM_FORMAT_U8;
			break;
		case MUS_BYTE:
			sampleFormat = SND_PCM_FORMAT_S8;
			break;
		case MUS_LSHORT:
//			sampleFormat = SND_PCM_FORMAT_S16_LE;
//			break;
		case MUS_BSHORT:
			sampleFormat = SND_PCM_FORMAT_S16;
			break;
		case MUS_L24INT:
//			sampleFormat = SND_PCM_FORMAT_S24_LE;
//			break;
		case MUS_B24INT:
			sampleFormat = SND_PCM_FORMAT_S24;
			break;
		case MUS_LINT:
//			sampleFormat = SND_PCM_FORMAT_S32_LE;
//			break;
		case MUS_BINT:
			sampleFormat = SND_PCM_FORMAT_S32;
			break;
		case MUS_LFLOAT:
//			sampleFormat = SND_PCM_FORMAT_FLOAT_LE;
//			break;
		case MUS_BFLOAT:
			sampleFormat = SND_PCM_FORMAT_FLOAT;
			break;
		default:
			break;
	};
	return sampleFormat;
}

int ALSAAudioDevice::doSetFormat(int sampfmt, int chans, double srate)
{
	int status;
	PRINT0("ALSAAudioDevice::doSetFormat: fmt: 0x%x chans: %d rate: %g\n",
		   sampfmt, chans, srate);
	int deviceFormat = MUS_GET_FORMAT(sampfmt);
	snd_pcm_format_t sampleFormat = ::convertSampleFormat(deviceFormat);
	if (sampleFormat == SND_PCM_FORMAT_UNKNOWN)
		return error("Unknown or unsupported sample format");
	// This is what we report back.  It gets modified during loop.
	// Note that if we supported short int frame input, 'deviceFormat'
	// would be initialized to NATIVE_SHORT_FMT.
	deviceFormat = NATIVE_FLOAT_FMT | MUS_NORMALIZED;
	// Find a suitable format, ratcheting down from float to short.
	while ((status = snd_pcm_hw_params_set_format(_handle,
												  _hwParams, 
												  sampleFormat)) < 0)
	{
		if (sampleFormat == SND_PCM_FORMAT_FLOAT) {
			sampleFormat = SND_PCM_FORMAT_S32;
			deviceFormat = NATIVE_32BIT_FMT;	// This is what we report back.
		}
		else if (sampleFormat == SND_PCM_FORMAT_S32) {
			sampleFormat = SND_PCM_FORMAT_S24;
			deviceFormat = NATIVE_24BIT_FMT;	// This is what we report back.
		}
		else if (sampleFormat == SND_PCM_FORMAT_S24) {
			sampleFormat = SND_PCM_FORMAT_S16;
			deviceFormat = NATIVE_SHORT_FMT;	// This is what we report back.
		}
		else
			break;	// give up here.
	}
	if (status < 0)
		return error("Cannot set sample format: ", snd_strerror(status));

	// set the subformat
	if ((status = snd_pcm_hw_params_set_subformat(_handle, _hwParams,
										  		  SND_PCM_SUBFORMAT_STD)) < 0) {
		return error("Cannot set sample subformat: ", snd_strerror(status));
	}
	unsigned reqRate = (unsigned) (srate + 0.5);
	if ((status = snd_pcm_hw_params_set_rate_near(_handle, 
												  _hwParams, 
												  &reqRate, 0)) < 0) {
		return error("cannot set sample rate: ", snd_strerror(status));
	}
	
	if (reqRate != (unsigned) (srate + 0.5)) {
		char msg[64];
		sprintf(msg, "Cannot set rate to %g (got %u)", srate, reqRate);
		return error(msg);
	}
	
	if ((status = snd_pcm_hw_params_set_channels(_handle,
												 _hwParams, 
												 chans)) < 0) {
		return error("Cannot set channel count: ", snd_strerror(status));
	}
	
	// Try setting interleave to match what we will be handed.
	snd_pcm_access_t hwAccess = isFrameInterleaved() ? 
			SND_PCM_ACCESS_RW_INTERLEAVED : SND_PCM_ACCESS_RW_NONINTERLEAVED;

	while ((status = snd_pcm_hw_params_set_access(_handle,
												  _hwParams,
												  hwAccess)) < 0)
	{
		// Couldn't do it.  Flip interleave.
		if (hwAccess == SND_PCM_ACCESS_RW_INTERLEAVED)
			hwAccess = SND_PCM_ACCESS_RW_NONINTERLEAVED;
		else if (hwAccess == SND_PCM_ACCESS_RW_NONINTERLEAVED)
			hwAccess = SND_PCM_ACCESS_RW_INTERLEAVED;
		else
			break;	// give up.
	}
	if (status < 0)
		return error("Cannot set access type: ", snd_strerror(status));

	// Store the device params to allow format conversion.
	if (hwAccess == SND_PCM_ACCESS_RW_INTERLEAVED) {
		PRINT1("\tHW uses interleaved channels\n");
		deviceFormat |= MUS_INTERLEAVED;
	}
	else {
		PRINT1("\tHW uses non-interleaved channels\n");
		deviceFormat |= MUS_NON_INTERLEAVED;
	}

	setDeviceParams(deviceFormat, chans, srate);

	return 0;
}

int ALSAAudioDevice::doSetQueueSize(int *pWriteSize, int *pCount)
{
	_bufSize = (snd_pcm_uframes_t) *pWriteSize * *pCount;
	int status;
 	if ((status = snd_pcm_hw_params_set_buffer_size_near(_handle,
														 _hwParams,
														 &_bufSize)) < 0)
	{
 		return error("Cannot set buffer size: ", snd_strerror(status));
 	}
	PRINT1("ALSAAudioDevice::doSetQueueSize: requested %d frames total, got %d\n",
			*pWriteSize * *pCount, (int) _bufSize);
	
	_periodSize = _bufSize / *pCount;

	int dir = 0;
	snd_pcm_uframes_t tryperiodsize = _periodSize;

	tryperiodsize = _periodSize;
	if ((status = snd_pcm_hw_params_set_period_size_near(_handle,
														 _hwParams,
														 &tryperiodsize,
														 &dir)) < 0)
	{
		return error("Failed to set ALSA period size: ", snd_strerror(status));
	}
	else {
		PRINT1("ALSAAudioDevice::doSetQueueSize: requested period size near %d, got %d\n",
			   (int) _periodSize, (int)tryperiodsize);
	}
	unsigned periods = *pCount;

	PRINT1("setting periods near %d\n", periods);
	if ((status = snd_pcm_hw_params_set_periods_near(_handle,
												_hwParams, 
												&periods, 
												&dir)) < 0)
	{
		return error("Failed to set ALSA periods: ", snd_strerror(status));
	}

	PRINT1("periods set to %d\n", periods);
 	if ((status = snd_pcm_hw_params_get_buffer_size(_hwParams, &_bufSize)) < 0) {
		return error("Cannot retrieve buffer size: ", snd_strerror(status));
	}

	if ((status = snd_pcm_hw_params (_handle, _hwParams)) < 0) {
		return error("Cannot set parameters: ", snd_strerror(status));
	}

	if (!isPassive()) {
		// Set up device to wake us whenever *pWriteSize frames can be read/written

		snd_pcm_sw_params_t *swParams;
		if ((status = snd_pcm_sw_params_malloc(&swParams)) < 0) {
			return error("Cannot allocate sw params structure: ", 
						 snd_strerror (status));
		}
		if ((status = snd_pcm_sw_params_current(_handle, swParams)) < 0) {
			return error("Cannot initialize sw params: ", snd_strerror (status));
		}
		PRINT0("Testing: HW will wake us when %u frames can be %s\n",
			   (unsigned)tryperiodsize, isPlaying() ? "written" : "read");
		if ((status = snd_pcm_sw_params_set_avail_min(_handle, swParams, tryperiodsize)) < 0) {
			return error("Cannot set minimum available count: ", 
						 snd_strerror (status));
		}
		if ((status = snd_pcm_sw_params_set_start_threshold(_handle,
															swParams,
															1L)) < 0) {
			return error("Cannot set start mode: ", snd_strerror (status));
		}
		if ((status = snd_pcm_sw_params(_handle, swParams)) < 0) {
			return error("Cannot set software params: ", snd_strerror (status));
		}
		snd_pcm_sw_params_free(swParams);
	}
	PRINT1("period size set to %ld\n", tryperiodsize);
	*pWriteSize = tryperiodsize;
	return getstatus(status);
}

int	ALSAAudioDevice::doGetFrames(void *frameBuffer, int frameCount)
{
	int fread = -1;
	while (fread < 0) {
		if (isDeviceInterleaved())
			fread = snd_pcm_readi(_handle, frameBuffer, frameCount);
		else
			fread = snd_pcm_readn(_handle, (void **) frameBuffer, frameCount);
		PRINT1("ALSAAudioDevice::doGetFrames: %d frames to read, %d read\n", frameCount, fread);
		if (fread == -EPIPE) {
			PRINT1("ALSAAudioDevice::doGetFrames: overrun on read -- recovering and calling again\n");
			snd_pcm_prepare(_handle);
		}
		else if (fread < 0) {
			fprintf(stderr, "ALSAAudioDevice::doGetFrames: error reading from device: %s\n", snd_strerror(fread));
			return error("Error reading from device: ", snd_strerror(fread));
		}
	}
	
	if (fread > 0) {
		if (fread < frameCount)
			printf("ALSAAudioDevice::doGetFrames: read %d out of %d frames!\n",
				   fread, frameCount);
		incrementFrameCount(fread);
		return fread;
	}
	return -1;
}

int	ALSAAudioDevice::doSendFrames(void *frameBuffer, int frameCount)
{
	int fwritten = -1;
	while (fwritten < 0) {
		if (isDeviceInterleaved())
			fwritten = snd_pcm_writei(_handle, frameBuffer, frameCount);
		else
			fwritten = snd_pcm_writen(_handle, (void **) frameBuffer, frameCount);
		PRINT1("ALSAAudioDevice::doSendFrames: %d frames to write, %d written\n", frameCount, fwritten);
		if (fwritten == -EPIPE) {
			PRINT1("ALSAAudioDevice::doSendFrames: underrun on write -- recovering and calling again\n");
			snd_pcm_prepare(_handle);
		}
		else if (fwritten < 0) {
			fprintf(stderr, "ALSAAudioDevice::doSendFrames: error writing to device: %s\n", snd_strerror(fwritten));
			return error("Error writing to device: ", snd_strerror(fwritten));
		}
	}
	if (fwritten > 0) {
		if (fwritten < frameCount && !stopping())
			PRINT1("ALSAAudioDevice::doSendFrames: wrote %d out of %d frames!\n",
				   fwritten, frameCount);
		incrementFrameCount(fwritten);
		return fwritten;
	}
	return -1;
}

static char interleavedZeroBuffer[32768];
static char *nonInterleavedZeroBuffer[] = {
	interleavedZeroBuffer,
	interleavedZeroBuffer,
	interleavedZeroBuffer,
	interleavedZeroBuffer,
	interleavedZeroBuffer,
	interleavedZeroBuffer,
	interleavedZeroBuffer,
	interleavedZeroBuffer
};

void ALSAAudioDevice::run()
{
	PRINT0("ALSAAudioDevice::run: top of loop\n");
	int ret;
	while ((ret = waitForDevice(isPlaying() ? 0 : 1000)) == 0) {
		if (runCallback() != true) {
			break;
		}
	}
	PRINT0("ALSAAudioDevice::run: after loop\n");
	if (ret >= 0 && !_stopDuringPause && isOpen() && isPlaying()) {
		const int zFrames = (int) _periodSize;
		PRINT0("ALSAAudioDevice::run: sending %d zeros to HW...\n", zFrames);
		if (isDeviceInterleaved())
			doSendFrames(interleavedZeroBuffer, zFrames);
		else
			doSendFrames(nonInterleavedZeroBuffer, zFrames * getDeviceChannels());
		snd_pcm_drain(_handle);
		_stopDuringPause = false;	// reset
	}
	if (isOpen())
		setState(Configured);	// no longer running
	PRINT0("ALSAAudioDevice::run: calling stop callback\n");
	stopCallback();
	PRINT0("ALSAAudioDevice::run: thread exiting\n");
}

bool ALSAAudioDevice::recognize(const char *desc)
{
	return desc == NULL
		|| strncmp(desc, "hw:", 3) == 0
		|| strncmp(desc, "plug", 4) == 0;
}

AudioDevice *ALSAAudioDevice::create(const char *inputDesc,
									 const char *outputDesc,
								   	 int mode)
{
	AudioDevice *theDevice = NULL;

	// ALSA does not support full duplex as of 05/2004.
	
	if ((mode & AudioDevice::DirectionMask) != RecordPlayback)
		theDevice = new ALSAAudioDevice(inputDesc ? inputDesc
										: outputDesc ? outputDesc
										: DEFAULT_DEVICE);
	return theDevice;
}

#endif	// ALSA
