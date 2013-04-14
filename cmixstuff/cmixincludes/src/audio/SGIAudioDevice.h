// SGIAudioDevice.h
//
#ifndef _SGI_AUDIODEVICE_H_
#define _SGI_AUDIODEVICE_H_

#include <dmedia/audio.h>
#include "ThreadedAudioDevice.h"

class SGIAudioDevice : public ThreadedAudioDevice {
public:
	SGIAudioDevice(const char *);
	virtual ~SGIAudioDevice();
	// Recognizer
	static bool			recognize(const char *);
	// Creator
	static AudioDevice*	create(const char *, const char *, int);
	
protected:
	// ThreadedAudioDevice reimplementation
	virtual void run();
	// AudioDeviceImpl reimplementation
	virtual int doOpen(int mode);
	virtual int doClose();
	virtual int doStart();
	virtual int doPause(bool);
	virtual int doSetFormat(int sampfmt, int chans, double srate);
	virtual int doSetQueueSize(int *pWriteSize, int *pCount);
	virtual int	doGetFrames(void *frameBuffer, int frameCount);
	virtual int	doSendFrames(void *frameBuffer, int frameCount);
private:
	struct Impl;
	Impl			*_impl;
};

#endif	// _SGI_AUDIODEVICE_H_

