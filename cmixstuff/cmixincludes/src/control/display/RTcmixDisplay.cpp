/* RTcmix - Copyright (C) 2005  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/
#include <RTcmixDisplay.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

const int kSleepMsec = 40;		// How long to nap between polling of events
const int kThrottle = 50;		// How many intervening values to skip drawing

RTcmixDisplay::RTcmixDisplay() : RTcmixWindow(kSleepMsec), _labelCount(0)
{
	for (int i = 0; i < kNumLabels; i++) {
		_prefix[i] = NULL;
		_units[i] = NULL;
		_label[i] = NULL;
		_last[i] = -1.0;
		_throttleCount[i] = 0;
	}
	_throttle = kThrottle;
}

RTcmixDisplay::~RTcmixDisplay()
{
	for (int i = 0; i < kNumLabels; i++) {
		delete [] _prefix[i];
		delete [] _units[i];
		delete [] _label[i];
	}
}

int RTcmixDisplay::configureLabel(const char *prefix, const char *units,
		const int precision)
{
	assert(prefix != NULL);
	if (_labelCount == kNumLabels)
		return -1;
	const int id = _labelCount;
	_labelCount++;

	doConfigureLabel(id, prefix, units, precision);

	return id;
}

void RTcmixDisplay::updateLabelValue(const int id, const double value)
{
	if (id < 0)							// this means user ran out of labels
		return;
	assert(id < _labelCount);

	if (--_throttleCount[id] < 0) {
		if (value == _last[id])
			return;
		doUpdateLabelValue(id, value);
		_last[id] = value;
		_throttleCount[id] = _throttle;
	}
}


#ifdef MACOSX
	#include <OSXDisplay.h>
#else
	#include <XDisplay.h>
#endif

RTcmixDisplay *createDisplayWindow()
{
#ifdef MACOSX
	RTcmixDisplay *displaywin = new OSXDisplay();
#else
	RTcmixDisplay *displaywin = new XDisplay();
#endif
	if (displaywin->run() != 0) {
		displaywin->ref();
		displaywin->unref();
		displaywin = NULL;
	}

	return displaywin;
}

