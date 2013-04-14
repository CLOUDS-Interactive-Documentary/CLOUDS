// MultiPortOSSAudioDevice.h

#ifndef _MULTI_PORT_OSS_AUDIO_DEVICE_H_
#define _MULTI_PORT_OSS_AUDIO_DEVICE_H_

#ifdef LINUX

#include "OSSAudioDevice.h"

class MultiPortOSSAudioDevice : public OSSAudioDevice {
public:
	MultiPortOSSAudioDevice(const char *devPath, int portCount);
	virtual ~MultiPortOSSAudioDevice();
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

private:
	int _devCount;
	int *_devices;
};

#endif	// LINUX

#endif	// _MULTI_PORT_OSS_AUDIO_DEVICE_H_
