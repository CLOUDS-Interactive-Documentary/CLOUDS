/* RTcmix - Copyright (C) 2004  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/
#ifndef _RTINLETPFIELD_H_
#define _RTINLETPFIELD_H_

#include <PField.h>

class RTInletPField : public RTNumberPField {
public:
	RTInletPField(
			const int			n_inlet,
			const double		defaultval);

	virtual double doubleValue(double dummy) const;

protected:
	virtual ~RTInletPField();

private:
	int 				_n_inlet;
};

#endif // _RTINLETPFIELD_H_
