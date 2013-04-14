/* RTcmix - Copyright (C) 2005  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/
#include <OSXMouse.h>
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

const char *kServerName = "localhost";

// If RTcmix fails to connect to MouseWindow, increase this.
const int kLaunchSleepMsec = 500;

const int kNumConnectAttempts = 10;
const int kConnectSleepMsec = 200;


OSXMouse::OSXMouse() : RTcmixMouse(), _sockdesc(0)
{
	_x = -1.0;	// force RTMousePField to use default vals until these are valid
	_y = -1.0;
	_sockport = kSockPort;
	_packet = new MouseSockPacket [1];
	_evtpacket = new MouseSockPacket [1];
	_servername = strdup(kServerName);
}

OSXMouse::~OSXMouse()
{
	delete [] _packet;
	delete [] _evtpacket;
	delete [] _servername;
}

// Open new socket and, as client, connect to the MouseWindow program.
// Returns 0 if connection accepted, -1 if any other error.
int OSXMouse::openSocket()
{
	_sockdesc = socket(AF_INET, SOCK_STREAM, 0);
	if (_sockdesc < 0)
		return reportError("OSXMouse::openSocket (socket)", true);

	struct hostent *server = gethostbyname(_servername);
	if (server == NULL)
		return reportError("OSXMouse::openSocket: Can't find mouse server "
		                   "address.", false);

	int val = sizeof(MouseSockPacket);
	int optlen = sizeof(int);
	if (setsockopt(_sockdesc, SOL_SOCKET, SO_RCVBUF, &val, optlen) < 0)
		return reportError("OSXMouse::openSocket (setsockopt)", true);

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
		// printf("connection attempt %d...\n", NUM_CONNECT_ATTEMPTS - attempts);
		int result = connect(_sockdesc, (struct sockaddr *) &servaddr,
	                                               sizeof(servaddr));
		if (result == 0) {
			connected = true;
			break;
		}
		else if (result < 0 && result != ECONNREFUSED)
			return reportError("OSXMouse::openSocket (connect)", true);
		usleep(kConnectSleepMsec * 1000L);
	}
	if (!connected)
		return reportError("OSXMouse::openSocket (connect)", true);

	return 0;
}

int OSXMouse::show()
{
	// Launch MouseWindow.app if it's not already running
#ifdef APP_PATH
	const char *app = APP_PATH;
#else
	const char *app = "MouseWindow";
#endif
// FIXME: should probably do this via an AppleEvent
	char buf[PATH_MAX];
	snprintf(buf, PATH_MAX, "open -a %s", app);
	system(buf);

	// Establish socket connection as client
	if (openSocket() == -1)
		return -1;

	return 0;
}

int OSXMouse::reportError(const char *err, const bool useErrno)
{
	if (useErrno)
		fprintf(stderr, "%s: %s\n", err, strerror(errno));
	else
		fprintf(stderr, "%s\n", err);
	return -1;
}

// Read one packet, and store in <packet>.  Return 0 if okay, -1 if error,
// and 1 if EOF.
int OSXMouse::readPacket(MouseSockPacket *packet)
{
	char *ptr = (char *) packet;
	const int packetsize = sizeof(MouseSockPacket);
	ssize_t amt = 0;
	do {
		ssize_t n = read(_sockdesc, ptr + amt, packetsize - amt);
		if (n == -1)
			return reportError("OSXMouse::readPacket", true);
		else if (n == 0)	// EOF
			return 1;
		amt += n;
	} while (amt < packetsize);

	return 0;
}

int OSXMouse::writePacket(const MouseSockPacket *packet)
{
	const char *ptr = (char *) packet;
	const int packetsize = sizeof(MouseSockPacket);
	ssize_t amt = 0;
	do {
		ssize_t n = write(_sockdesc, ptr + amt, packetsize - amt);
		if (n < 0)
			return reportError("OSXMouse::writePacket", true);
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

// Send prefix, units and precision to MouseWindow for given label id and axis.
void OSXMouse::sendLabel(const bool isXAxis, const int id, const char *prefix,
		const char *units, const int precision)
{
	_packet->id = id;

	_packet->type = isXAxis ? kPacketConfigureXLabelPrefix
									: kPacketConfigureYLabelPrefix;
	mystrncpy(_packet->data.str, prefix, kPartLabelLength);
	writePacket(_packet);

	if (units) {		// units string is optional
		_packet->type = isXAxis ? kPacketConfigureXLabelUnits
										: kPacketConfigureYLabelUnits;
		mystrncpy(_packet->data.str, units, kPartLabelLength);
		writePacket(_packet);
	}

	_packet->type = isXAxis ? kPacketConfigureXLabelPrecision
									: kPacketConfigureYLabelPrecision;
	_packet->data.ival = precision;
	writePacket(_packet);
}

void OSXMouse::doConfigureXLabel(const int id, const char *prefix,
		const char *units, const int precision)
{
	sendLabel(true, id, prefix, units, precision);
}

void OSXMouse::doConfigureYLabel(const int id, const char *prefix,
		const char *units, const int precision)
{
	sendLabel(false, id, prefix, units, precision);
}

// Send label value to MouseWindow for given label id and axis.
void OSXMouse::sendLabelValue(const bool isXAxis, const int id,
	const double value)
{
	_packet->id = id;
	_packet->type = isXAxis ? kPacketUpdateXLabel : kPacketUpdateYLabel;
	_packet->data.dval = value;
	writePacket(_packet);
}

void OSXMouse::doUpdateXLabelValue(const int id, const double value)
{
	sendLabelValue(true, id, value);
}

void OSXMouse::doUpdateYLabelValue(const int id, const double value)
{
	sendLabelValue(false, id, value);
}

int OSXMouse::pollInput(long usec)
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
		reportError("OSXMouse::pollInput", true);
	return result;
}

// Read any incoming data from MouseWindow program.  We read all input that
// is available, taking only the last pair of coordinates at a time.
bool OSXMouse::handleEvents()
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
				case kPacketMouseCoords:
					_x = _evtpacket->data.point.x;
					_y = _evtpacket->data.point.y;
					break;
				case kPacketQuit:
					if (close(_sockdesc) == -1)
						reportError("OSXMouse::handleEvents", true);
					return false;
					break;
				default:
					reportError("OSXMouse::handleEvents: Invalid packet type\n",
					                                                     false);
					return false;
					break;
			}
		}
	} while (result > 0);

	return true;
}

