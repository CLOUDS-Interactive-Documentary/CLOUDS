// AudioIODevice.h -- allows independent input and output audio device 
//					  classes to function as one full-duplex device.

#ifndef _AUDIOIODEVICE_H_
#define _AUDIOIODEVICE_H_

#include "AudioDevice.h"

class AudioIODevice : public AudioDevice {
public:
	AudioIODevice(AudioDevice *inDev, AudioDevice *outDev, bool inputIsActive=false);
	virtual ~AudioIODevice();
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
	inline AudioDevice *getActiveDevice() const;
	inline AudioDevice *getPassiveDevice() const;

private:
	AudioDevice	*_inputDevice;
	AudioDevice *_outputDevice;
	bool		_inputActive;
};

inline AudioDevice *
AudioIODevice::getActiveDevice() const {
	return (_inputActive) ? _inputDevice : _outputDevice;
}

inline AudioDevice *
AudioIODevice::getPassiveDevice() const {
	return (_inputActive) ? _outputDevice : _inputDevice;
}

#endif	// _AUDIOIODEVICE_H_
