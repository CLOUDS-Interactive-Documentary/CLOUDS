/* RTcmix - Copyright (C) 2004  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/
#ifndef _OSXMOUSE_H_
#define _OSXMOUSE_H_
#include <RTcmixMouse.h>
#include <Carbon/Carbon.h>
#include "mouse_ipc.h"

class OSXMouse : public RTcmixMouse {
public:
	OSXMouse();

	virtual int show();

protected:
	virtual ~OSXMouse();
	// RTcmixMouse reimplementations

	virtual inline double getPositionX() const { return _x; }
	virtual inline double getPositionY() const { return _y; }

	virtual void doConfigureXLabel(const int id, const char *prefix,
                                 const char *units, const int precision);
	virtual void doConfigureYLabel(const int id, const char *prefix,
                                 const char *units, const int precision);
	virtual void doUpdateXLabelValue(const int id, const double value);
	virtual void doUpdateYLabelValue(const int id, const double value);

	virtual bool handleEvents();

private:
	int openSocket();
	int reportError(const char *err, const bool useErrno);
	int readPacket(MouseSockPacket *packet);
	int writePacket(const MouseSockPacket *packet);
	void sendLabel(const bool isXAxis, const int id, const char *prefix,
                  const char *units, const int precision);
	void sendLabelValue(const bool isXAxis, const int id, const double value);
	int pollInput(long);

	int _sockport;
	int _sockdesc;
	double _x;
	double _y;
	MouseSockPacket *_packet;
	MouseSockPacket *_evtpacket;
	char *_servername;
};

#endif // _OSXMOUSE_H_
