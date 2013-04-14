// JackAudioDevice.cpp - by John Gibson and Douglas Scott

#if defined(JACK)

#include "JackAudioDevice.h"
#include <jack/jack.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define DEBUG 1
//#define USE_NON_INTERLEAVED

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


// -- JackAudioDevice::Impl ---------------------------------------------------

struct JackAudioDevice::Impl {
	Impl(const char *serverName)
		: serverName(serverName), client(NULL), numInPorts(0), numOutPorts(0),
		  inPorts(NULL), outPorts(NULL), inBuf(NULL), outBuf(NULL), frameCount(0),
		  srate(0), bufSize(0) {}
	~Impl();
	const char *serverName;
	jack_client_t *client;
	int numInPorts;
	int numOutPorts;
	jack_port_t **inPorts;
	jack_port_t **outPorts;
	jack_default_audio_sample_t **inBuf;
	jack_default_audio_sample_t **outBuf;
	int frameCount;
	jack_nframes_t srate;
	jack_nframes_t bufSize;

	static int runProcess(jack_nframes_t nframes, void *object);
	static int srateChanged(jack_nframes_t nframes, void *object);
	static int bufSizeChanged(jack_nframes_t nframes, void *object);
	static void shutdown(void *object);
	static void jackError(const char *msg);
};

JackAudioDevice::Impl::~Impl()
{
	delete [] inPorts;
	delete [] outPorts;
	delete [] inBuf;
	delete [] outBuf;
}

int JackAudioDevice::Impl::runProcess(jack_nframes_t nframes, void *object)
{
	PRINT1("JackAudioDevice::runProcess(nframes=%d)\n", nframes);
	JackAudioDevice *device = (JackAudioDevice *) object;
	if (!device->isRunning())	// callback runs before doStart called
		return 0;
	JackAudioDevice::Impl *impl = device->_impl;
	const int inchans = impl->numInPorts;
	const int outchans = impl->numOutPorts;
	jack_default_audio_sample_t **in = impl->inBuf;
	jack_default_audio_sample_t **out = impl->outBuf;

	// get non-interleaved buffer pointers from JACK
	for (int i = 0; i < inchans; i++)
		in[i] = (jack_default_audio_sample_t *)
		                        jack_port_get_buffer(impl->inPorts[i], nframes);
// FIXME: supposedly the out buffer ptrs can be cached btw. calls to blocksize
// callback -- see docs for jack_port_get_buffer
	for (int i = 0; i < outchans; i++)
		out[i] = (jack_default_audio_sample_t *)
		                        jack_port_get_buffer(impl->outPorts[i], nframes);

	// process sound, resulting in one call each to doGetFrames and doSendFrames
	bool keepGoing = device->runCallback();
	if (!keepGoing) {
		PRINT0("runProcess: runCallback returned false; calling stopCallback\n");
		device->stopCallback();
	}

	impl->frameCount += nframes;

	return 0;
}

int JackAudioDevice::Impl::srateChanged(jack_nframes_t nframes, void *object)
{
	PRINT0("JackAudioDevice::Impl::srateChanged()\n");
	JackAudioDevice *device = (JackAudioDevice *) object;
	int status = 0;
	if (device->isRunning()) {
		if (nframes != device->_impl->srate) {
			device->error("JACK sample rate changed; we can't cope with this.");
			status = -1;
			device->stopCallback();
		}
	}
	return status;
}

int JackAudioDevice::Impl::bufSizeChanged(jack_nframes_t nframes, void *object)
{
	PRINT0("JackAudioDevice::Impl::bufSizeChanged()\n");
	JackAudioDevice *device = (JackAudioDevice *) object;
	int status = 0;
	if (device->isRunning()) {
		if (nframes != device->_impl->bufSize) {
			device->error("JACK buffer size changed; we can't cope with this.");
			status = -1;
			device->stopCallback();
		}
	}
	return status;
}

// Called when JACK server shuts down.
void JackAudioDevice::Impl::shutdown(void *object)
{
	PRINT0("JackAudioDevice::shutdown()\n");
	JackAudioDevice *device = (JackAudioDevice *) object;
	device->stopCallback();
}

// FIXME: rework this later to allow GUI apps to provide a callback
void JackAudioDevice::Impl::jackError(const char *msg)
{
	fprintf(stderr, "JACK Error: %s\n", msg);
}


// -- JackAudioDevice ---------------------------------------------------------

JackAudioDevice::JackAudioDevice(const char *serverName)
	: _impl(new Impl(serverName))
{
}

JackAudioDevice::~JackAudioDevice()
{ 
	PRINT0("JackAudioDevice::~JackAudioDevice() - calling close()\n");
	close();
	delete _impl;
}

int JackAudioDevice::doOpen(int mode)
{
	jack_set_error_function(_impl->jackError);

// FIXME: RTcmix is not always the program name (e.g., cmixplay, RTcmixShell)
	const char *clientName = "RTcmix";

	jack_status_t status;
	jack_options_t options = jack_options_t(JackServerName | JackNoStartServer);
	_impl->client = jack_client_open(clientName, options, &status,
											_impl->serverName);
	if (_impl->client == NULL)
		return error("Unable to connect to JACK server.");
	if (status & JackServerStarted)
		printf("JACK server was not running...now starting it.\n");
	if (status & JackNameNotUnique) {
		char *name = jack_get_client_name(_impl->client);
		printf("Unique name \"%s\" assigned for this JACK client.\n", name);
	}

	if (jack_set_process_callback(_impl->client, _impl->runProcess,
			(void *) this) != 0)
		return error("Could not register JACK process callback.");

	if (jack_set_sample_rate_callback(_impl->client, _impl->srateChanged,
			(void *) this) != 0)
		return error("Could not register JACK sample rate change callback.");

	if (jack_set_buffer_size_callback(_impl->client, _impl->bufSizeChanged,
			(void *) this) != 0)
		return error("Could not register JACK buffer size change callback.");

	jack_on_shutdown(_impl->client, _impl->shutdown, (void *) this);

	return 0;
}

int JackAudioDevice::doClose()
{
	PRINT0("JackAudioDevice::doClose()\n");

	// If the jack daemon is running with elevated privs (i.e., with the
	// -R flag), but the RTcmix threads are not -- as happens when you
	// start jackd via jackstart, for example -- then there's a nasty
	// problem when we receive a cntl-C interrupt.  In that case, doClose
	// is called from an RTcmix thread, and calling jack_client_close will
	// then hang, causing bad stuff to happen.  So in the realtime case,
	// we just die without closing jack, since this seems to be okay.

	if (!jack_is_realtime(_impl->client)) {
		PRINT0("...calling jack_client_close(client=%p)\n", _impl->client);
		if (jack_client_close(_impl->client) != 0)
			return error("Error closing JACK.");
	}

	// We call this here only because when we're called from our interrupt
	// handler, this seems to be the only way to stop the process thread.
	// Note that this is safe for other situations, because stopCallback does
	// something only the first time you call it.

	stopCallback();

	_impl->frameCount = 0;

	return 0;
}

// Connect JACK ports (must be done after activating JACK)
int JackAudioDevice::connectPorts()
{
	if (0) {
		// Connect to ports specified by user via set_option, .rtcmixrc, etc.
		// FIXME: implement this
	}
	else {
		// Connect to hardware in/out ports (e.g., alsa_pcm)
		if (isRecording()) {
			const char **ports = jack_get_ports(_impl->client, NULL, NULL,
			                                JackPortIsPhysical | JackPortIsOutput);
			if (ports == NULL)
				return error("no physical capture ports");

			const int numports = getFrameChannels();
			for (int i = 0; i < numports; i++) {
				const char *srcport = ports[i];
				if (srcport == NULL)
					break;
				const char *destport = jack_port_name(_impl->inPorts[i]);
				if (jack_connect(_impl->client, srcport, destport) != 0)
					return error("cannot connect input ports");
			}
			free(ports);
		}
		if (isPlaying()) {
			const char **ports = jack_get_ports(_impl->client, NULL, NULL,
			                                JackPortIsPhysical | JackPortIsInput);
			if (ports == NULL)
				return error("no physical playback ports");

			const int numports = getFrameChannels();
			for (int i = 0; i < numports; i++) {
				const char *srcport = jack_port_name(_impl->outPorts[i]);
				const char *destport = ports[i];
				if (destport == NULL)
					break;
				if (jack_connect(_impl->client, srcport, destport) != 0)
					return error("cannot connect output ports");
			}
			free(ports);
		}
	}

	return 0;
}

int JackAudioDevice::doStart()
{
	PRINT0("JackAudioDevice::doStart()\n");

	// NB: This can't be done before negotiating buffer size.
	if (jack_activate(_impl->client) != 0)
		return error("Error activating JACK client.");

	return connectPorts();
}

int JackAudioDevice::doPause(bool)
{
	return error("Not implemented");
}

int JackAudioDevice::doStop()
{
	// NB: jack_client_close calls jack_deactivate, so we don't do that here.
#if 0	// this can lead to hangs on quitting
	if (jack_deactivate(_impl->client) != 0)
		return error("error deactivating JACK");
#endif
	return 0;
}

int JackAudioDevice::doSetFormat(int sampfmt, int chans, double srate)
{
	// Insure that RTcmix sampling rate matches JACK rate.
	jack_nframes_t jsrate = jack_get_sample_rate(_impl->client);
	if (jsrate != (jack_nframes_t) srate) {
		char msg[256];
		snprintf(msg, 256, "RTcmix sampling rate (set in rtsetparams) must match "
					"JACK sampling rate (currently %d)", jsrate);
		return error(msg);
// FIXME: can't we just change RTcmix::SR instead, and also notify user?
	}
	_impl->srate = jsrate;

#ifdef USE_NON_INTERLEAVED
	int deviceFormat = NATIVE_FLOAT_FMT | MUS_NON_INTERLEAVED | MUS_NORMALIZED;
#else
	int deviceFormat = NATIVE_FLOAT_FMT | MUS_INTERLEAVED | MUS_NORMALIZED;
#endif

	setDeviceParams(deviceFormat, chans, srate);

	return 0;
}

int JackAudioDevice::doSetQueueSize(int *pWriteSize, int *pCount)
{
	*pWriteSize = _impl->bufSize = jack_get_buffer_size(_impl->client);

	// NB: We ignore pCount, pretending it's always 1.  We don't change it
	// to 1 for the caller, though, because this would screw up caller's
	// recalculation of the buffer size.  We might want to warn user here.

	// Create JACK ports, one per channel, and audio buf pointer arrays.

	if (isRecording()) {
		// FIXME: we shouldn't assume that this is the same as output chans;
		// whatever happened to audioNCHANS?
		const int numports = getFrameChannels();
		_impl->inPorts = new jack_port_t * [numports];
		_impl->inBuf = new jack_default_audio_sample_t * [numports];
		char shortname[32];
		for (int i = 0; i < numports; i++) {
			snprintf(shortname, 31, "in_%d", i + 1);
			jack_port_t *port = jack_port_register(_impl->client, shortname,
			                                       JACK_DEFAULT_AUDIO_TYPE,
			                                       JackPortIsInput, 0);
			if (port == NULL)
				return error("no more JACK ports available");
			_impl->inPorts[i] = port;
			_impl->inBuf[i] = NULL;    // set in runProcess
		}
		_impl->numInPorts = numports;
	}
	if (isPlaying()) {
		const int numports = getFrameChannels();
		_impl->outPorts = new jack_port_t * [numports];
		_impl->outBuf = new jack_default_audio_sample_t * [numports];
		char shortname[32];
		for (int i = 0; i < numports; i++) {
			snprintf(shortname, 31, "out_%d", i + 1);
			jack_port_t *port = jack_port_register(_impl->client, shortname,
			                                       JACK_DEFAULT_AUDIO_TYPE,
			                                       JackPortIsOutput, 0);
			if (port == NULL)
				return error("no more JACK ports available");
			_impl->outPorts[i] = port;
			_impl->outBuf[i] = NULL;   // set in runProcess
		}
		_impl->numOutPorts = numports;
	}

	return 0;
}

#ifdef USE_NON_INTERLEAVED
int JackAudioDevice::doGetFrames(void *frameBuffer, int frameCount)
{
	const int chans = getFrameChannels();
	float **fFrameBuffer = (float **) frameBuffer;		// non-interleaved

	for (int ch = 0; ch < chans; ch++) {
		jack_default_audio_sample_t *src = _impl->inBuf[ch];
		float *dest = fFrameBuffer[ch];
		for (int i = 0; i < frameCount; i++)
			*dest++ = (float) *src++;
	}

	return frameCount;
}
#else
int JackAudioDevice::doGetFrames(void *frameBuffer, int frameCount)
{
	const int chans = getFrameChannels();
	float *dest = (float *) frameBuffer;

	for (int i = 0; i < frameCount; i++) {
		for (int ch = 0; ch < chans; ch++)
			dest[ch] = (float) _impl->inBuf[ch][i];
		dest += chans;
	}

	return frameCount;
}
#endif

#ifdef USE_NON_INTERLEAVED
int JackAudioDevice::doSendFrames(void *frameBuffer, int frameCount)
{
	const int chans = getFrameChannels();
	float **fFrameBuffer = (float **) frameBuffer;		// non-interleaved

	for (int ch = 0; ch < chans; ch++) {
		float *src = fFrameBuffer[ch];
		jack_default_audio_sample_t *dest = _impl->outBuf[ch];
		for (int i = 0; i < frameCount; i++)
			*dest++ = (jack_default_audio_sample_t) *src++;
	}

	return frameCount;
}
#else
int JackAudioDevice::doSendFrames(void *frameBuffer, int frameCount)
{
	const int chans = getFrameChannels();
	float *src = (float *) frameBuffer;

	for (int i = 0; i < frameCount; i++) {
		for (int ch = 0; ch < chans; ch++)
			_impl->outBuf[ch][i] = (jack_default_audio_sample_t) src[ch];
		src += chans;
	}

	return frameCount;
}
#endif

int JackAudioDevice::doGetFrameCount() const
{
	return _impl->frameCount;
}

bool JackAudioDevice::recognize(const char *desc)
{
	if (desc == NULL)
		return false;
	return strncmp(desc, "JACK", 4) == 0
	       || strncmp(desc, "jack", 4) == 0;
}

// <outputDesc> will begin with either "JACK" or "jack."  Optionally,
// a JACK server name immediately follows a colon delimiter.

AudioDevice *JackAudioDevice::create(const char *inputDesc,
	const char *outputDesc, int mode)
{
	const char *serverName = "default";

	// NB: we assume that inputDesc is the same as outputDesc
	if (outputDesc) {
		char *delim = strchr(outputDesc, ':');
		if (delim)
			serverName = delim + 1;
	}

	return new JackAudioDevice(serverName);
}

#endif	// JACK
