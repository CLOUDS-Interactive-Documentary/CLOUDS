// ExampleAudioDevice.cpp - by Douglas Scott
//
// This is designed to support an audio device that uses a classic UNIX
// device file-based approach to I/O, as is the case with the ALSA and 
// OSS drivers.  For audio devices that use a callback approach, like
// CoreAudio and JACK, let this audio device class inherit directly from
// AudioDeviceImpl, rather than from ThreadedAudioDevice.  See
// OSXAudioDevice.cpp/h and JackAudioDevice.cpp/h for more. -JGG

#include "ExampleAudioDevice.h"

// In RTcmix, methods are called in this order during create_audio_devices()...
//
// recognize()
// create()
// doOpen()
// doSetFormat()
// doSetQueueSize()
//
// Then these are called...
//
// doStart()          // during RTcmix::runMainLoop()
// doSendFrames()     // during RTcmix::rtsendsamps()
// doGetFrames()      // during RTcmix::rtgetsamps()
// doStop()           // 

// Here is a partial list of base class helper methods you may use to check
// the state of the system.  Not all of these have valid values at all times --
// some return state that you set via the init/open sequence.  Most should be
// self-explanatory:

// bool		isOpen() const;
// bool		isRunning() const;				-- has start() been called
// bool		isPaused() const;
// int		getFrameFormat() const;			-- MUS_BSHORT, etc.
// int		getDeviceFormat() const;
// bool		isFrameFmtNormalized() const;
// bool		isDeviceFmtNormalized() const;
// bool		isFrameInterleaved() const;
// bool		isDeviceInterleaved() const;
// int		getFrameChannels() const;
// int		getDeviceChannels() const;
// double	getSamplingRate() const;
// long		getFrameCount() const;			-- number of frames rec'd or played


// This struct allows us to hide all device implementation details within
// this source file.

struct ExampleAudioDevice::Impl {
	// Put all class-specific state in here.  You can also add a constructor
	// to make sure all state is initialized, or do it by hand in the main
	// class constructor directly below.  Ditto with destructor.
};


ExampleAudioDevice::ExampleAudioDevice() : _impl(new Impl)
{
	// Initialize any Impl state if not done in Impl::Impl().
}

ExampleAudioDevice::~ExampleAudioDevice()
{ 
	// Free any Impl state if not done in Impl::~Impl().
	delete _impl;
}

// This is by far the most complex method.  
// run() is called by ThreadedAudioDevice in a newly spawned thread.
// During runCallback(), the application will call sendFrames() or getFrames(),
// or if it is done, the callback returns false, which is our signal to
// finish up.

void ExampleAudioDevice::run()
{
	// waitForDevice() waits on the descriptor you passed to setDevice() until
	// the device is ready to give/get audio.  It returns non-zero if 
	// AudioDevice::stop() is called, to allow the loop to exit.
	
    while (waitForDevice(0) == 0) {
        if (runCallback() != true) {
            break;
        }
 	}
	// Do any HW-specific flushing, etc. here.
	
	// Now call the stop callback.
	stopCallback();
}

// doOpen() is called by AudioDevice::open() to do the class-specific opening
// of the audio port, HW, device, etc.  and set up any remaining Impl state.
//
// The assumption is that the open of the HW will return a integer file
// descriptor that we can wait on.  Before exiting this method, call
// setDevice() and hand it that descriptor.  It is used by waitForDevice().
//
// 'mode' has a direction portion and a bit to indicate if the device is being
// run in passive mode (does not spawn its own thread to handle I/O).
// You are guaranteed that doOpen() will NOT be called if you are already open.

int ExampleAudioDevice::doOpen(int mode)
{
	switch (mode & DirectionMask) {
	case Playback:
		break;
	case Record:
		break;
	case RecordPlayback:
		break;
	default:
		error("AudioDevice: Illegal open mode.");
	}
	return error("Not implemented");
}

// doClose() is called by AudioDevice::close() to do the class-specific closing
// of the audio port, HW, device, etc.
// You are guaranteed that doClose() will NOT be called if you are already closed.

int ExampleAudioDevice::doClose()
{
	return error("Not implemented");
}

// doStart() is called by AudioDevice::start() to do class-specific calls which
// notify the HW to begin recording, playing, or both.

int ExampleAudioDevice::doStart()
{
	return error("Not implemented");
}

// This does nothing under RTcmix, so can be left as-is.

int ExampleAudioDevice::doPause(bool)
{
	return error("Not implemented");
}

// doSetFormat() is called by AudioDevice::setFormat() and by AudioDevice::open().
// Here is where you configure your HW, setting it to the format which will
// best handle the format passed in.  Note that it is NOT necessary for the HW
// to match the input format except in sampling rate;  The base class can handle
// most format conversions.
// 'sampfmt' is the format of the data passed to AudioDevice::getFrames() or
//	AudioDevice::sendFrames(), and has three attributes:
//	1) The actual type of the format, retrieved via MUS_GET_FORMAT(sampfmt)
//	2) The interleave (true or false) retrieved via MUS_GET_INTERLEAVE(sampfmt)
//	3) Whether the samples (when float) are normalized, retrieved via
//		MUS_GET_NORMALIZED(sampfmt)
//
// At the end of this method, you must call setDeviceParams() to notify the
// base class what format *you* need the audio data to be in.

int ExampleAudioDevice::doSetFormat(int sampfmt, int chans, double srate)
{
	return error("Not implemented");
}

// doSetQueueSize() is called by AudioDevice::setQueueSize() to allow HW-specific
// configuration of internal audio queue sizes.  The values handed in via
// address represent the size **in frames** of the buffers that will be handed
// to doGetFrames() and/or doSendFrames(), and the number of such buffers the
// application would like to have queued up for robustness.  The actual frame
// count as determined by your HW *must* be reported back to the caller via
// 'pWriteSize'.  If you cannot match *pCount, just do the best you can, but
// do not fail if you cannot match it.

int ExampleAudioDevice::doSetQueueSize(int *pWriteSize, int *pCount)
{
	return error("Not implemented");
}

// doGetFrames() is called by AudioDevice::getFrames() during record.
// The format of 'frameBuffer' will be the format **YOU** specified via
// setDeviceParams() above.  It will be converted into the 'frame format'
// by a base class.  Here is where you fill frameBuffer from your audio HW.

int ExampleAudioDevice::doGetFrames(void *frameBuffer, int frameCount)
{
	return error("Not implemented");
}

// doSendFrames() is called by AudioDevice::sendFrames() during playback.
// The format of 'frameBuffer' will be the format **YOU** specified via
// setDeviceParams() above.  It was converted from the 'frame format'
// by a base class.   Here is where you hand the audio in frameBuffer to you
// HW.

int ExampleAudioDevice::doSendFrames(void *frameBuffer, int frameCount)
{
	return error("Not implemented");
}

// Return true if the passed in device descriptor matches one that this device
// can understand.

bool ExampleAudioDevice::recognize(const char *desc)
{
	return false;
}

// If your audio device(s) needs a string descriptor, it will come in via 'inputDesc'
// and/or 'outputDesc', allowing you to specify different HW for record and play.

AudioDevice *ExampleAudioDevice::create(const char *inputDesc, const char *outputDesc, int mode)
{
	return new ExampleAudioDevice;
}

