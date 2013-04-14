/*   sound i/o routines 
*/
/* this version uses pointers to differentiate between different flavours
   of i/o, FLOAT and INT */
/* version that just writes/reads to standard UNIXFILES, with sfheader.*/
/* TODO: keep one file from being opened twice for writing.
	 implement mag tape reads and wipeouts 
	?BUG after using layout, should not call flushbuf
	?in zap and alter routines, (when else..?

	 to add tape read/write need additional arg in open to indicate
	 file number, and this show that it is tape.
	 also need label checking.  first record on each tape file will be
	 header.  
	 only other change for tape is to use read instead of lseek, and 
	 only allow getin and wipeout routines.  also have to add librarian
	 to keep track of what file we are on, and watch out for close, and 
	 thus screw up in eof marks.  (need to know whether we have been 
	 reading or writing.

	 6/6 tape seems to work, need to add positioning, and file finds

	12/12/91 -- tape hooks are here, but are unimplemented, probably not
				worth it.  PL
	10/95 -- moved common play_on() routine here.  play_off() still in
	         soundio.c or soundio.m due to platform-specific stuff -DS
	6/99 -- many changes to accommodate sndlib support, etc.  -JGG
*/

#define SOUND
#include "prototypes.h"
#include <ugens.h>
#include <sndlibsupport.h>
#include <sfheader.h>
#include "byte_routines.h"
#include "dbug.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/times.h>
#include <string.h>
#include <Option.h>


/* Used to determine if we should swap endian-ness */
extern int swap;       /* defined in check_byte_order.c */
int swap_bytes[NFILES];
static short is_Next[NFILES];
extern short isNext;

/* size of buffer to allocate.  this is no longer user-configurable */
/* JGG: might want to make this 64*1024 in Linux */
off_t nbytes = 32768;          /* exported only for the sake of sfcopy.c */

static int play_is_on=0;
int  sfd[NFILES];            /* soundfile descriptors */
int  pointer[NFILES];	     /* to be used as pointer within sound buffer */
int  bufsize[NFILES];        /* word length of buffer */

char *sndbuf[NFILES];        /* address of buffer */
static char *peak[NFILES];          /* array to store peak amplitude for nchannels */
static char *peakloc[NFILES];       /* overall peak amplitude */
			   
char wipe_is_off[NFILES];    /* this is for wipeout */
char peakoff[NFILES];        /* this will set peak test on or off*/
static float punch[NFILES];	     /* punch alteration flags */
static char istape[NFILES];         /* flag to see if it is a tape unit */
static double starttime[NFILES];    /* to save starting time of note */
static off_t  originalsize[NFILES];  /* to save byte length of file */
off_t  filepointer[NFILES];   /* to save current pointer in file */
int status[NFILES];	     /* save read/write flage as well */
int isopen[NFILES];	     /* open status */
int headersize[NFILES];      /* to accomodate bsd and next headers */

SFHEADER      sfdesc[NFILES];
SFMAXAMP      sfm[NFILES];
struct stat   sfst[NFILES];

static SFCODE ampcode = {
	SF_MAXAMP,
	sizeof(SFMAXAMP) + sizeof(SFCODE)
};

static struct tms    clockin[NFILES];
 
float getpeakval(float peakflag, int fno);
void m_zapout(int fno, char *buffer, int nwrite, int *chlist);

static int _iaddout(float *out, int fno);
static int _faddout(float *out, int fno);
static int _igetin(float *in, int fno);
static int _fgetin(float *in, int fno);
static int _ilayout(float *out, int *chlist, int fno);
static int _flayout(float *out, int *chlist, int fno);
static int _iwipeout(float *out, int fno);
static int _fwipeout(float *out, int fno);

int (*addoutpointer[NFILES])();
int (*layoutpointer[NFILES])();
int (*wipeoutpointer[NFILES])();
int (*getinpointer[NFILES])();

/*****   for macros */
static float FTEMP1,FTEMP2,FTEMP3,FTEMP4;
static int ITEMP1,ITEMP2;
static float *PTEMP1;

static char *sfname[NFILES];
static float peakflag;

double m_open(float *p, short n_args, double *pp) 
{
	char  *name,*cp,*getsfcode();
	int   fno,i,inew;
	float *opk;

	name = DOUBLE_TO_STRING(pp[0]);
	fno = p[1];
// JGG: will name ptr be valid for entire program run? Is its memory held by
// parser? If not, we should malloc sfname[fno] below (with other mallocs)
	sfname[fno] = name;
	status[fno] = (n_args == 3) ? (int)p[2] : 2;

	if((fno >=  NFILES) || (fno < 0)) {
		rtcmix_warn("m_open", "Only %d files allowed\n", NFILES);
		closesf();
		}
	inew = 0;
	if(isopen[fno]) {
		close(sfd[fno]);
	}
	else inew = 1;

	istape[fno] = (n_args == 4) ? 1 : 0;
			/* in the case of a tape, there will be a 
			   4th argument listing the file number */

	rwopensf(name,sfd[fno],sfdesc[fno],sfst[fno],"CMIX",i,status[fno]);
	if (i < 0)
		closesf();

	if (status[fno] == O_RDWR
			&& !WRITEABLE_HEADER_TYPE(sfheadertype(&sfdesc[fno]))) {
		rtcmix_warn("m_open", "can't write this type of header.\n");
		closesf();
	}

	isopen[fno] = 1;

	swap_bytes[fno] = swap;  /* swap and isNext set in rwopensf */
	is_Next[fno] = isNext;
	headersize[fno] = getheadersize(&sfdesc[fno]);

	rtcmix_advise(NULL, "name: %s   sr: %.3f  nchans: %d  class: %d\n",name,
		sfsrate(&sfdesc[fno]),sfchans(&sfdesc[fno]), sfclass(&sfdesc[fno]));
	rtcmix_advise(NULL, "Soundfile type: %s\n",
			mus_header_type_name(sfheadertype(&sfdesc[fno])));
	rtcmix_advise(NULL, "   data format: %s\n",
			mus_data_format_name(sfdataformat(&sfdesc[fno])));
	rtcmix_advise(NULL, "Duration of file is %f seconds.\n",
		(float)(sfst[fno].st_size - headersize[fno])/(float)sfclass(&sfdesc[fno])/(float)sfchans(&sfdesc[fno])/sfsrate(&sfdesc[fno]));

	originalsize[fno] = istape[fno] ? 999999999 : sfst[fno].st_size;
	/*
	sfstats(sfd[fno]);
	*/
	if(inew) {
		if((sndbuf[fno] = (char *)malloc((unsigned)nbytes)) == NULL) {
			rtcmix_warn("CMIX", "malloc sound buffer error\n");
			closesf();
		}
		if((peakloc[fno] = (char *)malloc((unsigned)(sfchans(&sfdesc[fno]) * 
			LONG))) == NULL) {
			rtcmix_warn("CMIX", "malloc ovpeak buffer error\n");
			closesf();
		}
		if((peak[fno] = 
			(char *)malloc((unsigned)(sfchans(&sfdesc[fno])* FLOAT))) 
			== NULL) {
			rtcmix_warn("CMIX", "malloc peak buffer error!\n");
			closesf();
		}
		peakoff[fno] = 0; /* default to peakcheckon when opening file*/
		punch[fno] = 0; /* default to no punch when opening file*/
	}
	if(sfclass(&sfdesc[fno]) == SHORT) {
		addoutpointer[fno] = _iaddout;
		layoutpointer[fno] = _ilayout;
		wipeoutpointer[fno] = _iwipeout;
		getinpointer[fno] = _igetin;
	}
	else 			        {   
		addoutpointer[fno] = _faddout;
		layoutpointer[fno] = _flayout;
		wipeoutpointer[fno] = _fwipeout;
		getinpointer[fno] = _fgetin;
	}

	if(!SR()) set_SR(sfsrate(&sfdesc[fno]));	

	if(sfsrate(&sfdesc[fno])!= SR())
		rtcmix_advise("CMIX", "Note--> SR reset to %f\n",SR());

	/* read in former peak amplitudes, make sure zero'ed out to start.*/

	/* In the sndlib version, we store peak stats differently. See
	   comments in sndlibsupport.c for an explanation. The sndlib
	   version of rwopensf reads peak stats, so here we just have to
	   copy these into the sfm[fno] array. (No swapping necessary.)
	*/
	memcpy(&sfm[fno], &(sfmaxampstruct(&sfdesc[fno])), sizeof(SFMAXAMP));

	for(opk = (float *)peak[fno], i = 0; i<sfchans(&sfdesc[fno]); i++) 
		*(opk+i) = sfmaxamp(&sfm[fno],i);
	bufsize[fno] = nbytes / sfclass(&sfdesc[fno]);/* set size in words */

	return 0.0;
}

int
setnote(float start, float dur, int fno)
{
	int nsamps,offset;
	int i;

	if(!isopen[fno]) {
		rtcmix_warn("setnote","You haven't opened file %d yet!\n",fno);
		closesf();
	}
	if(start > 0.) /* if start < 0 it indicates number of samples to skip*/
	        offset = (int) (start * SR() + .5) * sfchans(&sfdesc[fno])
	    		* sfclass(&sfdesc[fno]);

	else    offset = -start * sfchans(&sfdesc[fno]) * sfclass(&sfdesc[fno]);

		/* make sure it falls on channel/block boundary */
	offset -= offset % (sfchans(&sfdesc[fno]) * sfclass(&sfdesc[fno]));
	offset = (offset < 0) ? 0 : offset;

	nsamps = (dur > 0.) ? (int)((start+dur) * SR() -
	(offset/(sfchans(&sfdesc[fno])*sfclass(&sfdesc[fno])))+ .5) : (int)-dur;

	if(!istape[fno]) {
		if((filepointer[fno] = 
		   lseek(sfd[fno],offset+headersize[fno],0)) == -1) {
			rtcmix_warn("setnote", "CMIX: bad lseek in setnote\n");
			closesf();
		}
	}
	pointer[fno] = 0;

	_readit(fno);   /* read in first buffer */

	for(i=0; i<(sfchans(&sfdesc[fno]) * FLOAT); i++)
		*(peak[fno] + i) = 0;

	wipe_is_off[fno] = 1;          /* for wipeout */

	starttime[fno] = (start<0) ? -start/SR() : start;

	times(&clockin[fno]);       /* read in starting time */

	return(nsamps);
}

static int
_iaddout(float *out, int fno)
{
  int i;
  int ipoint = pointer[fno];
  int incr = sfchans(&sfdesc[fno]);
  short *ibuf;
  
  for(i=0,ibuf = (short *)sndbuf[fno] + ipoint; i<incr; i++) {
    *(ibuf + i) +=  *(out+i); 
  }
  
  if((pointer[fno] += i) >= bufsize[fno] ) {
    _backup(fno);
    if(!peakoff[fno])
      _chkpeak(fno);
    _writeit(fno);
    _readit(fno);
    pointer[fno] = 0;
  }
  return 0;
}

static int
_faddout(float *out, int fno)
{
	register int i;
	register int ipoint = pointer[fno];
	register int incr = sfchans(&sfdesc[fno]);
	register float *fbuf;

	for(i=0,fbuf = (float *)sndbuf[fno] + ipoint; i<incr; i++) {
	  *(fbuf + i) +=  *(out+i);
	}
	if((pointer[fno] += i) >= bufsize[fno] ) {
	  _backup(fno);
	  if(!peakoff[fno]) _chkpeak(fno);
	  _writeit(fno);
	  _readit(fno);
	  pointer[fno] = 0;
	}
	return 0;
}

static int
_igetin(float *in, int fno)
{
	int i;
	int ipoint = pointer[fno];
	int incr = sfchans(&sfdesc[fno]);
	short *ibuf;

	for(i=0,ibuf = (short *)sndbuf[fno] + ipoint; i<incr; i++) {
	  *(in+i) = *(ibuf + i);
	}
	if((pointer[fno] += i) >= bufsize[fno] ) {
		pointer[fno] = 0;
		return(_readit(fno));
	}
	return(nbytes);
}

static int
_fgetin(float *in, int fno)
{
	register int i;
	register int ipoint = pointer[fno];
	register int incr = sfchans(&sfdesc[fno]);
	register float *fbuf;

	for(i=0,fbuf = (float *)sndbuf[fno] + ipoint; i<incr; i++) {
	  *(in+i) =  *(fbuf + i);
	}
	if((pointer[fno] += i) >= bufsize[fno] ) {
		pointer[fno] = 0;
		return(_readit(fno));
	}
	return(nbytes);
}

static int
_ilayout(float *out, int *chlist, int fno)
{
	register int i;
	register int ipoint = pointer[fno];
	register int incr = sfchans(&sfdesc[fno]);
	register short *ibuf;

	for(i=0,ibuf = (short *)sndbuf[fno] + ipoint; i<incr; i++) {
	  if(chlist[i]) {
	      *(ibuf + i) = *(out+i); 
	  }
	}
	if((pointer[fno] += i) >= bufsize[fno] ) {
		_backup(fno);
		if(!peakoff[fno]) _chkpeak(fno);
		_writeit(fno);
		_readit(fno);
		pointer[fno] = 0;
	}
	return 0;
}

static int
_flayout(float *out, int *chlist, int fno)
{
	register int i;
	register int ipoint = pointer[fno];
	register int incr = sfchans(&sfdesc[fno]);
	register float *fbuf;

	for(i=0,fbuf = (float *)sndbuf[fno] + ipoint; i<incr; i++) {
	  if(chlist[i]) {
	    *(fbuf + i) =  *(out+i);
	  }
	}
	if((pointer[fno] += i) >= bufsize[fno] ) {
		_backup(fno);
		if(!peakoff[fno]) _chkpeak(fno);
		_writeit(fno);
		_readit(fno);
		pointer[fno] = 0;
	}
	return 0;
}

static int
_iwipeout(float *out, int fno)   /* to force destructive writes */ 
{
	register int i;
	register int ipoint = pointer[fno];
	register int incr = sfchans(&sfdesc[fno]);
	register short *ibuf;

	for(i=0,ibuf = (short *)sndbuf[fno] + ipoint; i<incr; i++) {
	  *(ibuf + i) = *(out+i); 
	}
	if((pointer[fno] += i) >= bufsize[fno] ) {
		if(wipe_is_off[fno]) {   /*setnot positions after first read*/
			_backup(fno);
			wipe_is_off[fno] = 0;
			}
		if(!peakoff[fno])
			_chkpeak(fno);
		_writeit(fno);
		pointer[fno] = 0;
	}
	return 0;
}

static int
_fwipeout(float *out, int fno)   /* to force destructive writes */ 
{
	register int i;
	register int ipoint = pointer[fno];
	register int incr = sfchans(&sfdesc[fno]);
	register float *fbuf;

	for(i=0,fbuf = (float *)sndbuf[fno] + ipoint; i<incr; i++) {
	  *(fbuf + i) =  *(out+i);
	}
	if((pointer[fno] += i) >= bufsize[fno] ) {
		if(wipe_is_off[fno]) {   /*setnot positions after first read*/
			_backup(fno);
			wipe_is_off[fno] = 0;
			}
		if(!peakoff[fno])
			_chkpeak(fno);
		_writeit(fno);
		pointer[fno] = 0;
	}
	return 0;
}

int
bgetin(float *input, int fno, int size)
{
	register int i;
	register short *ibuf;
	register float *fbuf;
	register int todo;
	int n;
	int len = bufsize[fno]; 

refill:	todo = ((pointer[fno] + size) > len) 
				? len - pointer[fno] : size;

        /* If it's a short */
	if(sfclass(&sfdesc[fno]) == SHORT) {
	  for(i=0,ibuf = (short *)sndbuf[fno] + pointer[fno];i<todo;i++) {
	    *(input++) = (float) *(ibuf++);
	  }
	}
	
	/* If it's a float */
	else {
	  for(i=0,fbuf = (float *)sndbuf[fno] + pointer[fno];i<todo;i++) {
	    *(input++) =  *(fbuf++);
	  }
	}
	
	pointer[fno] += todo;
	
	if(pointer[fno] == len) {
		n = _readit(fno);
		pointer[fno] = 0;
		if(!n) return(n);
	}
	if(size -= todo) goto refill;
	return(i);
}

void
blayout(float *out, int *chlist, int fno, int size)
{
	register int i,j;
	register short *ibuf;
	register float *fbuf;
	register int todo;
	register int nchans;
	int len = bufsize[fno]; 

	nchans = sfchans(&sfdesc[fno]);

refill:	todo = ((pointer[fno] + size) > len) 
				? len - pointer[fno] : size;
	if(sfclass(&sfdesc[fno]) == SF_SHORT) {
	  for(i=0,ibuf = (short *)sndbuf[fno] + pointer[fno];i<todo;i += nchans) {
	    for(j=0; j<nchans; j++,ibuf++,out++) {
	      if(chlist[j]) {
		*ibuf = (short) *out;	
	      }
	    }
	  }
	}
	else {
	  for(i=0,fbuf = (float *)sndbuf[fno] + pointer[fno];i<todo;i += nchans) {
	    for(j=0; j<nchans; j++,fbuf++,out++) {
	      if(chlist[j]) {
		*fbuf = *out;
	      }
	    }
	  }
	}
	pointer[fno] += todo;

	if(pointer[fno] == len) {
		_backup(fno);
		if(!peakoff[fno])
			_chkpeak(fno);
		_writeit(fno);
		_readit(fno);
		pointer[fno] = 0;
	}

	if(size -= todo) goto refill;
}

void
baddout(float *out, int fno, int size)
{
	register int i;
	register short *ibuf;
	register float *fbuf;
	register int todo;
	int len = bufsize[fno]; 
	
 refill:	todo = ((pointer[fno] + size) > len) 
		  ? len - pointer[fno] : size;
	
	if(sfclass(&sfdesc[fno]) == SHORT) {
	  for(i=0,ibuf = (short *)sndbuf[fno] + pointer[fno];i<todo;i++) {
	    *(ibuf++) += (short) *(out++);
	  }
	}

	else {
	  for(i=0,fbuf = (float *)sndbuf[fno] + pointer[fno];i<todo;i++) {
	    *(fbuf++) += *(out++);
	  }
	}

	pointer[fno] += todo;

	if(pointer[fno] == len) {
		_backup(fno);
		if(!peakoff[fno])
			_chkpeak(fno);
		_writeit(fno);
		_readit(fno);
		pointer[fno] = 0;
	}
	
	if(size -= todo) goto refill;
}

void
bwipeout(float *out, int fno, int size)
{
	register int i;
	register short *ibuf;
	register float *fbuf;
	register int todo;
	int len = bufsize[fno]; 

	
refill:	todo = ((pointer[fno] + size) > len) 
				? len - pointer[fno] : size;

	if(sfclass(&sfdesc[fno]) == SHORT) {
	  for(i=0,ibuf = (short *)sndbuf[fno] + pointer[fno];i<todo;i++) {
	    *(ibuf++) = (short)*(out++);
	  }
	}
	else {
	  for(i=0,fbuf = (float *)sndbuf[fno] + pointer[fno];i<todo;i++) {
	    *(fbuf++) = *(out++);
	  }
	}

	pointer[fno] += todo;

	if(pointer[fno] == len) {
		if(wipe_is_off[fno]) {   
			_backup(fno);
			wipe_is_off[fno] = 0;
			}
		if(!peakoff[fno])
			_chkpeak(fno);
		_writeit(fno);
		pointer[fno] = 0;
	}
	if(size -= todo) goto refill;
}

int
endnote(int xno)
{
	struct timeval tp;	
	struct timezone tzp;	
	int i,j,final_bytes,fno;
	float notepeak,*pk;
	double total;
	long *pkloc;
	struct tms timbuf;
	float peakval;
	struct stat st;
	short tisamp,*tibuf;
	float tfsamp,*tfbuf;

	fno = ABS(xno);  /* if fno is negative it means don't write
					final buffer,just pretend to */
	if(wipe_is_off[fno]) 
		_backup(fno); 
	/* else _flushbuf(fno); */
	if(!peakoff[fno]) _chkpeak(fno);
	final_bytes =  pointer[fno]  * sfclass(&sfdesc[fno]);
	
	/* This was DS's and PL's version of real time */
	/* Not used in this version */
#ifdef OLDRT

	/*  SHOULD NOT PLAY HERE -- LAST BUFFERS ALREADY PLAYED */
	if ((sfclass(&sfdesc[fno]) == SF_SHORT) && play_is_on)
		playbuf(sndbuf[fno],final_bytes/SF_SHORT);
	else if ((sfclass(&sfdesc[fno]) == SF_FLOAT) && play_is_on) {
		peakval = getpeakval(peakflag,fno);
		playfbuf(sndbuf[fno],peakval,swap[fno],nbytes/SF_FLOAT);
	}
#endif

	/* write out only fractional part of last record, god bless unix!*/
	if(pointer[fno] && (play_is_on < 2)) {
		if(xno >= 0) {
			/* Swap bytes if necessary */
			if(final_bytes && swap_bytes[fno]) {
				/* SHORT file */
				if(sfclass(&sfdesc[fno]) == SF_SHORT) {
					tibuf = (short *)sndbuf[fno]; 
					for (i=0;i<final_bytes/SF_SHORT;i++) {
						tisamp = *(tibuf+i);
						*(tibuf+i) = reverse_int2(&tisamp);
					}
				}
				/* FLOAT file */
				if(sfclass(&sfdesc[fno]) == SF_FLOAT) {
					tfbuf = (float *)sndbuf[fno]; 
					for (i=0;i<final_bytes/SF_FLOAT;i++) {
						/* 	byte_reverse4(tfbuf+i); */
						/* 	tfsamp = *(tfbuf+i); */
						/* 	*(tfbuf+i) = (float)reverse_int4(&tfsamp); */
					  	tfsamp = *(tfbuf+i);
						byte_reverse4(&tfsamp);
					  	*(tfbuf+i) = tfsamp;
					}
				}
   			}
   			if((i = write(sfd[fno],sndbuf[fno],final_bytes)) 
											!= final_bytes) {
				rtcmix_warn("CMIX", "Bad UNIX write, file %d, nbytes = %d\n",
					fno,i);
				perror("write");
				closesf();
   			}
   		}
   		if((filepointer[fno] += final_bytes) > originalsize[fno]) 
   		if(xno >0)  originalsize[fno] = filepointer[fno];
	}
	/* DT: 	if(play_is_on) flush_buffers(); */
	
	pk = (float *)peak[fno];
	pkloc = (long *)peakloc[fno];
	total = ((double)filepointer[fno]-headersize[fno])
					/((double)sfclass(&sfdesc[fno]))
					/(double)sfchans(&sfdesc[fno])/SR();
	
	/* _writeit(fno);  write out final record */

	for(i = 0,notepeak=0; i<sfchans(&sfdesc[fno]); i++) { 
		if(*(pk+i) > sfmaxamp(&sfm[fno],i)) {
			sfmaxamp(&sfm[fno],i) = *(pk+i);
			sfmaxamploc(&sfm[fno],i) = *(pkloc+i);
		}
		if(*(pk+i) > notepeak) notepeak = *(pk+i);
	}
	
	gettimeofday(&tp,&tzp);
	sfmaxamptime(&sfm[fno]) = tp.tv_sec;
		
	if((filepointer[fno] = lseek(sfd[fno],0L,0)) < 0) {
		rtcmix_warn("CMIX", "Bad lseek to beginning of file\n");
		perror("lseek");
		closesf();
	}


	times(&timbuf);

#ifndef MAXMSP // this really isn't used...
	printf("\n(%6.2f)",(float)(
					(timbuf.tms_stime-clockin[fno].tms_stime)+
					(timbuf.tms_utime-clockin[fno].tms_utime))/60.);
	printf(" %9.4f .. %9.4f MM ",starttime[fno],total);
	
	if(!peakoff[fno]) {
		for(j=0;j<sfchans(&sfdesc[fno]);j++)
			printf(" c%d=%e",j,*(pk+j));
		printf("\n");
		if(punch[fno]) {
			printf("alter(%e,%e,%e/%e",
						(double)starttime[fno],(double)(total-starttime[fno]),
						punch[fno],notepeak);
			for(i=0; i<sfchans(&sfdesc[fno]); i++)
				printf(",1 ");
			printf(")\n");
			printf("mix(%g,%g,%g,%g/%g",
							(double)starttime[fno],(double)starttime[fno],-(double)(total-starttime[fno]),punch[fno],notepeak);
			for(i=0; i<sfchans(&sfdesc[fno]); i++)
				printf(",%d ",i);
			printf(")\n");
		}
	}
#endif // MAXMSP

	/* Copy the updated peak stats into the SFHEADER struct for this
	   output file. (No swapping necessary.)
	*/
	memcpy(&(sfmaxampstruct(&sfdesc[fno])), &sfm[fno], sizeof(SFMAXAMP));

	/* Write header to file. */
	if (wheader(sfd[fno], &sfdesc[fno])) {
		rtcmix_warn("endnote", "bad header write\n");
		perror("write");
		closesf();
	}
	return 0;
}

void
_flushbuf(int fno)
{
	register int i;
	for(i=pointer[fno]*sfclass(&sfdesc[fno]); i<nbytes; i++)
		*(sndbuf[fno] + i) = 0;
}

void
_chkpeak(int fno)
{
	register int i,incr;
	register short *ibuf,*bufend;
	register float *fbuf,*fbufend,*pk;
	short *ibufx;
	float *fbufx;
	long *pkloc,currentloc;

	pk = (float *)peak[fno];
	incr = sfchans(&sfdesc[fno]);
	pkloc = (long *)peakloc[fno];

	if(sfclass(&sfdesc[fno]) == SHORT) {
		ibufx = ibuf = (short *)sndbuf[fno];
		bufend = ibuf + pointer[fno]; /* to allow for final check */
		currentloc = (long)
				((filepointer[fno]-headersize[fno])/(SHORT * incr));
		while(ibuf<bufend)  {
			for(i=0; i<incr; i++)  {
				if(ABS(*(ibuf + i)) > (int)*(pk+i)) {
					*(pk+i) = ABS(*(ibuf + i)); 
					*(pkloc+i) = currentloc + 
					(long)((ibuf - ibufx)/incr);
				}
			}
			ibuf += incr;
		}
	}
	else {
		fbufx = fbuf = (float *)sndbuf[fno];
		fbufend = fbuf + pointer[fno];
		currentloc = (long)
				((filepointer[fno]-headersize[fno])/(FLOAT * incr));
		while(fbuf<fbufend) {
			for(i=0; i<incr; i++)  {
				if(ABS(*(fbuf + i)) > *(pk+i)) {
					*(pk+i) = ABS(*(fbuf + i));
					*(pkloc+i) = currentloc +
					(long)((fbuf - fbufx)/incr);
				}
			}
			fbuf += incr;	
		} 
	} 
}

double
peak_off(float p[], int n_args)
{
	peakoff[(int)p[0]] = (char)p[1];
	if(p[1]) rtcmix_advise(NULL, "      peak check turned off for file %d\n",(int)p[0]);
		else
		 rtcmix_advise(NULL, "      peak check turned on for file %d\n",(int)p[0]);
	return 0.0;
}

double
punch_on(float p[], int n_args)
{
	punch[(int)p[0]] = p[1];
	if(!p[1]) rtcmix_advise(NULL, "      punch turned off for file %d\n",(int)p[0]);
		else
		 rtcmix_advise(NULL,"      punch check turned on for file %d\n",(int)p[0]);
	return 0.0;
}

off_t
_readit(int fno)
{
	int i;
	off_t n, maxread;
	short tisamp,*tibuf;
	float tfsamp,*tfbuf;

	/* check to see if we are attempting to read beyond current
	 * EOF, and if so adjust amount to be read and flush rest of buffer
	 */

	/* From make warnings */
	n=0;

	if(filepointer[fno] > originalsize[fno])
		maxread = 0;
	else if((filepointer[fno]+nbytes) > originalsize[fno]) 
		maxread = originalsize[fno]-filepointer[fno];
	else
		maxread = nbytes;
	
	if((play_is_on <  3) || (status[fno] == 0)) {
		if((n = read(sfd[fno],sndbuf[fno],maxread)) != maxread) {
			if(!n) {
				/*if(istape[fno] && n) continue;*/
				perror("read");
				rtcmix_warn("CMIX", "Bad UNIX read, nbytes = %lld\n",(long long)n);
				rtcmix_warn(NULL, " sfd[fno]= %d\n",sfd[fno]);
			        closesf();
			}
		}
	}
	if(((play_is_on==2) && !maxread) || ((play_is_on==3) && (status[fno])))
	      bzero(sndbuf[fno],nbytes);  /* clean buffer out if not readin */

	/* Swap input buffer */
 	if(maxread && swap_bytes[fno]) {
		/* SHORT file */
		if(sfclass(&sfdesc[fno]) == SF_SHORT) {
			tibuf = (short *)sndbuf[fno]; 
			for (i=0;i<nbytes/SF_SHORT;i++) {
				tisamp = *(tibuf+i);
				*(tibuf+i) = reverse_int2(&tisamp);
			}
		}
		/* FLOAT file */
		if(sfclass(&sfdesc[fno]) == SF_FLOAT) {
			tfbuf = (float *)sndbuf[fno]; 
			for (i=0;i<nbytes/SF_FLOAT;i++) {
				/* byte_reverse4(tfbuf+i); */
				/* tfsamp = *(tfbuf+i); */
				/* *(tfbuf+i) = (float)reverse_int4(&tfsamp); */
				tfsamp = *(tfbuf+i);
				byte_reverse4(&tfsamp);
				*(tfbuf+i) = tfsamp;
			}
		}
	}

	/*  if we haven't read in full buffer, zero out rest of buffer,
	 *  and adjust filepointer with lseek.  Otherwise just update 
	 *  filepointer.  This will position pointer properly for any
	 *  situation.  Only a write will change physical size of file.
	 */
	if(play_is_on < 2) {        		
		if(maxread < nbytes) {
			for(n=maxread; n<nbytes; n++) *(sndbuf[fno] + n) = 0;
			filepointer[fno] = lseek(sfd[fno],(nbytes-maxread),1);
		}               
		else filepointer[fno] += nbytes;
	}
	else filepointer[fno] += nbytes;
	return(maxread ? n : maxread);
}

off_t
_writeit(int fno)
{
	int i;
	off_t n=0;
	short tisamp,*tibuf;
	float tfsamp,*tfbuf;
	float peakval;

	if(!status[fno]) {
		rtcmix_warn(NULL, "File %d is write-protected!\n",fno);
		closesf();
	}
  
#ifdef OLDRT
	/*  to play before writing */
	if((sfclass(&sfdesc[fno]) == SF_SHORT) && play_is_on)
		playbuf(sndbuf[fno],nbytes/SF_SHORT);
  
	/* swap and/or play floating point files */
	if(play_is_on && (sfclass(&sfdesc[fno]) == SF_FLOAT)) {
		peakval = getpeakval(peakflag,fno);
		playfbuf(sndbuf[fno],peakval,swap_bytes[fno],nbytes/SF_FLOAT);
	}
	else {	/* just swap if necessary */
		if(swap_bytes[fno] && (sfclass(&sfde sc[fno]) == SF_FLOAT))
		bytrev4(sndbuf[fno],nbytes);
	}
#endif

	if(swap_bytes[fno]) {
		/* SHORT file */
		if(sfclass(&sfdesc[fno]) == SF_SHORT) {
			tibuf = (short *)sndbuf[fno]; 
			for (i=0;i<nbytes/SF_SHORT;i++) {
				tisamp = *(tibuf+i);
				*(tibuf+i) = (short) reverse_int2(&tisamp);
			}
		}
		/* FLOAT file */
		if(sfclass(&sfdesc[fno]) == SF_FLOAT) {
			tfbuf = (float *)sndbuf[fno]; 
			for (i=0;i<nbytes/SF_FLOAT;i++) {
				/* byte_reverse4(tfbuf+i); */
				/* tfsamp = *(tfbuf+i); */
				/* *(tfbuf+i) = (float) reverse_int4(&tfsamp); */
				tfsamp = *(tfbuf+i);
				byte_reverse4(&tfsamp);
				*(tfbuf+i) = tfsamp;
			}
		}
	}

	if(play_is_on < 2) {
		if((n = write(sfd[fno],sndbuf[fno],nbytes)) != nbytes) {
			rtcmix_warn("CMIX", "Bad UNIX write, file %d, nbytes = %lld\n",fno,(long long)n);
			perror("write");
			closesf();
		}
		/* update output file size */
		if((filepointer[fno] += nbytes) > originalsize[fno]) 
			originalsize[fno] = filepointer[fno];
	}
  
	if(!play_is_on)
		printf(".");

	return(n);
}

void
_backup(int fno)     /* utility routine to backspace one 'record' */
{
	if(play_is_on >= 2) return; 

	if((filepointer[fno] = lseek(sfd[fno],(long)-nbytes,SEEK_CUR)) < 0) {
		rtcmix_warn("CMIX", "bad back space in file %d\n",fno);
		perror("lseek");
		closesf();
	}
}

void
_forward(int fno)     /* utility routine to forwardspace one 'record' */
{
	if((filepointer[fno] = lseek(sfd[fno],(long)nbytes,1)) < 0) {
		rtcmix_warn("CMIX", "bad forward space  in file %d\n",fno);
		perror("lseek");
		closesf();
	}
}

void
closesf()
{
	int i;

	for(i = 0; i<NFILES; i++) {
		if(isopen[i]) {
			if (status[i]) 
				putlength(sfname[i], sfd[i], &sfdesc[i]);
			close(sfd[i]);
		}
	}

	if (get_bool_option(kOptionExitOnError))
		exit(1);
}

void
closesf_noexit()
{
	int i;

	for(i = 0; i<NFILES; i++) {
		if(isopen[i]) {
			if (status[i]) 
				putlength(sfname[i], sfd[i], &sfdesc[i]);
			close(sfd[i]);
		}
	}
}

double
m_clean(float p[], int n_args) /* a fast clean of file, after header */
{
/* if p1-> = 0, clean whole file, else skip=p1, dur=p2, ch-on? p3--> */
	int i;
	off_t n, nwrite, todo;
	char *point;
	int fno,segment,chlist[4];
	int skipbytes;

	fno = (int) p[0];
	skipbytes = 0;
	if(!status[fno]) {
		rtcmix_warn(NULL,"fno %d is write-protected!\n",fno);
		closesf();
	}
	todo = originalsize[fno] - headersize[fno];

	segment = (n_args > 1) ? 1 : 0;

	if(segment) {
		skipbytes = (p[1] > 0) ? p[1] * sfclass(&sfdesc[fno]) *
			    SR() * sfchans(&sfdesc[fno]) 
			    : -p[1] * sfclass(&sfdesc[fno]) * 
							 sfchans(&sfdesc[fno]);
		todo =  (p[2] > 0) ? p[2] * sfclass(&sfdesc[fno]) * 
			SR() * sfchans(&sfdesc[fno])
			: -p[2] * sfclass(&sfdesc[fno]) * 
						sfchans(&sfdesc[fno]);
		for(i=0; i<sfchans(&sfdesc[fno]); i++) chlist[i] = p[i+3];
	}
	point = (char *)sndbuf[fno];
	if(!segment) for(i=0; i<nbytes; i++) *(point+i) = 0;

	if((filepointer[fno] = 
	   lseek(sfd[fno],skipbytes+headersize[fno],0)) == -1) {
		rtcmix_warn("CMIX", "bad sflseek in clean\n");
		closesf();
	}
	printf("Clean %lld bytes\n",(long long)todo);
	while(todo) {
		nwrite = (todo > nbytes) ? nbytes : todo;
		if(segment) {
			if((n = read(sfd[fno],sndbuf[fno],nwrite)) 
					== 0) { /* allow for fractional reads*/
				fprintf(stderr, "CMIX: Apparent eof in clean\n");
				return -1.0;
			}
			if(lseek(sfd[fno],-n,1) < 0) {
				fprintf(stderr,"Bad UNIX lseek in clean\n");
				closesf();
			}
			m_zapout(fno,sndbuf[fno],n,chlist);
			nwrite = n;
		}
		if((n = write(sfd[fno],sndbuf[fno],nwrite)) == 0) {
			fprintf(stderr, "CMIX: Apparent eof in clean\n");
	        	closesf();
		}
		todo -= n;
	}
	if(!segment) {
		if((lseek(sfd[fno],0,0)) == -1) {
			fprintf(stderr,"CMIX: bad lseek in clean\n");
			closesf();
		}

		for(i = 0; i<sfchans(&sfdesc[fno]); i++) { 
			sfmaxamp(&sfm[fno],i) = 0;
			sfmaxamploc(&sfm[fno],i) = 0;
		}

		putsfcode(&sfdesc[fno],(char *)&sfm[fno],&ampcode);

		if(wheader(sfd[fno],(char *)&sfdesc[fno])) {
			fprintf(stderr,"Bad header write\n");
			perror("write");
			closesf();
		}
	}
	else 
		if((lseek(sfd[fno],headersize[fno],0)) == -1) {
			fprintf(stderr,"CMIX: bad lseek in clean\n");
			closesf();
		}
	filepointer[fno] = headersize[fno];
	printf("Clean successfully finished.\n");

   return 0.0;
}

void
m_zapout(int fno, char *buffer, int nwrite, int *chlist)
{
	float *fbuf;
	int i,j,nchunks,chans;
	short *ibuf;

	chans = sfchans(&sfdesc[fno]);

	if(sfclass(&sfdesc[fno]) == SF_SHORT) {
		ibuf = (short *) buffer;
		nchunks = nwrite/SF_SHORT;
		for(i=0; i<nchunks; i += chans)
			for(j=0; j<chans; j++)
				if(chlist[j]) *(ibuf+j+i) = 0;
	}
	else {
		fbuf = (float *) buffer;
		nchunks = nwrite/SF_FLOAT;
		for(i=0; i<nchunks; i += chans) 
			for(j=0; j<chans; j++)
				if(chlist[j]) *(fbuf+j+i) = 0;
	}
}

float
getpeakval(float peakflag, int fno)
{
	float opeak;
	int i;
	float *pk;
	pk = (float *)peak[fno];

	if(peakflag < 0) {
		for(i=0,opeak=0; i<sfchans(&sfdesc[fno]); i++)
			if(pk[i] > opeak) 
					opeak=pk[i];
	}
	else if(peakflag == 0) {
		for(i=0,opeak=0; i<sfchans(&sfdesc[fno]); i++)
			if((float)sfmaxamp(&sfm[fno],i) > opeak) 
					opeak=sfmaxamp(&sfm[fno],i);
	}
	else opeak = peakflag;
/*	printf("peakflag=%f, peakval=%f\n",peakflag,opeak); */
	return(opeak);
}

#ifdef OBSOLETE
extern int init_sound();

double
play_on(p,n_args)
float *p;
{	
	int output;
	output = (int)p[0];
	if(!isopen[output]) {
		rtcmix_warn("play_on", "You haven't opened file %d yet!\n",output);
		closesf();
	}
	if(p[1] == 0) play_is_on = 1;  /* play and write to disk */
	if(p[1] == 1) play_is_on = 2;  /* play and read disk, but don't write */
	if(p[1] == 2) play_is_on = 3;  /* play but don't read disk */
	peakflag = p[2];
	rtcmix_advise("play_on", "%s\n",
	       (play_is_on == 1) ? "writing to and playing from disk"
	       : (play_is_on == 2) ? "playing and reading disk only"
	       : (play_is_on == 3) ? "playing without reading disk"
	       : ""
	);
	if(p[1] > 2. || p[1] < 0.) {
		rtcmix_warn("play_on", "illegal value for p[1]\n");
		closesf();
	}
	if(get_print_option()) {
	    printf(peakflag < 0 ? "scaling to current overall peak" :
	       peakflag == 0.0 ? "scaling to file peak value" :
	       "scaling to peak of %f", peakflag); 
	    printf("\n");
	}
	return init_sound((float)sfsrate(&sfdesc[output]),
		sfchans(&sfdesc[output]));
}

#else /* !OBSOLETE */

double
play_on(float p[], int n_args)
{	
   rtcmix_warn("CMIX", "Sorry, this version of RTcmix does not support the old cmix method of playing real-time audio.");
   return 0.0;
}

#endif /* !OBSOLETE */

