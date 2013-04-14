#include "globals.h"
#include <stdio.h>
#include <ugens.h>

extern double m_open(float *, short, double *);

double
m_input(float *p,short n_args,double *pp)
{
	p[1] = (n_args > 1) ? p[1] : 0.;
	p[2] = 0;
	n_args = 3;
	rtcmix_advise(NULL,"Opening input file as unit %d\n",(int)p[1]);
	return m_open(p,n_args,pp);
}

double
m_output(float *p,short n_args,double *pp)
{
	int i;
	p[1] = (n_args > 1) ? p[1] : 1.;
	p[2] = 2;
	n_args = 3;
	i = p[0];
	rtcmix_advise("output", "Opening output file as unit %d\n",(int)p[1]);
  	return m_open(p,n_args,pp);
}
