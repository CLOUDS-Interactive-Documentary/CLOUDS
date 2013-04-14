// AudioDevice.cpp
//
// This file exists in order to define the imbedded class, and to 
// contain the vtable symbol for the class.
//

#include "AudioDevice.h"

AudioDevice::Callback::Callback(AudioDevice *dev,
								CallbackFun fun,
								void *context)
	: _device(dev), _callbackFun(fun), _context(context)
{
}

bool AudioDevice::Callback::call()
{
	return (_callbackFun) ? _callbackFun(_device, _context) : true;
}

AudioDevice::~AudioDevice() {}

int AudioDevice::start(CallbackFun runCallback, void *context)
{
	return start(new Callback(this, runCallback, context));
}

int AudioDevice::setStopCallback(CallbackFun runCallback, void *context)
{
	return setStopCallback(new Callback(this, runCallback, context));
}
