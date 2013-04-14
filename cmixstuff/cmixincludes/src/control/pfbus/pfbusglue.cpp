/* RTcmix  - Copyright (C) 2004  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <rtcmix_types.h>
#include <PField.h>
#include <PFBusPField.h>
#include <utils.h>   // in ../../rtcmix
#include <ugens.h>		// for warn, die


// -----------------------------------------------------------------------------
//
//		stream = makeconnection("pfbus", pfbus_no, default);
//
//		<pfbus_no>		the max object inlet number associated with this handle
//    <default>      default value (delivered while a PFSCHED() instrument
//								hasn't started executing yet)
//
//		score to use this might be:
//
//		sent_envelope = maketable("line", 1000, 0,0.0,  1,1.0)
//		PFSCHED(0, 2.1, 1, sent_envelope)
//		value = makeconnection("pfbus", 1, 0.0)
//		INSTRUMENT(0, 5.6, value, p3)
//
//		the "value" PField for INSTRUMENT will be 0.0 until the PFSCHED
//		note is sent (has to be time 0 for now).  At that point,
//		the delayed_envelope table will start delivering values for 2.1 seconds
//		through the PFSCHED instrument.  The values will be set on pfbus #1
//
//		PFSCHED now has to operate at time 0; usually this will be sent
//		dynamically from an iOS or Max/MSP environment
//
//				BGG 11/2009, 12/2012


static RTNumberPField *
_pfbus_usage()
{
	die("makeconnection (pfbus)",
		"Usage: makeconnection(\"pfbus\", pfbus #, default)");
	return NULL;
}

// BGG mm
// can't use the 'normal' create_pfield() because of conflict with
// inletglue.cpp function (dyn loading keeps them separate, but we
// don't dynload in max/msp
static RTNumberPField *
#ifdef MAXMSP
create_pfbus_pfield(const Arg args[], const int nargs)
#else
create_pfield(const Arg args[], const int nargs)
#endif
{
	int pfbusval;
	double defaultval;

	if (nargs < 2)
		return _pfbus_usage();

	if (args[0].isType(DoubleType))
		pfbusval = (int)(args[0]);
	else
		return _pfbus_usage();

	if (args[1].isType(DoubleType))
		defaultval = args[1];
	else
		return _pfbus_usage();

	return new PFBusPField(pfbusval, defaultval);
}

// The following functions are the publicly-visible ones called by the
// system.

extern "C" {
#ifdef MAXMSP
	RtcHandle create_pfbus_handle(const Arg args[], const int nargs);
#else
	RtcHandle create_handle(const Arg args[], const int nargs);
	int register_dso();
#endif
};

RtcHandle
#ifdef MAXMSP
create_pfbus_handle(const Arg args[], const int nargs)
{
	PField *pField = create_pfbus_pfield(args, nargs);
#else
create_handle(const Arg args[], const int nargs)
{
	PField *pField = create_pfield(args, nargs);
#endif
	RtcHandle handle = NULL;
	if (pField != NULL) {
		handle = createPFieldHandle(pField);
	}
	return handle;
}

#ifndef MAXMSP
int register_dso()
{
   return 0;
}
#endif
