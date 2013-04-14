/* RTcmix  - Copyright (C) 2004  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <rtcmix_types.h>
#include <PField.h>
#include <utils.h>   // in ../../rtcmix
#include <RTInletPField.h>
#include <ugens.h>		// for warn, die


// ------------------------------------------------------ max/msp_connection ---
//
// BGG mm
//
// use this to connect inlets from the rtcmix~ object to PFields
//
//	create_handle calls create_pfield:
//
//    inlet = create_pfield(inlet_no, default);
//
//		<inlet_no>		the max object inlet number associated with this handle
//    <default>      default value
//
//		score to use this might be:
//
//		value = makeconnection("inlet", 1, 0.5)
//		INSTRUMENT(p1, p2, value, p3)
//

static RTNumberPField *
_inlet_usage()
{
	die("makeconnection (inlet)",
		"Usage: makeconnection(\"inlet\", inlet #, default)");
	return NULL;
}

static RTNumberPField *
create_pfield(const Arg args[], const int nargs)
{
	int inletval;
	double defaultval;

	if (nargs < 2)
		return _inlet_usage();

	if (args[0].isType(DoubleType))
		inletval = (int)(args[0]);
	else
		return _inlet_usage();

	if (args[1].isType(DoubleType))
		defaultval = args[1];
	else
		return _inlet_usage();

	return new RTInletPField(inletval, defaultval);
}

// The following functions are the publically-visible ones called by the
// system.

extern "C" {
	RtcHandle create_handle(const Arg args[], const int nargs);
};

RtcHandle
create_handle(const Arg args[], const int nargs)
{
	PField *pField = create_pfield(args, nargs);
	RtcHandle handle = NULL;
	if (pField != NULL) {
		handle = createPFieldHandle(pField);
	}
	return handle;
}
