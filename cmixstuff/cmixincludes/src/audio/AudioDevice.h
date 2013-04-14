// AudioDevice.h -- abstract base class for system-dependent audio

#ifndef _AUDIODEVICE_H_
#define _AUDIODEVICE_H_

#define ZERO_FRAMES_BEFORE 8192
#define ZERO_FRAMES_AFTER  0

class AudioDevice {
protected:
	class Callback;

public:
	typedef bool (*CallbackFun)(AudioDevice *, void *context);
	enum {
		Unset 			= 0x0,
		Record 			= 0x1,
		Playback 		= 0x2,
		RecordPlayback	= 0x3,
		DirectionMask 	= 0xf,
		Passive 		= 0x10,
		CheckPeaks 		= 0x100,
		ReportClipping 	= 0x200
	};
public:
	virtual				~AudioDevice();
	
	// Set the format of the frames to be handed to the device.
	virtual int			setFrameFormat(int sampfmt, int chans) = 0;
	// Open the HW in mode 'mode' with suggested audio format
	virtual int			open(int mode, int sampfmt, int chans, double sr) = 0;
	virtual int			close() = 0;
	// These are the only non-virtual methods, just wrappers for the
	// next calls
	int					start(CallbackFun runCallback, void *context);
	int					setStopCallback(CallbackFun stopCallback, void *callbackContext);
	virtual int			start(Callback *runCallback) = 0;
	virtual int			setStopCallback(Callback *stopCallback) = 0;
	virtual int			pause(bool) = 0;
	virtual int			stop() = 0;
	virtual int			setFormat(int fmt, int chans, double srate) = 0;
	// Returns actual size via pFrames.
	virtual int			setQueueSize(int *pWriteSize, int *pCount) = 0;
	virtual int			getFrames(void *frameBuffer, int frameCount) = 0;
	virtual int			sendFrames(void *frameBuffer, int frameCount) = 0;
	virtual bool		isOpen() const = 0;
	virtual bool		isRunning() const = 0;
	virtual bool		isPaused() const = 0;
	virtual double		getPeak(int chan, long *loc) const =0;
	virtual	const char *getLastError() const = 0;

protected:
	class Callback {
	public:
		Callback(AudioDevice *dev, CallbackFun fun, void *context);
		bool call();
	private:
		AudioDevice *_device;
		CallbackFun _callbackFun;
		void		*_context;
	};

	virtual bool		runCallback() = 0;
	virtual bool		stopCallback() = 0;

protected:
	// For use by all
	enum State {
		Closed = 0,
		Open = 1,
		Configured = 2,
		Running = 3,
		Paused = 4
	};
};

#endif	// _AUDIODEVICE_H_
