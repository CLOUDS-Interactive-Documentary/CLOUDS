#ifndef _MOUSE_IPC_H_
#define _MOUSE_IPC_H_

#include <labels.h>

const int kSockPort = 9797;

enum {
	kPacketConfigureXLabelPrefix = 0,
	kPacketConfigureYLabelPrefix,
	kPacketConfigureXLabelUnits,
	kPacketConfigureYLabelUnits,
	kPacketConfigureXLabelPrecision,
	kPacketConfigureYLabelPrecision,
	kPacketUpdateXLabel,
	kPacketUpdateYLabel,
	kPacketMouseCoords,
	kPacketQuit
};

typedef struct {
	short type;								// from enum above
	short id;								// assigned by RTcmixMouse object
	union {
		int ival;							// kPacketConfigure?LabelPrecision
		double dval;						// kPacketUpdate?Label
		struct s_point {					// kPacketMouseCoords
			double x;
			double y;
		} point;
		char str[kPartLabelLength];	// kPacketConfigure?LabelPrefix and *Units
	} data;
} MouseSockPacket;						// should be 20 bytes

#endif // _MOUSE_IPC_H_
