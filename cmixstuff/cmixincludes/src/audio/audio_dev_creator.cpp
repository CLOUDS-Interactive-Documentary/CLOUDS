// audio_dev_creator.cpp
//
// These functions coordinate the presence of multiple supported audio device
// types on a single machine.  It builds a database of creators and recognizers
// based upon the compile options, and uses that to determine which object
// to build based on the passed-in descriptor.

#include <ugens.h>

#ifdef LINUX
#include "SinglePortOSSAudioDevice.h"
#include "MultiPortOSSAudioDevice.h"
#include "TestAudioDevice.h"
#endif
#ifdef ALSA
#include "ALSAAudioDevice.h"
#endif
#ifdef NETAUDIO
#include "NetAudioDevice.h"
#endif
#ifdef MACOSX
#include "OSXAudioDevice.h"
#endif
#ifdef SGI
#include "SGIAudioDevice.h"
#endif
#ifdef JACK
#include "JackAudioDevice.h"
#endif

#include "AudioIODevice.h"

typedef AudioDevice * (*CreatorFun)(const char *, const char *, int mode);

struct AudioDevEntry {
	bool		(*recognizer)(const char *desc);
	CreatorFun	creator;
};

static const AudioDevEntry s_AudioDevEntries[] = {
#ifdef NETAUDIO
	{ &NetAudioDevice::recognize, &NetAudioDevice::create },
#endif
#ifdef JACK // before MACOSX, since OSXAudioDevice::recognize matches anything
	{ &JackAudioDevice::recognize, &JackAudioDevice::create },
#endif
#ifdef MACOSX
	{ &OSXAudioDevice::recognize, &OSXAudioDevice::create },
#endif
#ifdef ALSA
	{ &ALSAAudioDevice::recognize, &ALSAAudioDevice::create },
#endif
#ifdef LINUX
#ifdef TEST_AUDIO_DEVICE
	{ &TestAudioDevice::recognize, &TestAudioDevice::create },
#endif
	{ &MultiPortOSSAudioDevice::recognize, &MultiPortOSSAudioDevice::create },
	{ &SinglePortOSSAudioDevice::recognize, &SinglePortOSSAudioDevice::create },
#endif
#ifdef SGI
	{ &SGIAudioDevice::recognize, &SGIAudioDevice::create },
#endif
	{ NULL, NULL }
};

AudioDevice *
createAudioDevice(const char *inputDesc,
				  const char *outputDesc,
				  bool recording, bool playing)
{
	CreatorFun iCreator = NULL, oCreator = NULL;
	AudioDevice *theDevice = NULL;
	const AudioDevEntry *currentEntry;

	if (recording) {
		// Search for creator for inputDesc.
		for (currentEntry = s_AudioDevEntries;
			 currentEntry->recognizer != NULL;
			 ++currentEntry)
		{
			if (currentEntry->recognizer(inputDesc) == true) {
				iCreator = currentEntry->creator;
				break;
			}
		}
		if (iCreator == NULL) {
			fprintf(stderr, "Unrecognized input device name '%s'.\n",
				inputDesc);
			return NULL;
		}
	}
	if (playing) {
		// Search for creator for outputDesc.
		for (currentEntry = s_AudioDevEntries;
			 currentEntry->recognizer != NULL;
			 ++currentEntry)
		{
			if (currentEntry->recognizer(outputDesc) == true) {
				oCreator = currentEntry->creator;
				break;
			}
		}
		if (oCreator == NULL) {
			fprintf(stderr, "Unrecognized output device name '%s'.\n",
				outputDesc);
			return NULL;
		}
	}
	// RtcHandle full duplex requests.
	if (recording && playing) {
		AudioDevice *fullDuplexDevice = NULL;
		// If same HW for input and output, try to create a single
		// full-duplex device.
		if (iCreator == oCreator) {		
			fullDuplexDevice = iCreator(inputDesc,
										outputDesc,
										AudioDevice::RecordPlayback);
			
		}
		// If that failed, OR the input and output HW are different, create
		// a compound device from the two, and drive it via input.
		if (fullDuplexDevice == NULL) {
			AudioDevice *inDev = iCreator(inputDesc, NULL, AudioDevice::Record);
			AudioDevice *outDev = oCreator(NULL, outputDesc, AudioDevice::Playback);
			if (!inDev) {
				fprintf(stderr, "Failed to create input audio device!\n");
				delete outDev;
				return NULL;
			}
			else if (!outDev) {
				fprintf(stderr, "Failed to create output audio device!\n");
				delete inDev;
				return NULL;
			}

			bool forceInputActive = false;
			fullDuplexDevice = new AudioIODevice(inDev, outDev, forceInputActive);
		}
		theDevice = fullDuplexDevice;
	}
	// RtcHandle half-duplex requests.
	else theDevice = (recording) ?
						iCreator(inputDesc, NULL, AudioDevice::Record)
						: oCreator(NULL, outputDesc, AudioDevice::Playback);
						
	return theDevice;
}
