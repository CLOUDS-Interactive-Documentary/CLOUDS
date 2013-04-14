/* command_line.c */
/* to  return command line arguments */
#include <stdlib.h>
#include <string.h>
#include <ugens.h>
#include "../parser/rtcmix_parse.h"      /* for MAXARGS */

/* These are accessed only here and in Minc/parse_with_minc.c.  They're
   defined here, because the Perl-embedded executable also needs them,
   even though it doesn't use them (or the Minc commands in this file).
   The Perl interface stuff in the Perl dir insures that these commands
   can never be called.  -JGG
*/
char *aargv[MAXARGS + 1];
int aargc;


/* These functions return command line args passed to Minc.
   The first arg is arg 0, the second, arg 1, and so on.  This is
   not like the normal C convention, where the first arg is the
   name of the program.
*/

/* Return the p'th arg as a floating-point number. */
double f_arg(float p[], short n_args)
{
	return (((int)p[0]) < aargc) ? (atof(aargv[(int)p[0]])) : 0.0;
}

/* Return the p'th arg as an integer. */
double i_arg(float *p, short n_args)
{
	return (((int)p[0]) < aargc) ? (atoi(aargv[(int)p[0]])) : 0;
}

/* Return the p'th arg as a string. */
double s_arg(float *p,short n_args,double *pp)
{
	char *name;
	size_t temp = 0;
	if(((int)pp[0]) < aargc) {
		name = aargv[(int)pp[0]];
		temp = (size_t) strdup(name);
	}
	return (double) temp;  /* return string pointer, cast as a double, for Minc */
}

/* Return the number of args. */
double n_arg(float *p, short n_args)
{
	return(aargc);
}

