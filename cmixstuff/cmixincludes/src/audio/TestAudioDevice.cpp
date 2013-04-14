// TestAudioDevice.cpp 
//

#ifdef TEST_AUDIO_DEVICE

#include "TestAudioDevice.h"
#include "SinglePortOSSAudioDevice.h"
#include "sndlibsupport.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

TestAudioDevice::TestAudioDevice(const char *desc, int mode)
	: _descriptor(new char[strlen(desc) + 1]), _buffer(NULL), _device(NULL)
{
	strcpy(_descriptor, desc);
	_format = 0;
	_channels = 0;
	// desc should be of form test:/[uif]/[1-32]/[1-8]/[in]/Nn/
	char fc, ic, nc;
	int width = 0;
	if (sscanf(_descriptor, "test:/%c/%d/%d/%c/%c",
			   &fc, &width, &_channels, &ic, &nc) == 5)
	{
		switch (fc) {
			case 'u':
				if (width == 16)
					_format |= MUS_ULSHORT;
				else
					goto baddesc;
				break;
			case 'i':
				if (width == 16)
					_format |= MUS_LSHORT;
				else if (width == 24)
					_format |= MUS_L24INT;
				else if (width == 32)
					_format |= MUS_LINT;
				else
					goto baddesc;
				break;
			case 'f':
				_format = MUS_LFLOAT;
				if (nc == 'N')
					_format |= MUS_NORMALIZED;
				break;
			default:
				goto baddesc;
		}
		if (ic == 'i')
			_format |= MUS_INTERLEAVED;
		else if (ic == 'n')
			_format |= MUS_NON_INTERLEAVED;
		else
			goto baddesc;
		return;
	}
baddesc:
	fprintf(stderr, "Invalid test device descriptor: %s\n", desc);
	exit(1);
}

TestAudioDevice::~TestAudioDevice()
{
	close();
	delete (AudioDevice *) _device;
	delete [] _buffer;
	delete [] _descriptor;
}

int
TestAudioDevice::doOpen(int mode)
{
	_device = new SinglePortOSSAudioDevice("/dev/dsp");

	switch (mode & DirectionMask) {
	case Playback:
		break;
	case Record:
		break;
	default:
		return error("TestAudioDevice: Illegal open mode.");
	}
	return 0;
}

int
TestAudioDevice::doClose()
{
	return _device->close();
}

int
TestAudioDevice::start(AudioDevice::Callback *callback)
{
	return _device->start(callback);
}

int
TestAudioDevice::doStart()
{
	return 0;
}

int
TestAudioDevice::setStopCallback(Callback *stopCallback)
{
	return _device->setStopCallback(stopCallback);
}

int
TestAudioDevice::doStop()
{
	return _device->stop();
}

int
TestAudioDevice::doGetFrameCount() const
{
	return _device->getFrameCount();
}

// This does nothing under RTcmix, so can be left as-is.

int
TestAudioDevice::doPause(bool p)
{
	return _device->pause(p);
}

int
TestAudioDevice::doSetFormat(int sampfmt, int chans, double srate)
{
#if NO_LONGER_NEED_THIS_CHECK
	if (chans > _channels) {
		char errmsg[64];
		sprintf(errmsg, "Minc channel count (%d) must be <= test device count (%d)",
				chans, _channels);
		return error(errmsg);
	}
#endif
	int status = _device->open(getMode(),
						   NATIVE_SHORT_FMT | MUS_INTERLEAVED,
						   2,
						   srate);
	status = _device->setFrameFormat(NATIVE_SHORT_FMT | MUS_INTERLEAVED, 2);
	// Underlying device had its format set during open.
	setDeviceParams(_format, _channels, srate);
	printf("Test Audio Device: Internal format: 0x%x, chans: %d\n",
		   _format, _channels);
	return status;
}

int
TestAudioDevice::doSetQueueSize(int *pWriteSize, int *pCount)
{
	int status = _device->setQueueSize(pWriteSize, pCount);
	_buffer = new short[*pWriteSize * *pCount * 2];
	return status;
}

int
TestAudioDevice::doGetFrames(void *frameBuffer, int frameCount)
{
	return error("Not implemented");
}

int
TestAudioDevice::doSendFrames(void *frameBuffer, int frameCount)
{
	const int ichans = _channels;
	int n;
	memset(_buffer, 0, frameCount * 2 * sizeof(short));
	for (int ochan = 0; ochan < 2; ++ochan) {
		short *bufptr, *bufchan = &_buffer[ochan];
		
		if (IS_FLOAT_FORMAT(_format)) {
			float *fin;
			int incr;
			const float factor = IS_NORMALIZED_FORMAT(_format) ? 32767.0f/ichans : 1.0f/ichans;
			for (int ichan = ochan; ichan < ichans; ichan += 2) {
				if (IS_INTERLEAVED_FORMAT(_format)) {
					fin = (float *) frameBuffer + ichan;
					incr = ichans;
				}
				else {
					fin = ((float **) frameBuffer)[ichan];
					incr = 1;
				}
				for (n = 0, bufptr = bufchan;
				     n < frameCount; ++n, bufptr += 2, fin += incr) {
					*bufptr += (short) (*fin * factor);
				}
			}
		}

		else if (IS_32BIT_FORMAT(_format)) {
			int *iin;
			int incr;
			const float factor = 1.0 / (ichans * (1 << 16));
			for (int ichan = ochan; ichan < ichans; ichan += 2) {
				if (IS_INTERLEAVED_FORMAT(_format)) {
					iin = (int *) frameBuffer + ichan;
					incr = ichans;
				}
				else {
					iin = ((int **) frameBuffer)[ichan];
					incr = 1;
				}
				for (n = 0, bufptr = bufchan;
					 n < frameCount; ++n, bufptr += 2, iin += incr) {
					*bufptr += (short) (*iin * factor);
				}
			}
		}

		else if (IS_SHORT_FORMAT(_format)) {
			short *sin;
			int incr;
			const float factor = 1.0/ichans;
			for (int ichan = ochan; ichan < ichans; ichan += 2) {
				if (IS_INTERLEAVED_FORMAT(_format)) {
					sin = (short *) frameBuffer + ichan;
					incr = ichans;
				}
				else {
					sin = ((short **) frameBuffer)[ichan];
					incr = 1;
				}
				for (n = 0, bufptr = bufchan;
					 n < frameCount; ++n, bufptr += 2, sin += incr) {
					*bufptr += (short)(*sin * factor);
				}
			}
		}

	}
	return _device->sendFrames(_buffer, frameCount);
}

bool TestAudioDevice::recognize(const char *desc)
{
	return desc && strncmp(desc, "test:", 5) == 0;
}

AudioDevice *TestAudioDevice::create(const char *inputDesc, const char *outputDesc, int mode)
{
	if (mode & RecordPlayback == RecordPlayback)
		return NULL;
	return new TestAudioDevice(inputDesc ? inputDesc : outputDesc, mode);
}

#endif	// TEST_AUDIO_DEVICE

