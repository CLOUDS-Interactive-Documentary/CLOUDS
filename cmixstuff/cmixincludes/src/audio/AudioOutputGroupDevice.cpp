// AudioOutputGroupDevice.cpp

#include "AudioOutputGroupDevice.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>

#ifndef NULL
#define NULL 0
#endif

// This subclass is designed to allow up to three independent output AudioDevice
// instances to function and be controlled as a single unit.  It allows for the creation 
// of compound AudioDevices which play back to more than one HW device.
//
// In order to support the AudioDevice thread-driven model, only one of the
// AudioDevices can run in Active mode (where it spawns a child thread and
// invokes the run() method).  The others must run in Passive mode, where they
// waits for the application to call getFrames() or sendFrames().  The
// first AudioDevice * argument to the constructor, below, is the master device.

AudioOutputGroupDevice::AudioOutputGroupDevice(AudioDevice *masterDevice,
							 				   AudioDevice *outputDevice2,
							 				   AudioDevice *outputDevice3)
	: _count((outputDevice3 != NULL) ? 3 : 2), _devices(new AudioDevice *[_count])
{
	_devices[0] = masterDevice;
	_devices[1] = outputDevice2;
	if (outputDevice3)
		_devices[2] = outputDevice3;
}

AudioOutputGroupDevice::~AudioOutputGroupDevice()
{
	for (int dev = 0; dev < _count; ++dev)
		delete _devices[dev];
	delete [] _devices;
}

int AudioOutputGroupDevice::setFrameFormat(int sampfmt, int chans)
{
	int status = 0;
	for (int dev = 0; dev < _count && status == 0; ++dev) {
		status = _devices[dev]->setFrameFormat(sampfmt, chans);
	}
	return status;
}

int AudioOutputGroupDevice::open(int mode, int sampfmt, int chans, double sr)
{
	if ((mode & DirectionMask) != Playback)
		return -1;
	int status = 0;
	for (int dev = 0; dev < _count && status == 0; ++dev) {
		int outmode = (dev == 0) ? mode : mode | Passive;
		status = _devices[dev]->open(outmode, sampfmt, chans, sr);
	}
	return status;
}

int AudioOutputGroupDevice::close()
{
	int status = 0;
	for (int dev = 0; dev < _count && status == 0; ++dev) {
		status = _devices[dev]->close();
	}
	return status;
}

int AudioOutputGroupDevice::start(Callback *runCallback)
{
	int status = 0;
	for (int dev = _count - 1; dev > 0 && status == 0; --dev) {
		status = _devices[dev]->start(NULL, NULL);
	}
	if (status == 0)
		status = _devices[0]->start(runCallback);
	return status;
}

int AudioOutputGroupDevice::setStopCallback(Callback *stopCallback)
{
	return _devices[0]->setStopCallback(stopCallback);
}

bool AudioOutputGroupDevice::runCallback()
{
	assert(0);	// never supposed to be called!
	return false;
}

bool AudioOutputGroupDevice::stopCallback() {
	assert(0);	// never supposed to be called!
	return false;
}

int AudioOutputGroupDevice::pause(bool paused)
{
	int status = _devices[0]->pause(paused);
	return status;
}

int AudioOutputGroupDevice::stop()
{
	int status = 0;
	for (int dev = 0; dev < _count && status == 0; ++dev) {
		status = _devices[dev]->stop();
	}
	return status;
}

int AudioOutputGroupDevice::setFormat(int fmt, int chans, double srate)
{
	int status = 0;
	for (int dev = 0; dev < _count && status == 0; ++dev) {
		status = _devices[dev]->setFormat(fmt, chans, srate);
	}
	return status;
}

// This is tricky:  If the master device resets the queue size, we pass that to
// the other devices.  If it resets it again, we must repeat the sequence until
// they either agree or there is an error.

int AudioOutputGroupDevice::setQueueSize(int *pWriteSize, int *pCount)
{
	int status = 0;
	int queueSize = *pWriteSize;
	bool queuesAgree = true;
	do {
		int oldQueueSize = queueSize;
		for (int dev = 0; dev < _count && status == 0; ++dev) {
			status = _devices[dev]->setQueueSize(&queueSize, pCount);
		}
		if (!queuesAgree) {
			if (queueSize != oldQueueSize) {
				fprintf(stderr, "Cannot reconcile output queue sizes.\n");
				return -1;
			}
		}
		else if (!(queuesAgree = (queueSize == oldQueueSize))) {
			if (queueSize > oldQueueSize)
				queueSize = *pWriteSize * 2;
			else 
				queueSize = *pWriteSize / 2;
		}
	} while (status == 0 && !queuesAgree);
	*pWriteSize = queueSize;
	return status;
}

int AudioOutputGroupDevice::getFrames(void *frameBuffer, int frameCount)
{
	// If getFrames() is called on this class, we will assume that the first
	// device is full-duplex.  If this is not so, this will fail as expected.
	return _devices[0]->getFrames(frameBuffer, frameCount);
}

int AudioOutputGroupDevice::sendFrames(void *frameBuffer, int frameCount)
{
	int frames = 0;
	for (int dev = 0; dev < _count; ++dev) {
		frames = _devices[dev]->sendFrames(frameBuffer, frameCount);
		if (frames != frameCount)
			break;	// Error
	}
	return frames;
}

bool AudioOutputGroupDevice::isOpen() const
{
	bool open = true;
	for (int dev = 0; dev < _count; ++dev) {
		open = _devices[dev]->isOpen() && open;
	}
	return open;
}

bool AudioOutputGroupDevice::isRunning() const
{
	return _devices[0]->isRunning();
}

bool AudioOutputGroupDevice::isPaused() const
{
	return _devices[0]->isPaused();
}

double AudioOutputGroupDevice::getPeak(int chan, long *peakLoc) const
{
	return _devices[_count - 1]->getPeak(chan, peakLoc);
}

const char *AudioOutputGroupDevice::getLastError() const
{
	const char *theError = NULL;
	for (int dev = 0; dev < _count; ++dev) {
		theError = _devices[dev]->getLastError();
		if (strlen(theError) != 0)
			break;
	}
	return theError;
}
