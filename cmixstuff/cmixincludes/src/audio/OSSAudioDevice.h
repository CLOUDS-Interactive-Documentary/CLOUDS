// OSSAudioDevice.h

#ifndef _OSS_AUDIO_DEVICE_H_
#define _OSS_AUDIO_DEVICE_H_

#ifdef LINUX

#include "ThreadedAudioDevice.h"

class OSSAudioDevice : public ThreadedAudioDevice {
public:
	OSSAudioDevice(const char *devPath);
	virtual ~OSSAudioDevice();
	
protected:
	// ThreadedAudioDevice reimplementation
	virtual void run();
	// AudioDeviceImpl reimplementation
	virtual int doOpen(int mode)=0;
	virtual int doClose();
	virtual int doStart();
	virtual int doPause(bool);
	virtual int doSetFormat(int sampfmt, int chans, double srate)=0;
	virtual int doSetQueueSize(int *pWriteSize, int *pCount)=0;
	virtual int	doGetFrames(void *frameBuffer, int frameCount)=0;
	virtual int	doSendFrames(void *frameBuffer, int frameCount)=0;
	// Local methods
	int			ioctl(int req, void *argp);
	int			bufferSize() const { return _bufferSize; }
protected:
	// OSS device-level virtual implementation
	virtual int closeDevice()=0;
	// OSS device-level non-virtual implementation
	int setDeviceFormat(int dev, int sampfmt, int *chans, int srate);

protected:
	char *	_inputDeviceName;
	char *	_outputDeviceName;
	int		_bytesPerFrame;
	int		_bufferSize;
};

#endif	// LINUX

#endif	// _OSS_AUDIO_DEVICE_H_
