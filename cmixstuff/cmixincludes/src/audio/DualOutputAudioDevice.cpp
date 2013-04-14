// DualOutputAudioDevice.cpp

#include "DualOutputAudioDevice.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>

#ifndef NULL
#define NULL 0
#endif

DualOutputAudioDevice::DualOutputAudioDevice(AudioDevice *hwDevice,
											 AudioDevice *fileDevice,
											 bool fileDoesLimiting)
	: AudioOutputGroupDevice(hwDevice, fileDevice),
	  _fileDoesLimiting(fileDoesLimiting)
{
}

DualOutputAudioDevice::~DualOutputAudioDevice()
{
}

int DualOutputAudioDevice::sendFrames(void *frameBuffer, int frameCount)
{
	// If file does not need limiting, write to the file (dev 1) first.
	// If file does need limiting, write to HW first, then to file.
	
	int frames = _devices[!_fileDoesLimiting]->sendFrames(frameBuffer, frameCount);

	if (frames == frameCount)
		frames = _devices[_fileDoesLimiting]->sendFrames(frameBuffer, frameCount);
		
	return frames;
}

double DualOutputAudioDevice::getPeak(int chan, long *loc) const
{
	// Always get peaks from device which did the limiting.
	return _devices[_fileDoesLimiting ? 1 : 0]->getPeak(chan, loc);
}

