/* RTcmix  - Copyright (C) 2005  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>		// for DBL_MAX
#include <assert.h>
#include "tableutils.h"
#include <rtcmix_types.h>
#include <PField.h>
#include <ugens.h>		// for warn, die

#ifndef M_PI
  #define M_PI		3.14159265358979323846
#endif
#ifndef TWOPI
  #define TWOPI	M_PI * 2
#endif


// ---------------------------------------------------------------- is_table ---
// Return true if the PField is a TablePField, else return false.

bool is_table(const PField *pf)
{
	if (pf == NULL)
		return false;
	const double *table = (double *) *pf;
	const int len = pf->values();
	return (table != NULL && len > 0);
}


// -------------------------------------------------------- get_table_bounds ---
// Update the <min> and <max> references with the lower and upper bounds of
// the given <array> of length <len>.

void get_table_bounds(const double *array, const int len,
	double &min, double &max)
{
	min = DBL_MAX;
	max = -DBL_MAX;
	for (int i = 0; i < len; i++) {
		if (array[i] < min)
			min = array[i];
		if (array[i] > max)
			max = array[i];
	}
}


// ------------------------------------------------------ fill_linebrk_table ---
// Given a breakpoint specification, fill the table <array> of length <len>
// with a series of straight line segments.

void fill_linebrk_table(const Arg args[], const int nargs, double *array,
	const int len)
{
	double amp2 = args[0];
	int i = 0;
	for (int k = 1; k < nargs; k += 2) {
		const double amp1 = amp2;
		amp2 = args[k + 1];
		const int j = i + 1;
		i = j + (int) args[k] - 1;
		for (int l = j; l <= i; l++) {
			if (l <= len) {
				array[l - 1] = amp1
									+ (amp2 - amp1) * (double) (l - j) / (i - j + 1);
			}
			else {
				rtcmix_warn("maketable (linebrk)", "The number of points requested "
						"exceeds the table size...ignoring the excess.");
				k = nargs; // force the loop to exit
				break;
			}
		}
	}

	// fill the rest of the array with the last value if all locs weren't used
	if (i < len)
		for (int m = i; m < len; m++)
			array[m] = args[nargs - 1];

}


// --------------------------------------------------------- fill_wave_table ---
// Given a specification of a waveform comprising harmonic partials, fill
// the wavetable <array> of length <len>.

void fill_wave_table(const Arg args[], const int nargs, double *array,
	const int len)
{
	for (int i = 0; i < len; i++)
		array[i] = 0.0;
	int j = nargs;
	while (j--) {
		if ((double) args[j] != 0.0) {
			for (int i = 0; i < len; i++) {
				double val = TWOPI * (double) i / (len / (j + 1));
				array[i] += (sin(val) * (double) args[j]);
			}
		}
	}
}


// --------------------------------------------------- wavetable_from_string ---
// Create an appropriate TablePField from a string description.  Choose either
// ideal waveforms or ones built from the number of partials specified in the
// string.  For example, "square10" builds a square wave with 10 partials.
// The ideal waveforms are good for LFOs, where the ripple in band-limited
// waveforms is not always desirable.   -JGG, 3/17/05

static int wavestring_usage(const char *function)
{
	return die(function, "Invalid waveform string.  Choose from:\n"
				"\"sine\", \"saw\", \"sawX\", \"sawdown\", \"sawup\", "
				"\"square\", \"squareX\", \"tri\", \"triX\", \n"
				"\"buzz\", \"buzzX\", where 'X' is number of partials.");
}

static int string_to_partials(const char *str)
{
	char *pos = NULL;
	long num = strtol(str, &pos, 10);
	if (*pos != 0)
		return -1;
	return (int) num;
}

static const int kMaxPartials = 1000;
static const int kDefaultBuzzPartials = 10;

int wavetable_from_string(
	const char *wavestr,		// string describing waveform
	double *table,				// wavetable to fill
	const int tablesize,		// size of table to fill
	const char *function)	// calling function name used in error messages
{
	if (strncmp(wavestr, "sine", 3) == 0) {
		int numpartials = 1;
		Arg *args = new Arg [numpartials];
		args[0] = 1.0;
		fill_wave_table(args, numpartials, table, tablesize);
		delete [] args;
	}
	else if (strncmp(wavestr, "saw", 3) == 0) {
		if (strlen(wavestr) == 3 || strncmp(wavestr, "sawdown", 4) == 0) {
			int nargs = 3;
			Arg *args = new Arg [nargs];
			args[0] = 1.0;
			args[1] = tablesize;
			args[2] = -1.0;
			fill_linebrk_table(args, nargs, table, tablesize);
			delete [] args;
		}
		else if (strncmp(wavestr, "sawup", 4) == 0) {
			int nargs = 3;
			Arg *args = new Arg [nargs];
			args[0] = -1.0;
			args[1] = tablesize;
			args[2] = 1.0;
			fill_linebrk_table(args, nargs, table, tablesize);
			delete [] args;
		}
		else {		// "sawX"
			const char *p = wavestr + 3;
			int numpartials = string_to_partials(p);
			if (numpartials < 1 || numpartials > kMaxPartials)
				return wavestring_usage(function);
			Arg *args = new Arg [numpartials];
			for (int i = 1; i <= numpartials; i++)
				args[i - 1] = 1.0 / double(i);
			fill_wave_table(args, numpartials, table, tablesize);
			delete [] args;
		}
	}
	else if (strncmp(wavestr, "square", 6) == 0) {
		if (strlen(wavestr) == 6) {		// build ideal square wave
			int nargs = 7;
			Arg *args = new Arg [nargs];
			args[0] = -1.0;
			args[1] = tablesize >> 1;
			args[2] = -1.0;
			args[3] = 0.0;
			args[4] = 1.0;
			args[5] = tablesize >> 1;
			args[6] = 1.0;
			fill_linebrk_table(args, nargs, table, tablesize);
			delete [] args;
		}
		else {	// "squareX"
			const char *p = wavestr + 6;
			int numpartials = string_to_partials(p);
			if (numpartials < 1 || numpartials > kMaxPartials)
				return wavestring_usage(function);
			Arg *args = new Arg [numpartials];
			for (int i = 1; i <= numpartials; i++) {
				if (i % 2)		// odd partials
					args[i - 1] = 1.0 / double(i);
				else
					args[i - 1] = 0.0;
			}
			fill_wave_table(args, numpartials, table, tablesize);
			delete [] args;
		}
	}
	else if (strncmp(wavestr, "tri", 3) == 0) {
		if (strlen(wavestr) == 3) {		// build ideal triangle wave
			int nargs = 5;
			Arg *args = new Arg [nargs];
			args[0] = -1.0;
			args[1] = tablesize >> 1;
			args[2] = 1.0;
			args[3] = tablesize >> 1;
			args[4] = -1.0;
			fill_linebrk_table(args, nargs, table, tablesize);
			delete [] args;
		}
		else {	// "triX"
			const char *p = wavestr + 3;
			int numpartials = string_to_partials(p);
			if (numpartials < 1 || numpartials > kMaxPartials)
				return wavestring_usage(function);
			Arg *args = new Arg [numpartials];
			for (int i = 1; i <= numpartials; i++) {
				if (i % 2)		// odd partials
					args[i - 1] = 1.0 / pow(i, 2.0);
				else
					args[i - 1] = 0.0;
			}
			fill_wave_table(args, numpartials, table, tablesize);
			delete [] args;
		}
	}
	else if (strncmp(wavestr, "buzz", 4) == 0) {
		int numpartials = kDefaultBuzzPartials;
		if (strlen(wavestr) > 4) {		// "buzzX"
			const char *p = wavestr + 4;
			numpartials = string_to_partials(p);
			if (numpartials < 1 || numpartials > kMaxPartials)
				return wavestring_usage(function);
		}
		Arg *args = new Arg [numpartials];
		for (int i = 1; i <= numpartials; i++)
			args[i - 1] = 1.0;
		fill_wave_table(args, numpartials, table, tablesize);
		delete [] args;
	}
	else
		return wavestring_usage(function);

	return 0;
}

