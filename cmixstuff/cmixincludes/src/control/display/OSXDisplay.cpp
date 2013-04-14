/* RTcmix - Copyright (C) 2005  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/
#include <OSXDisplay.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netdb.h>
#include <netinet/in.h>
#include <errno.h>

// If RTcmix fails to connect to DisplayWindow, increase this.
const int kLaunchSleepMsec = 500;

const int kNumConnectAttempts = 10;
const int kConnectSleepMsec = 200;

const char *kServerName = "localhost";


OSXDisplay::OSXDisplay()
	: RTcmixDisplay(), _sockdesc(0), _running(false)
{
	_sockport = kSockPort;
	_packet = new DisplaySockPacket [1];
	_evtpacket = new DisplaySockPacket [1];
	_servername = new char [strlen(kServerName) + 1];
	strcpy(_servername, kServerName);
}

OSXDisplay::~OSXDisplay()
{
	delete [] _packet;
	delete [] _evtpacket;
	delete [] _servername;
}

// Open new socket and, as client, connect to the DisplayWindow program.
// Returns 0 if connection accepted, -1 if any other error.
int OSXDisplay::openSocket()
{
	_sockdesc = socket(AF_INET, SOCK_STREAM, 0);
	if (_sockdesc < 0)
		return reportError("OSXDisplay::openSocket (socket)", true);

	struct hostent *server = gethostbyname(_servername);
	if (server == NULL)
		return reportError("OSXDisplay::openSocket: Can't find display server "
		                   "address.", false);

	int val = sizeof(DisplaySockPacket);
	int optlen = sizeof(int);
	if (setsockopt(_sockdesc, SOL_SOCKET, SO_RCVBUF, &val, optlen) < 0)
		return reportError("OSXDisplay::openSocket (setsockopt)", true);

	struct sockaddr_in servaddr;
	memset((char *) &servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	memcpy((char *) &servaddr.sin_addr.s_addr, (char *) server->h_addr,
                                                       server->h_length);
	servaddr.sin_port = htons(_sockport);

	// This loop *should* work, but if the initial attempt is refused, all others
	// fail with EINVAL.  So instead, we sleep before attempting to connect.
#if 1
	usleep(kLaunchSleepMsec * 1000L);
#endif
	bool connected = false;
	int attempts = kNumConnectAttempts;
	while (attempts-- > 0) {
		// printf("connection attempt %d...\n", kNumConnectAttempts - attempts);
		int result = connect(_sockdesc, (struct sockaddr *) &servaddr,
	                                               sizeof(servaddr));
		if (result == 0) {
			connected = true;
			break;
		}
		else if (result < 0 && result != ECONNREFUSED)
			return reportError("OSXDisplay::openSocket (connect)", true);
		usleep(kConnectSleepMsec * 1000L);
	}
	if (!connected)
		return reportError("OSXDisplay::openSocket (connect)", true);

	return 0;
}

int OSXDisplay::show()
{
	// Launch DisplayWindow.app if it's not already running
#ifdef APP_PATH
	const char *app = APP_PATH;
#else
	const char *app = "DisplayWindow";
#endif
// FIXME: should probably do this via an AppleEvent
	char buf[PATH_MAX];
	snprintf(buf, PATH_MAX, "open -a %s", app);
	system(buf);

	// Establish socket connection as client
	if (openSocket() == -1)
		return -1;

	_running = true;

	return 0;
}

int OSXDisplay::reportError(const char *err, const bool useErrno)
{
	if (useErrno)
		fprintf(stderr, "%s: %s\n", err, strerror(errno));
	else
		fprintf(stderr, "%s\n", err);
	return -1;
}

// Read one packet, and store in <packet>.  Return 0 if okay, -1 if error,
// and 1 if EOF.
int OSXDisplay::readPacket(DisplaySockPacket *packet)
{
	char *ptr = (char *) packet;
	const int packetsize = sizeof(DisplaySockPacket);
	ssize_t amt = 0;
	do {
		ssize_t n = read(_sockdesc, ptr + amt, packetsize - amt);
		if (n == -1)
			return reportError("OSXDisplay::readPacket", true);
		else if (n == 0)	// EOF
			return 1;
		amt += n;
	} while (amt < packetsize);

	return 0;
}

int OSXDisplay::writePacket(const DisplaySockPacket *packet)
{
	const char *ptr = (char *) packet;
	const int packetsize = sizeof(DisplaySockPacket);
	ssize_t amt = 0;
	do {
		ssize_t n = write(_sockdesc, ptr + amt, packetsize - amt);
		if (n < 0)
			return reportError("OSXDisplay::writePacket", true);
		amt += n;
	} while (amt < packetsize);

	return 0;
}

// Ensures null termination.  <len> includes the null.
void mystrncpy(char *dest, const char *src, int len)
{
	strncpy(dest, src, len - 1);
	dest[len - 1] = 0;
}

// Send prefix, units and precision to DisplayWindow for given label id.
void OSXDisplay::sendLabel(const int id, const char *prefix,
		const char *units, const int precision)
{
	_packet->id = id;

	_packet->type = kPacketConfigureLabelPrefix;
	mystrncpy(_packet->data.str, prefix, kPartLabelLength);
	writePacket(_packet);

	if (units) {		// units string is optional
		_packet->type = kPacketConfigureLabelUnits;
		mystrncpy(_packet->data.str, units, kPartLabelLength);
		writePacket(_packet);
	}

	_packet->type = kPacketConfigureLabelPrecision;
	_packet->data.ival = precision;
	writePacket(_packet);
}

void OSXDisplay::doConfigureLabel(const int id, const char *prefix,
		const char *units, const int precision)
{
	sendLabel(id, prefix, units, precision);
}

// Send label value to DisplayWindow for given label id.
void OSXDisplay::sendLabelValue(const int id, const double value)
{
	_packet->id = id;
	_packet->type = kPacketUpdateLabel;
	_packet->data.dval = value;
	writePacket(_packet);
}

void OSXDisplay::doUpdateLabelValue(const int id, const double value)
{
	if (_running)
		sendLabelValue(id, value);
}

int OSXDisplay::pollInput(long usec)
{
	fd_set rfdset;
	FD_ZERO(&rfdset);
	FD_SET(_sockdesc, &rfdset);
	const int nfds = _sockdesc + 1;
	struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = usec;
	int result = select(nfds, &rfdset, NULL, NULL, &timeout);
	if (result == -1)
		reportError("OSXDisplay::pollInput", true);
	return result;
}

// Read any incoming data from DisplayWindow program.  We read all input that
// is available now.
// NOTE: This code is adapted from control/mouse/OSXMouse.cpp.  Here, all we're
// doing is sending values to the window program and listening for a quit msg
// coming back from it.
bool OSXDisplay::handleEvents()
{
	int result;
	do {
		result = pollInput(0);
		if (result == -1)
			return false;
		else if (result > 0) {
			if (readPacket(_evtpacket) != 0)
				return false;

			switch (_evtpacket->type) {
				case kPacketQuit:
					if (close(_sockdesc) == -1)
						reportError("OSXDisplay::handleEvents", true);
					_running = false;
					return false;
					break;
				default:
					reportError("OSXDisplay::handleEvents: Invalid packet type\n",
					                                                     false);
					return false;
					break;
			}
		}
	} while (result > 0);

	return true;
}

