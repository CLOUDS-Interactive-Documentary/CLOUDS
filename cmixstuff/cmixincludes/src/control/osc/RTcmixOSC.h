// RTcmix - Copyright (C) 2005  The RTcmix Development Team
// See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
// the license to this software and for a DISCLAIMER OF ALL WARRANTIES.

// RTcmixOSC.h - Wrapper class for liblo OSC library, by John Gibson
#ifndef _RTCMIXOSC_H_
#define _RTCMIXOSC_H_

#include <lo/lo.h>

class RTOscPField;

class RTcmixOSC {
public:
	RTcmixOSC();
	virtual ~RTcmixOSC();
	int init();
	int registerPField(RTOscPField *pfield, lo_method_handler handler);
	int unregisterPField(RTOscPField *pfield);
	bool ready() const { return _ready; }

private:
	static void oscError(int num, const char *msg, const char *path);

	lo_server_thread _serverThread;
	RTOscPField **_pfields;
	int _pfieldBlockSize;
	int _numpfields;
	bool _ready;
};

RTcmixOSC *createOSCServer();

#endif // _RTCMIXOSC_H_
