/* RTcmix - Copyright (C) 2004  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/
// RTcmixMouse.h - Abstract base class for system-dependent mouse input.
// by John Gibson, based on Doug Scott's audio device code design.
#ifndef _RTCMIXMOUSE_H_
#define _RTCMIXMOUSE_H_

#include <RTcmixWindow.h>
#include <labels.h>

class RTcmixMouse : public RTcmixWindow {
public:
	RTcmixMouse();

	// Deliver coordinate(s) in range [0, 1].  If coord. is negative, it means
	// that the coord. is invalid, and the caller should use a default instead.
	virtual double getPositionX() const = 0;
	virtual double getPositionY() const = 0;

	// Client PFields can have a label printed when their value changes.  Labels
	// have the format: "prefix: value units", where value is a formatted double
	// and units is optional.  Example: "cutoff: 2000.0 Hz"
	//
	// Copy the <prefix> and <units> strings into the RTcmixMouse subclass, and 
	// return an id for the caller to use when reporting its current values
	// during run.  If there is no more label space, return -1.  If caller
	// doesn't want a units string, pass NULL for <units>.  The <precision>
	// argument gives the number of digits after the decimal point to display.
	int configureXLabel(const char *prefix, const char *units,
                                                   const int precision);
	int configureYLabel(const char *prefix, const char *units,
                                                   const int precision);

	// Update the value used in the label.  Note: only the client PField knows
	// how the mouse coords, given to it in range [0,1], will be scaled.
	void updateXLabelValue(const int id, const double value);
	void updateYLabelValue(const int id, const double value);


protected:
	virtual ~RTcmixMouse();
	// Display the mouse window on the screen.
	virtual int show() = 0;

	virtual void doConfigureXLabel(const int id, const char *prefix,
                                 const char *units, const int precision) = 0;
	virtual void doConfigureYLabel(const int id, const char *prefix,
                                 const char *units, const int precision) = 0;
	virtual void doUpdateXLabelValue(const int id, const double value) = 0;
	virtual void doUpdateYLabelValue(const int id, const double value) = 0;
	virtual bool handleEvents() = 0;

	int _xlabelCount;
	int _ylabelCount;
	char *_xlabel[kNumLabels];
	char *_ylabel[kNumLabels];
	char *_xprefix[kNumLabels];
	char *_yprefix[kNumLabels];
	char *_xunits[kNumLabels];
	char *_yunits[kNumLabels];
	int _xprecision[kNumLabels];
	int _yprecision[kNumLabels];

private:
	double _lastx[kNumLabels];
	double _lasty[kNumLabels];
	int _xthrottleCount[kNumLabels];
	int _ythrottleCount[kNumLabels];
	int _throttle;
};

RTcmixMouse *createMouseWindow();

#endif // _RTCMIXMOUSE_H_
