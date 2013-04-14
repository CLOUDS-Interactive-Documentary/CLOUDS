// ExampleAudioDevice.h
//
#ifndef _EXAMPLE_AUDIODEVICE_H_
#define _EXAMPLE_AUDIODEVICE_H_

#include "ThreadedAudioDevice.h"

class ExampleAudioDevice : public ThreadedAudioDevice {
public:
	ExampleAudioDevice();
	virtual ~ExampleAudioDevice();
	// Recognizer
	static bool recognize(const char *);
	// Creator
	static AudioDevice* create(const char *, const char *, int);
	
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
	virtual int doGetFrames(void *frameBuffer, int frameCount);
	virtual int doSendFrames(void *frameBuffer, int frameCount);
private:
	struct Impl;
	Impl *_impl;
};

#endif // _EXAMPLE_AUDIODEVICE_H_

