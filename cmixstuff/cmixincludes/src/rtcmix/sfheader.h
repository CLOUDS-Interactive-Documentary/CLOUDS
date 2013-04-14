/* This ancient header file has been drastically revised to work with
   our sndlib emulation of the old cmix API (see sys/sndlibsupport.c).
   As a result, many things no longer used in this version of RTcmix
   are now gone. If you need to port an old program to current RTcmix,
   you'll have to look at an old version of sfheader.h.      -JGG, 5/27/00
*/
#ifndef _SFHEADER_H_ 
#define _SFHEADER_H_ 1

#ifdef sgi
#include <audiofile.h>
#endif

/* Some IRCAM sound formation information */
#define SF_SHORT sizeof(short)
#define SF_FLOAT sizeof(float)

#define SF_BUFSIZE	(16*1024)  /* JGG: should double this */
#define SF_MAXCHAN	4

#define MAXCOMM 512
#define MINCOMM 256

/* This is the maximum difference (in seconds) between the time peak
   stats have been written and the file's modification date.  If the
   the mod. date is more recent than the peak stats (timetag) by at
   least this many seconds, then parts of RTcmix will complain.
   If you have an NFS setup where the machines may not be time-sync'd
   (via ntp), then you may want to increase this by several minutes.
*/
#define MAX_PEAK_STATS_AGE 2

/* Codes for sfcode */
#define SF_END 0
#define SF_MAXAMP 1
#define SF_COMMENT 2
#define SF_LINKCODE 3

typedef struct sfcode {
	short	code;
	short	bsize;
} SFCODE;

typedef struct sfmaxamp {
	float	value[SF_MAXCHAN];
	long	samploc[SF_MAXCHAN];
	long	timetag;
} SFMAXAMP;

typedef struct sfcomment {
	char 	comment[MAXCOMM];
} SFCOMMENT;

typedef struct sflink {
	char 	reality[50];
	int 	startsamp;
	int	endsamp;
} SFLINK;


/* The strategy for adding sndlib support to disk-based cmix is to
   hook it in at the lowest level possible, so that old code can go
   about its business without changes, for the most part. The changes
   to SFHEADER let us keep a lot of the macros used by instruments
   and other code.
*/

/* NOTE: We don't read and write this to files directly.
   The sfinfo struct is unfortunate; it's here for backwards compatibility.
*/
typedef union sfheader {
   struct {
      short    header_type;    /* sndlib constant for "aif", "wav", etc */
      short    data_format;    /* sndlib constant (short, float, byte-order) */
      int      data_location;  /* offset in bytes to sound data */
      long     data_size;      /* size in bytes of sound data */
      float    sf_srate;
      int      sf_chans;
      int      sf_packmode;    /* aka: bytes per sample */
      SFMAXAMP sf_maxamp;
      char     sf_comment[MAXCOMM];
   } sfinfo;
} SFHEADER;


# define sfchans(x) (x)->sfinfo.sf_chans
# define sfsrate(x) (x)->sfinfo.sf_srate
# define sfclass(x) (x)->sfinfo.sf_packmode

/* Macros to access sndlib-specific fields */
# define sfheadertype(x) ((x)->sfinfo.header_type)
# define sfdataformat(x) ((x)->sfinfo.data_format)
# define sfdatalocation(x) ((x)->sfinfo.data_location)
# define sfmaxampstruct(x) ((x)->sfinfo.sf_maxamp)
# define sfcommentstr(x) ((x)->sfinfo.sf_comment)

/* Not enough info passed in for sfbsize, so can't use it (assumes a fixed
   header size). Caller should use the following macro instead, when using
   sndlib. Note that <x> is ptr to SFHEADER, not a stat buffer.
*/
# define sfdatasize(x) (((x))->sfinfo.data_size)

/* Don't need sfcodes(). Can't use islink(). */

# define sfmaxamp(mptr,chan) (mptr)->value[chan]
# define sfmaxamploc(mptr,chan) (mptr)->samploc[chan]
# define sfmaxamptime(x) (x)->timetag
# define ismaxampgood(x,s) (sfmaxamptime(x) + MAX_PEAK_STATS_AGE \
                                                      >= (s)->st_mtime)

# define sfcomm(x,n) (x)->comment[n]

# define realname(x) (x)->reality
# define startsmp(x) (x)->startsamp
# define endsmp(x) (x)->endsamp
# define sfoffset(x,h) ((x)->startsamp * sfchans(h) * sfclass(h))
# define sfendset(x,h) ((x)->endsamp * sfchans(h) * sfclass(h))

/* sflseek(): must fix caller so as not to assume header size when
   using sndlib (see lib/getsample.c for example of using lseek instead).
*/

/* These functions in src/audio/sndlibsupport.c
   <sfh> is pointer to an SFHEADER.
   Code that calls these should include sndlibsupport.h.
*/
# define wheader(fd, sfh) sndlib_wheader((fd), (SFHEADER *)(sfh))
# define rheader(fd, sfh) sndlib_rheader((fd), (SFHEADER *)(sfh))

/* Don't need the file magic macros with sndlib. */

/* prototypes to suppress compiler warnings */
int check_byte_order(SFHEADER *, char *, char *);
void printsf(SFHEADER *);


#define readopensf(name,fd,sfh,sfst,prog,result) \
if ((fd = open(name,O_RDONLY)) < 0) {  \
        fprintf(stderr,"%s: cannot access file %s\n",prog,name); \
	result = -1;  \
} \
else if (rheader(fd,&sfh)){ \
	fprintf(stderr,"%s: cannot read header from %s\n",prog,name); \
	result = -1;  \
} \
else if (check_byte_order(&sfh,prog,name)) { \
	fprintf(stderr,"%s: file is unrecognizable %s\n",prog,name); \
	result = -1;  \
} \
else if (stat(name,&sfst)){ \
	fprintf(stderr,"%s: cannot get status on %s\n",prog,name); \
	result = -1;  \
} \
else result = 0;

#define rwopensf(name,fd,sfh,sfst,prog,result,code) \
if ((fd = open(name, code))  < 0) {  \
        fprintf(stderr,"%s: cannot access file %s\n",prog,name); \
	result = -1;  \
} \
else if (rheader(fd,&sfh)){ \
	fprintf(stderr,"%s: cannot read header from %s\n",prog,name); \
	result = -1;  \
} \
else if (check_byte_order(&sfh,prog,name)) { \
	fprintf(stderr,"%s: file is unrecognizable %s\n",prog,name); \
	result = -1;  \
} \
else if (stat(name,&sfst)){ \
	fprintf(stderr,"%s: cannot get status on %s\n",prog,name); \
	result = -1;  \
} \
else result = 0;


#endif /* _SFHEADER_H_ */
