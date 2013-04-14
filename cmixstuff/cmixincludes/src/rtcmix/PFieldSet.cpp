/* RTcmix  - Copyright (C) 2000  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
 */

#include "PField.h"
#include "PFieldSet.h"
#ifndef NULL
#define NULL 0
#endif

PFieldSet::PFieldSet(int numfields)
	: _array(new PField *[numfields]), _size(numfields)
{
	for (int n = 0; n < numfields; ++n)
		_array[n] = NULL;
}

PFieldSet::~PFieldSet()
{
	for (int n = 0; n < _size; ++n)
		RefCounted::unref(_array[n]);
	delete [] _array;
}

void
PFieldSet::load(PField *pf, int index)
{
	if (_array[index] != pf) {
		RefCounted::unref(_array[index]);
		_array[index] = pf;
		pf->ref();
	}
}

