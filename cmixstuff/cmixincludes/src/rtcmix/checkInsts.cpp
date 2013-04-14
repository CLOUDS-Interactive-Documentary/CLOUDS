/* RTcmix  - Copyright (C) 2000  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for 
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/
#include <RTcmix.h>
#include "prototypes.h"
#include <maxdispargs.h>
#include <pthread.h>
#include <Instrument.h>
#include <PField.h>
#include <PFieldSet.h>
#include "utils.h"
#include "rt.h"
#include "rtdefs.h"
#include "mixerr.h"
#include "rtcmix_types.h"
#include "ugens.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <Option.h>
#include <MMPrint.h>

//#define DEBUG

static void
printargs(const char *instname, const Arg arglist[], const int nargs)
{
   int i;
   Arg arg;

   if (Option::print() >= MMP_PRINTALL) {
#ifdef MAXMSP
		MMPrint::mm_print_ptr += sprintf(MMPrint::mm_print_ptr, "========<rt-queueing>=======\n")+1;
		MMPrint::mm_print_ptr += sprintf(MMPrint::mm_print_ptr, "%s:  ", instname);
		for (i = 0; i < nargs; i++) {
			arglist[i].printInline(stdout);
		}
		MMPrint::mm_print_ptr += sprintf(MMPrint::mm_print_ptr, "\n")+1;
#else
      printf("========<rt-queueing>=======\n");
      printf("%s:  ", instname);
      for (i = 0; i < nargs; i++) {
         arglist[i].printInline(stdout);
      }
      putchar('\n');
      fflush(stdout);
#endif
   }
}

static InstCreatorFunction
findInstCreator(rt_item *inList, const char *inInstName)
{
	rt_item *item = inList;       // rt_item defined in rt.h
	while (item) {
		if (strcmp(item->rt_name, inInstName) == 0) {
			return item->rt_ptr;
		}
		item = item->rt_next;
	}
	return NULL;
}

// Load the argument list into a PFieldSet, hand to instrument, and call setup().  Does not destroy
// the instrument on failure.

static double loadPFieldsAndSetup(const char *inName, Instrument *inInst, const Arg arglist[], const int nargs)
{
	// Load PFieldSet with ConstPField instances for each
	// valid p field.
	mixerr = MX_NOERR;
	PFieldSet *pfieldset = new PFieldSet(nargs);
	if (!pfieldset) {
		mixerr = MX_FAIL;
		return DONT_SCHEDULE;
	}
	for (int arg = 0; arg < nargs; ++arg) {
		const Arg &theArg = arglist[arg];
		switch (theArg.type()) {
			case DoubleType:
				pfieldset->load(new ConstPField((double) theArg), arg);
				break;
			case StringType:
				pfieldset->load(new StringPField(theArg.string()), arg);
				break;
			case HandleType:
			{
				RtcHandle handle = (RtcHandle) theArg;
				if (handle != NULL) {
					if (handle->type == PFieldType) {
						assert(handle->ptr != NULL);
						pfieldset->load((PField *) handle->ptr, arg);
					}
					else if (handle->type == InstrumentPtrType) {
						assert(handle->ptr != NULL);
						pfieldset->load(new InstPField((Instrument *)handle->ptr), arg);
					}
					else {
						rtcmix_warn(inName, "arg %d: Unsupported handle type!\n", arg);
						mixerr = MX_FAIL;
					}
				}
				else {
					rtcmix_warn(inName, "arg %d: NULL handle!\n", arg);
					mixerr = MX_FAIL;
				}
			}
				break;
			case ArrayType:
				rtcmix_warn(inName,
						"arg %d: Array (list) types cannot be passed to instruments.\n\tUse maketable(\"literal\", ...) instead.\n", arg);
				mixerr = MX_FAIL;
				break;
			default:
				rtcmix_warn(inName, "arg %d: Illegal argument type!\n", arg);
				mixerr = MX_FAIL;
				break;
		}
	}
	if (mixerr == MX_FAIL) {
		delete pfieldset;
		return DONT_SCHEDULE;
	}
	return (double) inInst->setup(pfieldset);
}

double
RTcmix::checkInsts(const char *instname, const Arg arglist[],
				   const int nargs, Arg *retval)
{
	Instrument *Iptr = NULL;;

#ifdef DEBUG
   printf("ENTERING checkInsts() FUNCTION -----\n");
#endif

#ifndef MAXMSP
	if (!rtsetparams_was_called()) {
		die(instname, "You did not call rtsetparams!");
		return -1;
	}
#endif

	mixerr = MX_FNAME;

	*retval = 0.0;	// Default to float 0

	InstCreatorFunction instCreator = findInstCreator(rt_list, instname);

	if (instCreator) {
		
		::printargs(instname, arglist, nargs);

		/* Create the Instrument */

		Iptr = (*instCreator)();

		if (!Iptr) {
			mixerr = MX_FAIL;
			return -1;
		}

		Iptr->ref();   // We do this to assure one reference

		double rv = loadPFieldsAndSetup(instname, Iptr, arglist, nargs);
		
        if (rv != (double) DONT_SCHEDULE) { // only schedule if no setup() error
			// For non-interactive case, configure() is delayed until just
			// before instrument run time.
			if (rtInteractive) {
			   if (Iptr->configure(RTBUFSAMPS) != 0) {
				   rv = DONT_SCHEDULE;	// Configuration error!
			   }
			}
		}
		// Clean up if there was an error.
		else {
// BGG -- may have to comment this out for maxmsp
			Iptr->unref();
			mixerr = MX_FAIL;
			return rv;
		}

		/* schedule instrument */
		Iptr->schedule(rtHeap);

		mixerr = MX_NOERR;

		// Create RtcHandle for Iptr on return
		*retval = createInstHandle(Iptr);
#ifdef DEBUG
		 printf("EXITING checkInsts() FUNCTION -----\n");
#endif
		 return rv;
	}

   return 0.0;
}

static RtcHandle mkusage()
{
	die("makeinstrument", "Usage: makeinstrument(\"INSTRUMENTNAME\", p[0], p[1], ...)");
	return 0;
}

extern "C" {
	RtcHandle makeinstrument(const Arg args[], const int nargs);
}

// makeinstrument():  Create an instrument and return it as a handle.  Do not configure or schedule the instrument.
// This is specifically for use with the CHAIN instrument.

RtcHandle
makeinstrument(const Arg arglist[], const int nargs)
{
	if (nargs < 4) {
		return mkusage();
	}
	
	Instrument *Iptr = NULL;;
	const char *instName = (const char *) arglist[0];
	
	if (!instName)
		return mkusage();
	
	InstCreatorFunction instCreator = findInstCreator(RTcmix::rt_list, instName);
	
	if (instCreator) {
		/* Create the Instrument */
		
		Iptr = (*instCreator)();
		
		if (!Iptr) {
			mixerr = MX_FAIL;
			return 0;
		}
		
		Iptr->ref();   // We do this to assure one reference
		
		// Call same function as for checkInsts(), but strip off initial instrument name arg.
		
		double rv = loadPFieldsAndSetup(instName, Iptr, &arglist[1], nargs-1);
		
        if (rv == (double) DONT_SCHEDULE) {
			Iptr->unref();
			mixerr = MX_FAIL;
			return 0;
		}
		mixerr = MX_NOERR;
		
		// Create RtcHandle for Iptr on return
		return createInstHandle(Iptr);
	}
	return 0;
}
