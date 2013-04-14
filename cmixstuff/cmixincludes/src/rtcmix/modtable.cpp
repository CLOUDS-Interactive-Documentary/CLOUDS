/* RTcmix  - Copyright (C) 2005  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <rtcmix_types.h>
#include "tableutils.h"
#include <PField.h>
#include "utils.h"
#include <ugens.h>		// for warn, die

// Functions for modifying table PFields.   -JGG, 4/8/05

#define CONCAT_USAGE "table = modtable(table, \"concat\", table2)"
#define DRAW_USAGE "table = modtable(table, \"draw\", [\"literal\",] index, value[, width])"
#define NORMALIZE_USAGE "table = modtable(table, \"normalize\" [, peak])"
#define REVERSE_USAGE "table = modtable(table, \"reverse\")"
#define SHIFT_USAGE "table = modtable(table, \"shift\", shift)"


// =============================================================================
// local utilities

// FIXME: This is copied from filter.cpp; should put in utils.cpp?

// Given a valid argument (one appearing in the caller's Arg array with an
// index lower than caller's nargs), return the argument as a PField, creating
// one if necessary for a constant double argument.  Return NULL if arg is not
// already a PField or a constant double.

static PField *_get_pfield(const Arg& arg)
{
	PField *pf = (PField *) arg;
	if (pf == NULL && arg.isType(DoubleType))
		pf = new ConstPField((double) arg);
	return pf;
}

static PField *_modtable_usage(const char *msg)
{
	die(NULL, "Usage: %s", msg);
	return NULL;
}

static RtcHandle _modtable_usage()
{
	die("modtable",
		"\n   usage: " CONCAT_USAGE
		"\nOR"
		"\n   usage: " DRAW_USAGE
		"\nOR"
		"\n   usage: " NORMALIZE_USAGE
		"\nOR"
		"\n   usage: " REVERSE_USAGE
		"\nOR"
		"\n   usage: " SHIFT_USAGE
		"\n");
	return NULL;
}


// =============================================================================
// Set up the various table modifications.

// Concatenate the two tables.

static PField *
_concat_table(PField *intable, const Arg args[], const int nargs)
{
	if (nargs < 1)
		return _modtable_usage(CONCAT_USAGE);
	PField *table2 = (PField *) args[0];
	if (table2 == NULL || !is_table(table2))
		return _modtable_usage(CONCAT_USAGE);
	return new ConcatTablePField(intable, table2);
}


// NOTE NOTE NOTE - This modtable variant is an experimental feature!
//
// Rewrite a portion of the table, possibly while an instrument is accessing it.
//
//    table = modtable(table, "draw", ["literal",] index, value[, width])
//
// <table> is a table already created using maketable
// <index> is the central table index to affect.  Normally this is 0-1, acting
//    as a percentage of the table, but with the optional "literal" tag, <index>
//    is interpreted as a literal table index (from 0 to the table length - 1).
// <value> is the new value to place at <index>
// <width> indicates how many neighboring table slots are affected by the 
//    central value change.  Normally, this is a percentage (0 to 1) of the
//    slots, but with the "literal" tag, <width> is interpreted as the number
//    of slots affected.
// Both <index> and <value> can be dynamic control sources.
//
// -JGG, 6/18/05

static PField *
_draw_table(PField *intable, const Arg args[], const int nargs)
{
	if (nargs < 2)
		return _modtable_usage(DRAW_USAGE);
	int argoffset = 0;
	bool literalIndex = false;
	if (args[0].isType(StringType)) {
		if (args[0] == "literal")
			literalIndex = true;
		else
			return _modtable_usage(DRAW_USAGE);
		argoffset = 1;
	}
	PField *indexpf = _get_pfield(args[argoffset]);
	PField *valuepf = _get_pfield(args[argoffset + 1]);
	PField *widthpf = (nargs > argoffset + 2) ? _get_pfield(args[argoffset + 2])
	                                          : new ConstPField(0);
	return new DrawTablePField(intable, literalIndex, indexpf, valuepf, widthpf);
}


// Rescale the values in table so that the maximum absolute value is <peak>.
// If no <peak>, peak is 1.  Result depends on the sign of values in the table.
//
//    sign of values          resulting range of values
//    --------------------------------------------------------------
//    all positive            between 0 and peak
//    all negative            between 0 and -peak
//    positive and negative   between -peak and peak

static PField *
_normalize_table(PField *intable, const Arg args[], const int nargs)
{
	double peak = 1.0;
	if (nargs > 0)
		peak = (double) args[0];
	if (peak == 0.0)
		peak = 0.00001;
	const int len = intable->values();
	const double *array = (double *) *intable;
	double max = 0.0;
	for (int i = 0; i < len; i++) {
		double absval = fabs(array[i]);
		if (absval > max)
			max = absval;
	}
	max /= peak;
	if (max == 0.0)
		return intable;
	PField *factorpf = new ConstPField(1.0 / max);
	return new MultPField(intable, factorpf);
}


// Read the values of the table backward.

static PField *
_reverse_table(PField *intable, const Arg args[], const int nargs)
{
	return new ReversePField(intable);
}


// Shift values of the table by <shift> array locations.  Positive values of
// <shift> shift to the right; negative values to the left.  If a value is
// shifted off the end of the array in either direction, it reenters the other
// end of the array.  Two examples:
//
//    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]      original table, size = 10
//    [7, 8, 9, 0, 1, 2, 3, 4, 5, 6]      shift = 3
//    [3, 4, 5, 6, 7, 8, 9, 0, 1, 2]      shift = -3
//
// Note that the shifting is actually done on-the-fly by ShiftPField.

static PField *
_shift_table(PField *intable, const Arg args[], const int nargs)
{
	if (nargs < 1)
		return _modtable_usage(SHIFT_USAGE);
	const int shift = (int) args[0];
	return new ShiftPField(intable, shift);
}


// =============================================================================
// The remaining functions are public, callable from scripts.

extern "C" {
	RtcHandle modtable(const Arg args[], const int nargs);
}


// ---------------------------------------------------------------- modtable ---

RtcHandle modtable(const Arg args[], const int nargs)
{
	if (nargs < 2)
		return _modtable_usage();

	PField *intable = (PField *) args[0];
	if (intable == NULL || !is_table(intable))
		return _modtable_usage();

	PField *outtable = NULL;
	if (args[1].isType(StringType)) {
		if (args[1] == "concat")
			outtable = _concat_table(intable, &args[2], nargs - 2);
		else if (args[1] == "draw")
			outtable = _draw_table(intable, &args[2], nargs - 2);
		else if (args[1] == "normalize")
			outtable = _normalize_table(intable, &args[2], nargs - 2);
		else if (args[1] == "reverse")
			outtable = _reverse_table(intable, &args[2], nargs - 2);
		else if (args[1] == "shift")
			outtable = _shift_table(intable, &args[2], nargs - 2);
		else
			return _modtable_usage();
	}
	else
		return _modtable_usage();
	return (outtable == NULL) ? NULL : createPFieldHandle(outtable);
}

