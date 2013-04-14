/* RTcmix  - Copyright (C) 2000  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for 
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/
#include <RTcmix.h>
#include "prototypes.h"
#include <ugens.h>
#include <maxdispargs.h>
#include "mixerr.h"
#include <stdio.h>

int
RTcmix::dispatch(const char *func_label, const Arg arglist[], 
				 const int nargs, Arg *retval)
{
   /* Search non-rt and rt function lists for a match with <func_label>.
      If there is a match of either, checkfunc or checkInsts will call
      the appropriate function and return zero.  If both of these return
      non-zero status, print an error message if the error indicates that
	  no function was found.  Else just return proper status.
   */
   int status = checkfunc(func_label, arglist, nargs, retval);

   if (status != 0) {         /* search rt functions */
      mixerr = MX_NOERR;      /* clear old errors */
      checkInsts(func_label, arglist, nargs, retval);
      switch (mixerr) {
	  case MX_FNAME:		// Function not found
		 rtcmix_advise(NULL, 
				"Note: \"%s\" is an undefined function or instrument.",
				func_label);
		 break;
	  case MX_NOERR:		// Success
		 status = 0;
		 break;
	  case MX_FAIL:			// Some other failure.
	  default:
		 status = -1;
		 break;
	  }
   }
   return status;
}

#include <stdlib.h>

class Instrument;

double
dispatch(const char *str, double *pp, int n_args, void **inst)
{
	Arg retarg;
	double retval;

	Arg *rtcmixargs = new Arg[n_args];
	if (rtcmixargs == NULL) {
	  rterror("dispatch", "out of memory");
	  return -1.0;
	}
	for (int i = 0; i < n_args; i++) {
	  rtcmixargs[i] = pp[i];
	}

	int status = RTcmix::dispatch(str, rtcmixargs, n_args, &retarg);
	
	// RtcHandle state returned from dispatch()
	
	if (status == 0) {
		if (retarg.isType(DoubleType)) {
		   retval = (double) retarg;
		}
		else if (retarg.isType(StringType)) {
		   retval = STRING_TO_DOUBLE(retarg.string());
		}
		else if (retarg.isType(HandleType)) {
			// Retrieve instrument pointer from retval.handle.
			if (inst != NULL) {
				RtcHandle rethandle = (RtcHandle) retarg;
				if (rethandle->type == InstrumentPtrType)
					*inst = (Instrument *) rethandle->ptr;
			}
			retval = 0;
		}
		else {
			rterror("dispatch", "unhandled Arg return type!");
			retval = 0;
		}
	}
	else
		retval = (double) status;

	delete [] rtcmixargs;

	return retval;
}

// This is an extern "C" wrapper for trees.c

double parse_dispatch(const char *str, double *pp, int n_args, void **inst)
{
	return dispatch(str, pp, n_args, inst);
}

// This is an extern "C" wrapper for RT.c.  Make sure decl matches one in RT.xs

extern "C" {
int perl_dispatch(const char *str, const Arg args[], int n_args, Arg *retarg)
{
	return RTcmix::dispatch(str, args, n_args, retarg);
}
};
