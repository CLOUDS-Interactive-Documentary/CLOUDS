// SGIAudioDevice.cpp

#if defined(SGI)

#include "SGIAudioDevice.h"
#include <ulocks.h>

#define ZERO_FRAMES_BEFORE       0
#define ZERO_FRAMES_AFTER        16384

static ALport in_port;
static ALport out_port;

/* This macro is only in AL 2.0, so I use it to distinguish 2.0 from 1.0 */
#ifdef AL_MAX_STRLEN
  #define AL_2_0
#endif

#ifndef AL_2_0
  #define alSetSampFmt ALsetsampfmt
  #define alSetFloatMax ALsetfloatmax
  #define alSetWidth ALsetwidth
  #define alSetChannels ALsetchannels
  #define alOpenPort ALopenport
  #define alClosePort ALcloseport
  #define alSetFloatMax ALsetfloatmax
  #define alNewConfig ALnewconfig
  #define alFreeConfig ALfreeconfig
  #define alSetConfig ALsetconfig
  #define alSetQueueSize ALsetqueuesize

  /* this function doesn't exist under 1.0 */
  const char *alGetErrorString(int err) { return ""; }
#endif /* !AL_2_0 */

struct SGIAudioDevice::Impl {
	ALport	port;
	int		chans;
};

SGIAudioDevice::SGIAudioDevice(const char *selector) : _impl(new Impl) {}

SGIAudioDevice::~SGIAudioDevice() { delete _impl; }

void
SGIAudioDevice::run()
{
}

int
SGIAudioDevice::doOpen(int mode)
{
	return error("Not implemented");
}

int
SGIAudioDevice::doClose()
{
//	while (ALgetfilled(_impl->port) > 0) ;	/* wait till queue flushed */
	return error("Not implemented");
}

int
SGIAudioDevice::doStart()
{
	return error("Not implemented");
}

int
SGIAudioDevice::doPause(bool)
{
}

int
SGIAudioDevice::doSetFormat(int sampfmt, int chans, double srate)
{
	return error("Not implemented");
}

int
SGIAudioDevice::doSetQueueSize(int *pWriteSize, int *pCount)
{
	return error("Not implemented");
}

int
SGIAudioDevice::doGetFrames(void *frameBuffer, int frameCount)
{
//	ALreadsamps(_impl->port, frameBuffer, frameCount * _impl->chans);

	return error("Not implemented");
}

int
SGIAudioDevice::doSendFrames(void *frameBuffer, int frameCount)
{
	return error("Not implemented");
}

bool SGIAudioDevice::recognize(const char *desc)
{
	return true;
}

AudioDevice *SGIAudioDevice::create(const char *inputDesc, const char *outputDesc, int mode)
{
	return new SGIAudioDevice(outputDesc);
}

/* ------------------------------------------------- open_sgi_audio_input --- */
static int
open_sgi_audio_input(AudioPortType port_type, int nchans)
{
   int      port;
   long     pvbuf[4];
   long     buflen;
   ALconfig in_port_config;

   /* configure and open input audio port */
   in_port_config = ALnewconfig();
   ALsetwidth(in_port_config, AL_SAMPLE_16);

   if (nchans == 1)
      ALsetchannels(in_port_config, AL_MONO);
   else if (nchans == 2)
      ALsetchannels(in_port_config, AL_STEREO);

   if (ALsetqueuesize(in_port_config, RTBUFSAMPS * 4) == -1) {
      rterror("rtinput",
              "Could not configure the input audio port queue size to %d.\n",
              RTBUFSAMPS * 4);
      return -1;
   }

   switch (port_type) {
      case LINE:
         port = AL_INPUT_LINE;
         break;
      case DIGITAL:
         port = AL_INPUT_DIGITAL;
         break;
      default:
         port = AL_INPUT_MIC;
         break;
   }

   pvbuf[0] = AL_INPUT_RATE;
   pvbuf[1] = (long) SR;
   pvbuf[2] = AL_INPUT_SOURCE;
   pvbuf[3] = port;
   buflen = 4;

   ALsetparams(AL_DEFAULT_DEVICE, pvbuf, buflen);

   /* open up thet thar audio port! */
   in_port = ALopenport("rtcmixin", "r", in_port_config);

   ALfreeconfig(in_port_config);

   if (!in_port) {
      fprintf(stderr, "Could not open input audio port.\n");
      return -1;
   }

   return AUDIO_DEVICE_FD;          /* our fake fd for SGI audio devices */
}

#endif	// SGI

