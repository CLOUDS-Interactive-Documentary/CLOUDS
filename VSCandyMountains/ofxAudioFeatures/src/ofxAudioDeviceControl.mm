#include "ofxAudioDeviceControl.h"

#ifdef __APPLE__
#include <CoreFoundation/CoreFoundation.h>
#include <CoreAudio/CoreAudio.h>

#include <iostream>

bool inputMuted = false;
bool outputMuted = false;

//--------------------------------------------------------------
// getting system volume
float
ofxAudioDeviceControl::getVolume()
{
	Float32			outputVolume;
	
	UInt32 propertySize = 0;
	OSStatus status = noErr;
	AudioObjectPropertyAddress propertyAOPA;
	propertyAOPA.mElement = kAudioObjectPropertyElementMaster;
	propertyAOPA.mSelector = kAudioHardwareServiceDeviceProperty_VirtualMasterVolume;
	propertyAOPA.mScope = kAudioDevicePropertyScopeOutput;
	
	AudioDeviceID outputDeviceID = defaultOutputDeviceID();

	if (outputDeviceID == kAudioObjectUnknown)
	{
    std::cout << "Unknown device" << std::endl;
		return 0.0;
	}
	
	if (!AudioHardwareServiceHasProperty(outputDeviceID, &propertyAOPA))
	{
    std::cout << "No volume returned for device " << outputDeviceID << std::endl;
		return 0.0;
	}
	
	propertySize = sizeof(Float32);
	
	status = AudioHardwareServiceGetPropertyData(outputDeviceID, &propertyAOPA, 0, NULL, &propertySize, &outputVolume);
	
	if (status)
	{
    std::cout << "No volume returned for device" << outputDeviceID << std::endl;
		return 0.0;
	}
	
	if (outputVolume < 0.0 || outputVolume > 1.0) return 0.0;
	
	return outputVolume;
}

//--------------------------------------------------------------
// setting system volume - mutes if under threshhold
void
ofxAudioDeviceControl::setVolume(float volume)
{
  if (volume < 0.0 || volume > 1.0)
	{
    std::cout << "Requested volume out of range (" << volume << ")" << std::endl;
		return;
		
	}
  
	// get output device device
	UInt32 propertySize = 0;
	OSStatus status = noErr;
	AudioObjectPropertyAddress propertyAOPA;
	propertyAOPA.mElement = kAudioObjectPropertyElementMaster;
	propertyAOPA.mScope = kAudioDevicePropertyScopeOutput;
	
	if (volume < 0.001)
	{
    std::cout << "Requested mute" << std::endl;
		propertyAOPA.mSelector = kAudioDevicePropertyMute;
	}
	else
	{
    std::cout << "Requested volume " << volume << std::endl;
		propertyAOPA.mSelector = kAudioHardwareServiceDeviceProperty_VirtualMasterVolume;
	}
	
	AudioDeviceID outputDeviceID = defaultOutputDeviceID();
	
	if (outputDeviceID == kAudioObjectUnknown)
	{
    std::cout << "Unknown device" << std::endl;
		return;
	}
	
	if (!AudioHardwareServiceHasProperty(outputDeviceID, &propertyAOPA))
	{
    std::cout << "Device " << outputDeviceID << " does not support volume control" << std::endl;
		return;
	}

	Boolean canSetVolume = NO;
	
	status = AudioHardwareServiceIsPropertySettable(outputDeviceID, &propertyAOPA, &canSetVolume);
	
	if (status || canSetVolume == NO)
	{
    std::cout << "Device " << outputDeviceID << " does not support volume control" << std::endl;
		return;
	}
	
	if (propertyAOPA.mSelector == kAudioDevicePropertyMute)
	{
		propertySize = sizeof(UInt32);
		UInt32 mute = 1;
		status = AudioHardwareServiceSetPropertyData(outputDeviceID, &propertyAOPA, 0, NULL, propertySize, &mute);
	}
	else
	{
		propertySize = sizeof(Float32);
		
		status = AudioHardwareServiceSetPropertyData(outputDeviceID, &propertyAOPA, 0, NULL, propertySize, &volume);
		
		if (status)
		{
      std::cout << "Unable to set volume for device " << outputDeviceID << std::endl;
		}
		
		// make sure we're not muted
		propertyAOPA.mSelector = kAudioDevicePropertyMute;
		propertySize = sizeof(UInt32);
		UInt32 mute = 0;
		
		if (!AudioHardwareServiceHasProperty(outputDeviceID, &propertyAOPA))
		{
      std::cout << "Device " << outputDeviceID << " does not support muting" << std::endl;
			return;
		}
		
		Boolean canSetMute = NO;
		
		status = AudioHardwareServiceIsPropertySettable(outputDeviceID, &propertyAOPA, &canSetMute);
		
		if (status || !canSetMute)
		{
      std::cout << "Device " << outputDeviceID << " does not support muting" << std::endl;
			return;
		}
		
		status = AudioHardwareServiceSetPropertyData(outputDeviceID, &propertyAOPA, 0, NULL, propertySize, &mute);
	}
  
	if (status)
	{
    std::cout << "Unable to set volume for device " << outputDeviceID << std::endl;
	}  
}

//--------------------------------------------------------------
AudioDeviceID
ofxAudioDeviceControl::defaultOutputDeviceID()
{
	AudioDeviceID	outputDeviceID = kAudioObjectUnknown;
	
	// get output device device
	UInt32 propertySize = 0;
	OSStatus status = noErr;
	AudioObjectPropertyAddress propertyAOPA;
	propertyAOPA.mScope = kAudioObjectPropertyScopeGlobal;
	propertyAOPA.mElement = kAudioObjectPropertyElementMaster;
	propertyAOPA.mSelector = kAudioHardwarePropertyDefaultOutputDevice;
	
	if (!AudioHardwareServiceHasProperty(kAudioObjectSystemObject, &propertyAOPA))
	{
    std::cout << "Cannot find default output device!" << std::endl;
		return outputDeviceID;
	}
	
	propertySize = sizeof(AudioDeviceID);
	
	status = AudioHardwareServiceGetPropertyData(kAudioObjectSystemObject, &propertyAOPA, 0, NULL, &propertySize, &outputDeviceID);
	
	if(status)
	{
    std::cout << "Cannot find default output device!" << std::endl;
	}

	return outputDeviceID;
}

//--------------------------------------------------------------
float
ofxAudioDeviceControl::getInputVolume()
{
  AudioObjectPropertyAddress address;
  OSStatus err;
  UInt32 size;

  Float32 volume;
  AudioDeviceID deviceID = defaultInputDeviceID();

  address.mSelector = kAudioDevicePropertyVolumeScalar;
  address.mScope = kAudioDevicePropertyScopeInput;

  size = sizeof(volume);
  address.mElement = kAudioObjectPropertyElementMaster;
  // returns an error which we expect since it reported not having the property
  err = AudioObjectGetPropertyData(deviceID, &address, 0, nil, &size, &volume);
}

//--------------------------------------------------------------
void
ofxAudioDeviceControl::setInputVolume(float newVolume)
{
  AudioObjectPropertyAddress address;
  OSStatus err;
  UInt32 size;
  
  Float32 volume;
  AudioDeviceID deviceID = defaultInputDeviceID();

  address.mScope = kAudioDevicePropertyScopeInput;
  address.mElement = kAudioObjectPropertyElementMaster;

  if ( newVolume < 0.0 ) volume = 0.0;
  else if ( newVolume > 1.0 ) volume = 1.0;
  else volume = newVolume;

  if (volume < 0.05)
  {
    if (!inputMuted)
    {
      size = sizeof(UInt32);
      UInt32 mute = 1;
      address.mSelector = kAudioDevicePropertyMute;
      err = AudioObjectSetPropertyData(deviceID, &address, 0, nil, size, &mute);
      if (!err) inputMuted = true;
    }
  }
  else {
    if (inputMuted)
    {
      size = sizeof(UInt32);
      UInt32 mute = 0;
      address.mSelector = kAudioDevicePropertyMute;
      err = AudioObjectSetPropertyData(deviceID, &address, 0, nil, size, &mute);
      if (!err) inputMuted = false;
    }
  }

  size = sizeof(volume);
  address.mSelector = kAudioDevicePropertyVolumeScalar;
  err = AudioObjectSetPropertyData(deviceID, &address, 0, nil, size, &volume);
}

//--------------------------------------------------------------
AudioDeviceID
ofxAudioDeviceControl::defaultInputDeviceID()
{
  AudioObjectPropertyAddress address;
  OSStatus err;
  UInt32 size;
  
  AudioDeviceID deviceID;

  // get the default input device id
  address.mSelector = kAudioHardwarePropertyDefaultInputDevice;
  address.mScope = kAudioObjectPropertyScopeGlobal;
  address.mElement = kAudioObjectPropertyElementMaster;
  size = sizeof(deviceID);

  err = AudioObjectGetPropertyData( kAudioObjectSystemObject, &address, 0, nil, &size, &deviceID );
  
  return deviceID;
}

//--------------------------------------------------------------
size_t
ofxAudioDeviceControl::getInputChannels()
{
  AudioObjectPropertyAddress address;
  OSStatus err;
  UInt32 size;

  UInt32 channels[64];

  AudioDeviceID deviceID = defaultInputDeviceID();

  address.mSelector = kAudioDevicePropertyPreferredChannelsForStereo;
  address.mScope = kAudioDevicePropertyScopeInput;
  address.mElement = kAudioObjectPropertyElementWildcard;
  size = sizeof(channels);
  
  err = AudioObjectGetPropertyData(deviceID, &address, 0, nil, &size, &channels);

  return size;
}

/*
// run some tests to see what channels might respond to volume changes
if ( ! err )
{
  Boolean                     hasProperty;
  
  address.mSelector = kAudioDevicePropertyVolumeScalar;
  address.mScope = kAudioDevicePropertyScopeInput;
  
  // On my MacBook Pro using the default microphone input:
  address.mElement = kAudioObjectPropertyElementMaster;
  // returns true, VolumeScalar property for the master channel
  hasProperty = AudioObjectHasProperty( deviceID, &address );
}
*/
#endif // __APPLE__
