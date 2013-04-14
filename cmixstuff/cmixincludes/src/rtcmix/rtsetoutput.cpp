/* RTcmix  - Copyright (C) 2000  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/
#include <RTcmix.h>
#include <Instrument.h>
#include <ugens.h>

int Instrument::rtsetoutput(float start, float dur, Instrument *theInst)
{
  // DS: Adding check to be sure rtoutput() did not fail.

  if (!RTcmix::outputOpen()) {
  	 die(theInst->name(),
		 "rtsetoutput: No output open for this instrument (rtoutput failed?)!");
	 return -1;
  }
  
  theInst->_start = start;
  theInst->_dur = dur;
  theInst->_nsamps = (int)(0.5 + dur * RTcmix::sr());
  
  return 0;
}
