// TestAudioDevice.h
//
#ifndef _TEST_AUDIODEVICE_H_
#define _TEST_AUDIODEVICE_H_

#include "AudioDeviceImpl.h"

class OSSAudioDevice;

class TestAudioDevice : public AudioDeviceImpl {
public:
	TestAudioDevice(const char *desc, int mode);
	virtual ~TestAudioDevice();
	// Recognizer
	static bool			recognize(const char *);
	// Creator
	static AudioDevice*	create(const char *, const char *, int);
	
	// AudioDeviceImpl overrides
	virtual int	start(Callback *callback);
	virtual int setStopCallback(Callback *stopCallback);
protected:
	// AudioDeviceImpl reimplementation
	virtual int doOpen(int mode);
	virtual int doClose();
	virtual int doStart();
	virtual int doStop();
	virtual int doPause(bool);
	virtual int doSetFormat(int sampfmt, int chans, double srate);
	virtual int doSetQueueSize(int *pWriteSize, int *pCount);
	virtual int doGetFrameCount() const;
	virtual int	doGetFrames(void *frameBuffer, int frameCount);
	virtual int	doSendFrames(void *frameBuffer, int frameCount);
private:
	char *				_descriptor;
	int					_format;
	int					_channels;
	short *				_buffer;
	OSSAudioDevice *	_device;
};

#endif	// _TEST_AUDIODEVICE_H_

