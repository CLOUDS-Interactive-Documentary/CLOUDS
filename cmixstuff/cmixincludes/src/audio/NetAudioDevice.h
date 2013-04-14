// NetAudioDevice.h
//
#ifndef _NETAUDIODEVICE_H_
#define _NETAUDIODEVICE_H_

#if defined (NETAUDIO)

#include "ThreadedAudioDevice.h"

class NetAudioDevice : public ThreadedAudioDevice {
public:
	NetAudioDevice(const char *path);
	virtual ~NetAudioDevice();
	// Recognizer
	static bool			recognize(const char *);
	// Creator
	static AudioDevice*	create(const char *, const char *, int);
	
protected:
	// ThreadedAudioDevice override
	int			waitForDevice(unsigned int wTime=0);
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

protected:
	int			waitForConnect(unsigned int wTime);
	int			disconnect();
	inline bool	connected();
	int			configure();
private:
	struct Impl;
	Impl			*_impl;
};

#endif	// NETAUDIO

#endif	// _NETAUDIODEVICE_H_

