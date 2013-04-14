// MultiPortOSSAudioDevice.cpp

#if defined(LINUX)

#include "MultiPortOSSAudioDevice.h"
#include <sys/soundcard.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
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

MultiPortOSSAudioDevice::MultiPortOSSAudioDevice(const char *devPath,
												 int portCount)
	: OSSAudioDevice(devPath), _devCount(portCount), _devices(NULL)
{
	_devices = new int[_devCount];
	for (int dev = 0; dev < _devCount; ++dev)
		_devices[dev] = -1;
}

MultiPortOSSAudioDevice::~MultiPortOSSAudioDevice()
{
	delete [] _devices;
}

bool MultiPortOSSAudioDevice::recognize(const char *desc)
{
	return (desc != NULL) 
			&& strncmp(desc, "/dev/", 5) == 0
			&& strchr(desc + 5, '-') != NULL;
}

AudioDevice *
MultiPortOSSAudioDevice::create(const char *inputDesc, 
								const char *outputDesc, 
								int mode)
{
	const char *desc = inputDesc ? inputDesc : outputDesc;
	char devName[32];
	int devLen = 0;
	// Find first digit
	while (!isdigit(desc[devLen]))
		++devLen;
	const char *devnum = desc + devLen;	// skip "/dev/xxx"
	if (strchr(devnum, '-') != NULL) {		// "/dev/xxx0-7", etc.
		strcpy(devName, desc);
		devName[devLen] = '\0';
		int minDev = 0, maxDev=0;
		sscanf(devnum, "%d-%d", &minDev, &maxDev);
		return new MultiPortOSSAudioDevice(devName, maxDev - minDev + 1);
	}
	return NULL;
}

int
MultiPortOSSAudioDevice::doOpen(int mode)
{
	char deviceName[64];
	int devMode = -1;
	for (int dev = 0; dev < _devCount; ++dev)
	{
		switch (mode & DirectionMask) {
		case Playback:
			sprintf(deviceName, "%s%d", _outputDeviceName, dev);
			devMode = O_WRONLY;
			break;
		case Record:
			sprintf(deviceName, "%s%d", _inputDeviceName, dev);
			devMode = O_RDONLY;
			break;
		case RecordPlayback:
			sprintf(deviceName, "%s%d", _outputDeviceName, dev);
			devMode = O_RDWR;
			break;
		default:
			return error("AudioDevice: Illegal open mode.");
		}
		closing(false);
		PRINT0("MultiPortOSSAudioDevice::doOpen: opening %s\n", deviceName);
		int fd = ::open(deviceName, devMode);
		if (fd < 0) {
			return error("OSS multi-port device open failed: ", strerror(errno));
		}
		_devices[dev] = fd;
	}
	setDevice(_devices[0]);	// Sets the base class device.
	return 0;
}

int MultiPortOSSAudioDevice::doSetFormat(int sampfmt, int chans, double srate)
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
	if (chans > _devCount)
		return error("Channel count must be <= device channel count.");

	int status = 0;
	// Set format on each open monaural device.
	for (int dev = 0; dev < _devCount && status == 0; ++dev) {
		int monochans = 1;
		status = setDeviceFormat(_devices[dev], sampleFormat,
								 &monochans, (int) srate);
	}
	if (status == 0) {
		// Store the device params to allow format conversion.
		setDeviceParams(deviceFormat | MUS_NON_INTERLEAVED,
						_devCount,
						srate);
	}
	return status;
}

int
MultiPortOSSAudioDevice::doSetQueueSize(int *pWriteSize, int *pCount)
{
	int reqQueueBytes = *pWriteSize * getDeviceBytesPerFrame();
	int queuecode = ((int) (log(reqQueueBytes) / log(2.0)));
	int sizeCode = (*pCount << 16) | (queuecode & 0x0000ffff);
	int fragSize = 0;
	for (int dev = 0; dev < _devCount; ++dev) {
		int fd = _devices[dev];
		if (::ioctl(fd, SNDCTL_DSP_SETFRAGMENT, &sizeCode) == -1) {
			printf("ioctl(%d, SNDCTL_DSP_SETFRAGMENT, ...) returned -1\n",
				   fd);
		}
		if (::ioctl(fd, SNDCTL_DSP_GETBLKSIZE, &fragSize) == -1) {
			return error("OSS error while retrieving fragment size: ",
						 strerror(errno));
		}
	}
	*pWriteSize = fragSize / getDeviceBytesPerFrame();
	_bufferSize = *pWriteSize * *pCount;
	PRINT0("MultiPortOSSAudioDevice::doSetQueueSize: writesize = %d, count = %d\n", *pWriteSize, *pCount);
	return 0;
}

int
MultiPortOSSAudioDevice::doGetFrames(void *frameBuffer, int frameCount)
{
	int toRead = frameCount * _bytesPerFrame;
	int read = 0;
	void **multiChanBuffer = (void **) frameBuffer;
	for (int dev = 0; dev < _devCount; ++dev) {
		void *chanBuf = multiChanBuffer[dev];
		read = ::read(_devices[dev], chanBuf, toRead);
		if (read <= 0)
			break;
	}
	if (read > 0) {
		int frames = read / _bytesPerFrame;
		incrementFrameCount(frames);
		return frames;
	}
	else {
		return error("Error reading from multi-port OSS device: ", strerror(-read));
	}
}

int
MultiPortOSSAudioDevice::doSendFrames(void *frameBuffer, int frameCount)
{
	int toWrite = frameCount * _bytesPerFrame;
	void **multiChanBuffer = (void **) frameBuffer;
	int written = 0;
	for (int dev = 0; dev < _devCount; ++dev) {
		void *chanBuf = multiChanBuffer[dev];
		written = ::write(_devices[dev], chanBuf, toWrite);
		if (written <= 0)
			break;
	}
	if (written > 0) {
		int frames = written / _bytesPerFrame;
		incrementFrameCount(frames);
		return frames;
	}
	else {
		return error("Error writing to multi-port OSS device: ", strerror(-written));
	}
}

int
MultiPortOSSAudioDevice::closeDevice()
{
	int status = 0;
	for (int dev = 0; dev < _devCount; ++dev) {
		if (_devices[dev] > 0)
			status = ::close(_devices[dev]);
		_devices[dev] = -1;
	}
	setDevice(0);	// Sets the base class device to 0.
	return status;
}

#endif	// LINUX
