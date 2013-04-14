// DualOutputAudioDevice.h -- specialized version of AudioOutputGroupDevice for
// writing to file and HW simultaneously

#ifndef _DUALOUTPUTAUDIODEVICE_H_
#define _DUALOUTPUTAUDIODEVICE_H_

#include "AudioOutputGroupDevice.h"

class DualOutputAudioDevice : public AudioOutputGroupDevice {
public:
	DualOutputAudioDevice(AudioDevice *hw, AudioDevice *file, bool fileDoesLimiting);
	virtual		~DualOutputAudioDevice();
	// Override of OutputAudioGroupDevice
	int			sendFrames(void *frameBuffer, int frameCount);
	double		getPeak(int chan, long *loc) const;
private:
	bool		_fileDoesLimiting;
};

#endif 	// _DUALOUTPUTAUDIODEVICE_H_
