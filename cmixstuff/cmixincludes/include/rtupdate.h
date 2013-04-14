
#ifndef _RTUPDATE_H_ 
#define _RTUPDATE_H_ 1

#define MAXPUPARR 100
#define MAXPUPS 20
#define NOPUPDATE 78787878 // hopefully never a real p-value!

#define MAXNUMTAGS 100
#define MAXNUMPARAMS 100
#define MAXPARRAYSIZE 100

#define MAXNUMINSTS 100

#define MAXNUMCALLS 10

/* For rtperf */
typedef enum {
  NONE,
  RISE,
  SUSTAIN,
  DECAY
} EnvType;

#ifdef MAIN 
#define GLOBAL
#else
#define  GLOBAL extern
#endif


GLOBAL int curtag;                /* current note tag */
GLOBAL int tags_on;               /* using note tags for rtupdates */
GLOBAL int tag_sem;
GLOBAL int curinst;				  // current instrument tag
GLOBAL int curgen;				  // current gen tag


struct inst_list
{
	int num;
	char* name;
	struct inst_list *next;
}; //inst_list

GLOBAL struct inst_list *ilist;  // list of instrument-name/tag pairs
GLOBAL struct inst_list *genlist; // list of gen-name/tag pairs

#endif /* _RTUPDATE_H_ */
