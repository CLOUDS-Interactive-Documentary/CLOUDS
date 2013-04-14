#include <sfheader.h>
#include "byte_routines.h"
#include <stdio.h>
#include <math.h>
#include <ugens.h>
#include <sys/types.h>
#include <unistd.h>


#if defined(NeXT) && defined(i386)
#include <architecture/byte_order.h>
#endif

extern SFHEADER sfdesc[NFILES];
extern int sfd[NFILES];
extern int swap_bytes[NFILES];
extern int bufsize[NFILES];
extern char *sndbuf[NFILES];
extern int headersize[NFILES];

int (*getsample)();
static int getfsample();
static int getisample();


int
getsetnote(float start, float dur, int filenum)
{
	int nsamples = setnote(start,dur,filenum);
	_backup(filenum);
	if(sfclass(&sfdesc[filenum]) == SF_FLOAT) getsample = getfsample;
	else          	                          getsample = getisample;
	return(nsamples);

}

static int
getisample(double sampleno, float *c, int input)
{

	ssize_t RECSIZE = bufsize[input];
	ssize_t BPREC = RECSIZE * sizeof(short);
	ssize_t BPFRAME = sfchans(&sfdesc[input]) * sizeof(short);
	ssize_t FPREC = RECSIZE/sfchans(&sfdesc[input]);

	int sample,i,j;
    ssize_t nbytes;
	signed short *array = (short *)sndbuf[input];
	float tempsample1;
	float tempsample2;
	float fraction;
	static int oldsample = 0;
	static int endsample = 0;

	sample = (int)sampleno;
	fraction = sampleno - (double)sample;
	if(!((sample >= oldsample) && (sample < endsample))) {
		/* sflseek (sfheader.h) assumes header size, so can't use it */
		if(lseek(sfd[input], (sample * BPFRAME) + headersize[input],
							SEEK_SET) <= 0) {
			rterror(NULL, "badlseek on inputfile\n");
			closesf();
		}
		nbytes = read(sfd[input], (char *)array, BPREC);
		if (nbytes == -1) {
			perror("getisample: read");
			return 0;
		}
		if (nbytes == 0) {
			rterror(NULL, "reached eof on input file\n");
			return 0;
		}
		if (nbytes < BPREC) {    /* zero out rest of sndbuf */
			ssize_t n;
			for (n = nbytes; n < BPREC; n++)
				sndbuf[input][n] = 0;
		}
		oldsample = sample;
		endsample = oldsample + FPREC - 1;
		}
	for(i=(sample-oldsample)*sfchans(&sfdesc[input]),j=0; 
	    j<sfchans(&sfdesc[input]); i++,j++)  {
	  
	  if (swap_bytes[input]) {
	    tempsample1 = (signed short)reverse_int2(array+i);
	    tempsample2 = (signed short)reverse_int2(array+i+sfchans(&sfdesc[input]));
	    *(c+j) = tempsample1 + fraction * 
	      ((float)((signed short)tempsample2 - tempsample1));
	  }
	  else {
	    *(c+j) = (float)*(array+i) + fraction * 
	      ((float) *(array+i+sfchans(&sfdesc[input])) - 
	       (float) *(array+i));
	  }
	}
	return(1);

}

static int
getfsample(double sampleno, float *c, int input)
{
	int RECSIZE = bufsize[input];
	int BPREC = RECSIZE * sizeof(float);
	int BPFRAME = sfchans(&sfdesc[input]) * sizeof(float);
	int FPREC = RECSIZE/(float)sfchans(&sfdesc[input]);

	int sample,i,j,nbytes;
	float *array = (float *)sndbuf[input];
	float fraction,tempfloat1,tempfloat2;
	static int oldsample = 0;
	static int endsample = 0;
	extern float swapfloat();

	sample = (int)sampleno;
	fraction = sampleno - (double)sample;
	if(!((sample >= oldsample) && (sample < endsample))) {
		/* sflseek (sfheader.h) assumes header size, so can't use it */
		if(lseek(sfd[input], (sample * BPFRAME) + headersize[input],
							SEEK_SET) <= 0) {
			rterror(NULL,"badlseek on inputfile\n");
			closesf();
		}
		nbytes = read(sfd[input], (char *)array, BPREC);
		if (nbytes == -1) {
			perror("getfsample: read");
			return 0;
		}
		if (nbytes == 0) {
			rterror(NULL, "reached eof on input file\n");
			return 0;
		}
		if (nbytes < BPREC)     /* zero out rest of sndbuf */
			for (i = nbytes; i < BPREC; i++)
				sndbuf[input][i] = 0;
		oldsample = sample;
		endsample = oldsample + FPREC - 1;
		}
	for(i = (sample-oldsample)*sfchans(&sfdesc[input]),j=0; 
					j<sfchans(&sfdesc[input]); i++,j++)  {
	  if (swap_bytes[input]) {
	    tempfloat1 = *(array+i);
	    byte_reverse4(&tempfloat1);
	    tempfloat2 = (*(array+i+sfchans(&sfdesc[input])));
	    byte_reverse4(&tempfloat2)
	      *(c+j) = tempfloat1 + fraction * (tempfloat2-tempfloat1);
	  }
	  else {
	    *(c+j) = *(array+i) + fraction * 
	      (*(array+i+sfchans(&sfdesc[input])) - *(array+i));
	  }
	}
	return(1);
}

