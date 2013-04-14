// ALSAAudioDevice.h

#ifndef _ALSA_AUDIO_DEVICE_H_
#define _ALSA_AUDIO_DEVICE_H_

#ifdef ALSA

#include "ThreadedAudioDevice.h"
#include <pthread.h>
#define ALSA_PCM_NEW_HW_PARAMS_API
#include <alsa/asoundlib.h>

class ALSAAudioDevice : public ThreadedAudioDevice {
public:
	ALSAAudioDevice(const char *devName="hw:0,0");
	virtual ~ALSAAudioDevice();
	// Recognizer
	static bool			recognize(const char *);
	// Creator
	static AudioDevice*	create(const char *, const char *, int);
	
protected:
	// ThreadedAudioDevice reimplementation
	virtual void run();
	// ThreadedAudioDevice override
	virtual int doStop();
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
	const char *		_deviceName;
	snd_pcm_t *			_handle;
	snd_pcm_hw_params_t *_hwParams;
	snd_pcm_uframes_t 	_bufSize, _periodSize;
	bool				_stopDuringPause;
	int					_pipeFds[2];
};

#endif	// ALSA

#endif	// _ALSA_AUDIO_DEVICE_H_
