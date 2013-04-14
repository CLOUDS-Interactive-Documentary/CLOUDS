/* RTcmix - Copyright (C) 2005  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/

#include <DisplayPField.h>
#include <RTcmixDisplay.h>
#include <PField.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>

DisplayPField::DisplayPField(
			PField			*innerPField,
			RTcmixDisplay	*displaywin,
			const char		*prefix,
			const char		*units,
			const int		precision)
	: PFieldWrapper(innerPField), _len(innerPField->values()),
	  _displaywin(displaywin), _labelID(-1)
{
	assert(_displaywin != NULL);
	_displaywin->ref();

	if (prefix && prefix[0]) {	// no label if null or empty prefix string
		_labelID = _displaywin->configureLabel(prefix, units, precision);
		if (_labelID == -1)
			fprintf(stderr, "WARNING: Max. number of labels already in use.\n");
	}
}

DisplayPField::~DisplayPField()
{
	_displaywin->unref();
}

double DisplayPField::doubleValue(double didx) const
{
	const double val = field()->doubleValue(didx);
	_displaywin->updateLabelValue(_labelID, val);
	return val;
}

double DisplayPField::doubleValue(int idx) const
{
	const double val = field()->doubleValue(idx);
	_displaywin->updateLabelValue(_labelID, val);
	return val;
}

