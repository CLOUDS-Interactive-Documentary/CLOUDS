// RTcmix  - Copyright (C) 2005  The RTcmix Development Team
// See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
// the license to this software and for a DISCLAIMER OF ALL WARRANTIES.

//  Python addition to RTcmix

#include "pyRTcmix.h"

extern "C" {
#include "../parser/rtcmix_parse.h"
}

// Constructor with default SR, NCHANS, and RTBUFSAMPS
PYRTcmix::PYRTcmix() : RTcmix()
{
}

// Constructor with settable SR, NCHANS; default RTBUFSAMPS
PYRTcmix::PYRTcmix(float sr, int nchans) : RTcmix(sr, nchans)
{
}

// Constructor with settable SR, NCHANS, RTBUFSAMPS, and options
PYRTcmix::PYRTcmix(float sr, int nchans, int bsize,
		const char *opt1, const char *opt2, const char *opt3)
	: RTcmix(sr, nchans, bsize, opt1, opt2, opt3)
{
}

void PYRTcmix::pythonparse(char *inBuf)
{
	python_parse_buf(inBuf);
}

double PYRTcmix::getfval(char *val)
{
	return get_python_fval(val);
}

