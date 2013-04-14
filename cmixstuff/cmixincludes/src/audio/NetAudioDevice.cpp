// NetAudioDevice.cpp

#if defined (NETAUDIO)

#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>
#include <assert.h>
#include <stdio.h>

#include "NetAudioDevice.h"
#include "sndlibsupport.h"

// This subclass allows a network audio connection between two machines.  The
// input NetAudioDevice will block in start(), waiting for a connection.  When
// a connection is established, a one-way "handshake" verifies the format of
// the audio coming over the socket, and the input device (re)configures itself
// to handle the audio format, if possible.  If the input connection is lost,
// the input NetAudioDevice returns to the point where it blocks, and the cycle
// may repeat.
//
// The output NetAudioDevice can only be open()d if there is an input device
// waiting for it.  Once a connection is established, the output device sends
// its "handshake" data to notify the listener of the audio format.  Currently
// the NetAudioDevices only communicate using short integer audio.

#define DEBUG 0

#if DEBUG > 1
#define PRINT0 if (1) printf
#define PRINT1 if (1) printf
#elif DEBUG > 0
#define PRINT0 if (1) printf
#define PRINT1 if (0) printf
#else
#define PRINT0 if (0) printf
#define PRINT1 if (0) printf
#endif

#define WAIT_IN_IO_CALL

static const int kDefaultSockNumber = 9999;

// The cookie is used to identify whether the data coming over the socket
// is (1) a valid stream and (2) little- or big-endian.

static const int kAudioFmtCookie = 0x12345678;
static const int kAudioFmtCookieSwapped = 0x78563412;

// This is the struct sent as the one-way handshake.

struct NetAudioFormat {
	int		cookie;			// kAudioFmtCookie
	int		fmt;			// always SHORT, for now.
	int		chans;
	float	sr;
	int		blockFrames;	// Number of frames per write
};

static const int kNetAudioFormat_Size = sizeof(NetAudioFormat);
static const int kNetAudioSampfmt = NATIVE_SHORT_FMT | MUS_INTERLEAVED;

struct NetAudioDevice::Impl {
	Impl() : hostname(NULL), sockno(-1), sockdesc(-1), currentBuffer(0) { 
		doubleBuffers[0] = doubleBuffers[1] = NULL; 
	}
	~Impl() { free(doubleBuffers[0]); free(doubleBuffers[1]); }
	char				*hostname;
	int					sockno;
	struct sockaddr_in	sss;
	int					sockdesc;			// as opened by socket()
	int					framesPerWrite;		// set via doSetQueueSize()
	void *				doubleBuffers[2];
	int					currentBuffer;
};

inline bool NetAudioDevice::connected() { return device() > 0; }

NetAudioDevice::NetAudioDevice(const char *path) : _impl(new Impl)
{
	const char *substr = strstr(path, ":");
	if (substr != NULL) {	
		// Break path of form "hostname:sockno" into its components and store.
		int sepIndex = strlen(path) - strlen(substr);
		_impl->hostname = new char[sepIndex + 1];
		strncpy(_impl->hostname, path, sepIndex);
		_impl->hostname[sepIndex] = '\0';
		_impl->sockno = atoi(&path[sepIndex + 1]);
	}
	else {
		// Path was just "hostname"
		_impl->hostname = new char[strlen(path) + 1];
		strcpy(_impl->hostname, path);
		_impl->sockno = kDefaultSockNumber;
	}
}

NetAudioDevice::~NetAudioDevice()
{
	delete [] _impl->hostname;
	delete _impl;
}

int NetAudioDevice::waitForDevice(unsigned int wTime)
{
	while (!connected()) {
		PRINT1("NetAudioDevice::waitForDevice: not connected -- calling waitForConnect()...\n");
		if (waitForConnect(wTime) == 0) {
			if (configure() < 0) {
				PRINT1("NetAudioDevice::waitForDevice: configure() failed -- disconnecting and trying again...\n");
				disconnect();
				continue;	// try again
			}
		}
		else {
			PRINT1("NetAudioDevice::waitForDevice: waitForConnect returned non-zero -- returning 1\n");
			return 1;	// signals stop
		}
	}
	return ThreadedAudioDevice::waitForDevice(wTime);
}

void
NetAudioDevice::run()
{
	PRINT1("NetAudioDevice::run: top of loop\n");
#ifndef WAIT_IN_IO_CALL
	// waitForDevice() waits on the descriptor you passed to setDevice() until
	// the device is ready to give/get audio.  It returns nonzero if 
	// AudioDevice::stop() is called, to allow the loop to exit.
	unsigned waitMillis = 100;
	while (waitForDevice(waitMillis) == 0) {
#else
	while (true) {
#endif
       if (runCallback() != true) {
           break;
       }
	}
	PRINT1("NetAudioDevice::run: after loop\n");
	// If we stopped due to callback being done, set the state so that the
	// call to close() does not attempt to call stop, which we cannot do in
	// this thread.  Then, check to see if we are being closed by the main
	// thread before calling close() here, to avoid a reentrant call.
	
	if (!stopping()) {
		setState(Configured);
		if (!closing()) {
			close();
		}
	}
	PRINT1("NetAudioDevice::run: calling stop callback\n");	
	// Now call the stop callback.
	stopCallback();
}


int
NetAudioDevice::doOpen(int mode)
{
	struct hostent *hp;
	int len = sizeof(_impl->sss);

	PRINT0("NetAudioDevice::doOpen()\n");

	bzero(&_impl->sss, len);
	
	PRINT1("NetAudioDevice::doOpen: calling socket()\n");
	
	// Create the socket
	if( (_impl->sockdesc = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		return error("Network socket call failed: ", strerror(errno));
	}
	// set up the socket address
	_impl->sss.sin_family = AF_INET;
	_impl->sss.sin_port = htons(_impl->sockno);

	switch (mode & DirectionMask) {
	case Playback:
		if ((hp = gethostbyname(_impl->hostname)) == NULL) 
			return error("NetAudioDevice: gethostbyname failed: ",
						 strerror(errno));
		bcopy(hp->h_addr, &(_impl->sss.sin_addr.s_addr), hp->h_length);
		PRINT1("NetAudioDevice::doOpen: calling connect()\n");
		if (connect(_impl->sockdesc, (struct sockaddr *)&_impl->sss, len) < 0) {
			return error("Network socket connect failed: ",
						 strerror(errno));
		}
		setDevice(_impl->sockdesc);
		break;
	case Record:
		_impl->sss.sin_addr.s_addr = INADDR_ANY;
		if (bind(_impl->sockdesc, (struct sockaddr *)&_impl->sss, len) < 0) {
			return error("Network bind failed: ", strerror(errno));
		}
		break;
	default:
		return error("NetAudioDevice: Illegal open mode.");
	}
	return 0;
}

int
NetAudioDevice::doClose()
{
	// In record, the data device descriptor is not
	// the socket descriptor.
	PRINT0("NetAudioDevice::doClose()\n");
	closing(true);	// This allows waitForConnect() to exit.
	if (isRecording() && _impl->sockdesc > 0) {
		if (::close(_impl->sockdesc) == 0)
			_impl->sockdesc = -1;
	}
	return disconnect();
}

int
NetAudioDevice::doStart()
{
	if (isPlaying()) {
		NetAudioFormat netformat;
		netformat.cookie = kAudioFmtCookie;
		netformat.fmt = kNetAudioSampfmt;
		netformat.chans = getDeviceChannels();
		netformat.sr = (float) getSamplingRate();
		netformat.blockFrames = _impl->framesPerWrite;
		PRINT1("NetAudioDevice::doStart: writing header to stream...\n");
		int wr;
		if ((wr = ::write(device(), &netformat, kNetAudioFormat_Size)) != kNetAudioFormat_Size)
		{
			return error("NetAudioDevice: unable to write header: ",
				  		 (wr >= 0) ? "partial or zero write" : strerror(errno));
		}
		PRINT1("NetAudioDevice::doStart: wrote header\n");
	}
	else if (isRecording()) {
		bool ready = false;
		while (!ready) {
			if (waitForConnect(100) == 0) {
				if (configure() == 0)
					ready = true;	// connection OK
				else
					disconnect();	// connection was not compatible; retry.
			}
			else
				return -1;
		}
	}
	return ThreadedAudioDevice::startThread();
}

// This does nothing under RTcmix, so can be left as-is.

int
NetAudioDevice::doPause(bool)
{
	return error("Not implemented");
}

int
NetAudioDevice::doSetFormat(int sampfmt, int chans, double srate)
{
	// Always interleaved shorts.
	setDeviceParams(kNetAudioSampfmt, chans, srate);
	return 0;
}

int
NetAudioDevice::doSetQueueSize(int *pWriteSize, int *pCount)
{
	_impl->framesPerWrite = *pWriteSize;
	// We need double-buffering on the capture end.
	if (isRecording()) {
		int bufsize = _impl->framesPerWrite * getDeviceBytesPerFrame();
		_impl->doubleBuffers[0] = malloc(bufsize);
		if (_impl->doubleBuffers[0] == NULL)
			return error("Unable to allocate NetAudioDevice double buffers");
		_impl->doubleBuffers[1] = malloc(bufsize);
		if (_impl->doubleBuffers[1] == NULL)
			return error("Unable to allocate NetAudioDevice double buffers");
	}
	return 0;
}

int
NetAudioDevice::doGetFrames(void *frameBuffer, int frameCount)
{
	int bytesToRead = frameCount * getDeviceBytesPerFrame();
	int bytesRead = 0;
	char *cbuf = (char *) _impl->doubleBuffers[_impl->currentBuffer];
	while (bytesRead < bytesToRead) {
#ifdef WAIT_IN_IO_CALL
		unsigned waitMillis = 100;
		if (waitForDevice(waitMillis) == 0) {
//			printf("NetAudioDevice::doGetFrames: waitForDevice returned 0\n");
		}
		else {
			PRINT0("NetAudioDevice::doGetFrames: waitForDevice returned nonzero\n");
			return 0;
		}
#endif
		int bytesNeeded = bytesToRead - bytesRead;
//		printf("NetAudioDevice::doGetFrames: reading %d bytes from stream...\n", bytesNeeded);
		int bytes = ::read(device(), &cbuf[bytesRead], bytesNeeded);
//		printf("NetAudioDevice::doGetFrames: read %d bytes\n", bytes);
		if (bytes < 0) {
			if (errno == EAGAIN)
				continue;
			else 
				return error("Network socket read failed: ", strerror(errno));
		}
		else if (bytes == 0) {
			printf("NetAudioDevice::doGetFrames: connection broke -- disconnecting\n");
			memset(frameBuffer, 0, bytesToRead);
			disconnect();
			return 0;
		}
		else
			bytesRead += bytes;
	}
	memcpy(frameBuffer, _impl->doubleBuffers[_impl->currentBuffer], bytesRead);
	_impl->currentBuffer = !_impl->currentBuffer;	// swap
	int framesRead = bytesRead / getDeviceBytesPerFrame();
	incrementFrameCount(framesRead);
	return framesRead;
}

int
NetAudioDevice::doSendFrames(void *frameBuffer, int frameCount)
{
	int bytesToWrite = frameCount * getDeviceBytesPerFrame();
#ifdef WAIT_IN_IO_CALL
	unsigned waitMillis = 1000;
	if (waitForDevice(waitMillis) == 0) {
		PRINT1("NetAudioDevice::doSendFrames: waitForDevice returned 0\n");
	}
	else {
		PRINT0("NetAudioDevice::doSendFrames: waitForDevice returned nonzero\n");
		return 0;
	}
#endif
//	printf("NetAudioDevice::doSendFrames: writing %d bytes to stream...\n", bytesToWrite);
	int bytesWritten = ::write(device(), frameBuffer, bytesToWrite);
//	printf("NetAudioDevice::doSendFrames: wrote %d bytes\n", bytesWritten);
	if (bytesWritten <= 0) {
		return error("Network socket write failed: ",
					 (bytesWritten < 0) ? strerror(errno) : "wrote zero bytes");
	}
	int framesWritten = bytesWritten / getDeviceBytesPerFrame();
	incrementFrameCount(framesWritten);
	return framesWritten;
}

int NetAudioDevice::waitForConnect(unsigned int wTime)
{
	if (listen(_impl->sockdesc, 1) < 0) {
		return error("Network socket listen failed: ", strerror(errno));
	}
	printf("NetAudioDevice::waitForConnect: waiting for input connection...\n");
	// Make sure we wont block.
	fcntl(_impl->sockdesc, F_SETFL, O_NONBLOCK);
	// Select on socket, waiting until there is a connection to accept.
	// This allows us to break out if device stops or closes.
	fd_set rfdset;
	struct timeval timeout;
	const int nfds = _impl->sockdesc + 1;
	int ret;
	FD_ZERO(&rfdset);
	do {
		if (closing() || stopping()) {
			PRINT0("NetAudioDevice::waitForConnect:  breaking out of wait for stop or close\n");
			return -1;
		}
		FD_SET(_impl->sockdesc, &rfdset);
		timeout.tv_sec = unsigned(wTime / 1000);
		timeout.tv_usec = 1000 * (unsigned(wTime) - (timeout.tv_sec * 1000));
		//printf("NetAudioDevice::waitForConnect:  in select loop (timeout %g seconds) ...\n", wTime * 0.001);
	} while ((ret = select(nfds, &rfdset, NULL, NULL, &timeout)) == 0);
	if (ret == -1) {
		PRINT0("NetAudioDevice::waitForConnect: select() returned -1, breaking out of wait\n");
		return error("Error while waiting for network connection: ", strerror(errno));
	}
#ifdef JAGUAR
	int len = sizeof(_impl->sss);
#else
	socklen_t len = sizeof(_impl->sss);
#endif
	int sockdev = accept(_impl->sockdesc, (struct sockaddr *)&_impl->sss, &len);
	if (sockdev < 0) {
		::close(_impl->sockdesc);
		return error("Network accept failed: ", strerror(errno));
	}
	/* Set socket to blocking.  First get current flags */
	int flags;
	if ((flags = fcntl(sockdev, F_GETFL, 0)) >= 0) {
		if (fcntl(sockdev, F_SETFL, flags & (~O_NONBLOCK)) < 0)  {
			PRINT0("NetAudioDevice::waitForConnect: Failed to set socket to blocking: %s\n", strerror(errno));
		}
	}
	else {  
		PRINT0("NetAudioDevice::waitForConnect: Failed to get socket flags.\n");
	}
	setDevice(sockdev);
	printf("NetAudioDevice::waitForConnect: got connection\n");
	return 0;
}

int NetAudioDevice::disconnect()
{
	int status = 0;
	const int dev = device();
	if (dev > 0) {
		// NOTE:  Still possible to have race condition with _device.
		if ((status = ::close(dev)) == 0)
			setDevice(-1);
		else
			error("Network socket close failed: ", strerror(errno));
	}
	return status;
}

inline unsigned
swap(unsigned ul) {
   return (ul >> 24) | ((ul >> 8) & 0xff00) | ((ul << 8) & 0xff0000) | (ul << 24);
}

inline int swap(int x) { return swap((unsigned)x); }

inline float
swap(float uf) {
   union { unsigned l; float f; } u;
   u.f = uf;
   u.l = swap(u.l);
   return (u.f);
}

int NetAudioDevice::configure()
{
	NetAudioFormat netformat;
	int rd;
	bool swapped = false;
//	printf("NetAudioDevice::configure(): reading header from stream...\n");
	if ((rd = ::read(device(), &netformat, kNetAudioFormat_Size)) != kNetAudioFormat_Size)
	{
		fprintf(stderr, "NetAudioDevice: unable to read header: %s -- got %d of %d bytes\n",
				(rd >= 0) ? "partial or zero read" : strerror(errno), rd, kNetAudioFormat_Size);
		int err = errno;
		disconnect();
		return error("NetAudioDevice: unable to read header: ",
					 (rd >= 0) ? "partial or zero read" : strerror(err));
	}
//	printf("NetAudioDevice: read header\n");
	switch (netformat.cookie) {
	case kAudioFmtCookie:
		swapped = false;
		break;
	case kAudioFmtCookieSwapped:
		swapped = true;
		break;
	default:
		fprintf(stderr, "NetAudioDevice: missing or corrupt header: cookie = 0x%x\n", netformat.cookie);
		disconnect();
		return error("NetAudioDevice: missing or corrupt header");
	}
	if (swapped) {
		PRINT0("debug: header cookie was opposite endian\n");
		netformat.fmt = swap(netformat.fmt);
		// Indicate we are receiving opposite endian data
		if (MUS_GET_FORMAT(netformat.fmt) == MUS_LSHORT)
			netformat.fmt = MUS_BSHORT;		// We OR in interleaved below
		else if (MUS_GET_FORMAT(netformat.fmt) == MUS_BSHORT)
			netformat.fmt = MUS_LSHORT;		// We OR in interleaved below
		netformat.chans = swap(netformat.chans);
		netformat.sr = swap(netformat.sr);
		netformat.blockFrames = swap(netformat.blockFrames);
	}
	// Now make sure we can handle the format
	if (!IS_SHORT_FORMAT(netformat.fmt)) {
		fprintf(stderr, "NetAudioDevice: unknown or unsupported audio format\n");
		disconnect();
		return error("NetAudioDevice: unsupported audio format");
	}
	netformat.fmt |= MUS_INTERLEAVED;	// Just for good measure!

	// For now we cannot handle buffer size or channel mismatches.
	if (netformat.blockFrames != _impl->framesPerWrite) {
		fprintf(stderr, "NetAudioDevice: sender and receiver RTBUFSAMPS must match\n");
		disconnect();
		return error("NetAudioDevice: audio buffer size mismatch");
	}
	else if (netformat.chans != getDeviceChannels()) {
		fprintf(stderr, "NetAudioDevice: sender and receiver channel counts must match\n");
		disconnect();
		return error("NetAudioDevice: audio channel count mismatch");
	}
	// Just give a warning for SR mismatch.
	if (netformat.sr != getSamplingRate()) {
		fprintf(stderr, "NetAudioDevice: warning: SR mismatch.  Playing at incorrect rate.\n");
	}
	setDeviceParams(netformat.fmt, netformat.chans, netformat.sr);
	PRINT1("NetAudioDevice::configure(): resetting conversion routines\n");
	
	/* Set socket back to nonblocking.  First get current flags */
	int flags;
	if ((flags = fcntl(device(), F_GETFL, 0)) >= 0) {
		if (fcntl(device(), F_SETFL, flags | O_NONBLOCK) < 0)  {
			printf("NetAudioDevice::connect: Failed to set socket to nonblocking: %s\n", strerror(errno));
		}
	}
	else {  
		printf("NetAudioDevice::connect: Failed to get socket flags.\n");
	}
	
	return resetFormatConversion();
}

bool NetAudioDevice::recognize(const char *desc)
{
	return (desc != NULL) && strncmp(desc, "net:", 4) == 0;
}

AudioDevice *NetAudioDevice::create(const char *inputDesc, const char *outputDesc, int mode)
{
	AudioDevice *theDevice = NULL;
	// We dont support full duplex for this class.
	if ((mode & AudioDevice::DirectionMask) != RecordPlayback) {
		const char *desc = inputDesc ? inputDesc : outputDesc;
		// Strip off the "net:" from the beginning of the descriptor.
		theDevice  = new NetAudioDevice(&desc[4]);
	}
	return theDevice;
}

#endif	// NETAUDIO
