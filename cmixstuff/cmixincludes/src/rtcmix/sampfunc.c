#include "ugens.h"
#include "sfheader.h"
#include <stdio.h>
#include <sys/file.h>
#include <sys/types.h>

double
m_sampfunc(float p[], int n_args)
{
	int size, fnumber,skipin;
	double *thefunct;

	/* p0 == number of the function, p1 == amount to skip */


	fnumber = (int) p[0];
	thefunct = floc(fnumber);
	if (thefunct == NULL)
		return die("sampfunc", "You haven't created function number %d.\n", fnumber);
	size = fsize(fnumber);
	skipin = (p[1] < size) ? p[1] : size - 1; /* to avoid segvs */
	return(thefunct[skipin]);
}

double
m_sampfunci(float p[], int n_args)	/* interpolated version of sampfunc -- DAS 5/90 */
{
	int fnumber,size,skipin, skipin2;
	double *thefunct, frac;

	/* p0 == number of the function, p1 == amount to skip */

	fnumber = p[0];
	thefunct = floc(fnumber);
	if (thefunct == NULL)
		return die("sampfunci", "You haven't created function number %d.\n", fnumber);
	skipin = p[1];
	frac = p[1] - skipin;
	size = fsize(fnumber);
	if(skipin >= size-1) {         /* was (skipin >= size-2)  --JGG 7/99 */
		skipin = size - 1;
		skipin2 = skipin;
	}
	else
		skipin2 = skipin + 1;
	return(thefunct[skipin] + frac * (thefunct[skipin2]-thefunct[skipin]));
}

