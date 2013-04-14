/* RTcmix  - Copyright (C) 2004  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <math.h>
#include <stdint.h>		// for int32_t, etc.
#include <assert.h>
#include <unistd.h>
#include <sys/time.h>
#include <RTcmix.h>
#include <rtcmix_types.h>
#include "prototypes.h"
#include "tableutils.h"
#include "byte_routines.h"
#include <sndlibsupport.h>
#include <PField.h>
#include <Random.h>
#include "utils.h"
#include <ugens.h>		// for warn, die
#include <maxdispargs.h>

// Functions for creating and modifying double arrays.  These can be passed
// from a script to RTcmix functions that can accept them.  Much of this code
// is derived from the old RTcmix makegens.
//
// John Gibson and Doug Scott, May/June 2004

#define MAX_ARRAY_LEN 1000000		// guard against unintentionally huge allocs

#ifndef M_PI
  #define M_PI		3.14159265358979323846
#endif
#ifndef TWOPI
  #define TWOPI	M_PI * 2
#endif


typedef enum {
	InvalidTable = -1,
	TextfileTable = 0,
	SoundFileTable = 1,
	LiteralTable = 2,
	DatafileTable = 3,
	CurveTable = 4,
	ExpbrkTable = 5,
	LineTable = 6,
	LinebrkTable = 7,
	SplineTable = 8,
	Wave3Table = 9,
	WaveTable = 10,
	ChebyTable = 17,
	RandomTable = 20,
	WindowTable = 25,
	LinestepTable = 26,
	EndTable
} TableKind;

static const char *_table_name[] = {
	"textfile",	// 0
	"soundfile",
	"literal",
	"datafile",
	"curve",		// 4
	"expbrk",
	"line",
	"linebrk",
	"spline",	// 8
	"wave3",
	"wave",
	NULL,
	NULL,			// 12
	NULL,
	NULL,
	NULL,
	NULL,			// 16
	"cheby",
	"line",
	NULL,
	"random",	// 20
	NULL,
	NULL,
	NULL,
	"line",		// 24
	"window",
	"linestep",
	NULL
};

inline int _max(int x, int y) { return (x > y) ? x : y; }
inline int _min(int x, int y) { return (x < y) ? x : y; }
inline double _fmax(double x, double y) { return (x > y) ? x : y; }
inline double _fmin(double x, double y) { return (x < y) ? x : y; }


// --------------------------------------------------------- local utilities ---
static TablePField *
_getTablePField(const Arg *arg)
{
//FIXME: How do we know that this PField really is a TablePField?
// Should we test that pfield->values > 1?
	PField *tpf = NULL;
	if ((tpf = (PField *) *arg) != NULL) {
		return (TablePField *) tpf;
	}
	return NULL;
}

// ----------------------------------------------------- args_have_same_type ---
// FIXME: this belongs in a more general file
int
args_have_same_type(const Arg args[], const int nargs, const RTcmixType type)
{
	for (int i = 0; i < nargs; i++)
		if (!args[i].isType(type))
			return 0;
	return 1;
}


// -------------------------------------------------------- _normalize_table ---
// Rescale the values in <array> so that the maximum absolute value is <peak>,
// which cannot be zero.  Similar to cmix fnscl.

static void
_normalize_table(double *array, const int len, const double peak)
{
	int i;
	double max = 0.0;

	assert(peak != 0.0);

	for (i = 0; i < len; i++) {
		double absval = fabs(array[i]);
		if (absval > max)
			max = absval;
	}
	max /= peak;
	if (max != 0.0)
		for (i = 0; i < len; i++)
			array[i] /= max;
}


// --------------------------------------------------------- _textfile_table ---
// Fill a table with numbers read from a text file.  The syntax is
//
//    table = maketable("textfile", size, filename)
//
// The function loads as many as <size> numbers into the table.  If there
// are not that many numbers in the text file, it zeros out the extra
// table values.  The function reports one warning if at least one piece
// of text (delimited by whitespace) cannot be interpreted as a double.
// This means the file may contain any number of free text comments
// interspersed with the numbers, as long as the comments themselves
// do not contain numbers!
//
// Similar to gen 2, except it supports only the text file input, it takes
// a file name instead of a cmix file number, and it does not return the
// number of elements in the array.  Get this with tablelen().
//                                                           - JGG, 6/20/04

static int
_textfile_table(const Arg args[], const int nargs, double *array, const int len)
{
	if (len < 2)
		return die("maketable (textfile)", "Table length must be at least 2.");
	if (nargs <= 0)
		return die("maketable (textfile)",
					"Usage: table = maketable(\"textfile\", size, filename)");

	if (!args[0].isType(StringType))
		return die("maketable (textfile)", "File name must be a string.");
	const char *fname = (const char *) args[0];

	FILE *stream = fopen(fname, "r");
	if (stream == NULL)
		return die("maketable (textfile)", "Can't open file \"%s\".", fname);

	bool non_convertible_found = false;
	int i;
	for (i = 0; i < len; i++) {
		char buf[128 + 1];
		if (fscanf(stream, "%128s", buf) == EOF)
			break;
		char *pos = NULL;
		errno = 0;
		double val = strtod(buf, &pos);
		if (val == 0.0 && pos == buf) {
			// no conversion performed; warn once
			if (!non_convertible_found)
				rtcmix_warn("maketable (textfile)",
					 "File contains some text that can't be converted to a number.");
			non_convertible_found = true;
			i--;
			continue;
		}
		if (errno == ERANGE)					// overflow or underflow
			return die("maketable (textfile)",
						  "A number in the file can't be represented as a double.");
		array[i] = val;
	}

	float bogusval;
	if (fscanf(stream, "%f", &bogusval) != EOF)
		rtcmix_warn("maketable (textfile)", "File \"%s\" has more than %d numbers.",
																	fname, len);
	fclose(stream);

	if (i != len) {
		rtcmix_warn("maketable (textfile)", "Only %d values loaded into table, "
									  "followed by %d padding zeros.", i, len - i);
		// Fill remainder with zeros.
		for ( ; i < len; i++)
			array[i] = 0.0;
	}
	else
		rtcmix_advise("maketable (textfile)", "%d values loaded into table.", i);

	return 0;
}


// -------------------------------------------------------- _soundfile_table ---
// Fill a table with numbers read from a sound file.  The syntax is
//
// table = maketable("soundfile", size, filename[, duration[, inskip[, inchan]]])
//
// The <size> argument is ignored; set it to zero.
//
// <filename> is obligatory.  The other arguments are optional, but if an
// argument further to the right is given, all the ones to its left must also
// be given.
//
//    filename    name of a sound file in any of the header types RTcmix can
//                read; data formats: 16bit signed int, 32bit float, 24bit
//                3-byte signed int; either endian.
//
//    duration    duration (in seconds) to read from file. If negative, its
//                absolute value is the number of sample frames to read.
//
//                If <duration> is missing, or if it's zero, then the whole
//                file is read.  Beware with large files -- there is no check
//                on memory consumption!
//
//    inskip      time (in seconds) to skip before reading, or if negative,
//                its absolute value is the number of sample frames to skip.
//
//                If <inskip> is missing, it's assumed to be zero.
//
//    inchan      channel number to read (with zero as first channel).
//
//                If <inchan> is missing all channels are read, with samples
//                from each frame interleaved.
//
// NOTE: We dispose the array passed in and allocate a new one to replace it.
//       This is because we can't know how big the array must be before we
//       look at the sound file header.
//
// Similar to gen 1, but no normalization.  The arguments are different also,
// and it does not return the number of frames.  Get the number of samples
// (not frames) with tablelen().
//                                              - JGG 2/7/01, rev. 6/19/04


#define ALL_CHANS -1
#define BUFSAMPS	1024 * 16

static int
_soundfile_table(const Arg args[], const int nargs, double **array, int *len)
{
	delete [] *array;		// need to allocate our own

	if (nargs <= 0)
		return die("maketable (soundfile)",
					  "\nUsage: table = maketable(\"soundfile\", size=0, "
										"filename[, duration[, inskip[, inchan]]])");

	if (!args[0].isType(StringType))
		return die("maketable (soundfile)", "File name must be a string.");
	const char *fname = (const char *) args[0];

	double request_dur = 0.0;
	double inskip = 0.0;
	int inchan = ALL_CHANS;
	if (nargs > 1) {
		if (!args[1].isType(DoubleType))
			return die("maketable (soundfile)", "<duration> must be a number.");
		request_dur = args[1];
		if (nargs > 2) {
			if (!args[2].isType(DoubleType))
				return die("maketable (soundfile)", "<inskip> must be a number.");
			inskip = args[2];
			if (nargs > 3) {
				if (!args[3].isType(DoubleType))
					return die("maketable (soundfile)", "<inchan> must be a number.");
				inchan = args[3];
			}
		}
	}

	int data_format, data_location, file_chans;
	long file_samps;
	double srate;

	int fd = open_sound_file("maketable (soundfile)", (char *) fname, NULL,
                &data_format, &data_location, &srate, &file_chans, &file_samps);
	if (fd == -1)
		return -1;

	if (srate != RTcmix::sr())
		rtcmix_warn("maketable (soundfile)", "The input file sampling rate is %g, but "
			  "the output rate is currently %g.", srate, RTcmix::sr());

	long file_frames = file_samps / file_chans;

	int table_chans = file_chans;
	if (inchan != ALL_CHANS) {
		if (inchan >= file_chans)
			return die("maketable (soundfile)",
						  "You asked for channel %d of a %d-channel file. (\"%s\")",
						  inchan, file_chans, fname);
		table_chans = 1;
	}

	long table_frames = file_frames;			// if request_dur == 0
	if (request_dur < 0.0)
		table_frames = (long) -request_dur;
	else if (request_dur > 0.0)
		table_frames = (long) (request_dur * srate + 0.5);

	long start_frame = (long) (inskip * srate + 0.5); 
	if (inskip < 0.0)
		start_frame = (long) -inskip;

	if (start_frame + table_frames > file_frames)
		table_frames = file_frames - start_frame;

	long table_samps = table_frames * table_chans;
 
	double *block = new double[table_samps];
	if (block == NULL)
		return die("maketable (soundfile)", "Not enough memory for table.");

	int bytes_per_samp = mus_data_format_to_bytes_per_sample(data_format);

	char *buf = new char[BUFSAMPS * bytes_per_samp];
	if (buf == NULL)
		return die("maketable (soundfile)",
									"Not enough memory for temporary buffer.");

	off_t seek_to = data_location + (start_frame * file_chans * bytes_per_samp);
	if (lseek(fd, seek_to, SEEK_SET) == -1)
		return die("maketable (soundfile)", "File seek error: %s", strerror(errno));

#if MUS_LITTLE_ENDIAN
	bool byteswap = IS_BIG_ENDIAN_FORMAT(data_format);
#else
	bool byteswap = IS_LITTLE_ENDIAN_FORMAT(data_format);
#endif
	bool is_float = IS_FLOAT_FORMAT(data_format);
	bool is_24bit = IS_24BIT_FORMAT(data_format);

	int buf_frames = BUFSAMPS / file_chans;
	int end_frame = start_frame + table_frames;

	double *blockp = block;
	long frames_read = 0;
	long buf_start_frame = start_frame;
	for ( ; buf_start_frame < end_frame; buf_start_frame += frames_read) {
		long bytes_read;

		if (buf_start_frame + buf_frames > end_frame) {		  // last buffer
			long samps = (end_frame - buf_start_frame) * file_chans;
			bytes_read = read(fd, buf, samps * bytes_per_samp);
		}
		else
			bytes_read = read(fd, buf, BUFSAMPS * bytes_per_samp);
		if (bytes_read == -1)
			return die("maketable (soundfile)", "File read error: %s",
															strerror(errno));
		if (bytes_read == 0)				// EOF, somehow
			break;

		long samps_read = bytes_read / bytes_per_samp;
		frames_read = samps_read / file_chans;

		int increment = 1;
		if (is_float) {
			float *bufp = (float *) buf;

			if (inchan != ALL_CHANS) {
				bufp += inchan;
				increment = file_chans;
			}
			if (byteswap) {
				for (long i = 0; i < samps_read; i += increment) {
					byte_reverse4(bufp);				// modify *bufp in place
					*blockp++ = (double) *bufp;
					bufp += increment;
				}
			}
			else {
				for (long i = 0; i < samps_read; i += increment) {
					*blockp++ = (double) *bufp;
					bufp += increment;
				}
			}
		}
		else if (is_24bit) {
			unsigned char *bufp = (unsigned char *) buf;

			if (inchan != ALL_CHANS) {
				bufp += inchan * 3;					// 3-byte samples
				increment = file_chans;
			}
			if (data_format == MUS_L24INT) {
				for (long i = 0; i < samps_read; i += increment) {
					int samp = (int) (((bufp[2] << 24)
										  + (bufp[1] << 16)
										  + (bufp[0] << 8)) >> 8);
					*blockp++ = (double) samp / (double) (1 << 8);
					bufp += increment * 3;
				}
			}
			else {	// data_format == MUS_B24INT
				for (long i = 0; i < samps_read; i += increment) {
					int samp = (int) (((bufp[0] << 24)
										  + (bufp[1] << 16)
										  + (bufp[2] << 8)) >> 8);
					*blockp++ = (double) samp / (double) (1 << 8);
					bufp += increment * 3;
				}
			}
		}
		else {	  // is 16bit integer file
			short *bufp = (short *) buf;

			if (inchan != ALL_CHANS) {
				bufp += inchan;
				increment = file_chans;
			}
			if (byteswap) {
				for (long i = 0; i < samps_read; i += increment) {
					int16_t samp = reverse_int2(bufp);
					*blockp++ = (double) samp;
					bufp += increment;
				}
			}
			else {
				for (long i = 0; i < samps_read; i += increment) {
					*blockp++ = (double) *bufp;
					bufp += increment;
				}
			}
		}
	}

	delete [] buf;
	sndlib_close(fd, 0, 0, 0, 0);

	*array = block;
	*len = table_samps;

	return 0;
}


// ---------------------------------------------------------- _literal_table ---
// Fill a table with just the values specified as arguments.  The syntax is
//
//    table = maketable("literal", "nonorm", size, a, b, c, d ...)
//
// <a>, <b>, <c>, etc. are the numbers that go into the table.  The
// "nonorm" tag is recommended, unless you want the numbers to be
// normalized to [-1,1] or [0,1].
//
// The function loads as many as <size> numbers into the table.  If there
// are not that many number arguments, it zeros out the extra table values.
// If <size> is zero, the table will be sized to fit the number arguments
// exactly.
//
// Similar to gen 2, except it supports only the "new way" described in
// gen2.c, it has the option to size the table to fit the number of arguments,
// and it does not return the number of elements in the array.  Get this
// with tablelen().
//                                                           - JGG, 6/20/04

static int
_literal_table(const Arg args[], const int nargs, double **array, int *len)
{
	double *block = *array;
	int length = *len;

	if (length == 0) {
		length = nargs;
		delete [] block;
		block = new double[length];
	}

	const int fill_len = _min(nargs, length);

	int j = 0;
	for (int i = 0; i < nargs && j < fill_len; i++)
		block[j++] = args[i];

	if (nargs < length) {
		for (int i = nargs; i < length; i++)
			block[i] = 0.0;
		rtcmix_advise("maketable (literal)",
							"Table is larger than the number of elements given "
							"to fill it.  Adding zeros to pad.");
	}
	else if (length < nargs)
		rtcmix_warn("maketable (literal)",
							"Table is large enough for only %d numbers.", length);

	*array = block;
	*len = length;

	return 0;
}


// --------------------------------------------------------- _datafile_table ---
// Fill a table with numbers read from a data file.  The syntax is
//
//    table = maketable("datafile", size, filename[, number_type])
//
// The function loads as many as <size> numbers into the table.  If there
// are not that many numbers in the file, it zeros out the extra table values.
// If <size> is zero, the table will be sized to fit the data exactly.  Be
// careful if you use this option with a very large file: you may run out
// of memory!
//
// If the data file has a header of the sort written when recording a
// PField data stream, then the type of number is taken from this header.
// Otherwise, use the <number_type> argument.
//
// <number_type> can be any of "float" (the default), "double", "int",
// "int64", "int32", "int16" or "byte".  This just means that with the
// "double" type, for example, every 8 bytes will be interpreted as one
// floating point number; with the "int16" type, every 2 bytes will be
// interpreted as one integer; and so on.  "int" is "int32" on a 32-bit
// platform and "int64" on a 64-bit platform.  No byte-swapping is done.
//
// Similar to gen 3, except that it accepts a file name rather than a cmix
// file number, it has choices for the type of number, it has the option to
// size the table to fit the data, and it does not return the number of
// elements in the array.  Get this with tablelen().
//                                                          - JGG, 6/20/04

#include "DataFile.h"

static int
_datafile_table(const Arg args[], const int nargs, double **array, int *len)
{
	double *block = *array;
	long length = *len;

	if (nargs <= 0)
		return die("maketable (datafile)",
			"Usage: table = maketable(\"datafile\", size, filename, number_type)");

	const char *fname = (const char *) args[0];
	if (fname == NULL || fname[0] == 0)
		return die("maketable (datafile)", "File name must be a string.");

	int format = kDataFormatFloat;
	if (nargs > 1) {
		format = DataFile::formatStringToCode(args[1]);
		if (format == -1)
			return die("maketable (datafile)", "File number type must be a string "
						  "\"double\", \"float\", \"int\", \"int64\", \"int32\", "
						  "\"int16\" or \"byte\"");
	}

	// NB: errors reported within DataFile.
	DataFile *dfile = new DataFile(fname);
	if (dfile->openFileRead() != 0)
		return die("maketable (datafile)", "Can't open file.");
	long fileitems = dfile->readHeader(1000, format, false);
	if (fileitems == -1)
		return die("maketable (datafile)", "Can't read file.");

	if (length == 0) {
		delete [] block;
		block = new double[fileitems];
		length = fileitems;
	}

	// Read data file until EOF or table is filled, using specified word size.
	const long readitems = (length > fileitems) ? fileitems : length;
	dfile->readFile(block, readitems);
	delete dfile;

	if (length < fileitems)
		rtcmix_warn("maketable (datafile)",
			  "File \"%s\" has %ld more numbers than table, which has %ld.",
											fname, fileitems - length, length);
	else if (length > fileitems) {
		const long diff = length - fileitems;
		rtcmix_warn("maketable (datafile)", "Only %ld values loaded into table, "
											  "followed by %ld padding zero%s.",
											  fileitems, diff, diff > 1 ? "s" : "");
		// Fill remainder with zeros.
		for (long i = fileitems; i < length; i++)
			block[i] = 0.0;
	}
	else
		rtcmix_advise("maketable (datafile)",
					"%ld values loaded into table.", readitems);

	*array = block;
	*len = length;

	return 0;
readerr:
	return die("maketable (datafile)", "Error reading file \"%s\".", fname);
}


// ------------------------------------------------- _curve_table and helper ---
// Fill a table with line or curve segments, defined by
// <time, value, curvature> arguments.  The syntax is...
//
//    table = maketable("curve", size, time1, value1, curvature1, [ timeN-1,
//                                   valueN-1, curvatureN-1, ] timeN, valueN)
//
// <curvature> controls the curvature of the line segment from point N
// to point N+1.  The values are
//
//    curvature = 0
//                makes a straight line
//
//    curvature < 0
//                makes a logarithmic (convex) curve
//
//    curvature > 0
//                makes an exponential (concave) curve
//
// Derived from gen4 from the UCSD Carl package, described in F.R. Moore, 
// "Elements of Computer Music."
//                                                -JGG, 12/2/01, rev 1/25/04


// _transition(a, alpha, b, n, output) makes a transition from <a> to <b> in
// <n> steps, according to transition parameter <alpha>.  It stores the
// resulting <n> values starting at location <output>.
//    alpha = 0 yields a straight line,
//    alpha < 0 yields a logarithmic transition, and 
//    alpha > 0 yields an exponential transition.
// All of this in accord with the formula:
//    output[i] = a + (b - a) * (1 - exp(i * alpha / (n-1))) / (1 - exp(alpha))
//    for 0 <= i < n

static void
_transition(double a, double alpha, double b, int n, double *output)
{
	int	 i;
	double delta, interval = 0.0;

	delta = b - a;

	if (n <= 1) {
		rtcmix_warn("maketable (curve)", "Trying to transition over 1 array slot; "
										  "time between points is too short");
		*output = a;
		return;
	}
	interval = 1.0 / (n - 1.0);

	if (alpha != 0.0) {
		double denom = 1.0 / (1.0 - exp(alpha));
		for (i = 0; i < n; i++)
			*output++ = (a + delta
								* (1.0 - exp((double) i * alpha * interval)) * denom);
	}
	else
		for (i = 0; i < n; i++)
			*output++ = a + delta * i * interval;
}

#define MAX_CURVE_PTS 256

static int
_curve_table(const Arg args[], const int nargs, double *array, const int len)
{
	int	 i, points, seglen = 0;
	double factor, *ptr;
	double time[MAX_CURVE_PTS], value[MAX_CURVE_PTS], alpha[MAX_CURVE_PTS];

	if (len < 2)
		return die("maketable (curve)", "Table length must be at least 2.");
	if (nargs < 5 || (nargs % 3) != 2)			// check number of args
		return die("maketable (curve)", "\nUsage: table = maketable(\"curve\", "
					  "size, time1, value1, curvature1, [ timeN-1, valueN-1, "
					  "curvatureN-1, ] timeN, valueN)");
	if ((nargs / 3) + 1 > MAX_CURVE_PTS)
		return die("maketable (curve)", "Too many arguments.");
	if (!args_have_same_type(args, nargs, DoubleType))
		return die("maketable (curve)",
									"<time, value, curvature> pairs must be numbers.");
	if ((double) args[0] != 0.0)
		return die("maketable (curve)", "First time must be zero.");

	for (i = points = 0; i < nargs; points++) {
		time[points] = (double) args[i++];
		if (points > 0 && time[points] < time[points - 1])
			goto time_err;
		value[points] = (double) args[i++];
		if (i < nargs)
			alpha[points] = (double) args[i++];
	}

	factor = (double) (len - 1) / time[points - 1];
	for (i = 0; i < points; i++)
		time[i] *= factor;

	ptr = array;
	for (i = 0; i < points - 1; i++) {
		seglen = (int) (floor(time[i + 1] + 0.5) - floor(time[i] + 0.5)) + 1;
		_transition(value[i], alpha[i], value[i + 1], seglen, ptr);
		ptr += seglen - 1;
	}

	return 0;
time_err:
	return die("maketable (curve)", "Times must be in ascending order.");
}


// ----------------------------------------------------------- _expbrk_table ---
// Similar to gen 5, but no normalization.

static int
_expbrk_table(const Arg args[], const int nargs, double *array, const int len)
{
	if (len < 2)
		return die("maketable (expbrk)", "Table length must be at least 2.");
	double amp2 = args[0];
	if (amp2 <= 0.0)
		amp2 = 0.00001;
	int i = 0;
	for (int k = 1; k < nargs; k += 2) {
		double amp1 = amp2;
		amp2 = args[k + 1];
		if (amp2 <= 0.0)
			amp2 = 0.00001;
		int j = i + 1;
		array[i] = amp1;
		double c = pow((amp2 / amp1), (1.0 / (double) args[k]));
		int increment = (int) args[k];
		if (increment <= 0)
			return die("maketable (expbrk)", "Bad time argument.");
		i = (j - 1) + increment;
		for (int l = j; l < i; l++) {
			if (l < len) {
				array[l] = array[l - 1] * c;
			}
			else {
				rtcmix_warn("maketable (expbrk)", "The number of points requested "
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

	return 0;
}


// ------------------------------------------------------------- _line_table ---
// Create a function table comprising straight line segments, specified by
// using any number of <time, value> pairs.  The table is not normalized.
//
// Derived from cmix gen 24, but no table normalization, and it spreads values
// evenly across the array in such a way that the last array element has the
// same value as the last argument to maketable.  (None of the cmix setline
// workalikes -- gen6, gen18, gen24 -- do this unless they normalize the table
// result with fnscl.)  This helps you avoid clicks when using as an amp
// envelope and ramping to zero at the end.  The gens never get to zero.  On
// the other hand, this function gets to zero too early, so it's important to
// use tables large enough that the last array slot controls a very short
// amount of time.  (I.e., total time / array len should be just a few msecs
// for critical situations.)  This new behavior is turned on by defining
// NEWWAY symbol below.  This function also works when the first time is
// non-zero.  (This seems to work in gen24, but not in gen6 and gen18.)
//
// -JGG, 1/25/04

#define NEWWAY
static int
_line_table(const Arg args[], const int nargs, double *array, const int len)
{
	if (len < 2)
		return die("maketable (line)", "Table length must be at least 2.");
	if (!args_have_same_type(args, nargs, DoubleType))
		return die("maketable (line)", "<time, value> pairs must be numbers.");
	if ((nargs % 2) != 0)
		return die("maketable (line)", "Incomplete <time, value> pair.");

	double endtime = (double) args[nargs - 2];
	double starttime = (double) args[0];
	if (endtime - starttime <= 0.0)
		return die("maketable (line)", "Times must be in ascending order.");
#ifdef NEWWAY
	double scaler = (double) (len - 1) / (endtime - starttime);
#else
	double scaler = (double) len / (endtime - starttime);
#endif
	double nextval = (double) args[1];
	double thistime = starttime;
	int i = 0;
	for (int k = 1; k < nargs; k += 2) {
		double nexttime;
		double thisval = nextval;
		if (k < nargs - 1) {
			nexttime = (double) args[k + 1] - starttime;
			if (nexttime - thistime < 0.0)	// okay for them to be the same
				return die("maketable (line)", "Times must be in ascending order.");
			nextval = (double) args[k + 2];
		}
		else
			nextval = nexttime = 0.0;			// don't read off end of args array
		int j = i + 1;
#ifdef NEWWAY
		i = (int) ((nexttime * scaler) + 0.5);
#else
		i = (int) ((nexttime * scaler) + 1.0);
#endif
		//printf("j=%d, i=%d\n", j, i);
		for (int l = j; l <= i; l++) {
			if (l <= len)
				array[l - 1] = thisval + (nextval - thisval)
														* (double) (l - j) / ((i - j) + 1);
		}
	}
#ifdef NEWWAY
	array[len - 1] = (double) args[nargs - 1];
#endif

	return 0;
}


// ---------------------------------------------------------- _linebrk_table ---
// Similar to gen 7, but no normalization.

static int
_linebrk_table(const Arg args[], const int nargs, double *array, const int len)
{
	if (len < 2)
		return die("maketable (linebrk)", "Table length must be at least 2.");

	fill_linebrk_table(args, nargs, array, len);

	return 0;
}


// ----------------------------------------------- _spline_table and helpers ---
// Fill a table with a spline curve, defined by at least three x,y points.
// The curve travels smoothly between the points, and all points lie on the
// curve.  The syntax is
//
//    table = maketable("spline", size, ["closed",] curvature,
//                                        time1, value1, ... timeN, valueN)
//
// <curvature> controls the character of the slope between points.  Start
// with zero; try positive numbers up to around 200 to see the difference.
//
// The option "closed" tag is another way of affecting the curvature.
// You just have to experiment with "closed" and <curvature> to get a
// feel for what they do to a particular shape.
//
// It is possible that the curve will loop outside of the area you expect,
// especially with the "nonorm" tag, so use plottable to be sure.
//
// Adapted from cspline from the UCSD Carl package, described in F.R. Moore, 
// "Elements of Computer Music."
//                                                         - JGG, 6/20/04


#define MAX_KNOTS 1024

typedef struct {
	bool manual_lb, manual_ub;
	float lower_bound, upper_bound, val[MAX_KNOTS];
} SplineSpec;


static void
_spline_interp(const float *inbuf, const int count, double *outbuf,
																		const int outbuflen)
{
	float rat = count * 1.0 / (float) outbuflen;
	int c = 0;
	float fc = 0.0;
	for (int i = outbuflen - 1; i >= 0; fc += rat, i--) {
		c = (int) fc;
		float frat = fc - (float) c;			// get fraction
		outbuf[i] = (double) ((1.0 - frat) * inbuf[c] + frat * inbuf[c + 1]);
	}
}


inline float
_rhs(const int knot, const int nknots, const SplineSpec *x, const SplineSpec *y)
{
	int i = (knot == nknots - 1) ? 0 : knot;
	double zz = (y->val[knot] - y->val[knot - 1])
											/ (x->val[knot] - x->val[knot - 1]);
	return (6.0 * ((y->val[i + 1] - y->val[i])
											/ (x->val[knot + 1] - x->val[knot]) - zz));
}


static int
_spline(const int closed, const float konst, const int nknots, double *outbuf,
		  const int outbuflen, const SplineSpec *x, const SplineSpec *y)
{
	int count = 0;
	int buflen = BUFSIZ;
	float *buf = (float *) malloc(buflen * sizeof(float));  // realloc'd below
	if (buf == NULL)
		return die("maketable (spline)", "Out of memory.");

	float *diag = new float[nknots + 1];
	float *r = new float[nknots + 1];

	r[0] = 0.0;

	float a = 0.0;
	float d = 1.0;
	float u = 0.0;
	float v = 0.0;
	float s = closed ? -1.0 : 0.0;
	for (int i = 0; ++i < nknots - !closed; ) {			// triangularize
		float hi = x->val[i] - x->val[i - 1];
		float hi1 = (i == nknots - 1) ? x->val[1] - x->val[0]
										 : x->val[i + 1] - x->val[i];
		if (hi1 * hi <= 0)
			return -1;
		u = i == 1 ? 0.0 : u - s * s / d;
		v = i == 1 ? 0.0 : v - s * r[i - 1] / d;
		r[i] = _rhs(i, nknots, x, y) - hi * r[i - 1] / d;
		s = -hi * s / d;
		a = 2.0 * (hi + hi1);
		if (i == 1)
			a += konst * hi;
		if (i == nknots - 2)
			a += konst * hi1;
		diag[i] = d = (i == 1) ? a : a - hi * hi / d;
	}
	float D2yi = 0.0;
	float D2yn1 = 0.0;
	for (int i = nknots - !closed; --i >= 0; ) {			// back substitute
		int end = i == nknots - 1;
		float hi1 = end ? x->val[1] - x->val[0] : x->val[i + 1] - x->val[i];
		float D2yi1 = D2yi;
		if (i > 0) {
			float hi = x->val[i] - x->val[i - 1];
			float corr = end ? 2.0 * s + u : 0.0;
			D2yi = (end * v + r[i] - hi1 * D2yi1 - s * D2yn1) /
				 (diag[i] + corr);
			if (end)
				D2yn1 = D2yi;
			if (i > 1) {
				a = 2 * (hi + hi1);
				if (i == 1)
					a += konst * hi;
				if (i == nknots - 2)
					a += konst * hi1;
				d = diag[i - 1];
				s = -s * d / hi;
			}
		}
		else
			D2yi = D2yn1;
		if (!closed) {
			if (i == 0)
				D2yi = konst * D2yi1;
			if (i == nknots - 2)
				D2yi1 = konst * D2yi;
		}
		if (end)
			continue;
		int m = (hi1 > 0.0) ? outbuflen : -outbuflen;
		m = (int) (1.001 * m * hi1 / (x->upper_bound - x->lower_bound));
		if (m <= 0)
			m = 1;
		float h = hi1 / m;
		for (int j = m; j > 0 || (i == 0 && j == 0); j--) {
			// interpolate
			float x0 = (m - j) * h / hi1;
			float x1 = j * h / hi1;
			float yy = D2yi * (x0 - x0 * x0 * x0) + D2yi1 * (x1 - x1 * x1 * x1);
			yy = y->val[i] * x0 + y->val[i + 1] * x1 - hi1 * hi1 * yy / 6.0;

			if (count < buflen)
				buf[count++] = yy;
			else {
				buflen += BUFSIZ;
				buf = (float *) realloc(buf, buflen * sizeof(float));
				if (buf == NULL)
					return die("maketable (spline)", "Out of memory.");
				buf[count++] = yy;
			}
		}
	}

	delete [] diag;
	delete [] r;

	_spline_interp(buf, count, outbuf, outbuflen);

	free(buf);

	return 0;
}


static void
_spline_getlimit(SplineSpec *p, int nknots)
{
	for (int i = 0; i < nknots; i++) {
		if (!p->manual_lb && p->lower_bound > p->val[i])
			p->lower_bound = p->val[i];
		if (!p->manual_ub && p->upper_bound < p->val[i])
			p->upper_bound = p->val[i];
	}
}


#define INF 1.e37

static int
_spline_table(const Arg args[], const int nargs, double *array, const int len)
{
	if (len < 2)
		return die("maketable (spline)", "Table length must be at least 2.");

	int closed = 0;
	float curvature = 0.0;

	int firstknot = 1;
	if (args[0].isType(StringType)) {
		if (args[0] == "closed")
			closed = 1;
		firstknot++;
	}
	else
		curvature = (float) args[0];

	if (((nargs - firstknot) % 2) != 0)
		return die("maketable (spline)", "Incomplete <time, value> pair.");
	if ((nargs - firstknot) < 6)
		return die("maketable (spline)", "Need at least 3 <time, value> pairs.");
	if ((nargs - firstknot) > MAX_KNOTS * 2)
		return die("maketable (spline)", "Too many <time, value> pairs.");

	SplineSpec x, y;

	x.manual_lb = x.manual_ub = y.manual_lb = y.manual_ub = 0;
	x.lower_bound = y.lower_bound = INF;
	x.upper_bound = y.upper_bound = -INF;
	// It's possible to manually set the lower and upper bounds here
	// (set the manual_lb and manual_ub flags as well), but it didn't
	// seem worth supporting this.   -JGG

	float last_timeval = -INF;
	int nknots = 0;
	for (int i = firstknot; i < nargs; i += 2) {
		float this_timeval = args[i];
		x.val[nknots] = this_timeval;
		y.val[nknots] = args[i + 1];
		if (this_timeval <= last_timeval)
			return die("maketable (spline)", "Times must be ascending.");
		last_timeval = this_timeval;
		nknots++;
	}

	_spline_getlimit(&x, nknots);
	//_spline_getlimit(&y, nknots);   JGG: y bounds not used

	if (_spline(closed, curvature, nknots, array, len, &x, &y) != 0)
		return -1;

	return 0;
}


// ------------------------------------------------------------ _wave3_table ---
// Similar to cmix gen 9, but no normalization.

static int
_wave3_table(const Arg args[], const int nargs, double *array, const int len)
{
	if (len < 2)
		return die("maketable (wave3)", "Table length must be at least 2.");
	if ((nargs % 3) != 0)
		return die("maketable (wave3)",
									"Incomplete <partial, amp, phase> triplet.");

	for (int i = 0; i < len; i++)
		array[i] = 0.0;

	for (int j = nargs - 1; j > 0; j -= 3) {
		assert(j > 0);
		assert(j < nargs);

		if ((double) args[j - 1] != 0.0) {
			for (int i = 0; i < len; i++) {
				double val;
				if ((double)args[j - 2] == 0.0) val = 1.0; // BGG: harmonic 0 (DC)
				else {
					val = sin(TWOPI * ((double) i
										/ ((double) len / (double) args[j - 2])
										+ (double) args[j] / 360.0));
				}
				array[i] += (val * (double) args[j - 1]);
			}
		}
	}

	return 0;
}


// ------------------------------------------------------------- _wave_table ---
// Similar to cmix gen 10, but no normalization.

static int
_wave_table(const Arg args[], const int nargs, double *array, const int len)
{
	if (len < 2)
		return die("maketable (wave)", "Table length must be at least 2.");

	if (args[0].isType(StringType))
		return wavetable_from_string(args[0], array, len, "maketable (wave)");
	else
		fill_wave_table(args, nargs, array, len);

	return 0;
}


// ------------------------------------------------------------ _cheby_table ---
// Equivalent to cmix gen 17.  Computes transfer function using Chebyshev
// polynomials.  First argument is the index value for which the function will
// create the harmonics specified by the following arguments.

static int
_cheby_table(const Arg args[], const int nargs, double *array, const int len)
{
	if (len < 2)
		return die("maketable (cheby)", "Table length must be at least 2.");

	double d = (double) ((len / 2) - 0.5);
	for (int i = 0; i < len; i++) {
		double x = (i / d - 1.0) / (double) args[0];
		array[i] = 0.0;
		double Tn1 = 1.0;
		double Tn = x;
		for (int j = 1; j < nargs; j++) {
			array[i] += (double) args[j] * Tn;
			double Tn2 = Tn1;
			Tn1 = Tn;
			Tn = (2.0 * x * Tn1) - Tn2;
		}
	}

	return 0;
}


// ----------------------------------------------------------- _random_table ---
// Fill a table with random numbers.  The syntax is
//
//    table = maketable("random", size, type, min, max[, seed])
//
// The distribution types are:
//    0 = even distribution ["even" or "linear"]
//    1 = low weighted linear distribution ["low"]
//    2 = high weighted linear distribution ["high"]
//    3 = triangle linear distribution ["triangle"]
//    4 = gaussian distribution ["gaussian"]
//    5 = cauchy distribution ["cauchy"]
//    6 = Mara Helmuth's probability distribution ["prob"]  *see below for usage
//
// Give either the string or number specification of type.
//
// <min> and <max> define the range (inclusive) for the random numbers.
// Both args must be present.
//
// NOTE: If you don't give the "nonorm" argument after "random", then the 
// table will be normalized, thus disregarding your min and max!
//
// If <seed> is zero, seed comes from the microsecond clock, otherwise <seed>
// is used as the seed.  If no <seed> argument, the seed used is 1.
//
// *Note: type 6 ("prob") has different arguments:
//
// table = maketable("random", size, "prob", min, max, mid, tight[, seed])
//
//    <min> and <max> set the range within which the random numbers fall,
//    as before.
//
//    <mid> sets the mid-point of the range, which has an effect whenever
//    <tight> is not 1.
//
//    <tight> governs the degree to which the random numbers adhere either
//    to the mid-point or to the extremes of the range:
//
//       tight         effect
//       -------------------------------------------------------------
//       0             only the <min> and <max> values appear
//       1             even distribution
//       > 1           numbers cluster ever more tightly around <mid>
//       100           almost all numbers are equal to <mid>
//
//
// Similar to gen 20. the original version of which was written by Luke Dubois;
// later additions and this adaptation by John Gibson.  Some distribution
// equations adapted from Dodge and Jerse.


static int
_random_table_usage()
{
	die("maketable (random)",
		"\n   usage: table = maketable(\"random\", size, type, min, max[, seed])"
		"\n          where <type> is \"linear\", \"low\", \"high\", "
						"\"triangle\", \"gaussian\", \"cauchy\""
		"\nOR"
		"\n   usage: table = maketable(\"random\", size, \"prob\", min, max, "
						"mid, tight[, seed])"
		"\n");
	return -1;
}

static int
_random_table(const Arg args[], const int nargs, double *array, const int len)
{
	if (nargs < 3)
		return _random_table_usage();

	if (len < 2)
		return die("maketable (random)", "Table length must be at least 2.");

	int type = 0;
	if (args[0].isType(StringType)) {
		if (args[0] == "linear" || args[0] == "even")
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
			return _random_table_usage();
	}
	else if (args[0].isType(DoubleType)) {
		// NB: enum vals in Random.h must match makegen ones for backward compat.
		type = (int) args[0];
	}
	else
		return die("maketable (random)",
								"Distribution type must be string or number.");

	double min = args[1];
	double max = args[2];
	if (min == max)
		return die("maketable (random)", "<min> must be lower than <max>.");
	if (min > max) {		// make sure these are in increasing order
		const double tmp = max;
		max = min;
		min = tmp;
	}

	double mid = 0.0, tight = 0.0;
	int seed = 0;

	if (type == kProbRandom) {		// Mara's function has special args
		if (nargs < 5)
			return _random_table_usage();
		mid = args[3];
		if (mid < min || mid > max)
			return die("maketable (random)",
										"<mid> must be between <min> and <max>.");
		tight = args[4];
		if (tight < 0.0) {
			rtcmix_warn("maketable (random)", "<tight> must be zero or greater "
												"... setting to zero.");
			tight = 0.0;
		}
		if (nargs > 5)
			seed = (int) args[5];
	}
	else if (nargs > 3)
		seed = (int) args[3];

	if (seed == 0) {
		struct timeval tv;
		gettimeofday(&tv, NULL);
		seed = (int) tv.tv_usec;
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
		default:
			return die("maketable (random)", "Unsupported distribution type %d.",
																					type);
			break;
	}
	for (int i = 0; i < len; i++)
		array[i] = gen->value();

	delete gen;

	return 0;
}


// ----------------------------------------------------------- _window_table ---
// Similar to gen 25, but no normalization.

static int
_window_table(const Arg args[], const int nargs, double *array, const int len)
{
	int window_type = 0;

	if (len < 2)
		return die("maketable (window)", "Table length must be at least 2.");
	if (nargs != 1)
		return die("maketable (window)", "Missing window type.");

	if (args[0].isType(StringType)) {
		if (args[0] == "hanning")
			window_type = 1;
		else if (args[0] == "hamming")
			window_type = 2;
		else
			return die("maketable (window)", "Unsupported window type \"%s\".",
															(const char *) args[0]);
	}
	else if (args[0].isType(DoubleType)) {
		  window_type = (int) args[0];
	}
	else
		return die("maketable (window)",
								"Window type must be string or numeric code.");

	switch (window_type) {
		case 1:		// hanning window
			for (int i = 0; i < len; i++)
				array[i] = -cos(2.0 * M_PI * (double) i / (double) len) * 0.5 + 0.5;
			break;
		case 2:		// hamming window
			for (int i = 0; i < len; i++) {
				double val = cos(2.0 * M_PI * (double) i / (double) len);
				array[i] = 0.54 - 0.46 * val;
			}
			break;
		default:
			return die("maketable (window)", "Unsupported window type (%d).",
																		window_type);
	}

	return 0;
}


// --------------------------------------------------------- _linestep_table ---
// Create a function table comprising flat line segments, specified by
// using any number of <time, value> pairs.  The table is not normalized.

static int
_linestep_table(const Arg args[], const int nargs, double *array, const int len)
{
	if (len < 2)
		return die("maketable (linestep)", "Table length must be at least 2.");
	if (!args_have_same_type(args, nargs, DoubleType))
		return die("maketable (linestep)", "<time, value> pairs must be numbers.");
	if ((nargs % 2) != 0)
		return die("maketable (linestep)", "Incomplete <time, value> pair.");

	double endtime = (double) args[nargs - 2];
	double starttime = (double) args[0];
	if (endtime - starttime <= 0.0)
		return die("maketable (linestep)", "Times must be in ascending order.");
	double scaler = (double) (len - 1) / (endtime - starttime);
	double nextval = (double) args[1];
	double thistime = starttime;
	int i = 0;
	for (int k = 1; k < nargs; k += 2) {
		double nexttime;
		double thisval = nextval;
		if (k < nargs - 1) {
			nexttime = (double) args[k + 1] - starttime;
			if (nexttime - thistime < 0.0)	// okay for them to be the same
				return die("maketable (linestep)", "Times must be in ascending order.");
			nextval = (double) args[k + 2];
		}
		else
			nextval = nexttime = 0.0;			// don't read off end of args array
		int j = i + 1;
		i = (int) ((nexttime * scaler) + 0.5);
		//printf("j=%d, i=%d\n", j, i);
		for (int l = j; l <= i; l++) {
			if (l <= len)
				array[l - 1] = thisval;
		}
	}
	array[len - 1] = (double) args[nargs - 1];

	return 0;
}


// --------------------------------------------------------- _dispatch_table ---

static TableKind
_string_to_tablekind(const char *str)
{
	int i;

	for (i = 0; i < (int) EndTable; i++) {
		if (_table_name[i] == NULL)
			continue;
		if (strncmp(str, _table_name[i], 16) == 0)
			return (TableKind) i;
	}
	return InvalidTable;
}

int
_dispatch_table(const Arg args[], const int nargs, const int startarg,
	double **array, int *len)
{
	int status;
	TableKind tablekind;

	// Call the appropriate factory function, skipping over first two args.
	if (args[0].isType(DoubleType))
		tablekind = (TableKind) (int) args[0];
	else if (args[0].isType(StringType)) {
		tablekind = _string_to_tablekind((const char *) args[0]);
		if (tablekind == InvalidTable)
			return die("maketable", "Invalid table type string \"%s\"",
																		(const char *) args[0]);
	}
	else
		return die("maketable", "First argument must be a number or string.");

	// Flatten any one-dimensional parser arrays into a new argument list,
	// beginning from <startarg>.

	Arg xargs[MAXDISPARGS];
	int nxargs = 0;
	for (int i = startarg; i < nargs; i++) {
		if (args[i].isType(ArrayType)) {
			// NB: currently if we get this far, the array must contain only doubles;
			// otherwise it's an error (e.g., in src/parser/minc/callextfunc.cpp).
			Array *a = args[i];
			const int alen = a->len;
			const double *adata = a->data;
			for (int j = 0; j < alen; j++) {
				xargs[nxargs++] = adata[j];
				if (nxargs == MAXDISPARGS)
					return die("maketable", "Exceeded maximum number of arguments (%d)",
									MAXDISPARGS);
			}
		}
		else {
			xargs[nxargs++] = args[i];
			if (nxargs == MAXDISPARGS)
				return die("maketable", "Exceeded maximum number of arguments (%d)",
								MAXDISPARGS);
		}
	}

	// NOTE: passing addresses of array and len is correct for some of these
   //       tables, because they might need to recreate their array.

	switch (tablekind) {
		case TextfileTable:
			status = _textfile_table(xargs, nxargs, *array, *len);
			break;
		case SoundFileTable:
			status = _soundfile_table(xargs, nxargs, array, len);
			break;
		case LiteralTable:
			status = _literal_table(xargs, nxargs, array, len);
			break;
		case DatafileTable:
			status = _datafile_table(xargs, nxargs, array, len);
			break;
		case CurveTable:
			status = _curve_table(xargs, nxargs, *array, *len);
			break;
		case ExpbrkTable:
			status = _expbrk_table(xargs, nxargs, *array, *len);
			break;
		case LineTable:
			status = _line_table(xargs, nxargs, *array, *len);
			break;
		case LinebrkTable:
			status = _linebrk_table(xargs, nxargs, *array, *len);
			break;
		case SplineTable:
			status = _spline_table(xargs, nxargs, *array, *len);
			break;
		case Wave3Table:
			status = _wave3_table(xargs, nxargs, *array, *len);
			break;
		case WaveTable:
			status = _wave_table(xargs, nxargs, *array, *len);
			break;
		case ChebyTable:
			status = _cheby_table(xargs, nxargs, *array, *len);
			break;
		case RandomTable:
			status = _random_table(xargs, nxargs, *array, *len);
			break;
		case WindowTable:
			status = _window_table(xargs, nxargs, *array, *len);
			break;
		case LinestepTable:
			status = _linestep_table(xargs, nxargs, *array, *len);
			break;
		default:
			return die("maketable", "Invalid table type.");
			break;
	}

	return status;
}


// =============================================================================
// The remaining functions are public, callable from scripts.

extern "C" {
	RtcHandle maketable(const Arg args[], const int nargs);
	double tablelen(const Arg args[], const int nargs);
	double tablemin(const Arg args[], const int nargs);
	double tablemax(const Arg args[], const int nargs);
	RtcHandle copytable(const Arg args[], const int nargs);
	double samptable(const Arg args[], const int nargs);
	double dumptable(const Arg args[], const int nargs);
	double plottable(const Arg args[], const int nargs);
};


// --------------------------------------------------------------- maketable ---
// Make a table of values using one of the table types. Numeric arguments
// can be scalars or one-dimensional scalar arrays created in the scripting
// language.

static void
_maketable_usage()
{
	die("maketable",
		"\n	 usage: table = maketable(type, [option, ] length, ...)\n");
}

typedef enum {
	kTruncate,
	kInterp1stOrder,
	kInterp2ndOrder
} InterpType;

RtcHandle
maketable(const Arg args[], const int nargs)
{
	if (nargs < 3) {
		_maketable_usage();
		return NULL;
	}

	bool normalize = true;
	InterpType interp = kInterp1stOrder;
	bool dynamic = false;

	int lenindex = 1;					// following table type string w/ no options
	for (int i = lenindex; i < nargs; i++) {
		if (!args[i].isType(StringType))
			break;
		if (args[i] == "nonorm")
			normalize = false;
		else if (args[i] == "nointerp")
			interp = kTruncate;
		else if (args[i] == "norm")
			normalize = true;
		else if (args[i] == "interp")
			interp = kInterp1stOrder;
		else if (args[i] == "interp2")
			interp = kInterp2ndOrder;
		else if (args[i] == "dynamic") {
			dynamic = true;
			normalize = false;
		}
		else {
			die("maketable", "Invalid string option \"%s\".",
													(const char *) args[i]);
			return NULL;
		}
		lenindex++;
	}

	if (!args[lenindex].isType(DoubleType)) {
		_maketable_usage();
		return NULL;
	}
	int len = args[lenindex];
	if (len < 0) {		// NOTE: It's okay for len to be zero (cf _soundfile_table)
		die("maketable", "Negative table size.");
		return NULL;
	}
	if (len > MAX_ARRAY_LEN) {
		rtcmix_warn("maketable", "Requesting larger than maximum table length.  "
													"Setting to %d.", MAX_ARRAY_LEN);
		len = MAX_ARRAY_LEN;
	}

	// Allocate table array.  TablePField will own and delete this.
	
	double *data = NULL;
	if (len > 0) {
		data = new double[len];
		if (data == NULL) {
			die("maketable", "Out of memory.");
			return NULL;
		}
	}

	if (!dynamic) {
		if (_dispatch_table(args, nargs, lenindex + 1, &data, &len) != 0) {
			delete [] data;
			return NULL;				// error message already given
		}
	} else { // setup for dynamic tables (PFSCHED/pfbus)
		Arg targs[MAXDISPARGS];
	
		for (int i  = 0; i < lenindex; i++)
			targs[i] = args[i];

		for (int j = lenindex; j < nargs;  j++) {
			if (args[j].isType(StringType)) targs[j] = DYNTABLETOKEN;
			else targs[j] = args[j];
		}

		if (_dispatch_table(targs, nargs, lenindex + 1, &data, &len) != 0) {
			delete [] data;
			return NULL;				// error message already given
		}
		// BGG -- pass the maketable() spec in through the data for
		// on-the-fly construction, here's the setup:
		// data[0] == flag for DYNTABLE
		// data[1] == number of spec vals
		// data[2] == table size
		// data[3...n] == original maketable() specs for the table

		int nspecs = nargs-(lenindex+1);
		data[0] = DYNTABLETOKEN;
		data[1] = nspecs;
		data[2] = (double)targs[lenindex];

		for (int j = 0; j < nspecs; j++)
			data[j+3] = (double)targs[lenindex+j+1];

		interp = kTruncate;
	}

	if (normalize)
		_normalize_table(data, len, 1.0);

	TablePField *table;
	if (interp == kInterp1stOrder)
		table = new TablePField(data, len);
	else if (interp == kTruncate)
		table = new TablePField(data, len, TablePField::Truncate);
	else // interp == kInterp2ndOrder
		table = new TablePField(data, len, TablePField::Interpolate2ndOrder);

	return createPFieldHandle(table);
}


// ---------------------------------------------------------------- tablelen ---
// length = tablelen(table_handle)

double
tablelen(const Arg args[], const int nargs)
{
	if (nargs != 1)
		return die("tablelen", "Takes only one argument: a valid table handle.");
	PField *pf = args[0];
	if (pf == NULL)
		return die("tablelen", "Argument must be a valid table handle.");
	const double *table = (double *) *pf;
	if (table == NULL)
		return die("tablelen", "Argument must be a valid table handle.");

	return (double) pf->values();
}


// ---------------------------------------------------------------- tablemin ---
// min = tablemin(table_handle)

double
tablemin(const Arg args[], const int nargs)
{
	if (nargs != 1)
		return die("tablemin", "Takes only one argument: a valid table handle.");
	PField *pf = args[0];
	if (pf == NULL)
		return die("tablemin", "Argument must be a valid table handle.");
	const double *table = (double *) *pf;
	if (table == NULL)
		return die("tablemin", "Argument must be a valid table handle.");
	const int len = pf->values();

	double min = 0.0, max = 0.0;
	get_table_bounds(table, len, min, max);

	return min;
}


// ---------------------------------------------------------------- tablemax ---
// max = tablemax(table_handle)

double
tablemax(const Arg args[], const int nargs)
{
	if (nargs != 1)
		return die("tablemax", "Takes only one argument: a valid table handle.");
	PField *pf = args[0];
	if (pf == NULL)
		return die("tablemax", "Argument must be a valid table handle.");
	const double *table = (double *) *pf;
	if (table == NULL)
		return die("tablemax", "Argument must be a valid table handle.");
	const int len = pf->values();

	double min = 0.0, max = 0.0;
	get_table_bounds(table, len, min, max);

	return max;
}


// --------------------------------------------------------------- copytable ---
// Make a copy of the given table, and return the copy.	If the optional
// <newsize> differs from the original size, resample the table to fit the
// new size.  By default, resampling is done with linear interpolation,
// but the optional <interp> argument allows for no interpolation.  Syntax:
//
//    newtable = copytable(table_to_copy, [newsize, [interp]])
//
// where <interp> can be "interp" or "nointerp".
//                                                           -JGG, 7/21/04

static void
_do_copy_table(const double *srcarray, const int srclen, double *destarray,
											const int destlen, const char *interp)
{
	if (destlen == srclen) {
		for (int i = 0; i < destlen; i++)
			destarray[i] = srcarray[i];
	}
	else {
		double incr = (double) srclen / (double) destlen;

		if (strcmp(interp, "nointerp") == 0) {
			double f = 0.0;
			for (int i = 0; i < destlen; i++) {
				int n = (int) f;
				destarray[i] = srcarray[n];
				f += incr;
			}
		}
		else if (strcmp(interp, "interp") == 0) {
			double frac, next, diff = 0.0;
			double f = 0.0;
			for (int i = 0; i < destlen; i++) {
				int n = (int) f;
				frac = f - (double) n;
				if (frac) {
					next = (n + 1 < srclen) ? srcarray[n + 1] : srcarray[srclen - 1];
					diff = next - srcarray[n];
				}
				destarray[i] = srcarray[n] + (diff * frac);
				f += incr;
			}
		}
		else
			assert("_do_copy_table: invalid interp" && 0);
	}
}

RtcHandle
copytable(const Arg args[], const int nargs)
{
	if (nargs < 1 || nargs > 3) {
		die("copytable",
			 "Usage: newtable = copytable(table_to_copy[, newsize, [interp]])");
		return NULL;
	}
	TablePField *oldtable = _getTablePField(&args[0]);
	if (oldtable == NULL) {
		die("copytable",
			 "Usage: newtable = copytable(table_to_copy[, newsize, [interp]])");
		return NULL;
	}

	int newsize = 0;
	if (nargs > 1) {
		if (!args[1].isType(DoubleType)) {
			die("copytable",
				 "Usage: newtable = copytable(table_to_copy[, newsize, [interp]])");
			return NULL;
		}
		newsize = args[1];
	}
	else
		newsize = oldtable->values();

	const char *interp = NULL;
	if (nargs > 2) {
		if (!args[2].isType(StringType)) {
			die("copytable",
				 "Usage: newtable = copytable(table_to_copy[, newsize, [interp]])");
			return NULL;
		}
		if (args[2] != "nointerp" && args[2] != "interp") {
			die("copytable",
				 "Valid interpolation types: \"interp\", \"nointerp\"");
			return NULL;
		}
		interp = args[2];
	}
	else
		interp = "interp";

	double *oldarray = new double[oldtable->values()];
	oldtable->copyValues(oldarray);
	double *newarray = new double[newsize];
	_do_copy_table(oldarray, oldtable->values(), newarray, newsize, interp);
	delete [] oldarray;
	TablePField *newtable = new TablePField(newarray, newsize);

	return createPFieldHandle(newtable);
}


// --------------------------------------------------------------- samptable ---
// Return the value of the table given in args[0] at the index given in
// args[1].  Normally, or if the "interp" tag is given, the return value is
// generated by linear interpolation between two adjacent array locations.
// If the "nointerp" tag is given, the index is truncated before reading
// the table value.  A negative index is an error; an index that is higher
// than the highest valid index returns the last table value.
// 
// Based on sampfunc and sampfunci in RTcmix 3.6 source.    -JGG, 6/21/04

double
samptable(const Arg args[], const int nargs)
{
	if (nargs > 1 && nargs < 4) {
		TablePField *table = _getTablePField(&args[0]);
		if (table) {
			bool interp = true;
			int indexarg = 1;
			if (args[1].isType(StringType)) {
				if (args[1] == "nointerp")
					interp = false;
				else if (args[1] != "interp")
					return die("samptable",
									"<interp> can be \"interp\" or \"nointerp\".");
				indexarg++;
			}
			if (args[indexarg].isType(DoubleType)) {
				if ((double) args[indexarg] < 0.0)
					return die("samptable", "Index must be zero or greater.");
				if (interp) {
					double index = args[indexarg];
					return table->doubleValue(index / (table->values() - 1));
				}
				else {
					int index = args[indexarg];					// truncate float
					return table->doubleValue(index);
				}
			}
		}
	}
	return die("samptable", "Usage: value = samptable(table, [interp,] index)");
}


// --------------------------------------------------------------- dumptable ---
// Dump the table given in args[0] to stdout, or if args[1] is a filename,
// to that file as text.  The dump format is a line of text for each table
// entry, which is printed with 6 digits of precision.

double
dumptable(const Arg args[], const int nargs)
{
	if (nargs < 1 || nargs > 2)
		return die("dumptable", "Usage: dumptable(table [, out_file])");
	PField *table = (PField *) args[0];
	if (table == NULL)
		return die("dumptable",
					  "First argument must be a handle to the table to dump.");

	FILE *f = NULL;
	if (nargs > 1) {
		if (!args[1].isType(StringType))
			return die("dumptable", "Second argument must be output file name.");
		const char *fname = (const char *) args[1];
		f = fopen(fname, "w+");
		if (f == NULL)
			return die("dumptable",
						  "Can't open file \"%s\": %s.", fname, strerror(errno));
	}
	else
		f = stdout;

	int chars = table->print(f);

	if (f != stdout)
		fclose(f);

	return (chars > 0) ? 0.0 : -1.0;
}


// --------------------------------------------------------------- plottable ---
// Plot the table given in args[0] using gnuplot.  Two optional arguments
// (in args[1] and args[2]):
//
//    <pause>       amount of time to show the gnuplot window [default: 10]
//    <plot_cmds>   commands passed to gnuplot [default: "with lines"]
//
// These optional arguments can appear in either order.  If there is a fourth
// argument, it's a string to incorporate into the title of the plot.
//
// Plot commands are most useful for controlling the type of plot, e.g., with
// lines, points, dots, impulses, etc.  See the gnuplot manual for more info.
//
// Examples:
//
//    tbl = maketable("wave", 1000, 1)      // a sine wave
//
//    plottable(a, 30)                      // plot on screen for 30 seconds,
//                                          //   using lines
//    plottable(a, 15, "with points")       // plot with points
//    plottable(a, "with linespoints")      // plot with both lines and points
//
// Other useful options are "with impulses," "with steps," "with dots,"
// "with boxes," etc.
//
// Note that gnuplot quits when <pause> seconds elapse.  But you can get rid
// of the window earlier by typing 'q' at it.
//
// NOTE: Under MacOS X, using Aquaterm to display output from gnuterm, <pause>
// has no effect.
//
// If something goes wrong with the gnuplot syntax, there'll be some leftover
// files in the /tmp directory, with names like "rtcmix_plot_data_xAb5x8."
//
//                                                  -JGG, 12/10/01, rev 6/04

#define DEFAULT_PLOTCMD "with lines"

double
plottable(const Arg args[], const int nargs)
{
#ifdef MACOSX
	static int plot_count = 1;
#endif
	int pause = 10;
	char cmd[256];
	const char *plotcmds = DEFAULT_PLOTCMD;

	if (nargs < 1 || nargs > 3)
		return die("plottable",
					  "Usage: plottable(table [, pause] [, plot_commands])");

	PField *table = (PField *) args[0];
	if (table == NULL)
		return die("plottable", "First argument must be the table to plot.");

	// 2nd and 3rd args are optional and can be in either order.  If there's
	// a 4th arg, it must be a string for the plot label.
	const char *table_name = "Table";
	if (nargs > 1) {
		if (args[1].isType(DoubleType))
			pause = (int) args[1];
		else if (args[1].isType(StringType))
			plotcmds = (const char *) args[1];
		else
			return die("plottable",
						  "Second argument can be pause length or plot commands.");
		if (nargs > 2) {
			if (args[2].isType(DoubleType))
				pause = (int) args[2];
			else if (args[2].isType(StringType))
				plotcmds = (const char *) args[2];
			else
				return die("plottable",
							  "Third argument can be pause length or plot commands.");
		}
		if (nargs > 3) {
			if (args[3].isType(StringType))
				table_name = (const char *) args[3];
		}
	}

	char data_file[256] = "/tmp/rtcmix_plot_data_XXXXXX";
	char cmd_file[256] = "/tmp/rtcmix_plot_cmds_XXXXXX";

	if (mkstemp(data_file) == -1 || mkstemp(cmd_file) == -1)
		return die("plottable", "Can't make temp files for gnuplot.");

	FILE *fdata = fopen(data_file, "w");
	FILE *fcmd = fopen(cmd_file, "w");
	if (fdata == NULL || fcmd == NULL)
		return die("plottable", "Can't open temp files for gnuplot.");

	int chars = table->print(fdata);
	fclose(fdata);
	
	if (chars <= 0)
		return die("plottable", "Cannot print this kind of table");

	fprintf(fcmd, 
#ifdef MACOSX	// NB: requires installation of Aquaterm and gnuplot >=3.8
		"set term aqua %d\n"
#endif
		"set title \"%s\"\n"
		"set grid\n"
		"plot '%s' notitle %s\n"
		"!rm '%s' '%s'\n"
		"pause %d\n",
#ifdef MACOSX
		plot_count++,
#endif
		table_name,
		data_file, plotcmds, data_file, cmd_file, pause);
	fclose(fcmd);

	snprintf(cmd, 255, "gnuplot %s &", cmd_file);
	cmd[255] = 0;
	system(cmd);

	return 0.0;
}

