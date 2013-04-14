/* RTcmix - Copyright (C) 2004  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
  the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/

#include <PFBusPField.h>
#include <PFBusData.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>


PFBusPField::PFBusPField(
			const int			n_pfbus,
			const double		defaultval)
	: RTNumberPField(0),
	_n_pfbus(n_pfbus)
{
	PFBusData::val[n_pfbus] = defaultval;
	PFBusData::pfbus_is_connected[n_pfbus] = 1;
	PFBusData::dq_now[n_pfbus] = 0;
}

PFBusPField::~PFBusPField() {}

double PFBusPField::doubleValue(double dummy) const
{
	// the 'percent' struct member is set up in the PFSCHED instrument
	// to cover the appropriate duration of the PField

	if ( (PFBusData::drawflag[_n_pfbus] == 1) && (PFBusData::percent[_n_pfbus] < 1.0) )  {
		double pct = PFBusData::percent[_n_pfbus];
		const PField *PF = PFBusData::thepfield[_n_pfbus];

		PFBusData::val[_n_pfbus] = (*PF).doubleValue(pct);
		pct += PFBusData::theincr[_n_pfbus];
		PFBusData::percent[_n_pfbus] = pct;
		if (pct >= 1.0) { // continue to read last value
			PFBusData::val[_n_pfbus] = (*PF).doubleValue(1.0);

			// PField end, and dqflag is set, signal Instrument.cpp to de-queue
			if (PFBusData::dqflag[_n_pfbus] == 1) {
				PFBusData::dq_now[_n_pfbus] = 1;
				PFBusData::drawflag[_n_pfbus] = 0; // set in case other PFSCHEDs
				PFBusData::pfbus_is_connected[_n_pfbus] = 0;
			}
		}
	}
	
	return PFBusData::val[_n_pfbus];
}

