/* RTcmix  - Copyright (C) 2004  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/
// Functions for creating converter PFields.  This type of PField converts
// values output by another pfield, using standard RTcmix conversion functions
// like ampdb, cpsoct, etc.
//                                                     -John Gibson, 8/22/04

#include <stdlib.h>
#include <stdio.h>
#include "rtcmix_types.h"
#include <PField.h>
#include "utils.h"
#include <ugens.h>		// for warn, die


// =============================================================================
// The remaining functions are public, callable from scripts.

extern "C" {
	RtcHandle makeconverter(const Arg args[], const int nargs);
};


// ----------------------------------------------------------- makeconverter ---
RtcHandle
makeconverter(const Arg args[], const int nargs)
{
	if (!args[0].isType(HandleType)) {
		die("makeconverter", "First argument must be a valid pfield handle.");
		return NULL;
	}
	PField *pfield = (PField *) args[0];

	if (!args[1].isType(StringType)) {
		die("makeconverter", "Second argument must be a string giving "
									"converter type, e.g. \"ampdb\", \"cpsoct\".");
		return NULL;
	}

	ConverterPField *converter = NULL;

	if (args[1] == "ampdb")
		converter = new ConverterPField(pfield, ConverterPField::ampdb);
	else if (args[1] == "cpsoct")
		converter = new ConverterPField(pfield, ConverterPField::cpsoct);
	else if (args[1] == "octcps")
		converter = new ConverterPField(pfield, ConverterPField::octcps);
	else if (args[1] == "octpch")
		converter = new ConverterPField(pfield, ConverterPField::octpch);
	else if (args[1] == "cpspch")
		converter = new ConverterPField(pfield, ConverterPField::cpspch);
	else if (args[1] == "pchoct")
		converter = new ConverterPField(pfield, ConverterPField::pchoct);
	else if (args[1] == "pchcps")
		converter = new ConverterPField(pfield, ConverterPField::pchcps);
	else if (args[1] == "midipch")
		converter = new ConverterPField(pfield, ConverterPField::midipch);
	else if (args[1] == "pchmidi")
		converter = new ConverterPField(pfield, ConverterPField::pchmidi);
	else if (args[1] == "octmidi")
		converter = new ConverterPField(pfield, ConverterPField::octmidi);
	else if (args[1] == "boost")
		converter = new ConverterPField(pfield, ConverterPField::boost);

	if (converter == NULL) {
		die("makeconverter", "Invalid converter type \"%s\".", (const char *) args[1]);
		return NULL;
	}

	return createPFieldHandle(converter);
}

