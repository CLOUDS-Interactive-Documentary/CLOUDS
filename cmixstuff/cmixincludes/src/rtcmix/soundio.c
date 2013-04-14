#ifdef OBSOLETE

#include <stdio.h>
#include <math.h>
#include "../H/sfheader.h"
#include "../H/ugens.h"

/* DT:  #include <audio.h> */
extern int play_is_on;
extern int print_is_on;
extern  SFHEADER        sfdesc[NFILES];

#ifdef sgi

/*
    This portion of soundio.c is specific for use on an SGI machine.

    See soundio.m for NeXT code.

	See bottom of file for default for other platforms.
	
    -Doug Scott, 10/95
*/

static ALconfig audio_port_config;
static ALport audio_port;

int
init_sound(sr,chans)
float sr;
int chans;
{
	long pvbuf[2];
	audio_port_config = ALnewconfig();
	if(!audio_port_config)
		closesf();
	if(ALsetchannels(audio_port_config, chans) < 0)
		closesf();
	/* make output queue twice the size of our buffer */
	if(ALsetqueuesize(audio_port_config, 32768 * 2) < 0)
		closesf();

	if(audio_port) ALcloseport(audio_port);
	audio_port = ALopenport("cmix", "w", audio_port_config);
	if(!audio_port)
		closesf();

	ALfreeconfig(audio_port_config);
	audio_port_config = 0;

	pvbuf[0] = AL_OUTPUT_RATE;
	pvbuf[1] = (long) sr;
	if(ALsetparams(AL_DEFAULT_DEVICE, pvbuf, 2) < 0) { /* set output SR */
		fprintf(stderr, "Failed to set converter SR!\n");
		closesf();
	}
	/* tell port to accept refill at 25% capacity */
/*	ALsetfillpoint(audio_port, 32768/2); */
	return 1;
}

int
playbuf(buf,bufsize)
short *buf;
int bufsize;
{
	ALwritesamps(audio_port, buf, bufsize);
}

int audio_port;

double
play_off(p,n_args)
float *p;
{
	play_is_on=0;
	if(audio_port) close(audio_port);
	audio_port = 0;
	return 0;
}

void
flush_buffers()
{	
#ifdef DEBUG
	printf("flushing...\n");
#endif
	while(ALgetfilled(audio_port) > 0) { sginap(1); }
}

void playfbuf(inbuf,peak,swap,samples)
 float *inbuf,peak;
 int  samples,swap;
{
	short outbuf[32768]; /* hardwired buffer size */
        register int n = samples;
        float factor = 32767./peak;
	do {
	     outbuf[samples-n]= (short)(inbuf[samples-n] * factor);
	} while (--n);
	playbuf(outbuf,samples);
}

#else /* sgi */

/* default routine for all other platforms other than SGI (above) and
   NeXT (in soundio.m)
*/

int
init_sound(sr,chans)
float sr;
int chans;
{
	fprintf(stderr, "Sorry -- real time playback not supported here.\n");
	play_is_on = 0;
	return 0;
}

#endif /* sgi */


#endif /* OBSOLETE */
