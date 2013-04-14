/* RTcmix  - Copyright (C) 2004  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <rtcmix_types.h>
#include <PField.h>
#include <utils.h>	// in ../../rtcmix
#include <RTMousePField.h>
#include <RTcmixMouse.h>
#include <ugens.h>		// for warn, die


// ------------------------------------------------------- _mouse_connection ---
//
//    mouse = makeconnection("mouse", "x", min, max, default, lag,
//                                 [prefix[, units,]] [precision])
//
//    First argument is either "x" or "y", depending on which axis
//    you want to read.  Other arguments:
//
//    <min>          minimum value [number]
//    <max>          maximum value [number]
//    <default>      value returned before mouse first enters window [number]
//    <lag>          amount of smoothing for value stream [percent: 0-100]
//    <prefix>       label to display in window [string]
//    <units>        units (e.g., "Hz") to display in window [string]
//    <precision>    digits after decimal point to display in window [number]
//
//    Making <min> greater than <max> lets you invert values.
//
//    If <prefix> is missing or an empty string, then no label printed in
//    mouse window.
//                                                               JGG, 8/14/04

// Note that the first argument passed to this routine is the "X" / "Y" selector
// but that the score command starts with "mouse".

static RTNumberPField *
_mouse_usage()
{
	die("makeconnection (mouse)",
		"Usage: makeconnection(\"mouse\", \"x\" or \"y\", min, max, default, "
		"lag, [prefix[, units,]] [precision])");
	return NULL;
}

static RTNumberPField *
create_pfield(const Arg args[], const int nargs)
{
	if (nargs < 5)
		return _mouse_usage();

	RTMouseAxis axis;
	const char *str = args[0];
	if (str[0] == 'x' || str[0] == 'X')
		axis = kRTMouseAxisX;
	else if (str[0] == 'y' || str[0] == 'Y')
		axis = kRTMouseAxisY;
	else
		return _mouse_usage();

	double minval, maxval, defaultval, lag;
	const char *prefix = NULL, *units = NULL;
	int precision = 3;

	if (args[1].isType(DoubleType))
		minval = args[1];
	else
		return _mouse_usage();

	if (args[2].isType(DoubleType))
		maxval = args[2];
	else
		return _mouse_usage();

	if (args[3].isType(DoubleType))
		defaultval = args[3];
	else
		return _mouse_usage();

	if (args[4].isType(DoubleType))
		lag = args[4];
	else
		return _mouse_usage();
	if (lag < 0.0 || lag > 100.0) {
		die("makeconnection (mouse)", "<lag> must be between 0 and 100");
		return NULL;
	}
	lag *= 0.01;

	// RtcHandle the optional arguments.
	// <prefix> and <units> strings must appear in that order.  <precision>
	// follows either or both of the strings, or replaces both.  (I.e., it
	// appears after of a list of 0, 1, or 2 strings.)
	if (nargs > 5) {
		if (args[5].isType(StringType)) {
			prefix = args[5];
			if (nargs > 6) {
				if (args[6].isType(StringType)) {
					units = args[6];
					if (nargs > 7) {
						if (args[7].isType(DoubleType))
							precision = (int) args[7];
						else
							return _mouse_usage();
					}
				}
				else
					precision = (int) args[6];
			}
		}
		else
			precision = (int) args[5];
	}

	static RTcmixMouse *mousewin = NULL;
	if (mousewin == NULL)					// first time, so make window
		mousewin = createMouseWindow();
	if (mousewin == NULL) {
		die("makeconnection (mouse)", "Failed to create mouse window");
		return NULL;
	}

	return new RTMousePField(mousewin, axis, minval, maxval, defaultval, lag,
										prefix, units, precision);
}

// The following functions are the publically-visible ones called by the
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
