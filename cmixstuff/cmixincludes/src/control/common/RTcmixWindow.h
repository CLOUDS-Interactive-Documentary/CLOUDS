/* RTcmix - Copyright (C) 2004  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/
// RTcmixWindow.h - Abstract base class for input/output display and events.
// by Doug Scott, extracted from John Gibson's code

#ifndef _RTCMIXWINDOW_H_
#define _RTCMIXWINDOW_H_

#include <pthread.h>
#include <RefCounted.h>

class RTcmixWindow : public RefCounted {
public:
	RTcmixWindow(int sleepTimeInMs);
	// Display the window on the screen and run event handler.
	virtual int run();

protected:
	virtual ~RTcmixWindow();
	virtual int show() = 0;
	int spawnEventLoop();
	void shutdownEventLoop();
	virtual bool handleEvents() = 0;
	inline unsigned long getSleepTime() { return _sleeptime; }

private:
	inline bool runThread() { return _runThread; }
	static void *_eventLoop(void *);

private:
	unsigned long _sleeptime;
	pthread_t _eventThread;
	bool _runThread;
};

#endif	// _RTCMIXWINDOW_H_
