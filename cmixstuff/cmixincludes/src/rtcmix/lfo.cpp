/* RTcmix  - Copyright (C) 2004  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/
#include <stdlib.h>
#include <string.h>
#include <PField.h>
#include "utils.h"
#include "tableutils.h"
#include <ugens.h>		// for warn, die

// Functions for creating LFO PFields.    -John Gibson, 11/20/04

extern int resetval;		// declared in src/rtcmix/minc_functions.c


// --------------------------------------------------------- local utilities ---


// =============================================================================
// The remaining functions are public, callable from scripts.

extern "C" {
	RtcHandle makeLFO(const Arg args[], const int nargs);
	RtcHandle makerandom(const Arg args[], const int nargs);
}

typedef enum {
	kTruncate,
	kInterp1stOrder
} InterpType;


// ----------------------------------------------------------------- makeLFO ---
static RtcHandle
_makeLFO_usage()
{
	die("makeLFO",
		"\n   usage: lfo = makeLFO(wave, [interp,] freq, amp)"
		"\nOR"
		"\n   usage: lfo = makeLFO(wave, [interp,] freq, min, max)\n");
	return NULL;
}

static const int kLFOWavetableSize = 1000;

RtcHandle
makeLFO(const Arg args[], const int nargs)
{
	if (nargs < 3)
		return _makeLFO_usage();

	TablePField *tablePField = (TablePField *) ((PField *) args[0]);
	if (tablePField == NULL || tablePField->values() < 2) {	// not a TablePField
		if (args[0].isType(StringType)) {
			const int len = kLFOWavetableSize;
			double *wavetable = new double [len];
			if (wavetable_from_string(args[0], wavetable, len, "makeLFO") != 0) {
				delete wavetable;
				return NULL;
			}
			tablePField = new TablePField(wavetable, len);
		}
		else
			return _makeLFO_usage();
	}

	InterpType interp = kInterp1stOrder;
	int index = 1;
	if (args[index].isType(StringType)) {
		if (args[index] == "nointerp")
			interp = kTruncate;
		else if (args[index] == "interp")
			interp = kInterp1stOrder;
		else {
			die("makeLFO", "Invalid string option \"%s\".",
													(const char *) args[index]);
			return NULL;
		}
		index++;
	}

	PField *freqpf = (PField *) args[index];
	if (freqpf == NULL) {
		if (args[index].isType(DoubleType))
			freqpf = new ConstPField((double) args[index]);
		else
			return _makeLFO_usage();
	}
	index++;
	if (index >= nargs)
		return _makeLFO_usage();

	PField *amppf = NULL;
	PField *minpf = NULL;
	PField *maxpf = NULL;
	if (nargs - index > 1) {		// min, max instead of amp
		minpf = (PField *) args[index];
		if (minpf == NULL) {
			if (args[index].isType(DoubleType))
				minpf = new ConstPField((double) args[index]);
			else
				return _makeLFO_usage();
		}
		index++;
		if (index >= nargs)
			return _makeLFO_usage();
		maxpf = (PField *) args[index];
		if (maxpf == NULL) {
			if (args[index].isType(DoubleType))
				maxpf = new ConstPField((double) args[index]);
			else
				return _makeLFO_usage();
		}
	}
	else {
		amppf = (PField *) args[index];
		if (amppf == NULL) {
			if (args[index].isType(DoubleType))
				amppf = new ConstPField((double) args[index]);
			else
				return _makeLFO_usage();
		}
	}

	PField *lfo = NULL;
	if (interp == kInterp1stOrder)
		lfo = new LFOPField(resetval, tablePField, freqpf);
	else // (interp == kTruncate)
		lfo = new LFOPField(resetval, tablePField, freqpf, LFOPField::Truncate);

	if (amppf != NULL)
		lfo = new MultPField(lfo, amppf);
	else
		lfo = new RangePField(lfo, minpf, maxpf, RangePField::BipolarSource);

	return createPFieldHandle(lfo);
}


// -------------------------------------------------------------- makerandom ---
#include <Random.h>
#include <sys/time.h>

static RtcHandle
_makerandom_usage()
{
	die("makerandom",
		"\n   usage: rand = makerandom(type, freq, min, max[, seed])"
		"\n          where <type> is \"linear\", \"low\", \"high\","
		"\n          \"triangle\", \"gaussian\" or \"cauchy\""
		"\nOR"
		"\n   usage: rand = makerandom(\"prob\", freq, min, max, mid, "
						"tight[, seed])"
		"\n");
	return NULL;
}

RtcHandle
makerandom(const Arg args[], const int nargs)
{
	if (nargs < 4)
		return _makerandom_usage();

	int type;
	if (args[0].isType(StringType)) {
		if (args[0] == "even" || args[0] == "linear")
			type = kLinearRandom;
		else if (args[0] == "low")
			type = kLowLinearRandom;
		else if (args[0] == "high")
			type = kHighLinearRandom;
		else if (args[0] == "triangle")
			type = kTriangleRandom;
		else if (args[0] == "gaussian")
			type = kGaussianRandom;
		else if (args[0] == "cauchy")
			type = kCauchyRandom;
		else if (args[0] == "prob")
			type = kProbRandom;
		else
			return _makerandom_usage();
	}
	else
		return _makerandom_usage();

	int index = 1;
	PField *freqpf = (PField *) args[index];
	if (freqpf == NULL) {
		if (args[index].isType(DoubleType))
			freqpf = new ConstPField((double) args[index]);
		else
			return _makerandom_usage();
	}
	index++;

	PField *minpf = (PField *) args[index];
	if (minpf == NULL) {
		if (args[index].isType(DoubleType))
			minpf = new ConstPField((double) args[index]);
		else
			return _makerandom_usage();
	}
	double min = minpf->doubleValue(0);
	index++;
	if (index >= nargs)
		return _makerandom_usage();

	PField *maxpf = (PField *) args[index];
	if (maxpf == NULL) {
		if (args[index].isType(DoubleType))
			maxpf = new ConstPField((double) args[index]);
		else
			return _makerandom_usage();
	}
	double max = maxpf->doubleValue(0);
	index++;

	PField *midpf = NULL;
	if (type == kProbRandom) {
		if (index >= nargs)
			return _makerandom_usage();
		midpf = (PField *) args[index];
		if (midpf == NULL) {
			if (args[index].isType(DoubleType))
				midpf = new ConstPField((double) args[index]);
			else
				return _makerandom_usage();
		}
		index++;
		if (index >= nargs)
			return _makerandom_usage();
	}
	double mid = midpf ? midpf->doubleValue(0) : 0.0;

	PField *tightpf = NULL;
	if (type == kProbRandom) {
		tightpf = (PField *) args[index];
		if (tightpf == NULL) {
			if (args[index].isType(DoubleType))
				tightpf = new ConstPField((double) args[index]);
			else
				return _makerandom_usage();
		}
		index++;
	}
	double tight = tightpf ? tightpf->doubleValue(0) : 0.0;

	int seed = 0;
	if (nargs - index > 0)		// explicit seed
		seed = (int) args[index];
	if (seed == 0) {
		struct timeval tv;
		gettimeofday(&tv, NULL);
		seed = (int) tv.tv_usec;
	}

	// check initial values
	if (min > max) {
		die("makerandom", "<min> must be less than or equal to <max>.");
		return NULL;
	}
	if (tight < 0.0) {
		die("makerandom", "<tight> must be zero or greater.");
		return NULL;
	}

	Random *gen = NULL;
	switch (type) {
		case kLinearRandom:
			gen = new LinearRandom(min, max, seed);
			break;
		case kLowLinearRandom:
			gen = new LowLinearRandom(min, max, seed);
			break;
		case kHighLinearRandom:
			gen = new HighLinearRandom(min, max, seed);
			break;
		case kTriangleRandom:
			gen = new TriangleRandom(min, max, seed);
			break;
		case kGaussianRandom:
			gen = new GaussianRandom(min, max, seed);
			break;
		case kCauchyRandom:
			gen = new CauchyRandom(min, max, seed);
			break;
		case kProbRandom:
			gen = new ProbRandom(min, max, mid, tight, seed);
			break;
		default:		// can't get here if type inited correctly above
			break;
	}

	PField *rand = new RandomPField(resetval, gen, freqpf, minpf, maxpf, midpf, tightpf);

	return createPFieldHandle(rand);
}


