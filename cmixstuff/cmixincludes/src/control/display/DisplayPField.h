/* RTcmix - Copyright (C) 2005  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/
#ifndef _DISPLAYPFIELD_H_
#define _DISPLAYPFIELD_H_

#include <PField.h>

class RTcmixDisplay;

class DisplayPField : public PFieldWrapper {
public:
	DisplayPField(	
			PField			*innerPField,
			RTcmixDisplay	*displaywin,
			const char		*prefix,
			const char		*units,
			const int		precision);

	virtual double	doubleValue(double didx) const;
	virtual double	doubleValue(int idx) const;
	virtual int		values() const { return _len; }

protected:
	virtual ~DisplayPField();

private:
	int				_len;
	RTcmixDisplay	*_displaywin;
	int				_labelID;
};

#endif // _DISPLAYPFIELD_H_
