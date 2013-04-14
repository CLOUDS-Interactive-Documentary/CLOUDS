/* RTcmix  - Copyright (C) 2005  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/
#include <stdlib.h>
#include <string.h>
#include <rtcmix_types.h>
#include <PField.h>
#include "DataFile.h"
#include <Option.h>		// for clobber
#include "utils.h"
#include <ugens.h>		// for warn, die

// Functions for creating signal monitoring wrapper PFields.  This type of
// PField passes on the value computed by the inner PField, while sending this
// value somewhere else, e.g. to a display window or data file.
//
// -John Gibson, 4/1/05

extern int resetval;		// declared in src/rtcmix/minc_functions.c

extern "C" {
	RtcHandle makemonitor(const Arg args[], const int nargs);

	// defined in makeconnection.cpp, called below
	RtcHandle makeconnection(const Arg args[], const int nargs);
}


// ------------------------------------------------------------- makemonitor ---

enum {
	kDatafileMonitor,
	kDisplayMonitor
};

static RtcHandle
_makemonitor_usage()
{
	die("makemonitor",
		"\n   usage: pfield = makemonitor(pfield, \"datafile\", filename, [filerate[, format]])"
		"\nOR"
		"\n   usage: pfield = makemonitor(pfield, \"display\"[, prefix[, units,]] [precision])"
		"\n");
	return NULL;
}

RtcHandle
makemonitor(const Arg args[], const int nargs)
{
	if (nargs < 2)
		return _makemonitor_usage();

	PField *innerpf = (PField *) args[0];
	if (innerpf == NULL)
		return _makemonitor_usage();

	int type;
	if (args[1].isType(StringType)) {
		if (args[1] == "datafile")
			type = kDatafileMonitor;
		else if (args[1] == "display")
			type = kDisplayMonitor;
		else {
			die("makemonitor", "Unsupported monitor type \"%s\".",
								(const char *) args[1]);
			return NULL;
		}
	}
	else
		return _makemonitor_usage();

	PField *monitor = NULL;

	if (type == kDatafileMonitor) {
		const char *filename = args[2];
		if (filename == NULL || filename[0] == 0)
			return _makemonitor_usage();

		int filerate = resetval;
		int formatcode = kDataFormatFloat;
		if (nargs > 3) {
			filerate = args[3];
			if (filerate < 1) {
				rtcmix_warn("makemonitor (datafile)", "<filerate> must be greater than "
						"zero...fixing.");
				filerate = resetval / 10;
			}
			else if (filerate > resetval)
				rtcmix_warn("makemonitor (datafile)", "Requesting a data file control rate"
					" that is higher than the synthesis control rate is wasteful.");

			if (nargs > 4) {
				formatcode = DataFile::formatStringToCode(args[4]);
				if (formatcode == -1) {
					rtcmix_warn("makemonitor (datafile)", "Invalid format string.  Valid "
							"strings are:\n"
							"\"double\", \"float\", \"int64\", \"int32\", \"int16\", "
							"\"byte\"");
					formatcode = kDataFormatFloat;
				}
			}
		}

		monitor = new DataFileWriterPField(innerpf, filename, Option::clobber(),
									resetval, filerate, formatcode, false);

	}
	else if (type == kDisplayMonitor) {
		// This doesn't really count as a connection type in the way that MIDI
		// and Mouse input do, because it's not providing any input at all, and
		// it's not invoked using makeconnection.  But we do it this way now to
		// make use of the connection DSO-loading mechanism, so that we don't
		// have to link all this GUI code into RTcmix.  Therefore, all this code
		// does is manipulate the user's argument list in order to let
		// makeconnection do all the real work.
		Arg *newargs = new Arg[nargs];
		newargs[0] = args[1];				// "display"
		newargs[1] = args[0];				// pfield to display
		for (int i = 2; i < nargs; i++)	// optional args
			newargs[i] = args[i];
		RtcHandle handle = makeconnection(newargs, nargs);
		delete [] newargs;
		return handle;		// already have a handle from makeconnection
	}

	return createPFieldHandle(monitor);
}

