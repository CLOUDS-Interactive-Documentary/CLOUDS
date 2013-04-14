/* RTcmix - Copyright (C) 2004  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
  the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/

#include <RTInletPField.h>
#include <PField.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>

// BGG -- this is set by pfield_set() in main.cpp
//		the index is the inlet, the value is the value
extern float inletvals[];

RTInletPField::RTInletPField(
			const int			n_inlet,
			const double		defaultval)
	: RTNumberPField(0),
	  _n_inlet(n_inlet)
{
	inletvals[n_inlet-1] = defaultval; // inlets are numbered from "1"
}

RTInletPField::~RTInletPField() {}


double RTInletPField::doubleValue(double dummy) const
{
	return inletvals[_n_inlet-1];
}

