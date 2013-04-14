#ifndef _DISPLAY_IPC_H_
#define _DISPLAY_IPC_H_

#include <labels.h>

const int kSockPort = 9798;

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
} DisplaySockPacket;						// should be 20 bytes

#endif // _DISPLAY_IPC_H_
