//  pRTcmix.C -- Perl addition to RTcmix

/* RTcmix  - Copyright (C) 2000  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/

#include "pRTcmix.h"

extern "C" {
#include "../parser/rtcmix_parse.h"
}

//  Constructor with default SR, NCHANS, and RTBUFSAMPS
PRTcmix::PRTcmix() : RTcmix()
{
}

//  Constructor with settable SR, NCHANS; default RTBUFSAMPS
PRTcmix::PRTcmix(float tsr, int tnchans) : RTcmix(tsr, tnchans)
{
}

//  Constructor with settable SR, NCHANS, RTBUFSAMPS, and options
PRTcmix::PRTcmix(float tsr, int tnchans, int bsize,
				 const char *opt1, const char *opt2, const char *opt3)
	: RTcmix(tsr, tnchans, bsize, opt1, opt2, opt3)
{
}

void PRTcmix::perlparse(char *inBuf)
{
	perl_parse_buf(inBuf);
}

double PRTcmix::getfval(char *val)
{
	double retval;
	retval = get_perl_fval(val);
	return retval;
}

