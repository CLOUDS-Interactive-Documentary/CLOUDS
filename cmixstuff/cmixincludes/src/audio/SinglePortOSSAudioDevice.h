// SinglePortOSSAudioDevice.h

#ifndef _SINGLE_PORT_OSS_AUDIO_DEVICE_H_
#define _SINGLE_PORT_OSS_AUDIO_DEVICE_H_

#ifdef LINUX

#include "OSSAudioDevice.h"

class SinglePortOSSAudioDevice : public OSSAudioDevice {
public:
	SinglePortOSSAudioDevice(const char *devPath);
	virtual ~SinglePortOSSAudioDevice();
	// Recognizer
	static bool			recognize(const char *);
	// Creator
	static AudioDevice*	create(const char *, const char *, int);
	
protected:
	// AudioDeviceImpl reimplementation
	virtual int doOpen(int mode);
	virtual int doSetFormat(int sampfmt, int chans, double srate);
	virtual int doSetQueueSize(int *pWriteSize, int *pCount);
	virtual int	doGetFrames(void *frameBuffer, int frameCount);
	virtual int	doSendFrames(void *frameBuffer, int frameCount);
protected:
	// OSSAudioDevice implementation
	virtual int closeDevice();
};

#endif	// LINUX

#endif	// _SINGLE_PORT_OSS_AUDIO_DEVICE_H_
