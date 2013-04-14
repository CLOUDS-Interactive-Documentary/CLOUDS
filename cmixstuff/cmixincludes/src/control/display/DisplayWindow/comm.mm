/* RTcmix - Copyright (C) 2011  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <assert.h>
#include "comm.h"
#include "display_ipc.h"
#include "LabelView-CInterface.h"

//#define DEBUG
//#define IGNORE_QUIT_FROM_RTCMIX

// Important tuning parameter: how often to nap between polling for 
// incoming packets.
const int kSleepMsec = 20;

static int _servdesc;
static int _newdesc;
static int _sockport = kSockPort;
static bool _runThread;
static pthread_t _listenerThread;

void *labelView = NULL;

void setLabelViewInstance(void *instance)
{
	labelView = instance;
}

int reportConsoleError(const char *err, const bool useErrno);
int reportError(const char *err, const bool useErrno);
int readPacket(DisplaySockPacket *packet);
int writePacket(const DisplaySockPacket *packet);
int pollInput(long usec);
void *listenerLoop(void *context);
int createListenerThread();
int openSocket();
void sendQuit();
int closeSocket();

// =============================================================================
// Utilities

int reportConsoleError(const char *err, const bool useErrno)
{
	if (useErrno)
		fprintf(stderr, "%s: %s\n", err, strerror(errno));
	else
		fprintf(stderr, "%s\n", err);
	return -1;
}

int reportError(const char *err, const bool useErrno)
{
	reportConsoleError(err, useErrno);
	return -1;
}


// =============================================================================
// IPC stuff

// Read one packet, and store in <packet>.  Return 0 if okay, -1 if error,
// and 1 if EOF.
int readPacket(DisplaySockPacket *packet)
{
	if (_newdesc == -1)
		return -1;
	char *ptr = (char *) packet;
	const int packetsize = sizeof(DisplaySockPacket);
	ssize_t amt = 0;
	do {
		ssize_t n = read(_newdesc, ptr + amt, packetsize - amt);
		if (n == -1)
			return reportError("readPacket", true);
		else if (n == 0)	// EOF
			return 1;
		amt += n;
	} while (amt < packetsize);

	return 0;
}

int writePacket(const DisplaySockPacket *packet)
{
	if (_newdesc == -1)
		return -1;
	const char *ptr = (char *) packet;
	const int packetsize = sizeof(DisplaySockPacket);
	ssize_t amt = 0;
	do {
		ssize_t n = write(_newdesc, ptr + amt, packetsize - amt);
		if (n < 0)
			return reportError("writePacket", true);
		amt += n;
	} while (amt < packetsize);

	return 0;
}

int pollInput(long usec)
{
	fd_set rfdset;
	FD_ZERO(&rfdset);
	FD_SET(_newdesc, &rfdset);
	const int nfds = _newdesc + 1;
	struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = (int) usec;
	int result = select(nfds, &rfdset, NULL, NULL, &timeout);
	if (result == -1)
		reportError("pollInput", true);
	return result;
}

// Read any incoming data from RTcmix, and dispatch messages appropriately.
void *listenerLoop(void *context)
{
	DisplaySockPacket *packet = new DisplaySockPacket [1];

	while (_runThread) {
		bool labelsUpdated = false;
		int result;
		do {
			result = pollInput(0);
			if (result == -1)
				_runThread = false;
			else if (result > 0) {
				if (readPacket(packet) != 0) {
					_runThread = false;
					break;
				}
				if (labelView) {
					switch (packet->type) {
						case kPacketConfigureLabelPrefix:
							LabelViewConfigureLabelPrefix(labelView, packet->id, packet->data.str);
							break;
						case kPacketConfigureLabelUnits:
							LabelViewConfigureLabelUnits(labelView, packet->id, packet->data.str);
							break;
						case kPacketConfigureLabelPrecision:
							LabelViewConfigureLabelPrecision(labelView, packet->id, packet->data.ival);
							break;
						case kPacketUpdateLabel:
							LabelViewUpdateLabelValue(labelView, packet->id, packet->data.dval);
							labelsUpdated = true;
							break;
#ifndef IGNORE_QUIT_FROM_RTCMIX
						case kPacketQuit:
							_runThread = false;
							break;
#endif
						default:
							reportError("listenerLoop: Invalid packet type\n", false);
							_runThread = false;
							break;
					}
				}
			}
		} while (result > 0);
		if (labelsUpdated && labelView)	// draw only the final state for this polling
			LabelViewDoUpdate(labelView);
		usleep(kSleepMsec * 1000L);
	}
	delete [] packet;

#ifndef IGNORE_QUIT_FROM_RTCMIX
#if 0 // doesn't work
	[NSApp performSelector:@selector(terminate:) withObject:nil afterDelay:0.0];
#else
	finalize();
	exit(0);
#endif
#endif

	return NULL;
}

int createListenerThread()
{
	_runThread = true;
	int retcode = pthread_create(&_listenerThread, NULL, listenerLoop, NULL);
	if (retcode != 0)
		return reportError("createListenerThread", true);
	return 0;
}

// Open new socket and, as server, block while listening for connection
// to RTcmix.  Returns 0 if connection accepted, -1 if any other error.
int openSocket()
{
	_servdesc = socket(AF_INET, SOCK_STREAM, 0);
	if (_servdesc < 0)
		return reportError("openSocket (socket)", true);

	int val = sizeof(DisplaySockPacket);
	int optlen = sizeof(int);
	if (setsockopt(_servdesc, SOL_SOCKET, SO_RCVBUF, &val, optlen) < 0)
		return reportError("openSocket (setsockopt)", true);

	// This allows us to bind to the same address as last time, even if
	// the kernel is still keeping the old binding active.  In practical
	// terms, this means we can quit the MouseWindow program while RTcmix
	// is running, and then the next launch of MouseWindow will not fail
	// with "Address already in use."  It takes a minute or so for the
	// kernel to relinquish the old binding, so that can be annoying.
	val = 1;
	optlen = sizeof(int);
	if (setsockopt(_servdesc, SOL_SOCKET, SO_REUSEADDR, &val, optlen) < 0)
		return reportError("openSocket (setsockopt)", true);

	struct sockaddr_in servaddr;
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(_sockport);

	if (bind(_servdesc, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)
		return reportError("openSocket (bind)", true);

	if (listen(_servdesc, 1) < 0)
		return reportError("openSocket (listen)", true);

	socklen_t len = sizeof(servaddr);
	_newdesc = accept(_servdesc, (struct sockaddr *) &servaddr, &len);
	if (_newdesc < 0)
		return reportError("openSocket (accept)", true);

	return 0;
}

void sendQuit()
{
	DisplaySockPacket packet;
	packet.type = kPacketQuit;
	writePacket(&packet);
}

int closeSocket()
{
	sendQuit();

	if (_servdesc > -1) {
		if (close(_servdesc) == -1)
			return reportError("closeSocket", true);
		_servdesc = -1;
	}
	if (_newdesc > -1) {
		if (close(_newdesc) == -1)
			return reportError("closeSocket", true);
		_newdesc = -1;
	}

	return 0;
}

int initialize()
{
	_servdesc = -1;
	_newdesc = -1;

	if (openSocket() != 0)		// Make server listen asap
		return -1;
	if (createListenerThread() != 0)
		return -1;
	
	return 0;
}

int finalize()
{
	_runThread = false;
	pthread_join(_listenerThread, NULL);
	return closeSocket();
}

