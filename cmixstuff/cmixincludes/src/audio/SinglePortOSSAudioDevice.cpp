// SinglePortOSSAudioDevice.cpp

#if defined(LINUX)

#include "SinglePortOSSAudioDevice.h"
#include <sys/soundcard.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>	// strerror()

#define DEFAULT_DEVICE "/dev/dsp"

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


SinglePortOSSAudioDevice::SinglePortOSSAudioDevice(const char *devPath)
	: OSSAudioDevice(devPath)
{
}

SinglePortOSSAudioDevice::~SinglePortOSSAudioDevice()
{
}

bool SinglePortOSSAudioDevice::recognize(const char *desc)
{
	return desc == NULL || strncmp(desc, "/dev/", 5) == 0;
}

AudioDevice *
SinglePortOSSAudioDevice::create(const char *inputDesc, const char *outputDesc, int mode)
{
	const char *desc = inputDesc ? inputDesc : outputDesc ? outputDesc : DEFAULT_DEVICE;
	return new SinglePortOSSAudioDevice(desc);
}

int
SinglePortOSSAudioDevice::doOpen(int mode)
{
	int fd = 0;
	switch (mode & DirectionMask) {
	case Playback:
		setDevice(fd = ::open(_outputDeviceName, O_WRONLY));
		closing(false);
		break;
	case Record:
		setDevice(fd = ::open(_inputDeviceName, O_RDONLY));
		closing(false);
		break;
	case RecordPlayback:
		setDevice(fd = ::open(_outputDeviceName, O_RDWR));
		closing(false);
		break;
	default:
		return error("AudioDevice: Illegal open mode.");
	}
	if (fd < 0)
		return error("OSS device open failed: ", strerror(errno));
	else
		return 0;
}

int SinglePortOSSAudioDevice::doSetFormat(int sampfmt, int chans, double srate)
{
	int sampleFormat;
	int deviceFormat = MUS_GET_FORMAT(sampfmt);
	switch (MUS_GET_FORMAT(sampfmt)) {
		case MUS_UBYTE:
			sampleFormat = AFMT_U8;
			_bytesPerFrame = 1;
			break;
		case MUS_BYTE:
			sampleFormat = AFMT_S8;
			_bytesPerFrame = 1;
			break;
		case MUS_LFLOAT:
			deviceFormat = NATIVE_SHORT_FMT;
		case MUS_LSHORT:
			_bytesPerFrame = 2;
			sampleFormat = AFMT_S16_LE;
			break;
		case MUS_BFLOAT:
			deviceFormat = NATIVE_SHORT_FMT;
		case MUS_BSHORT:
			_bytesPerFrame = 2;
			sampleFormat = AFMT_S16_BE;
			break;
		default:
			_bytesPerFrame = 0;
			return error("Unsupported sample format");
	};
	int realChans = chans;
	// realChans can return a different value than requested chans.
	int status = setDeviceFormat(device(), sampleFormat, &realChans, (int) srate);
	if (status == 0) {
		_bytesPerFrame *= realChans;
		// Store the device params to allow format conversion.
		setDeviceParams(deviceFormat | MUS_INTERLEAVED, realChans, srate);
	}
	return status;
}

int
SinglePortOSSAudioDevice::doSetQueueSize(int *pWriteSize, int *pCount)
{
	int reqQueueBytes = *pWriteSize * getDeviceBytesPerFrame();
	int reqCount = *pCount;
	PRINT0("SinglePortOSSAudioDevice::doSetQueueSize: Asking for queue of %d %d-byte fragments\n",
		   reqCount, reqQueueBytes);
	int queuecode = ((int) (log(reqQueueBytes) / log(2.0)));
	int sizeCode = (reqCount << 16) | (queuecode & 0x0000ffff);
	if (ioctl(SNDCTL_DSP_SETFRAGMENT, &sizeCode) == -1) {
		printf("ioctl(SNDCTL_DSP_SETFRAGMENT, ...) returned -1\n");
	}
	int fragSize = 0;
	if (ioctl(SNDCTL_DSP_GETBLKSIZE, &fragSize) == -1) {
		return error("OSS error while retrieving fragment size: ", strerror(errno));
	}
	PRINT0("SinglePortOSSAudioDevice::doSetQueueSize: OSS returned fragment size of %d bytes\n",
		   fragSize);
	*pWriteSize = fragSize / getDeviceBytesPerFrame();
	int bufferSize = fragSize * reqCount;
	PRINT0("SinglePortOSSAudioDevice::doSetQueueSize: writesize = %d frames, count = %d\n",
		   *pWriteSize, *pCount);
	PRINT1("SinglePortOSSAudioDevice::doSetQueueSize: audio buffer will be %d bytes\n",
		   bufferSize);
	return 0;
}

int
SinglePortOSSAudioDevice::doGetFrames(void *frameBuffer, int frameCount)
{
	int toRead = frameCount * _bytesPerFrame;
	int read = ::read(device(), frameBuffer, toRead);
	PRINT1("SinglePortOSSAudioDevice::doGetFrames: %d bytes to read, %d read\n", toRead, read);
	if (read > 0) {
		int frames = read / _bytesPerFrame;
		incrementFrameCount(frames);
		return frames;
	}
	else {
		return error("Error reading from OSS device: ", strerror(-read));
	}
}

int
SinglePortOSSAudioDevice::doSendFrames(void *frameBuffer, int frameCount)
{
	int toWrite = frameCount * _bytesPerFrame;
	int written = ::write(device(), frameBuffer, toWrite);
	PRINT1("SinglePortOSSAudioDevice::doSendFrames: %d bytes to write, %d written\n", toWrite, written);
	if (written > 0) {
		int frames = written / _bytesPerFrame;
		incrementFrameCount(frames);
		return frames;
	}
	else {
		return error("Error writing to OSS device: ", strerror(-written));
	}
}

// Implementation of OSSAudioDevice virtual methods

int
SinglePortOSSAudioDevice::closeDevice()
{
	int status = ::close(device());
	setDevice(0);
	return status;
}

#endif	// LINUX
