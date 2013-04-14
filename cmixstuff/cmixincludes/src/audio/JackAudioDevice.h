// JackAudioDevice.h
//
#ifndef _JACK_AUDIODEVICE_H_
#define _JACK_AUDIODEVICE_H_

#if defined(JACK)

#include "AudioDeviceImpl.h"

class JackAudioDevice : public AudioDeviceImpl {
public:
	JackAudioDevice(const char *serverName);
	virtual ~JackAudioDevice();
	// Recognizer
	static bool			recognize(const char *);
	// Creator
	static AudioDevice*	create(const char *, const char *, int);
	
protected:
	// AudioDeviceImpl reimplementation
	virtual int doOpen(int mode);
	virtual int doClose();
	virtual int doStart();
	virtual int doPause(bool);
	virtual int doStop();
	virtual int doSetFormat(int sampfmt, int chans, double srate);
	virtual int doSetQueueSize(int *pWriteSize, int *pCount);
	virtual int	doGetFrames(void *frameBuffer, int frameCount);
	virtual int	doSendFrames(void *frameBuffer, int frameCount);
	virtual int doGetFrameCount() const;
private:
	int connectPorts();
	struct Impl;
	Impl			*_impl;
};

#endif

#endif	// _JACK_AUDIODEVICE_H_

