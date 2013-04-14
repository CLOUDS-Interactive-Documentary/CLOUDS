// RTcmix  - Copyright (C) 2005  The RTcmix Development Team
// See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
// the license to this software and for a DISCLAIMER OF ALL WARRANTIES.

// glue.cpp - user interface for OSC server, by John Gibson

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <rtcmix_types.h>
#include <Option.h>
#include <PField.h>
#include <utils.h>	// in ../../rtcmix
#include <RTcmixOSC.h>
#include <RTOscPField.h>
#include <ugens.h>		// for warn, die

// -----------------------------------------------------------------------------
//
//   osc = makeconnection("osc", path, index, inmin, inmax, outmin, outmax,
//                        default, lag)
//
//   <path>      OSC path string to match (e.g., "/synth/oscil2/frequency")
//
//   <index>     zero-based index of item in message to read.  (The item will
//               be coerced to a double, unless it's not a number, in which case
//               it won't be read.)
//
//   <inmin,     range of raw data to expect
//    inmax>
//
//   <outmin,    range of numbers to output
//    outmax>
//
//   <default>   number to output before first matching OSC message received
//
//   <lag>       amount of smoothing of output signal (0-100)
//
//
//   Example:
//
//   osc = makeconnection("osc", "/synth/freqs", index=1, inmin=-1, inmax=1,
//                         outmin=100, outmax=1000, default=200, lag=40)
//
//   If the OSC server receives messages with a path of "/synth/freqs",
//   this PField returns the second item of the OSC message (i.e., the one
//   having a zero-based index of 1) as a double, scaled to fit between
//   100 and 1000 inclusive.  Incoming OSC values are expected to fall between
//   -1 and 1.  The PField returns 200 if accessed prior to receiving any
//   matching OSC messages.  The stream of values is smoothed using a lag
//   value of 40.

static RTNumberPField *
_osc_usage()
{
	die("makeconnection (osc)",
		"Usage: makeconnection(\"osc\", path, index, inmin, inmax, "
		                       "outmin, outmax, default, lag)");
	return NULL;
}

static RTNumberPField *
create_pfield(const Arg args[], const int nargs)
{
	if (nargs < 8)
		return _osc_usage();

	const char *path;
	if (args[0].isType(StringType))
		path = args[0];
	else
		return _osc_usage();

	int index;
	if (args[1].isType(DoubleType))
		index = (int) args[1];
	else
		return _osc_usage();
	if (index < 0) {
		die("makeconnection (osc)", "<index> must be 0 or greater");
		return NULL;
	}

	double inmin, inmax, outmin, outmax, defaultval, lag;

	if (args[2].isType(DoubleType))
		inmin = args[2];
	else
		return _osc_usage();

	if (args[3].isType(DoubleType))
		inmax = args[3];
	else
		return _osc_usage();

	if (args[4].isType(DoubleType))
		outmin = args[4];
	else
		return _osc_usage();

	if (args[5].isType(DoubleType))
		outmax = args[5];
	else
		return _osc_usage();

	if (args[6].isType(DoubleType))
		defaultval = args[6];
	else
		return _osc_usage();

	if (args[7].isType(DoubleType))
		lag = args[7];
	else
		return _osc_usage();
	if (lag < 0.0 || lag > 100.0) {
		die("makeconnection (osc)", "<lag> must be between 0 and 100");
		return NULL;
	}
	lag *= 0.01;

	static RTcmixOSC *oscserver = NULL;
	if (oscserver == NULL)				// first time, so init OSC server
		oscserver = createOSCServer();
	if (oscserver == NULL)
		return NULL;

	return new RTOscPField(oscserver, path, index, inmin, inmax, outmin, outmax,
	                       defaultval, lag);
}


// The following functions are the publicly-visible ones called by the
// system.

extern "C" {
	RtcHandle create_handle(const Arg args[], const int nargs);
	int register_dso();
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

int register_dso()
{
	 return 0;
}
