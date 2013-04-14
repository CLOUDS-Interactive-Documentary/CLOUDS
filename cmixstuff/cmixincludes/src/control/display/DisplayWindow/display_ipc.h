/* RTcmix - Copyright (C) 2011  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/
#ifndef _DISPLAY_IPC_H_
#define _DISPLAY_IPC_H_

const int kSockPort = 9798;

const int kPartLabelLength = 32;		// Total number of chars in label component
                                    // (i.e., prefix and units strings)

enum {
	kPacketConfigureLabelPrefix = 0,
	kPacketConfigureLabelUnits,
	kPacketConfigureLabelPrecision,
	kPacketUpdateLabel,
	kPacketQuit
};

typedef struct {
	short type;								// from enum above
	short id;								// assigned by RTcmixDisplay object
	union {
		int ival;							// kPacketConfigureLabelPrecision
		double dval;						// kPacketUpdateLabel
		char str[kPartLabelLength];	// kPacketConfigureLabelPrefix and *Units
	} data;
} DisplaySockPacket;

#endif // _DISPLAY_IPC_H_
