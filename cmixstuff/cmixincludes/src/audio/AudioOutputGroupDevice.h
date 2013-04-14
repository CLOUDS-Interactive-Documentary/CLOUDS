// AudioOutputGroupDevice.h -- allows multiple output audio device instances to 
// function as one device.

#ifndef _AUDIOOUTPUTGROUPDEVICE_H_
#define _AUDIOOUTPUTGROUPDEVICE_H_

#include "AudioDevice.h"

class AudioOutputGroupDevice : public AudioDevice {
public:
	AudioOutputGroupDevice(AudioDevice *masterDev, AudioDevice *outDev2=0, AudioDevice *outDev3=0);
	virtual ~AudioOutputGroupDevice();
	// AudioDevice redefinitions.
	int			setFrameFormat(int sampfmt, int chans);
	int			open(int mode, int sampfmt, int chans, double sr);
	int			close();
	int			setStopCallback(Callback *stopCallback);
	int			pause(bool);
	int			stop();
	int			setFormat(int fmt, int chans, double srate);
	int			setQueueSize(int *pWriteSize, int *pCount);
	int			getFrames(void *frameBuffer, int frameCount);
	int			sendFrames(void *frameBuffer, int frameCount);
	bool		isOpen() const;
	bool		isRunning() const;
	bool		isPaused() const;
	double		getPeak(int chan, long *peakLoc) const;
	const char *getLastError() const;

protected:
	// AudioDevice redefinitions.
	int			start(Callback *runCallback);
	bool		runCallback();
	bool		stopCallback();

protected:
	int			_count;
	AudioDevice **_devices;
};

#endif	// _AUDIOOUTPUTGROUPDEVICE_H_
