/* RTcmix - Copyright (C) 2004  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/
#include <RTcmixMouse.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

const int kSleepMsec = 10;		// How long to nap between polling of events
const int kThrottle = 50;		// How many intervening values to skip drawing

RTcmixMouse::RTcmixMouse() 
	: RTcmixWindow(kSleepMsec), _xlabelCount(0), _ylabelCount(0)
{
	for (int i = 0; i < kNumLabels; i++) {
		_xprefix[i] = NULL;
		_yprefix[i] = NULL;
		_xunits[i] = NULL;
		_yunits[i] = NULL;
		_xlabel[i] = NULL;
		_ylabel[i] = NULL;
		_lastx[i] = -1.0;
		_lasty[i] = -1.0;
		_xthrottleCount[i] = 0;
		_ythrottleCount[i] = 0;
	}
	_throttle = kThrottle;
}

RTcmixMouse::~RTcmixMouse()
{
	for (int i = 0; i < kNumLabels; i++) {
		free(_xprefix[i]);
		free(_yprefix[i]);
		free(_xunits[i]);
		free(_yunits[i]);
		delete [] _xlabel[i];
		delete [] _ylabel[i];
	}
}

int RTcmixMouse::configureXLabel(const char *prefix, const char *units,
		const int precision)
{
	assert(prefix != NULL);
	if (_xlabelCount == kNumLabels)
		return -1;
	const int id = _xlabelCount;
	_xlabelCount++;

	doConfigureXLabel(id, prefix, units, precision);

	return id;
}

int RTcmixMouse::configureYLabel(const char *prefix, const char *units,
		const int precision)
{
	assert(prefix != NULL);
	if (_ylabelCount == kNumLabels)
		return -1;
	const int id = _ylabelCount;
	_ylabelCount++;

	doConfigureYLabel(id, prefix, units, precision);

	return id;
}

void RTcmixMouse::updateXLabelValue(const int id, const double value)
{
	if (id < 0)							// this is valid if caller wants no label
		return;
	assert(id < _xlabelCount);

	if (--_xthrottleCount[id] < 0) {
		if (value == _lastx[id])
			return;
		doUpdateXLabelValue(id, value);
		_lastx[id] = value;
		_xthrottleCount[id] = _throttle;
	}
}

void RTcmixMouse::updateYLabelValue(const int id, const double value)
{
	if (id < 0)							// this is valid if caller wants no label
		return;
	assert(id < _ylabelCount);

	if (--_ythrottleCount[id] < 0) {
		if (value == _lasty[id])
			return;
		doUpdateYLabelValue(id, value);
		_lasty[id] = value;
		_ythrottleCount[id] = _throttle;
	}
}

#ifdef MACOSX
	#include <OSXMouse.h>
#else
	#include <XMouse.h>
#endif

RTcmixMouse *createMouseWindow()
{
#ifdef MACOSX
	RTcmixMouse *mousewin = new OSXMouse();
#else
	RTcmixMouse *mousewin = new XMouse();
#endif
	if (mousewin->run() != 0) {
		mousewin->ref();
		mousewin->unref();
		mousewin = NULL;
	}

	return mousewin;
}

