/* RTcmix  - Copyright (C) 2000  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/
#include "globals.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#ifndef MAXMSP
#include <iostream>
#endif
#include "Instrument.h"
#include <RTcmix.h>
#include "rt.h"
#include "rtdefs.h"
#include "buffers.h"
#include <notetags.h>
#include <sndlibsupport.h>
#include <bus.h>
#include "BusSlot.h"
#include <assert.h>
#include <ugens.h>
#include "heap/heap.h"
#include <PField.h>
#include <PFieldSet.h>
#include <maxdispargs.h>
#include <PFBusData.h>

using namespace std;

#ifdef MAXMSP
InputState::InputState()
   : fdIndex(NO_DEVICE_FDINDEX), fileOffset(0), inputsr(0.0), inputchans(0), my_mm_buf(NULL)
#else
InputState::InputState()
	: fdIndex(NO_DEVICE_FDINDEX), fileOffset(0), inputsr(0.0), inputchans(0)
#endif
{
}

int				Instrument::RTBUFSAMPS = 0;
int				Instrument::NCHANS = 0;
float			Instrument::SR     = 0;

/* ----------------------------------------------------------- Instrument --- */
Instrument::Instrument()
	: _start(0.0), _dur(0.0), cursamp(0), chunksamps(0), i_chunkstart(0),
	  endsamp(0), output_offset(0), outputchans(0), _name(NULL),
	  needs_to_run(true), _nsamps(0), inputChainBuf(NULL)
{
   // Here we initialize the Instrument class globals (over and over, I know)
   // which replace the old system globals
   
   NCHANS = RTcmix::chans();
   SR = RTcmix::sr();
   RTBUFSAMPS = RTcmix::bufsamps();
   
   sfile_on = 0;                // default is no input soundfile

   outbuf = NULL;
   _busSlot = NULL;
   _pfields = NULL;

   for (int i = 0; i < MAXBUS; i++)
	   bufferWritten[i] = false;

	my_pfbus = PFBusData::connect_val;
	PFBusData::connect_val = -1;
}


/* ---------------------------------------------------------- ~Instrument --- */
Instrument::~Instrument()
{
	if (sfile_on)
		gone();                   // decrement input soundfile reference

	delete [] outbuf;

	RefCounted::unref(_busSlot);	// release our reference	

	delete _pfields;
	delete [] _name;
}

/* ------------------------------------------------------- setName --- */
/* This is only called by set_bus_config
*/

void Instrument::setName(const char *name)
{
	_name = new char[strlen(name) + 1];
	strcpy(_name, name);
}

/* ------------------------------------------------------- set_bus_config --- */
/* Set the _busSlot pointer to the right bus_config for this inst.
   Then set the inputchans and outputchans members accordingly.
  
   Instruments *must* call this from within their makeINSTNAME method. E.g.,
  
       WAVETABLE *inst = new WAVETABLE();
       inst->set_bus_config("WAVETABLE");
*/
void Instrument::set_bus_config(const char *inst_name)
{
  setName(inst_name);

  _busSlot = RTcmix::get_bus_config(inst_name);
  _busSlot->ref();		// add our reference to this
  
  _input.inputchans = _busSlot->in_count + _busSlot->auxin_count;
  outputchans = _busSlot->out_count + _busSlot->auxout_count;
}

double Instrument::s_dArray[MAXDISPARGS];

/* ------------------------------------------------------------ setup () --- */

// This function is now the one called by checkInsts().  It calls init().

int Instrument::setup(PFieldSet *pfields)
{
	_pfields = pfields;
	int nargs = MAXDISPARGS;
	update(s_dArray, nargs);
	int samps = init(s_dArray, pfields->size());
	_skip = int(SR / (float) resetval);
	if (_skip < 1)
		_skip = 1;
	else if (_skip > nSamps())
		_skip = nSamps();
	return samps;
}

/* ------------------------------------------------------------ update () --- */

// This function is called during run() by Instruments which want updated
// values for each pfield slot.  'nvalues' is size of p[].
// 'fields' is a bitmask of fields between [0] and [nvalues - 1] to fill.
// If fields is zero or missing, all of the first <nvalues> pfields are updated.
// Note that the bitmask supports only 31 pfields; if an instrument has
// more than that, don't use the fields argument.

int Instrument::update(double p[], int nvalues, unsigned fields)
{
	int n, args = _pfields->size();
	int frame = currentFrame();
	double percent = (frame == 0) ? 0.0 : (double) frame / nSamps();
	if (nvalues < args)
		args = nvalues;
	if (fields == 0) {
		for (n = 0; n < args; ++n)
			p[n] = (*_pfields)[n].doubleValue(percent);
	}
	else {
		for (n = 0; n < args; ++n) {
			if (fields & (1 << n))
				p[n] = (*_pfields)[n].doubleValue(percent);
		}
	}
	for (; n < nvalues; ++n)
		p[n] = 0.0;

	// my_pfbus is set using the bus_link() thing, check this bus for de-queing
	if (my_pfbus != -1) {
		if (PFBusData::dq_now[my_pfbus] == 1)
			setendsamp(0);
	}

	return 0;
}

// This alternative update method serves one main purpose: sometimes a pfield
// needs to span a duration that's different from the total note duration (for
// example, an envelope that controls only the input duration for an instrument
// that has a long ring-down duration.)  Calling this update() with the optional
// <totframes> argument makes the pfield span the duration corresponding to
// <totframes> instead of nSamps().  Note that it's much more efficient to grab
// all the pfields using the first update method than to use this update method
// multiple times, so try to avoid this method when you can use the other one.
//                                                              -JGG, 6/5/05

double Instrument::update(int index, int totframes)
{
	assert(index < _pfields->size());
	const int nframes = (totframes == 0) ? nSamps() : totframes;
	double percent = double(currentFrame()) / nframes;
	if (percent > 1.0)
		percent = 1.0;

	// my_pfbus is set using the bus_link() thing, check this bus for de-queing
	if (my_pfbus != -1) {
		if (PFBusData::dq_now[my_pfbus] == 1)
			setendsamp(0);
	}

	return (*_pfields)[index].doubleValue(percent);
}


/* ------------------------------------------------------------ init() --- */

// This function is now called by setup().

int Instrument::init(double p[], int n_args)
{
#ifndef MAXMSP
   cout << "You haven't defined an init member of your Instrument class!"
                                                                 << endl;
#endif
   return -1;
}

/* ----------------------------------------------------- configure(int) --- */

// This function performs any internal configuration and allocation needed
// to run the instrument following the call to init() and preceeding the
// first call to run().  It allows the majority of the memory allocation to
// be postponed until the note is run.  
//

/* 
   It is called by the RTcmix system only and never by derived classes.
   This method allocates the instrument's private interleaved output buffer
   and inits a buffer status array, then calls the virtual class-specific 
   configure(void) method.
   Note: We allocate here, rather than in ctor or init method, because this
   will mean less memory overhead before the inst begins playing.
*/

int Instrument::configure(int bufsamps)
{
	assert(outbuf == NULL);	// configure called twice, or recursively??
	outbuf = new BUFTYPE [bufsamps * outputchans];
	bzero((void *)outbuf, sizeof(BUFTYPE) * bufsamps * outputchans);
	return configure();		// Class-specific configuration.
}

/* ----------------------------------------------------- configure(void) --- */

// This is the virtual function that derived classes override.  We supply a
// default base class version because not all subclasses need/use this method.

int Instrument::configure(void)
{
	return 0;	// 0 is success, -1 is failure.
}

/* ------------------------------------------------------------------ run --- */
/* 
   This function is called by the RTcmix system only and never by derived
   classes.  This method calls the virtual class-specific run(void) method.
   Note that Instrument::run() no longer needs to allocate memory.  Now handled
   by Instrument::configure(int).
*/

int Instrument::run(bool needsTo)
{
   if (needsTo) {
	   obufptr = outbuf;

	   for (int i = 0; i < outputchans; i++)
		   bufferWritten[i] = false;
	   
	   int status = run();	// Class-specific run().

	   needs_to_run = false;

	   return status;
   }
   return 0;
}

/* ------------------------------------------------------------- schedule --- */
/* Called from checkInsts to place the instrument into the scheduler heap */

void Instrument::schedule(heap *rtHeap)
{
  // Calculate variables for heap insertion  
  float start = getstart();
  FRAMETYPE startsamp = (FRAMETYPE) (0.5 + start * SR);	// Rounded to nearest - DS
  
  if (RTcmix::interactive()) {
  	// Adjust start frame based on elapsed frame count
  	startsamp += RTcmix::getElapsedFrames();
  }
  
  FRAMETYPE endsamp = startsamp+nSamps();
  setendsamp(endsamp);  // used by intraverse.cpp
  
  // place instrument into heap
  rtHeap->insert(this, startsamp);
}

/* ----------------------------------------------------------------- exec --- */
/* Called from inTraverse to do one or both of these tasks:

      1) run the instrument for the current timeslice, and/or

      2) call addout to transfer a channel from the private instrument
         output buffer to one of the global output buffers.

   exec(), run() and addout() work together to decide whether a given
   call to exec needs to run or addout. The first call to exec for a
   given timeslice does both; subsequent calls during the same timeslice
   only addout.

   If we've written (i.e., called addout) all the buses for the current
   chunk, then return 1.  Else, return 0.
*/
int Instrument::exec(BusType bus_type, int bus)
{
   bool done;

//   printf("Instrument::exec(%p, bus_type %d, bus %d\n", this, (int)bus_type, bus);
   
   run(needs_to_run);	// Only does anything if true.

   addout(bus_type, bus);

   /* Decide whether we'll call run() next time. */
   done = true;
//   printf("Instrument::exec(%p) bufferWritten[0]: %d bufferWritten[1]: %d\n",
//		  this, (int) bufferWritten[0], (int) bufferWritten[1]);
   for (int i = 0; i < outputchans; i++) {
	   if (bufferWritten[i] == false) {
         done = false;
         break;
      }
   }
   if (done)
      needs_to_run = true;
//   printf("Instrument::exec(%p) returning %d\n", this, (int) needs_to_run);
   return (int) needs_to_run;
}


/* ------------------------------------------------------------- rtaddout --- */
/* Replacement for the old rtaddout (in rtaddout.C, now removed).
   This one copies (not adds) into the inst's outbuf. Later the
   scheduler calls the insts addout method to add outbuf into the 
   appropriate output buses. 
   Assumes that <samps> contains at least outputchans interleaved samples.
   Returns outputchans (i.e., number of samples written).
*/
int Instrument::rtaddout(BUFTYPE samps[])
{
	BUFTYPE *out = obufptr;
	for (int i = 0; i < outputchans; i++)
		out[i] = samps[i];
	obufptr += outputchans;
	return outputchans;
}

/* ------------------------------------------------------------ rtbaddout --- */
/* Block version of rtaddout.  Useful for instruments which generate samples
   in chunks, and can write them to the output buffer in one operation.
*/

int Instrument::rtbaddout(BUFTYPE samps[], int length)
{
	const int sampcount = length * outputchans;
	BUFTYPE *out = obufptr;
	for (int i = 0; i < sampcount; i++)
		out[i] = samps[i];
	obufptr += sampcount;
	return sampcount;
}


/* --------------------------------------------------------------- addout --- */
/* Add signal from one channel of instrument's private interleaved buffer
   into the specified output bus.
*/
void Instrument::addout(BusType bus_type, int bus)
{
   int      endframe, src_chan, buses;
   short    *bus_list;

   assert(bus >= 0 && bus < MAXBUS);

	if (bus_type != BUS_NONE_OUT) {

	   bus_list = _busSlot->getBusList(bus_type, &buses);

	   src_chan = -1;
	   for (int i = 0; i < buses; i++) {
		  if (bus_list[i] == bus) {
			 src_chan = i;
			 break;
		  }
	   }

	   assert(src_chan != -1);

	   endframe = output_offset + framesToRun();

		// Add outbuf to appropriate bus at offset
#ifdef DEBUG
		printf("%s::addout(this=%p %d, %d): doing normal addToBus\n", name(), this, (int)bus_type, bus);
#endif
		RTcmix::addToBus(bus_type, bus,
						 &outbuf[src_chan], output_offset,
						 endframe, outputchans);

		/* Show exec() that we've written this chan. */
		bufferWritten[src_chan] = true;
		//   printf("Instrument::addout(%p): bufferWritten[%d] now 1\n", this, src_chan);
	}
#ifdef DEBUG
	else {
		printf("%s::addout(this=%p %d, %d): NO-OP\n", name(), this, (int)bus_type, bus);
	}
#endif
}

/* ----------------------------------------------------------------- gone --- */
/* If the reference count on the file referenced by the instrument
   reaches zero, close the input soundfile and set the state to 
   make sure this is obvious.
*/
void Instrument::gone()
{
#ifdef DEBUG
   printf("Instrument::gone(this=%p): index %d\n", this, _input.fdIndex);
#endif

   if (_input.fdIndex >= 0) {
      RTcmix::releaseInput(_input.fdIndex);
      _input.fdIndex = NO_DEVICE_FDINDEX;
   }
}

/* ----------------------------------------------------------------- setChainedInputBuffer --- */
/* Notifies the Instrument that its input comes from the specified buffer */

int Instrument::setChainedInputBuffer(BUFTYPE *inputBuf, int inputChans)
{
	if (_input.inputchans != inputChans) {
		return die(name(), "Previous chained inst output (%d) != our input chans (%d)", inputChans, _input.inputchans);
	}
	inputChainBuf = inputBuf;
	return 0;
}

const PField &
Instrument::getPField(int index) const
{
	return (*_pfields)[index];
}

// Utility for returning a double array to the subclass.
// Returns array from PField if available.

const double *
Instrument::getPFieldTable(int index, int *tableLen) const
{
	const PField &pf = getPField(index);
	double *tableArray = (double *) pf;
	if (tableArray != NULL) {
		*tableLen = pf.values();
	}
	else
		*tableLen = 0;
	return tableArray;
}
