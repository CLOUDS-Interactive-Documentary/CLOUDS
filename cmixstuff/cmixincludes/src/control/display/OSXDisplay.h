/* RTcmix - Copyright (C) 2005  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/
#ifndef _OSXDISPLAY_H_
#define _OSXDISPLAY_H_
#include <RTcmixDisplay.h>
#include <Carbon/Carbon.h>
#include "display_ipc.h"

class OSXDisplay : public RTcmixDisplay {
public:
	OSXDisplay();
	virtual ~OSXDisplay();
	virtual int show();

protected:
	// RTcmixDisplay reimplementations
	virtual void doConfigureLabel(const int id, const char *prefix,
                                 const char *units, const int precision);
	virtual void doUpdateLabelValue(const int id, const double value);
	virtual bool handleEvents();

private:
	int openSocket();
	int reportError(const char *err, const bool useErrno);
	int readPacket(DisplaySockPacket *packet);
	int writePacket(const DisplaySockPacket *packet);
	void sendLabel(const int id, const char *prefix,
                  const char *units, const int precision);
	void sendLabelValue(const int id, const double value);
	int pollInput(long);

	int _sockport;
	int _sockdesc;
	DisplaySockPacket *_packet;
	DisplaySockPacket *_evtpacket;
	char *_servername;
	bool _running;
};

#endif // _OSXDISPlAY_H_
