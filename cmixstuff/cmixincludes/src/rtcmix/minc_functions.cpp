#include "globals.h"
#include <stdio.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <math.h>
#include <ugens.h>
#include <spray.h>
#include <sfheader.h>
#include <maxdispargs.h>
#include <Option.h>
#include "prototypes.h"
#include <MMPrint.h>

#define ARRAY_SIZE 256
#define NUM_ARRAYS  32
static double minc_array[NUM_ARRAYS][ARRAY_SIZE],minc_array_size[NUM_ARRAYS];

extern "C" {

double m_boost(float p[])
{ return(boost(p[0])); }

double m_pchmidi(float p[])
{ return(pchmidi((int)p[0])); }

double m_cpsmidi(float p[])
{ return(cpspch(pchmidi(((int)p[0])))); }

double m_octmidi(float p[])
{ return(octmidi(p[0])); }

double m_midipch(float p[])
{ /*printf("%f\n",midipch(p[0]));*/ return(midipch(p[0])); }

double m_cpspch(float p[])
{ return(cpspch(p[0])); }

double m_pchoct(float p[])
{ return(pchoct(p[0])); }

double m_octpch(float p[])
{ return(octpch(p[0])); }

double m_pchcps(float p[])
{ return(pchcps(p[0])); }

double m_cpsoct(float p[])
{ return(cpsoct(p[0])); }

double m_octcps(float p[])
{ return(octcps(p[0])); }

double m_octlet(float p[], int nargs, double pp[])
{
	if (nargs > 0 && pp[0] > 0.0)
		return octlet((unsigned char *) DOUBLE_TO_STRING(pp[0]));
	die("octlet", "usage: octlet(\"pitch\"), where pitch is \"Ab3\", etc.");
	return 8.00;
}

double m_cpslet(float p[], int nargs, double pp[])
{
	if (nargs > 0 && pp[0] > 0.0)
		return cpslet((unsigned char *) DOUBLE_TO_STRING(pp[0]));
	die("cpslet", "usage: cpslet(\"pitch\"), where pitch is \"Ab3\", etc.");
	return 0.0;
}

double m_pchlet(float p[], int nargs, double pp[])
{
	if (nargs > 0 && pp[0] > 0.0)
		return pchlet((unsigned char *) DOUBLE_TO_STRING(pp[0]));
	die("pchlet", "usage: pchlet(\"pitch\"), where pitch is \"Ab3\", etc.");
	return 8.00;
}

double m_pchadd(float p[], int nargs, double pp[])
{
	return pchoct(octpch(pp[0]) + octpch(pp[1]));
}

double m_rand()
{ return rrand(); }

double m_random()
{ return (rrand() * 0.5) + 0.5; }

double m_srand(float p[], int n_args)
{
   unsigned int randx;

   if (n_args == 0) {
      struct timeval tv;
      gettimeofday(&tv, NULL);
      randx = (unsigned int) tv.tv_usec;
	  rtcmix_advise("srand", "Seed initialized internally with value %u", randx);
   }
   else
      randx = (unsigned int) p[0];
   srrand(randx);
   return 0.0;
}

double m_time_beat(float p[])
{ float time_beat(float); return(time_beat(p[0])); }

double m_beat_time(float p[])
{ float beat_time(float); return(beat_time(p[0])); }

double m_trunc(float p[], int nargs, double pp[])
{ return((double)(long long)(pp[0])); }		// D.S. 08/09

double m_ampdb(float p[])
{ return(ampdb(p[0])); }

double m_dbamp(float p[])
{ return(dbamp(p[0])); }

double m_stringify(float p, int n_args, double pp[])
{
	/* coerces a string passed in from Minc in quotes in p[0]
	   to a 'floating point' pointer suitable for use in
	   further cmix calls */

	return STRINGIFY(pp[0]);
}

/* get the transposed length for a given input length at interval */
/* p0 = orig len   p1 = interval in octave point pc */
double m_translen(float p[], int n_args, double pp[])
{
	double origLen = pp[0];
	// This is exactly how TRANS does it, to assure a match.
	double interval = octpch(pp[1]);
	double newLen = origLen / (cpsoct(10.0 + interval) / cpsoct(10.0));
	return newLen;
}

double m_log(float p[], int n_args)
{
   double val;

   val = log10((double)p[0]);

   return(val);
}

double m_ln(float p[], int n_args)
{
   double val;

   val = log((double)p[0]);

   return(val);
}

double m_pow(float p[], int n_args, double pp[])
{
	double val;

	val = pow(pp[0], pp[1]);
	return(val);
}

double m_round(float p[], int n_args)
{
	int val = p[0] + .5;
	return (double) val;
}

double m_wrap(float p[], int n_args)
{
	/* keeps value between 0 and p[1] */
	int val = p[0];
	int range = (int) p[1];
	if(p[1] >= 1.0) {
		while(val > range) val -= range;
		while(val < 0) val += range;
	}
	else if(p[1] <= -1.0) {
		while(val < range) val -= range;
		while(val > 0) val += range;
	}
	else val = 0;
	return (double) val;
}

double m_print(float p[], int n_args, double pp[])
{
// BGG -- NOTE:  This is now done in parser/minc/builtin.c
	printf("Value = %10.8f\n", pp[0]);
	return 0.0;
}

double m_abs(float p[], int n_args)
{
	return((p[0] >= 0.0) ? p[0] : -(p[0]));
}

double m_mod(float p[], int n_args)
{
	int i,j;
	i = (int)p[0] % (int)p[1];
	return((float)i);
}

double m_max(float p[], int n_args)
{
	int i;
	float max = -1e+22;
	for(i=0; i<n_args; i++)
		if(p[i] > max) max=p[i];
	return(max);
}

double m_min(float p[], int n_args)
{
	int i;
	float min = 1e+22;
	for(i=0; i<n_args; i++)
		if(p[i] < min) min=p[i];
	return(min);
}

double m_exit(float p[], int n_args, double pp[])
{
	const char *message = DOUBLE_TO_STRING(p[0]);
	if (message) fprintf(stderr, "%s\n", message);
	closesf();
	return 0.0;
}

double m_load_array(float p[], int n_args, double pp[])
{
	int i,j;
	if(n_args > ARRAY_SIZE) n_args = ARRAY_SIZE+1;
	j = p[0];
	for(i=1; i<n_args; i++) minc_array[j][i-1] = pp[i];
	minc_array_size[j] = n_args;
	return(n_args-1);
}

double m_get_array(float p[], int n_args)
{
	int i, size, index;

	i = p[0];
	size = minc_array_size[i];
	index = (int) p[1];

	if (index >= size)
		index = size - 1;

	return (minc_array[i][index]);
}

double m_put_array(float p[], int n_args)
{ /* to load a single value from minc */
	int i,j;
	i=p[0];
	j=p[1];
	minc_array[i][j] = p[2];
	minc_array_size[i]=minc_array_size[i] > j+1 ? minc_array_size[i] : j+1;
	if(j < ARRAY_SIZE) return(j);
	else return(-1);
}

double m_get_sum(float p[], int n_args)
{
	int i,j,k;
	float sum;
	i = p[0];
	if((j=p[1]) >= minc_array_size[i]) return(0);
	for(k=0,sum=0; k<=j; k++) sum += minc_array[i][k];
	return(sum);
}

double m_get_size(float p[], int n_args)
{
	/* returns same value as load_array would */
	return((double)minc_array_size[(int)p[0]]-1);
}

double m_getpch(float p[], int n_args, double pp[])
{
	int pchfd;
	int frameno,nbframe;
	long skipbytes;
	float vals[200]; /* enough for 46 poles + 4 data values */
	char  *input;

/*	p0=name of pchanal file; p1=framenumber to get */

	input = DOUBLE_TO_STRING(pp[0]);

	if((pchfd = open(input,0)) < 0)
		die("getpch", "Can't open pitch analysis file");

	nbframe = 2*FLOAT; 
	frameno = (int)p[1];

	skipbytes = frameno * nbframe;
	if (lseek(pchfd, skipbytes, 0) < 0)
		die("getpch", "Error on pchanal lseek");

	if (read(pchfd, (char *)vals, nbframe) != nbframe)
		die("getpch", "Bad read on pchanal file");

	close(pchfd);

	return((double) vals[0]);
}

double m_getamp(float p[], int n_args, double pp[])
{
	int pchfd;
	int frameno,nbframe;
	long skipbytes;
	float vals[200]; /* enough for 46 poles + 4 data values */
	char  *input;

/*	p0=name of pchanal file; p1=framenumber to get */

	input = DOUBLE_TO_STRING(pp[0]);

	if((pchfd = open(input,0)) < 0)
		die("getamp", "Can't open pitch analysis file");

	nbframe = 2*FLOAT; 
	frameno = (int)p[1];

	skipbytes = frameno * nbframe;
	if (lseek(pchfd, skipbytes, 0) < 0)
		die("getamp", "Error on pchanal lseek");

	if (read(pchfd, (char *)vals, nbframe) != nbframe)
		die("getamp", "Bad read on pchanal file");

	close(pchfd);

	return((double) vals[1]);
}

double str_num(float p[], int n_args, double pp[])
{
	char *name;
	int i,j;
	char buf[16];

	for (j=0; j<n_args; j=j+2) {
		buf[0] = 0;
		name = DOUBLE_TO_STRING(pp[j]);
		if (((j+1) < (n_args-1)) || !(n_args % 2))
			sprintf(buf, "%g", pp[j+1]);
#ifdef MAXMSP
		MMPrint::mm_print_ptr += sprintf(MMPrint::mm_print_ptr, "%s%s", name, buf);
#else
		printf("%s%s", name, buf);
#endif
	}
#ifdef MAXMSP
	MMPrint::mm_print_ptr += (sprintf(MMPrint::mm_print_ptr, "\n")+1);
#else
	printf("\n");
#endif
	return 0.0;
}

double m_print_is_on(float p[], int n_args)
{
	if (n_args > 0)
		set_bool_option(kOptionPrint, (int)p[0]); // this does an int...
	else
		set_bool_option(kOptionPrint, MMP_PRINTALL); // BGG not really "bool" now
	return 1.0;
}

double m_print_is_off(float p[], int n_args)
{
	set_bool_option(kOptionPrint, 0);
	return 0.0;
}

static struct slist slist[NUM_SPRAY_ARRAYS];

double m_get_spray(float p[], int n_args)
{
	int table_num = (int) p[0];

   if (table_num < 0 || table_num >= NUM_SPRAY_ARRAYS)
      die("get_spray", "Spray table number must be between 0 and %d.",
                                                   NUM_SPRAY_ARRAYS - 1);
   if (slist[table_num].size == 0)
      die("get_spray", "Spray table number %d was not initialized.", table_num);

	return (double) (spray(&slist[table_num]));
}

double m_spray_init(float p[], int n_args)
{
	int   table_num, size;
   unsigned int seed;

	table_num = (int) p[0];
   if (table_num < 0 || table_num >= NUM_SPRAY_ARRAYS)
      die("spray_init", "Spray table number must be between 0 and %d.",
                                                   NUM_SPRAY_ARRAYS - 1);
   size = (int) p[1];
   if (size < 2 || size > MAX_SPRAY_SIZE)
      die("spray_init", "Spray table size must be between 2 and %d.",
                                                   MAX_SPRAY_SIZE);
   seed = (unsigned int) p[2];

	sprayinit(&slist[table_num], size, seed);
	return 0.0;
}


static int line_array_size = 1000;      /* modified by m_setline_size */

double m_setline_size(float p[], int n_args)
{
	if (p[0] < 2)
		die("setline_size", "Setline array size must be at least 2!");
	line_array_size = p[0];
	rtcmix_advise("setline_size", "Setline arrays will have %d elements.",
                                                      line_array_size);
	return 0.0;
}


double m_setline(float p[], int n_args)
{
	float	pp[MAXDISPARGS];
	int	i;

	pp[0] = 1;
	pp[1] = 18;           /* not sure whether this should be gen18 or gen24 */
	pp[2] = line_array_size;

	for (i = 0; i < n_args; i++)
		pp[i+3] = p[i];

	makegen(pp, n_args+3, NULL);

	return 0.0;
}

/* create exponential curve */

double m_setexp(float p[], int n_args)
{
	float	pp[MAXDISPARGS];
	float   prevloc, minloc, locRange;
	int	i;

	pp[0] = 1;
	pp[1] = 5;           /* gen 5 creates exponential curve */
	pp[2] = line_array_size;

    minloc = p[0];	/* loc from first <loc, value> pair */
	prevloc = minloc;
    locRange = p[n_args - 2] - minloc;	/* delta betw. first & last loc */

	/* copy args, but guard against negatives and zeroes! */
	/* we convert <loc, val>, <loc2, val2>, ... into
	 * val, deltaloc1, val2, deltaloc2, ... 
	 */
	for (i = 1; i < n_args - 1; i += 2)
	{
		float val = p[i];
		float loc = p[i+1];
		pp[i+2] = val > 0.0f ? val : 0.00001;
		pp[i+3] = (int) (line_array_size * ((loc - prevloc) / locRange));
		prevloc = loc;
	}
	/* add final value to arg list */
	pp[i+2] = p[i] > 0.0f ? p[i] : 0.00001;

	makegen(pp, n_args+2, NULL);

	return 0.0;
}

/* Note: MIX used 200 as default; WAVETABLE used 1000. It needs a faster
   rate, since it's changing an oscillator at that speed, rather than just
   an amplitude envelope. I'm just making 1000 the default for everyone.  -JGG
*/
int resetval = 1000;                 /* modified by m_reset; read by insts */

double m_reset(float p[], int n_args)
{
	if (p[0] <= 0)
		die("reset", "Control rate must be greater than 0!");
	resetval = p[0];
	rtcmix_advise("reset", "Control rate set to %d updates per second.", resetval);

	return 0.0;
}

/* returns a randomly-interpolated value between its two input values */
double m_irand(float p[], int n_args, double pp[])
{
	double min, max;
	double frac = m_random();
	if (n_args == 1) {
		min = 0.0;
		max = pp[0];
	}
	else if (n_args == 2) {
		min = pp[0];
		max = pp[1];
	}
	else {
		die("irand", "Usage: irand([min,] max)\nDefault <min> is zero\n");
		return -1.0;
	}
	return (frac * min) + (1.0 - frac) * max;
}

double m_trand(float p[], int n_args, double pp[])
{
	if (n_args > 2 || n_args == 0)
		die("trand", "Usage: trand([min,] max)\nDefault <min> is zero\n");

	int trunc = m_irand(p, n_args, pp);

	// this is to make the lower bound excluded for negative lower bound
	if (p[0] < 0 || p[1] < 0) {
		if (n_args == 2 && (int)p[0] != (int)p[1])
			while (trunc == (int)p[0])
				trunc = m_irand(p, n_args, pp);
	}

	return (double) trunc;
}

}	// end extern "C"

//-----------------------------------------------------------------------------
// C++ functions

extern "C" {
	double m_pickrand(const Arg args[], const int nargs);
	double m_pickwrand(const Arg args[], const int nargs);
	double get_time(); // returns number of seconds that have elapsed
}

// pickrand returns random choice from its arguments
double m_pickrand(const Arg args[], const int nargs) 
{
	if (nargs == 0)
		die("pickrand", "Must have at least one value to choose from!");

//FIXME: would be nice to have the array-unpacking in a function for
// use by others, instead of cutting/pasting it into each function. -JG
	// Load all args, including ones from flattened arrays, into xargs.
	double xargs[MAXDISPARGS];
	int nxargs = 0;
	for (int i = 0; i < nargs; i++) {
		if (args[i].isType(ArrayType)) {
			// NB: currently if we get this far, the array must contain only
			// doubles; otherwise error (e.g., in src/parser/minc/callextfunc.cpp).
			Array *a = args[i];
			const int alen = a->len;
			const double *adata = a->data;
			for (int j = 0; j < alen; j++) {
				xargs[nxargs++] = adata[j];
				if (nxargs == MAXDISPARGS)
					return die("pickrand", "Exceeded maximum number of arguments (%d)", MAXDISPARGS);
			}
		}
		else if (args[i].isType(DoubleType)) {
			xargs[nxargs++] = args[i];
			if (nxargs == MAXDISPARGS)
				return die("pickrand", "Exceeded maximum number of arguments (%d)", MAXDISPARGS);
		}
		else
			return die("pickrand", "Arguments must be numbers or arrays of numbers");
	}
	float rindex;
	rindex = (m_random() * nxargs) - 0.000001; // 0 to 1.9999 for 2 args
	return xargs[(int) rindex];
}

// pickwrand returns choice based on <value, probability> pairs
double m_pickwrand(const Arg args[], const int nargs) 
{
	if (nargs == 0 || (nargs & 1))
		die("pickwrand", "Arguments must be in <value, probability> pairs!");

	// Load all args, including ones from flattened arrays, into xargs.
	double xargs[MAXDISPARGS];
	int nxargs = 0;
	for (int i = 0; i < nargs; i++) {
		if (args[i].isType(ArrayType)) {
			// NB: currently if we get this far, the array must contain only
			// doubles; otherwise error (e.g., in src/parser/minc/callextfunc.cpp).
			Array *a = args[i];
			const int alen = a->len;
			const double *adata = a->data;
			for (int j = 0; j < alen; j++) {
				xargs[nxargs++] = adata[j];
				if (nxargs == MAXDISPARGS)
					return die("pickwrand", "Exceeded maximum number of arguments (%d)", MAXDISPARGS);
			}
		}
		else if (args[i].isType(DoubleType)) {
			xargs[nxargs++] = args[i];
			if (nxargs == MAXDISPARGS)
				return die("pickrand", "Exceeded maximum number of arguments (%d)", MAXDISPARGS);
		}
		else
			return die("pickrand", "Arguments must be numbers or arrays of numbers");
	}

	// sum up chances
	float totalchance = 0, psum = 0;
	for (int n = 1; n < nxargs; n += 2)
		totalchance += xargs[n];
	const float rindex = m_random() * totalchance;
	for (int n = 1; n < nxargs; n += 2) {
		psum += xargs[n];
		if (rindex <= psum)
			return xargs[n - 1];
	}
	return xargs[nxargs - 1];
}

double get_time() {
	double tval = RTcmix::getElapsedFrames()/RTcmix::sr();
	return tval;
}
