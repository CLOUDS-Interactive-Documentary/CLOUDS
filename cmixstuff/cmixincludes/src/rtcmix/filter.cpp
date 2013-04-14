/* RTcmix  - Copyright (C) 2004  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/
#include <stdlib.h>
#include <string.h>
#include "rtcmix_types.h"
#include "tableutils.h"
#include <PField.h>
#include "utils.h"
#include <ugens.h>		// for warn, die

// Functions for creating signal conditioning wrapper PFields.
// -John Gibson, 11/25/04

extern int resetval;		// declared in src/rtcmix/minc_functions.c

#define CLIP_USAGE "filt = makefilter(pfield, \"clip\", min[, max])"
#define CONSTRAIN_USAGE "filt = makefilter(pfield, \"constrain\", table, strength)"
#define DELAY_USAGE "filt = makefilter(pfield, \"delay\", time)"
#define FITRANGE_USAGE "filt = makefilter(pfield, \"fitrange\", min, max [, \"bipolar\"])"
#define INVERT_USAGE "filt = makefilter(pfield, \"invert\" [, center])"
#define MAP_USAGE "filt = makefilter(pfield, \"map\", transferTable[, inputMin, inputMax])"
#define QUANTIZE_USAGE "filt = makefilter(pfield, \"quantize\", quantum)"
#define SMOOTH_USAGE "filt = makefilter(pfield, \"smooth\", lag [, initval])"


// =============================================================================
// local utilities

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

static PField *_filter_usage(const char *msg)
{
	die(NULL, "Usage: %s", msg);
	return NULL;
}


// =============================================================================
// Set up the various filter types

static PField *
_clip_filter(PField *innerpf, const Arg args[], const int nargs)
{
	if (nargs < 1)
		return _filter_usage(CLIP_USAGE);
	PField *minpf = _get_pfield(args[0]);
	PField *maxpf = NULL;
	if (nargs > 1)
		maxpf = _get_pfield(args[1]);
	// NB: It's okay for max PField to be NULL.
	return new ClipPField(innerpf, minpf, maxpf);
}

static PField *
_constrain_filter(PField *innerpf, const Arg args[], const int nargs)
{
	if (nargs < 2)
		return _filter_usage(CONSTRAIN_USAGE);
	PField *tablepf = _get_pfield(args[0]);
	if (!is_table(tablepf))
		return _filter_usage(CONSTRAIN_USAGE);
	PField *strengthpf = _get_pfield(args[1]);
	if (strengthpf == NULL)
		return _filter_usage(CONSTRAIN_USAGE);
	return new ConstrainPField(innerpf, (TablePField *) tablepf, strengthpf);
}

static PField *
_delay_filter(PField *innerpf, const Arg args[], const int nargs)
{
	if (nargs < 1)
		return _filter_usage(DELAY_USAGE);
	PField *deltimepf = _get_pfield(args[0]);
	if (deltimepf == NULL)
		return _filter_usage(DELAY_USAGE);
	return new DelayPField(innerpf, resetval, deltimepf);
}

static PField *
_fitrange_filter(PField *innerpf, const Arg args[], const int nargs)
{
	if (nargs < 2)
		return _filter_usage(FITRANGE_USAGE);
	PField *minpf = _get_pfield(args[0]);
	PField *maxpf = _get_pfield(args[1]);
	if (minpf == NULL || maxpf == NULL)
		return _filter_usage(FITRANGE_USAGE);
	if (nargs > 2 && args[2] == "bipolar")
		return new RangePField(innerpf, minpf, maxpf, RangePField::BipolarSource);
	return new RangePField(innerpf, minpf, maxpf);
}

// Invert <innerpf> values around a center of symmetry given by arg[0].
// If this arg is missing, assume 0.5, unless <innerpf> is a table -- in 
// which case compute the center.

static PField *
_invert_filter(PField *innerpf, const Arg args[], const int nargs)
{
	PField *centerpf = NULL;
	if (nargs > 0) {
		centerpf = _get_pfield(args[0]);
		if (centerpf == NULL)
			return _filter_usage(INVERT_USAGE);
	}
	else {
		double center = 0.5;
		if (is_table(innerpf)) {	// find center of symmetry
			const double *array = (double *) *innerpf;
			const int len = innerpf->values();
			double min, max;
			get_table_bounds(array, len, min, max);
			center = min + ((max - min) / 2.0);
		}
		centerpf = new ConstPField(center);
	}
	return new InvertPField(innerpf, centerpf);
}

static PField *
_map_filter(PField *innerpf, const Arg args[], const int nargs)
{
	if (nargs < 1)
		return _filter_usage(MAP_USAGE);
	PField *xferfunc = _get_pfield(args[0]);
	if (!is_table(xferfunc))
		return _filter_usage(MAP_USAGE);
	double min = nargs > 1 ? (double) args[1] : 0.0;
	double max = nargs > 2 ? (double) args[2] : 1.0;
	return new MapPField(innerpf, (TablePField *) xferfunc, min, max);
}

static PField *
_quantize_filter(PField *innerpf, const Arg args[], const int nargs)
{
	if (nargs < 1)
		return _filter_usage(QUANTIZE_USAGE);
	PField *quantumpf = _get_pfield(args[0]);
	if (quantumpf == NULL)
		return _filter_usage(QUANTIZE_USAGE);
	return new QuantizePField(innerpf, quantumpf);
}

static PField *
_smooth_filter(PField *innerpf, const Arg args[], const int nargs)
{
	if (nargs < 1)
		return _filter_usage(SMOOTH_USAGE);
	PField *lagpf = _get_pfield(args[0]);
	if (lagpf == NULL)
		return _filter_usage(SMOOTH_USAGE);
	double initval = nargs > 1 ? (double) args[1] : 0.0;
	return new SmoothPField(innerpf, resetval, lagpf, initval);
}


// =============================================================================
// The remaining functions are public, callable from scripts.

extern "C" {
	RtcHandle makefilter(const Arg args[], const int nargs);
}


// -------------------------------------------------------------- makefilter ---
static RtcHandle _makefilter_usage()
{
	die("makefilter",
		"\n   usage: " CLIP_USAGE
		"\nOR"
		"\n   usage: " CONSTRAIN_USAGE
		"\nOR"
		"\n   usage: " DELAY_USAGE
		"\nOR"
		"\n   usage: " FITRANGE_USAGE
		"\nOR"
		"\n   usage: " INVERT_USAGE
		"\nOR"
		"\n   usage: " MAP_USAGE
		"\nOR"
		"\n   usage: " QUANTIZE_USAGE
		"\nOR"
		"\n   usage: " SMOOTH_USAGE
		"\n");
	return NULL;
}

RtcHandle makefilter(const Arg args[], const int nargs)
{
	if (nargs < 2)
		return _makefilter_usage();

	PField *innerpf = (PField *) args[0];
	if (innerpf == NULL)
		return _makefilter_usage();

	PField *filt = NULL;
	if (args[1].isType(StringType)) {
		if (args[1] == "clip")
			filt = _clip_filter(innerpf, &args[2], nargs - 2);
		else if (args[1] == "constrain")
			filt = _constrain_filter(innerpf, &args[2], nargs - 2);
		else if (args[1] == "delay")
			filt = _delay_filter(innerpf, &args[2], nargs - 2);
		else if (args[1] == "fitrange")
			filt = _fitrange_filter(innerpf, &args[2], nargs - 2);
		else if (args[1] == "invert")
			filt = _invert_filter(innerpf, &args[2], nargs - 2);
		else if (args[1] == "map")
			filt = _map_filter(innerpf, &args[2], nargs - 2);
		else if (args[1] == "quantize")
			filt = _quantize_filter(innerpf, &args[2], nargs - 2);
		else if (args[1] == "smooth")
			filt = _smooth_filter(innerpf, &args[2], nargs - 2);
		else {
			die("makefilter", "Unsupported filter type \"%s\".",
								(const char *) args[1]);
			return NULL;
		}
	}
	else
		return _makefilter_usage();
	return (filt == NULL) ? NULL : createPFieldHandle(filt);
}

