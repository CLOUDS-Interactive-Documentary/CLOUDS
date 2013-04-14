// rt_ug_intro.cpp

#include	"ugens.h"
#include <ug_intro.h>
#include <RTcmix.h>

extern "C" {

void
rt_ug_intro()
{
	UG_INTRO("rtsetparams", RTcmix::rtsetparams);
	UG_INTRO("set_option",RTcmix::set_option);
	UG_INTRO("bus_config", RTcmix::bus_config);
	UG_INTRO("rtinput",RTcmix::rtinput);
	UG_INTRO("rtoutput",RTcmix::rtoutput);
	UG_INTRO("CHANS",RTcmix::input_chans);  /* returns channels for rtinput files */
	UG_INTRO("DUR",RTcmix::input_dur);  /* returns duration for rtinput files */
	UG_INTRO("SR",RTcmix::input_sr);  /* returns rate for rtinput files */
	UG_INTRO("PEAK",RTcmix::input_peak);  /* returns peak amp for rtinput files */
	UG_INTRO("LEFT_PEAK",RTcmix::left_peak);
	UG_INTRO("RIGHT_PEAK",RTcmix::right_peak);
}

};
