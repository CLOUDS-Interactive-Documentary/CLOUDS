/* RTcmix - Copyright (C) 2004  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/
#ifndef _PFBUSPFIELD_H_
#define _PFBUSPFIELD_H_

#include <PField.h>

class PFBusPField : public RTNumberPField {
public:
	PFBusPField(
			const int			n_pfbus,
			const double		defaultval);

	virtual double doubleValue(double dummy) const;

protected:
	virtual ~PFBusPField();

private:
	int 				_n_pfbus;

};

#endif // _PFBUSPFIELD_H_
