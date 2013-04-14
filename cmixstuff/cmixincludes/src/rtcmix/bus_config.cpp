/* RTcmix  - Copyright (C) 2000  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <ugens.h>
#include <bus.h>
#include "BusSlot.h"
#include <RTcmix.h>
#include "prototypes.h"
#include "InputFile.h"
#include <lock.h>
#include <Option.h>
  
//#define PRINTPLAY
//#define DEBUG
//#define PRINTALL
//#define DEBUGMEM

#ifdef DEBUGMEM
#define MPRINT(string, ptr) printf(string, (unsigned) ptr)
#else
#define MPRINT(string, ptr)
#endif

struct BusQueue {
	BusQueue(char *name, BusSlot *theQueue);
	~BusQueue();
	char *instName() { return inst_name; }
	char *inst_name;
	BusSlot *slot;
	BusQueue *next;
};

//
// BusSlot "class" methods
//

BusSlot::BusSlot() : next(NULL), prev(NULL), in_count(0), out_count(0),
					 auxin_count(0), auxout_count(0)
{
	for (int n=0; n<MAXBUS; ++n)
	    in[n] = out[n] = auxin[n] = auxout[n] = 0;
}

BusSlot::~BusSlot()
{
	MPRINT("BusSlot 0x%x destroyed\n", this);
}

//
// BusQueue class methods
//

BusQueue::BusQueue(char *name, BusSlot *theSlot)
		: inst_name(strdup(name)), slot(theSlot), next(NULL)
{
	slot->ref();
}

BusQueue::~BusQueue()
{
	MPRINT("BusQueue 0x%x destroyed\n", this);
	free(inst_name);
	slot->unref();
}

/* Special flags and whatnot */
enum ConfigStatus {
  CONFIG,
  NOT_CONFIG
};

// Local classes for configuration checking

struct CheckNode : public RefCounted {
   CheckNode() : bus_list(new short[MAXBUS]), bus_count(0) {}
   CheckNode(short *list, short count) : bus_list(list), bus_count(count) {}
   ~CheckNode() { MPRINT("CheckNode 0x%x destroyed\n", this); }
   short *bus_list;
   short bus_count;
};

struct CheckQueue {
   CheckQueue(CheckNode *theNode) : node(theNode), next(NULL) { node->ref(); }
   ~CheckQueue() { RefCounted::unref(node); }
   CheckNode *node;
   CheckQueue *next;
};


/* Prototypes (in order)------------------------------------------------------*/
static int strtoint(char*, int *);  /* Helper */
static void print_bus_slot(BusSlot *);  /* Debugging */
static void bf_traverse(int, Bool);
static void create_play_order();

/* ------------------------------------------------------------- strtoint --- */
static inline int
strtoint(char *str, int *num)
{
   long  along;
   char  *pos;

   pos = NULL;
   errno = 0;
   along = strtol(str, &pos, 10);
   if (pos == str)                           /* no digits to convert */
      return -1;
   if (errno == ERANGE)                      /* overflow or underflow */
      return -1;

   *num = (int)along;
   return 0;
}

/* ------------------------------------------------------- print_parents -----*/
void
RTcmix::print_parents() {
  int i;
  printf("Aux buses w/o aux inputs:  "); 
  for(i=0;i<MAXBUS;i++) {
	pthread_mutex_lock(&aux_in_use_lock);
	if (AuxInUse[i]) {
	  pthread_mutex_lock(&has_parent_lock);
	  if (!HasParent[i]) {
		printf(" %d",i);
	  }
	  pthread_mutex_unlock(&has_parent_lock);
	}
	pthread_mutex_unlock(&aux_in_use_lock);
  }
  printf("\n");
}

/* ------------------------------------------------------ print_children -----*/
void
RTcmix::print_children() {
  int i;
  printf("Aux buses w/o aux outputs:  "); 
  for(i=0;i<MAXBUS;i++) {
	pthread_mutex_lock(&aux_in_use_lock);
	if (AuxInUse[i]) {
	  pthread_mutex_lock(&has_child_lock);
	  if (!HasChild[i]) {
		printf(" %d",i);
	  }
	  pthread_mutex_unlock(&has_child_lock);
	}
	pthread_mutex_unlock(&aux_in_use_lock);
  }
  printf("\n");
}

/* ------------------------------------------------------- print_bus_slot --- */
static void
print_bus_slot(BusSlot *bs)
{
   int i;

   printf("\n   in_count=%d :", bs->in_count);
   for (i = 0; i < bs->in_count; i++)
      printf(" %d", bs->in[i]);
   printf("\n   out_count=%d :", bs->out_count);
   for (i = 0; i < bs->out_count; i++)
      printf(" %d", bs->out[i]);
   printf("\n   auxin_count=%d :", bs->auxin_count);
   for (i = 0; i < bs->auxin_count; i++)
      printf(" %d", bs->auxin[i]);
   printf("\n   auxout_count=%d :", bs->auxout_count);
   for (i = 0; i < bs->auxout_count; i++)
      printf(" %d", bs->auxout[i]);
   printf("\n");
}

/* ----------------------------------------------------- print_bus_config --- */
/* Prints config from Inst. point of view */
ErrCode
RTcmix::print_inst_bus_config() {
   BusQueue *qEntry;
   BusSlot *check_slot;

   pthread_mutex_lock(&inst_bus_config_lock);
   qEntry = Inst_Bus_Config;
   pthread_mutex_unlock(&inst_bus_config_lock);

   while (qEntry) {

	  printf("%s",qEntry->instName());
	  check_slot = qEntry->slot;
	  
	  if (check_slot == NULL) {
		 printf("done\n");
		 return NO_ERR;
	  }
	  
	  while (check_slot) {
		 print_bus_slot(check_slot);
		 check_slot = check_slot->next;
	  }
	  qEntry = qEntry->next; 
   }
   return NO_ERR;
}

/* ----------------------------------------------------- print_play_order --- */
void
RTcmix::print_play_order() {
  int i;
  printf("Output buffer playback order:  ");
  for(i=0;i<MAXBUS;i++) {
	pthread_mutex_lock(&aux_to_aux_lock);
	if (AuxToAuxPlayList[i] != -1) {
	  printf(" %d",AuxToAuxPlayList[i]);
	}
	pthread_mutex_unlock(&aux_to_aux_lock);
  }
  printf("\n");
}

/* ------------------------------------------------ check_bust_inst_config -- */
/* Parses bus graph nodes */

ErrCode
RTcmix::check_bus_inst_config(BusSlot *slot, Bool visit) {
	int i,j,aux_ctr,out_ctr;
	short *in_check_list;
	short in_check_count;
	CheckQueue *in_check_queue,*last;
	static Bool Visited[MAXBUS];
	Bool Checked[MAXBUS];
	short r_p_count=0;

	/* If we haven't gotten a config yet ... allocate the graph array */
	/* and the playback order list */
	Bus_Config_Status.lock();
	if (Bus_Config_Status == NO) {
		for (i=0;i<MAXBUS;i++) {
			CheckNode *t_node = new CheckNode;
			pthread_mutex_lock(&bus_in_config_lock);
			Bus_In_Config[i] = t_node;
			t_node->ref();
			pthread_mutex_unlock(&bus_in_config_lock);
		}
		Bus_Config_Status = YES;
	}
	Bus_Config_Status.unlock();

	aux_ctr = out_ctr = 0;
	j=0;
	for(i=0;i<MAXBUS;i++) {
		if (visit)
			Visited[i] = NO;
		Checked[i] = NO;
		pthread_mutex_lock(&revplay_lock);
		RevPlay[i] = -1;
		pthread_mutex_unlock(&revplay_lock);
		pthread_mutex_lock(&out_in_use_lock);
		if (OutInUse[i]) {  // DJT For scheduling
			pthread_mutex_lock(&to_out_lock);
			ToOutPlayList[out_ctr++] = i;
			pthread_mutex_unlock(&to_out_lock);
		}
		pthread_mutex_unlock(&out_in_use_lock);
		pthread_mutex_lock(&aux_out_in_use_lock);
		if (AuxOutInUse[i]) {
			pthread_mutex_lock(&to_aux_lock);
			ToAuxPlayList[aux_ctr++] = i;
			pthread_mutex_unlock(&to_aux_lock);
		}
		pthread_mutex_unlock(&aux_out_in_use_lock);
	}

	/* Put the slot being checked on the list of "to be checked" */
	CheckNode *t_node = new CheckNode(slot->auxin, slot->auxin_count);
	last = in_check_queue = new CheckQueue(t_node);
	CheckQueue *savedQueueHead = in_check_queue;

	/* Go down the list of things (nodes) to be checked */
	while (in_check_queue) {
		CheckNode *t_check_node = in_check_queue->node;
		in_check_list = t_check_node->bus_list;
		in_check_count = t_check_node->bus_count;

		for (i=0;i<in_check_count;i++) {
			short t_in = in_check_list[i];

			/* Compare to each of the input slot's output channels */
			for (j=0;(j<slot->auxout_count) && (!Checked[t_in]);j++) {
				const short t_out = slot->auxout[j];
#ifdef PRINTALL
				printf("check_bus_inst_config: checking in=%d out=%d\n",t_in,t_out);
#endif
				/* If they're equal, then return the error */
				if (t_in == t_out) {
					rterror(NULL, "ERROR:  bus_config loop ... config not allowed.\n");
					return LOOP_ERR;
				}
			}
			if (!Checked[t_in]) {
				Checked[t_in] = YES;
			}

			/* If this input channel has other input channels */
			/* put them on the list "to be checked" */

			pthread_mutex_lock(&bus_in_config_lock);
			if ((Bus_In_Config[t_in]->bus_count > 0) && !Visited[t_in]) {
#ifdef PRINTALL
				printf("check_bus_inst_config: adding Bus[%d] to list\n",t_in);
#endif
				pthread_mutex_lock(&has_parent_lock);
				if (HasParent[t_in]) {
#ifdef PRINTPLAY
					printf("check_bus_inst_config: RevPlay[%d] = %d\n",r_p_count,t_in);
#endif
					pthread_mutex_lock(&revplay_lock);
					RevPlay[r_p_count++] = t_in;
					pthread_mutex_unlock(&revplay_lock);
				}
				pthread_mutex_unlock(&has_parent_lock);
				Visited[t_in] = YES;
				CheckQueue *t_queue = new CheckQueue(Bus_In_Config[t_in]);
				last->next = t_queue;
				last = t_queue;
			}
			pthread_mutex_unlock(&bus_in_config_lock);
		}
#ifdef PRINTALL
		printf("check_bus_inst_config: popping ...\n");
#endif
		in_check_queue = in_check_queue->next;
	}

#ifdef PRINTALL
	printf("check_bus_inst_config: cleaning up\n");
#endif
	// Now clean up
	CheckQueue *queue = savedQueueHead;
	while (queue) {
		CheckQueue *next = queue->next;
		delete queue;
		queue = next;
	}
	
	return NO_ERR;
}

/* ------------------------------------------------------ insert_bus_slot --- */
/* Inserts bus configuration into structure used by insts */
/* Also inserts into bus graph */
/* Special case when called by bf_traverse->check_bus_inst_config-> */
/*     s_in set to 333 and filtered out below */
ErrCode
RTcmix::insert_bus_slot(char *name, BusSlot *slot) {
  
	short i,j,t_in_count,s_in,s_out;

	/* Insert into bus graph */
	for(i=0;i<slot->auxout_count;i++) {
		s_out = slot->auxout[i];
		pthread_mutex_lock(&aux_in_use_lock);
		if (!AuxInUse[s_out]) {
			AuxInUse[s_out] = YES;
		}
		pthread_mutex_unlock(&aux_in_use_lock);
		for(j=0;j<slot->auxin_count;j++) {
			s_in = slot->auxin[j];
			pthread_mutex_lock(&has_parent_lock);
			if ((!HasParent[s_out]) && (s_in != 333)) {
#ifdef PRINTALL
				printf("insert_bus_slot: HasParent[%d]\n",s_out);
#endif
				HasParent[s_out] = YES;
			}
			pthread_mutex_unlock(&has_parent_lock);

			pthread_mutex_lock(&bus_in_config_lock);
			t_in_count = Bus_In_Config[s_out]->bus_count;
			pthread_mutex_unlock(&bus_in_config_lock);
#ifdef PRINTALL
			printf("insert_bus_slot: Inserting Bus_In[%d] = %d\n",s_out,s_in);
#endif
			if (s_in != 333) {
				pthread_mutex_lock(&bus_in_config_lock);
				Bus_In_Config[s_out]->bus_list[t_in_count] = s_in;
				Bus_In_Config[s_out]->bus_count++;

// BGG -- my bus-wrapping hackeroo!  go brad go!  :-)
				if (Bus_In_Config[s_out]->bus_count >= MAXBUS)
					Bus_In_Config[s_out]->bus_count = 0;
				pthread_mutex_unlock(&bus_in_config_lock);
				pthread_mutex_lock(&has_child_lock);
				HasChild[s_in] = YES;
				pthread_mutex_unlock(&has_child_lock);
				pthread_mutex_lock(&aux_in_use_lock);
				AuxInUse[s_in] = YES;
				pthread_mutex_unlock(&aux_in_use_lock);
			}
		}
	}

	/* Create initial node for Inst_Bus_Config */
	pthread_mutex_lock(&inst_bus_config_lock);
	if (Inst_Bus_Config == NULL) {
		Inst_Bus_Config = new BusQueue(name, slot);
		pthread_mutex_unlock(&inst_bus_config_lock);
		return NO_ERR;
	}

	BusQueue *qEntry = Inst_Bus_Config;
	pthread_mutex_unlock(&inst_bus_config_lock);

	Lock lock(&inst_bus_config_lock);	// unlocks when out of scope
	/* Traverse down each list */
	while (qEntry) {	
		/* If names match, then put onto the head of the slot's list */
		if (strcmp(qEntry->instName(), name) == 0) {
			BusSlot *next = qEntry->slot->next;
			// Remove our reference to this slot and replace.
			qEntry->slot->unref();
#ifdef PRINTALL
			printf("insert_bus_slot: replacing slot entry for '%s'\n", name);
#endif
			slot->next = next;
			qEntry->slot = slot;
			slot->ref();
			return NO_ERR;
		}

		/* We've reached the end ... so put a new node on with inst's name */
		if (qEntry->next == NULL) {
			qEntry->next = new BusQueue(name, slot);
  			return NO_ERR;
		}
		qEntry = qEntry->next;
	}
	return NO_ERR;
}


/* ----------------------------------------------------- bf_traverse -------- */
/* sets fictitious parent node to 333 */
/* filtered out in insert() */
void
RTcmix::bf_traverse(int bus, Bool visit) {
#ifdef PRINTPLAY
  printf("entering bf_traverse(%d)\n", bus);
#endif
  BusSlot *temp = new BusSlot;
  temp->auxin[0] = bus;
  temp->auxin_count=1;
  temp->auxout[0] = 333;
  temp->auxout_count=1;
  check_bus_inst_config(temp, visit);
  temp->unref();
#ifdef PRINTPLAY
  printf("exiting bf_traverse(%d)\n", bus);
#endif
}

/* ----------------------------------------------------- create_play_order -- */
void
RTcmix::create_play_order() {
  int i,j;
  Bool visit = YES;
  short aux_p_count = 0;

  /* Put all the parents on */
  for(i=0;i<MAXBUS;i++) {
	pthread_mutex_lock(&aux_in_use_lock);
	if (AuxInUse[i]) {
	  pthread_mutex_lock(&has_parent_lock);
	  if (!HasParent[i]) {
#ifdef PRINTPLAY
		printf("create_play_order: AuxPlay[%d] = %d\n",aux_p_count,i);
#endif
		pthread_mutex_lock(&aux_to_aux_lock);
		AuxToAuxPlayList[aux_p_count++] = i;
		pthread_mutex_unlock(&aux_to_aux_lock);
	  }
	  pthread_mutex_unlock(&has_parent_lock);
	}
	pthread_mutex_unlock(&aux_in_use_lock);
  }
  for (i=0;i<MAXBUS;i++) {
	pthread_mutex_lock(&aux_in_use_lock);
	if (AuxInUse[i]) {
	  pthread_mutex_lock(&has_child_lock);
	  if (!HasChild[i]) {
		bf_traverse(i,visit);
		if (visit) 
		  visit = NO;
		for (j=MAXBUS-1;j>=0;j--) {
		  pthread_mutex_lock(&revplay_lock);
		  if (RevPlay[j] != -1) {
#ifdef PRINTPLAY
			printf("create_play_order: AuxPlay[%d](%d) = Rev[%d](%d)\n",
					aux_p_count,AuxToAuxPlayList[aux_p_count],j,RevPlay[j]);
#endif
			pthread_mutex_lock(&aux_to_aux_lock);
			AuxToAuxPlayList[aux_p_count++] = RevPlay[j];
			pthread_mutex_unlock(&aux_to_aux_lock);
		  }
		  pthread_mutex_unlock(&revplay_lock);
		}
	  }
	  pthread_mutex_unlock(&has_child_lock);
	}
	pthread_mutex_unlock(&aux_in_use_lock);
  }
}

/* ------------------------------------------------------- get_bus_config --- */
/* Given an instrument name, return a pointer to the most recently
   created BusSlot node for that instrument name. If no instrument name
   match, return a pointer to the default node.
*/
BusSlot *
RTcmix::get_bus_config(const char *inst_name)
{
   BusSlot  *slot, *default_bus_slot;
   BusQueue *q;
   ErrCode     err;
   int index,in_chans,i;

   assert(inst_name != NULL);

   slot = NULL;

   Lock lock(&bus_slot_lock);	// unlocks when out of scope

   /* Maybe also need to lock q since it's accessing a BusSlot */
   /* that intraverse might also be checking? */
   /* But the values don't change, so I don't see why */

   pthread_mutex_lock(&inst_bus_config_lock);
   for (q = Inst_Bus_Config; q; q = q->next) {
	 if (strcmp(inst_name, q->instName()) == 0) {
	   pthread_mutex_unlock(&inst_bus_config_lock);   
	   return q->slot;
	 }
   }
   pthread_mutex_unlock(&inst_bus_config_lock);
   
   /* Default bus_config for backwards compatibility with < 3.0 scores */
   
   rtcmix_advise(NULL, "No bus_config defined, setting default (in/out).");
   
   /* Some init stuff normally done in check_bus_inst_config */
   Bus_Config_Status.lock();
   if (Bus_Config_Status == NO) {
	 for (i=0;i<MAXBUS;i++) {
	   pthread_mutex_lock(&aux_to_aux_lock);
	   AuxToAuxPlayList[i] = -1;
	   pthread_mutex_unlock(&aux_to_aux_lock);
	   pthread_mutex_lock(&to_aux_lock);
	   ToAuxPlayList[i] = -1;
	   pthread_mutex_unlock(&to_aux_lock);
	   pthread_mutex_lock(&to_out_lock);
	   ToOutPlayList[i] = -1;
	   pthread_mutex_unlock(&to_out_lock);
	   pthread_mutex_lock(&out_in_use_lock);
	   OutInUse[i] = NO;
	   pthread_mutex_unlock(&out_in_use_lock);
	   // Added this initialization as well -- DS 5/2005
	   CheckNode *t_node = new CheckNode;
	   pthread_mutex_lock(&bus_in_config_lock);
	   Bus_In_Config[i] = t_node;
	   t_node->ref();
	   pthread_mutex_unlock(&bus_in_config_lock);
	 }
	 Bus_Config_Status = YES;
   }
   Bus_Config_Status.unlock();

   for(i=0;i<NCHANS;i++) {
	 pthread_mutex_lock(&out_in_use_lock);
	 OutInUse[i] = YES;
	 pthread_mutex_unlock(&out_in_use_lock);
	 pthread_mutex_lock(&to_out_lock);
	 ToOutPlayList[i] = i;
	 pthread_mutex_unlock(&to_out_lock);
   }

   default_bus_slot = new BusSlot;
   /* Grab input chans from file descriptor table */
   index = get_last_input_index();
   /* Otherwise grab from audio device, if active */
   if (index == -1) {
	 if (Option::record() && Option::play())
	   in_chans = NCHANS;
	 else
	   in_chans = 0;
   }
   else
     in_chans = inputFileTable[index].channels();
   
   default_bus_slot->in_count = in_chans;
   default_bus_slot->out_count = NCHANS;
   
   for(i=0;i<in_chans;i++) {
	 default_bus_slot->in[i] = i;
   }
   for(i=0;i<NCHANS;i++) {
	 default_bus_slot->out[i] = i;
   }

   err = check_bus_inst_config(default_bus_slot, YES);
   if (!err) {
      err = insert_bus_slot((char *)inst_name, default_bus_slot);
   }
   if (err) {
		die("bus_config", "couldn't get_bus_config, this is not good");
#ifndef MAXMSP
      exit(1);        /* This is probably what user wants? */
#endif
	}
	
	// Print out the default bus config (if verbosity permits)
	
	char buslist[64];
	switch (default_bus_slot->in_count) {
		case 0:
			sprintf(buslist, "() => ");
			break;
		case 1:
			sprintf(buslist, "(in 0) => ");
			break;
		default:
			sprintf(buslist, "(in 0-%d) => ", default_bus_slot->in_count - 1);
			break;
	}
	strcat(buslist, inst_name);
	if (default_bus_slot->out_count == 1)
		strcat(buslist, " => (out 0)");
	else
		sprintf(buslist + strlen(buslist), " => (out 0-%d)", default_bus_slot->out_count - 1);
	
	rtcmix_advise(NULL, "default: %s\n", buslist);

   return default_bus_slot;
}

/* ------------------------------------------------------- addToBus --------- */
/* This is called by each instrument during addout() to insert a request for a mix.
   All requests are mixed at the same time via mixToBus().
 */

#ifdef MULTI_THREAD

void
RTcmix::addToBus(BusType type, int bus, BufPtr src, int offset, int endfr, int chans)
{
	pthread_mutex_lock(&vectorLock);
    mixVector.push_back(
						MixData(
								src,
								(type == BUS_AUX_OUT) ? aux_buffer[bus] + offset : out_buffer[bus] + offset,
								endfr - offset,
								chans)
                        );
	
	pthread_mutex_unlock(&vectorLock);
}

void
RTcmix::mixToBus()
{
    for (std::vector<RTcmix::MixData>::iterator i = mixVector.begin(); i != mixVector.end(); ++i) {
        MixData &m = *i;
        BufPtr src = m.src;
        BufPtr dest = m.dest;
        const int framesOverFour = m.frames >> 2;
        const int framesRemaining = m.frames - (framesOverFour << 2);
        const int chans = m.channels;
        const int chansx2 = chans << 1;
        const int chansx3 = chansx2 + chans;
        const int chansx4 = chansx2 + chansx2;
        for (int n = 0; n < framesOverFour; ++n) {
            dest[0] += src[0];
            dest[1] += src[chans];
            dest[2] += src[chansx2];
            dest[3] += src[chansx3];
            dest += 4;
            src += chansx4;
        }
        for (int n = 0; n < framesRemaining; ++n) {
            dest[n] += *src;
            src += chans;
        }
    }
    mixVector.clear();
}

#else

/* ------------------------------------------------------- addToBus --------- */
/* This is called by each instrument during addout() to mix itself into bus. */

void
RTcmix::addToBus(BusType type, int bus, BufPtr src, int offset, int endfr, int chans)
{
	register BufPtr dest;
	pthread_mutex_t *pMutex;
	
	if (type == BUS_AUX_OUT) {
		dest = aux_buffer[bus];
	}
	else {
		dest = out_buffer[bus];
	}
	assert(dest != NULL);
	for (int frame = offset; frame < endfr; frame++) {
		dest[frame] += *src;
		src += chans;
	}
}

#endif	// MULTI_THREAD

/* ------------------------------------------------------- parse_bus_chan --- */
static ErrCode
parse_bus_chan(char *numstr, int *startchan, int *endchan)
{
   char  *p;

   if (strtoint(numstr, startchan) == -1)
      return INVAL_BUS_CHAN_ERR;

   p = strchr(numstr, '-');
   if (p) {
      p++;                                           /* skip over '-' */
      if (strtoint(p, endchan) == -1)
         return INVAL_BUS_CHAN_ERR;
   }
   else
      *endchan = *startchan;

	/* NOTE: with the current code, only MAXBUS-1 channels are allowed */
	if (*startchan >= MAXBUS-1 || *endchan >= MAXBUS-1)
		return INVAL_BUS_CHAN_ERR;

   return NO_ERR;
}

/* ------------------------------------------------------- parse_bus_name --- */
ErrCode
parse_bus_name(char *busname, BusType *type, int *startchan, int *endchan)
{
   char     *p;
   ErrCode  status = NO_ERR;

   if (busname == NULL)
	   status = INVAL_BUS_ERR;
   else switch (busname[0]) {
      case 'i':                                      /* "in*" */
         *type = BUS_IN;
         p = &busname[2];                            /* skip over "in" */
         status = parse_bus_chan(p, startchan, endchan);
         break;
      case 'o':                                      /* "out*" */
         *type = BUS_OUT;
         p = &busname[3];                            /* skip over "out" */
         status = parse_bus_chan(p, startchan, endchan);
         break;
      case 'a':                                      /* "aux*" */
         if (strchr(busname, 'i'))
            *type = BUS_AUX_IN;
         else if (strchr(busname, 'o'))
            *type = BUS_AUX_OUT;
         else
            return INVAL_BUS_ERR;
         p = &busname[3];                            /* skip over "aux" */
         status = parse_bus_chan(p, startchan, endchan);
         break;
	  case 'c':										 /* "chain*" */
		p = &busname[5];                            /* skip over "chain" */
		if (strchr(p, 'i'))
		   *type = BUS_NONE_IN;
		else if (strchr(p, 'o'))
		   *type = BUS_NONE_OUT;
		else
		   return INVAL_BUS_ERR;
		status = parse_bus_chan(p, startchan, endchan);
		break;
      default:
	  	 rtcmix_warn("bus_config", "Invalid bus specifier: '%s'", busname);
         return INVAL_BUS_ERR;
   }
   if (status != NO_ERR)
		rtcmix_warn("bus_config", "Invalid bus specifier: '%s'", busname);
   return status;
}

// D.S. This is now a static function

/* ----------------------------------------------------------- bus_config --- */
double 
RTcmix::bus_config(float p[], int n_args, double pp[])
{
   ErrCode     err;
   int         i, j, k, startchan, endchan, chain_incount=0, chain_outcount=0;
   char        *str, *instname, *busname;
   BusType     type;
   BusSlot     *bus_slot;
   char		   inbusses[80], outbusses[80];	// for verbose message

   if (n_args < 2)
      die("bus_config", "Wrong number of args.");

   bus_slot = new BusSlot;
   if (bus_slot == NULL)
      return -1.0;

   inbusses[0] = outbusses[0] = '\0';
   
   Lock localLock(&bus_slot_lock);	// This will unlock when going out of scope.

   /* do the old Minc casting rigamarole to get string pointers from a double */
   str = DOUBLE_TO_STRING(pp[0]);
   instname = strdup(str);	// Note:  If we exit nonfatally, we have to free.

   for (i = 1; i < n_args; i++) {
      busname = DOUBLE_TO_STRING(pp[i]);
      err = parse_bus_name(busname, &type, &startchan, &endchan);
      if (err)
         goto error;

      switch (type) {
         case BUS_IN:
			if (bus_slot->in_count > 0) strcat(inbusses, ", ");
			strcat(inbusses, busname);
            if (bus_slot->auxin_count > 0) {
                return die("bus_config",
                      	"Can't have 'in' and 'aux-in' buses in same bus_config.");
            }
            if (chain_incount > 0) {
            	return die("bus_config",
            		"Can't have 'in' and 'chain-in' buses in same bus_config.");
            }
            j = bus_slot->in_count;
            for (k = startchan; k <= endchan; k++)
               bus_slot->in[j++] = k;
            bus_slot->in_count += (endchan - startchan) + 1;
			/* Make sure max channel count set in rtsetparams can accommodate
               the highest input chan number in this bus config.
			*/
			if (endchan >= NCHANS) {
				return die("bus_config", "You specified %d channels in rtsetparams,\n"
							"but this bus_config requires %d channels.",
							NCHANS, endchan + 1);
			}
			break;
         case BUS_OUT:
			if (bus_slot->out_count > 0) strcat(outbusses, ", ");
			strcat(outbusses, busname);
            if (bus_slot->auxout_count > 0) {
                return die("bus_config",
                    		"Can't have 'out' and 'aux-out' buses in same bus_config.");
            }
            if (chain_outcount > 0) {
            	return die("bus_config",
            				"Can't have 'out' and 'chain-out' buses in same bus_config.");
            }
            j = bus_slot->out_count;
            for (k = startchan; k <= endchan; k++) {
               bus_slot->out[j++] = k;
			   pthread_mutex_lock(&out_in_use_lock);
               OutInUse[k] = YES;  // DJT added
			   pthread_mutex_unlock(&out_in_use_lock);
            }
            bus_slot->out_count += (endchan - startchan) + 1;

            /* Make sure max output chans set in rtsetparams can accommodate
               the highest output chan number in this bus config.
            */
            if (endchan >= NCHANS) {
               die("bus_config", "You specified %d output channels in rtsetparams,\n"
                         "but this bus_config requires %d channels.",
                         NCHANS, endchan + 1);
            }
            break;
         case BUS_AUX_IN:
			if (bus_slot->auxin_count > 0) strcat(inbusses, ", ");
			strcat(inbusses, busname);
            if (bus_slot->in_count > 0) {
                return die("bus_config",
                    	  "Can't have 'in' and 'aux-in' buses in same bus_config.");
            }
            if (chain_incount > 0) {
            	return die("bus_config",
                     	 "Can't have 'chain-in' and 'aux-in' buses in same bus_config.");
            }
            j = bus_slot->auxin_count;
            for (k = startchan; k <= endchan; k++)
               bus_slot->auxin[j++] = k;
            bus_slot->auxin_count += (endchan - startchan) + 1;
            break;
         case BUS_AUX_OUT:
			if (bus_slot->auxout_count > 0) strcat(outbusses, ", ");
			strcat(outbusses, busname);
            if (bus_slot->out_count > 0) {
                return die("bus_config",
                  	  "Can't have 'out' and 'aux-out' buses in same bus_config.");
            }
            if (chain_outcount > 0) {
            	return die("bus_config",
            			"Can't have 'aux-out' and 'chain-out' buses in same bus_config.");
            }
            j = bus_slot->auxout_count;
            for (k = startchan; k <= endchan; k++) {
               bus_slot->auxout[j++] = k;
			   pthread_mutex_lock(&aux_out_in_use_lock);
               AuxOutInUse[k] = YES;
			   pthread_mutex_unlock(&aux_out_in_use_lock);
            }
            bus_slot->auxout_count += (endchan - startchan) + 1;
            break;
		  case BUS_NONE_IN:
			  strcat(inbusses, busname);
			  if (bus_slot->in_count + bus_slot->auxin_count > 0) {
				  return die("bus_config",
                     	 "Can't have 'chain-in' combined with any other in type in same bus_config.");
			  }
			  chain_incount += (endchan - startchan) + 1;
			  break;
		  case BUS_NONE_OUT:
			  strcat(outbusses, busname);
			  if (bus_slot->out_count + bus_slot->auxout_count > 0) {
				  return die("bus_config",
					  	"Can't have 'chain-out' combined with any other out type in same bus_config.");
			  }
			  chain_outcount = (endchan - startchan) + 1;
			  break;
		 default:
		 	break;
      }
   }

   err = check_bus_inst_config(bus_slot, YES);
   if (!err) {
      err = insert_bus_slot(instname, bus_slot);
   }
   if (err) {
		die("bus_config", "couldn't configure the busses");
#ifndef MAXMSP
      exit(1);        /* This is probably what user wants? */
#endif
	}

   /* Make sure specified aux buses have buffers allocated. */
   for (i = 0; i < bus_slot->auxin_count; i++)
      allocate_aux_buffer(bus_slot->auxin[i], RTBUFSAMPS);
   for (i = 0; i < bus_slot->auxout_count; i++)
      allocate_aux_buffer(bus_slot->auxout[i], RTBUFSAMPS);

	// We have to set these after all the above code to prevent chain assignments
	// from generating conflicts or bus allocations.
	
	bus_slot->in_count += chain_incount;
	bus_slot->out_count += chain_outcount;
	
#ifdef PRINTALL
   print_children();
   print_parents();
#endif
   create_play_order();
#ifdef PRINTPLAY
   print_play_order();
#endif
#ifdef DEBUG
   err = print_inst_bus_config();
#endif

   rtcmix_advise("bus_config", "(%s) => %s => (%s)", inbusses, instname, outbusses);
   free(instname);
   return 0.0;

 error:
   free(instname);
   die("bus_config", "Cannot parse arguments.");
   return -1.0;
}

void
RTcmix::free_bus_config()
{
   for (BusQueue *q = Inst_Bus_Config; q;) {
      BusQueue *next = q->next;
      delete q;
      q = next;
   }
   for (int i=0 ; i<MAXBUS; i++) {
	   RefCounted::unref(Bus_In_Config[i]);
   }
}
