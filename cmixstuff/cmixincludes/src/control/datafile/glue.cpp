/* RTcmix  - Copyright (C) 2005  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <rtcmix_types.h>
#include <PField.h>
#include <DataFileReaderPField.h>
#include <ugens.h>		// for warn, die
#include <utils.h>

extern int resetval;		// declared in src/rtcmix/minc_functions.c


// -----------------------------------------------------------------------------
//
//    stream = makeconnection("datafile", filename, lag,
//                   [skiptime[, timefactor[, filerate, format, swap]]])
//
//    <filename>     full or relative path to data file [string]
//
//    <lag>          amount of smoothing for value stream [percent: 0-100]
//
//    <skiptime>     time to skip before reading data file, prior to applying
//                   <timefactor>.  This is optional; default is 0.
//
//    <timefactor>   scale time it takes to consume file: 1 means use the same
//                   amount of time it took to create the file; 2 means take
//                   twice as long to play the file data; 0.5 means take half
//                   as long; etc.  This argument is optional; if used, 
//                   <skiptime> must be given.  Default for <timefactor> is 1.0.
//
//    The next three are optional, used only if the data file has no header.
//    If you give any, you must give all three.  We recommend that you give a
//    filerate that's significantly less than the current control rate (maybe
//    1/5 as fast), so as to thin the file data a bit.
//
//    <filerate>     file control rate to assume [int]
//    <format>       "double", "float", "int64", "int32", "int16" or "byte"
//    <swap>         whether to byte-swap data [1: yes, 0: no]
//
//    If the file has no header, and you don't provide these arguments, the
//    code assumes that <filerate> is the same as the current control rate,
//    <format> is "float", and <swap> is zero.
//                                                               JGG, 3/15/05

static RTNumberPField *_datafile_usage()
{
	die("makeconnection (datafile)",
		"Usage: makeconnection(\"datafile\", filename, lag, "
		"[skiptime[, timefactor[, filerate, format, swap]]])");
	return NULL;
}

static RTNumberPField *
create_pfield(const Arg args[], const int nargs)
{
	if (nargs < 2)
		return _datafile_usage();

	const char *filename = args[0];
	if (filename == NULL || filename[0] == 0)
		return _datafile_usage();

	double lag;
	if (args[1].isType(DoubleType))
		lag = args[1];
	else
		return _datafile_usage();
	if (lag < 0.0 || lag > 100.0) {
		die("makeconnection (datafile)", "<lag> must be between 0 and 100.");
		return NULL;
	}
	lag *= 0.01;

	double skiptime = 0.0;
	double timefactor = 1.0;
	int filerate = -1;
	int formatcode = -1;
	bool swap = false;

	// RtcHandle the optional arguments.
	if (nargs > 2) {
		skiptime = args[2];
		if (skiptime < 0.0) {
			die("makeconnection (datafile)",
								"<skiptime> must be zero or greater.");
			return NULL;
		}
		if (nargs > 3) {
			timefactor = args[3];
			if (timefactor <= 0.0) {
				die("makeconnection (datafile)",
									"<timefactor> must be greater than zero.");
				return NULL;
			}
			if (nargs == 7) {
				filerate = (int) args[4];
				formatcode = DataFile::formatStringToCode(args[5]);
				if (formatcode == -1) {
					rtcmix_warn("makeconnection (datafile)", "Invalid format string. "
									"Valid strings are:");
					rtcmix_warn("makeconnection (datafile)", "\"double\", \"float\", "
									"\"int64\", \"int32\", \"int16\", \"byte\"");
					formatcode = kDataFormatFloat;
				}
				swap = (bool) ((int) args[6]);
			}
		}
	}

	DataFileReaderPField *pfield;

	if (formatcode == -1)	// use defaults in the PField
		pfield = new DataFileReaderPField(filename, lag, resetval, skiptime,
                                                              timefactor);
	else
		pfield = new DataFileReaderPField(filename, lag, resetval, skiptime,
                                  timefactor, filerate, formatcode, swap);

	return pfield;
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

