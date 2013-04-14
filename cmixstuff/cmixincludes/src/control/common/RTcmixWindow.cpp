/* RTcmix - Copyright (C) 2005  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/
#include <RTcmixWindow.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

RTcmixWindow::RTcmixWindow(int sleepTimeInMs)
	: _sleeptime(sleepTimeInMs * 1000), _eventThread(0), _runThread(false)
{
}

RTcmixWindow::~RTcmixWindow()
{
	shutdownEventLoop();
}

int
RTcmixWindow::run()
{
	return show() ? -1 : spawnEventLoop();
}

void *
RTcmixWindow::_eventLoop(void *context)
{
	RTcmixWindow *obj = (RTcmixWindow *) context;
	while (obj->runThread()) {
		if (obj->handleEvents() == false)
			break;
		usleep(obj->getSleepTime());
	}
	return NULL;
}

int
RTcmixWindow::spawnEventLoop()
{
	_runThread = true;
	int retcode = pthread_create(&_eventThread, NULL, _eventLoop, this);
	if (retcode != 0)
		fprintf(stderr, "Error creating window thread (%d).\n", retcode);
	return retcode;
}

void
RTcmixWindow::shutdownEventLoop()
{
	if (_eventThread) {
		_runThread = false;
		pthread_join(_eventThread, NULL);
	}
	_eventThread = 0;
}
