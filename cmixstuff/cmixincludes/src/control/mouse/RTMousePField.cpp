/* RTcmix - Copyright (C) 2004  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/

#include <RTMousePField.h>
#include <RTcmixMouse.h>
#include <PField.h>
#include <Ougens.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>

extern int resetval;		// declared in src/rtcmix/minc_functions.c


RTMousePField::RTMousePField(
			RTcmixMouse			*mousewin,
			const RTMouseAxis	axis,
			const double		minval,
			const double		maxval,
			const double		defaultval,
			const double		lag,				// in range [0, 1]
			const char			*prefix,
			const char			*units,
			const int			precision)
	: RTNumberPField(0),
	  _mousewin(mousewin), _axis(axis), _filter(NULL),
	  _min(minval), _default(defaultval)
{
	assert(_mousewin != NULL);

	_mousewin->ref();
	
	_labelID = -1;
	if (prefix && prefix[0]) {	// no label if null or empty prefix string
		if (_axis == kRTMouseAxisX)
			_labelID = _mousewin->configureXLabel(prefix, units, precision);
		else
			_labelID = _mousewin->configureYLabel(prefix, units, precision);
		if (_labelID == -1)
			fprintf(stderr, "WARNING: Max. number of labels already in use.\n");
	}

	_diff = maxval - minval;

	// NOTE: We rely on the control rate in effect when this PField is created.
	_filter = new Oonepole(resetval);
	_filter->sethist(_default);
	_filter->setlag(lag);
}

RTMousePField::~RTMousePField()
{
	delete _filter;
	_mousewin->unref();
}

double RTMousePField::doubleValue(double dummy) const
{
	double val = (_axis == kRTMouseAxisX) ? computeValueX() : computeValueY();
	return _filter->next(val);
}


double RTMousePField::computeValueX() const
{
	double val;
	double rawval = _mousewin->getPositionX();
	if (rawval < 0.0)
		val = _default;
	else
		val = _min + (_diff * rawval);
	_mousewin->updateXLabelValue(_labelID, val);
	return val;
}

double RTMousePField::computeValueY() const
{
	double val;
	double rawval = _mousewin->getPositionY();
	// NB: roundoff error in getPositionY can cause rawval to be a very
	// small negative number when y-coord is bottom-most of window.
	// A truly invalid value is a much larger negative number.
	if (rawval < -0.000001)
		val = _default;
	else
		val = _min + (_diff * rawval);
	_mousewin->updateYLabelValue(_labelID, val);
	return val;
}
