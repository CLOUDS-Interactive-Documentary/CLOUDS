/* RTcmix - Copyright (C) 2004  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/
#ifndef _RTMOUSEPFIELD_H_
#define _RTMOUSEPFIELD_H_

#include <PField.h>

class RTcmixMouse;
class Oonepole;

enum RTMouseAxis {
	kRTMouseAxisX,
	kRTMouseAxisY
};

class RTMousePField : public RTNumberPField {
public:
	RTMousePField(
			RTcmixMouse			*mousewin,
			const RTMouseAxis	axis,
			const double		minval,
			const double		maxval,
			const double		defaultval,
			const double		lag,
			const char			*prefix,
			const char			*units,
			const int			precision);

	virtual double doubleValue(double dummy) const;

protected:
	virtual ~RTMousePField();

private:
	double computeValueX() const;
	double computeValueY() const;

	RTcmixMouse *	_mousewin;
	RTMouseAxis		_axis;
	Oonepole *		_filter;
	double			_min;
	double			_default;

	double			_diff;
	int				_labelID;
};

#endif // _RTMOUSEPFIELD_H_
