// OK -- this is now RTcmix.C, hacked from the original "main.C"
// 	in standalone RTcmix.  The idea is to make this a compile-in
//	imbRTcmix.o lib that can be called from within other main()
//	progs.  -- BGG 11/2002

/* RTcmix  - Copyright (C) 2000  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/

#include "RTcmix.h"

//#define DBUG
//#define DENORMAL_CHECK
#include <pthread.h>
#include <sys/resource.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>
#include <signal.h>

#include "prototypes.h"
#include "InputFile.h"
#include <ugens.h>
#include <RTcmix.h>
#include <Option.h>
#include "utils.h"
#include <ug_intro.h>
#include <AudioDevice.h>
#ifdef MULTI_THREAD
#include "TaskManager.h"
#endif
#include "rt.h"
#include "heap.h"
#include "maxdispargs.h"
#include "dbug.h"
#include "globals.h"
#include <MMPrint.h>


// This is declared (still) in globals.h for use in gen routines.

FILE *	infile_desc[MAX_INFILE_DESC + 1];

extern "C" {
#ifdef SGI
  void flush_all_underflows_to_zero();
#endif
#ifdef LINUX
  void sigfpe_handler(int sig);
#endif
}

// Static RTcmix member initialization

int				RTcmix::NCHANS 			= 2;
int				RTcmix::RTBUFSAMPS      = 0;
int				RTcmix::audioNCHANS 	= 0;
float			RTcmix::SR				= 0.0;
FRAMETYPE		RTcmix::bufStartSamp 	= 0;

#ifdef MAXMSP
int				RTcmix::rtInteractive = 0;
#else
int				RTcmix::rtInteractive = 1; // keep the heap going for this object
#endif

int				RTcmix::rtsetparams_called = 0; // will call at object instantiation, though
int				RTcmix::audioLoopStarted = 0;
int				RTcmix::audio_config 	= 1;
FRAMETYPE		RTcmix::elapsed 		= 0;
RTstatus		RTcmix::run_status      = RT_GOOD;
AudioDevice *	RTcmix::audioDevice     = NULL;

heap *			RTcmix::rtHeap			= NULL;
RTQueue *		RTcmix::rtQueue			= NULL;
rt_item *		RTcmix::rt_list 		= NULL;

int				RTcmix::output_data_format 		= -1;
int				RTcmix::output_header_type 		= -1;
int				RTcmix::normalize_output_floats	= 0;
int				RTcmix::is_float_format 		= 0;
char *			RTcmix::rtoutsfname 			= NULL;

BufPtr 		RTcmix::audioin_buffer[MAXBUS];    /* input from ADC, not file */
BufPtr 		RTcmix::aux_buffer[MAXBUS];
BufPtr 		RTcmix::out_buffer[MAXBUS];

int			RTcmix::rtrecord 	= 0;		// indicates reading from audio device
int			RTcmix::rtfileit 	= 0;		// signal writing to soundfile
int			RTcmix::rtoutfile 	= 0;

InputFile *	RTcmix::inputFileTable = NULL;
long		RTcmix::max_input_fds = 0;

pthread_mutex_t RTcmix::pfieldLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t RTcmix::audio_config_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t RTcmix::aux_to_aux_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t RTcmix::to_aux_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t RTcmix::to_out_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t RTcmix::inst_bus_config_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t RTcmix::bus_in_config_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t RTcmix::has_child_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t RTcmix::has_parent_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t RTcmix::aux_in_use_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t RTcmix::aux_out_in_use_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t RTcmix::out_in_use_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t RTcmix::revplay_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t RTcmix::bus_slot_lock = PTHREAD_MUTEX_INITIALIZER;

short			RTcmix::AuxToAuxPlayList[MAXBUS];
short			RTcmix::ToOutPlayList[MAXBUS];
short			RTcmix::ToAuxPlayList[MAXBUS];

#ifdef MULTI_THREAD
pthread_mutex_t RTcmix::aux_buffer_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t RTcmix::out_buffer_lock = PTHREAD_MUTEX_INITIALIZER;
TaskManager *	RTcmix::taskManager = NULL;
std::vector<RTcmix::MixData> RTcmix::mixVector;
pthread_mutex_t RTcmix::vectorLock = PTHREAD_MUTEX_INITIALIZER;
#endif

// Bus config state

BusQueue *		RTcmix::Inst_Bus_Config;
Locked<Bool>	RTcmix::Bus_Config_Status(NO);
CheckNode *		RTcmix::Bus_In_Config[MAXBUS];
Bool			RTcmix::HasChild[MAXBUS];
Bool			RTcmix::HasParent[MAXBUS];
Bool			RTcmix::AuxInUse[MAXBUS];
Bool			RTcmix::AuxOutInUse[MAXBUS];
Bool			RTcmix::OutInUse[MAXBUS];
short			RTcmix::RevPlay[MAXBUS];

// Function registry
FunctionEntry *	RTcmix::_functionRegistry = NULL;

// Function table state
struct _func *	RTcmix::_func_list = NULL;



/* --------------------------------------------------------- init_globals --- */
void
RTcmix::init_globals(bool fromMain, const char *defaultDSOPath)
{
   Option::init();
   if (defaultDSOPath && defaultDSOPath[0])
      Option::dsoPathPrepend(defaultDSOPath);

   if (fromMain) {
#ifndef MAXMSP
      Option::readConfigFile(Option::rcName());
      Option::exitOnError(true); // we do this no matter what is in config file
#else
      Option::exitOnError(false);
#endif
      rtInteractive = 0;
   }
   else {
      SR = 44100.0; // what the heck...
      Option::print(0);
      Option::reportClipping(false);
   }

   RTBUFSAMPS = (int) Option::bufferFrames();  /* modifiable with rtsetparams */

   rtHeap = new heap;
   rtQueue = new RTQueue[MAXBUS*3];
#ifdef MULTI_THREAD
   taskManager = new TaskManager;
   mixVector.reserve(MAXBUS);
#endif

   for (int i = 0; i < MAXBUS; i++) {
      AuxToAuxPlayList[i] = -1; /* The playback order for AUX buses */
      ToOutPlayList[i] = -1;    /* The playback order for AUX buses */
      ToAuxPlayList[i] =-1;     /* The playback order for AUX buses */
   }

	max_input_fds = sysconf(_SC_OPEN_MAX);
	if (max_input_fds == -1)	// call failed
		max_input_fds = 128;		// what we used to hardcode
	else
		max_input_fds -= RESERVE_INPUT_FDS;
	
	inputFileTable = new InputFile[max_input_fds];

   init_buf_ptrs();

   if (Option::autoLoad()) {
      const char *dsoPath = Option::dsoPath();
      if (strlen(dsoPath) == 0)
         registerDSOs(SHAREDLIBDIR);
      else
         registerDSOs(dsoPath);
   }
}

void
RTcmix::free_globals()
{
	free_buffers();
	free_bus_config();
	freefuncs();
	delete [] rtQueue;
	rtQueue = NULL;
	delete rtHeap;
	rtHeap = NULL;
#ifdef MULTI_THREAD
	delete taskManager;
	taskManager = NULL;
	InputFile::destroyConversionBuffers();
#endif

}

/* ----------------------------------------------------- detect_denormals --- */
/* Unmask "denormalized operand" bit of the x86 FPU control word, so that
   any operations with denormalized numbers will raise a SIGFPE signal,
   and our handler will be called.  NOTE: This is for debugging only!
   This will not tell you how many denormal ops there are, so just because
   the exception is thrown doesn't mean there's a serious problem.  For
   more info, see: http://www.smartelectronix.com/musicdsp/text/other001.txt.
*/
#ifdef LINUX
#ifdef DENORMAL_CHECK
static void
detect_denormals()
{
   #include <fpu_control.h>
   int cw = 0;
   _FPU_GETCW(cw);
   cw &= ~_FPU_MASK_DM;
   _FPU_SETCW(cw);
}
#endif /* DENORMAL_CHECK */
#endif /* LINUX */

//  The RTcmix constructor with default SR, NCHANS, and RTBUFSAMPS
RTcmix::RTcmix() 
{
	init_globals(false, NULL);
	init(SR, NCHANS, RTBUFSAMPS, NULL, NULL, NULL);
}

//  The RTcmix constructor with settable SR, NCHANS; default RTBUFSAMPS
RTcmix::RTcmix(float tsr, int tnchans)
{
	init_globals(false, NULL);
	init(tsr, tnchans, RTBUFSAMPS, NULL, NULL, NULL);
}

//  The RTcmix constructor with settable SR, NCHANS, RTBUFSAMPS, and up to
//  3 "opt=value" options

RTcmix::RTcmix(float tsr, int tnchans, int bsize,
			   const char *opt1, const char *opt2, const char *opt3)
{
   init_globals(false, NULL);
   init(tsr, tnchans, bsize, opt1, opt2, opt3);
}

// This is called by RTcmixMain constructor, which does all the work.

RTcmix::RTcmix(bool dummy) {}

// This is called when the entire system goes away.

RTcmix::~RTcmix()
{
#ifndef MAXMSP
	run_status = RT_SHUTDOWN;
	waitForMainLoop();
#endif
	free_globals();
}

//  The actual initialization method called by the imbedded constructors

void
RTcmix::init(float tsr, int tnchans, int bsize,
			 const char *opt1, const char *opt2, const char *opt3)
{
	// for rtsetparams -- I forget why it's set up with both double
	// and float p-field arrays.  Also, these aren't 0-ed out
	// so no need to dimension them at MAXDISPARGS
	float p[3];
	double pp[3];

#ifdef SGI
   flush_all_underflows_to_zero();
#endif

	// set up the command lists, etc.
	ug_intro();		/* introduce standard routines */
	// no profiles!  See the note above about DSOs
 
	setbuf(stdout, NULL);	/*  Want to see stdout errors */

	int nargs = 0;
	// set options if any are non-null
	p[0] = pp[0] = STRINGIFY(opt1);
	if (opt1) ++nargs;
	p[1] = pp[1] =  STRINGIFY(opt2);
	if (opt2) ++nargs;
	p[2] = pp[2] = STRINGIFY(opt3);
	if (opt3) ++nargs;

	if (nargs)
		set_option(p, nargs, pp);

	// set the sampling rate and nchannels
	p[0] = pp[0] = tsr;
	p[1] = pp[1] = tnchans;
	p[2] = pp[2] = bsize;
	nargs = 3;
	rtsetparams(p, nargs, pp);

	if (Option::play() || Option::record()) {
		int retcode = runMainLoop();
		if (retcode != 0)
			fprintf(stderr, "runMainLoop() failed\n");
	}
	else {
		// If we are not playing or recording from HW, we cannot be interactive
		rtInteractive = 0;
	}
}


// numeric p-field sending command.  The first "double" is to disambiguate
// from the string-sending command (below).  An old holdover from ancient
// cmix, but it's a handy thing
Instrument *
RTcmix::cmd(const char *name, int n_args, double p0, ...)
{
	va_list ap;
	int i;
	double p[MAXDISPARGS];
	void   *retval;

	p[0] = p0;
	va_start(ap, p0); // start variable list after p0
		for (i = 1; i < n_args; i++)
			p[i] = va_arg(ap, double);
	va_end(ap);

	(void) ::dispatch(name, p, n_args, &retval);

	return (Instrument *) retval;
}

// string p-field sending command.  the first "const char*" is to disambiguate
// from the double version above
Instrument *
RTcmix::cmd(const char *name, int n_args, const char* p0, ...)
{
	// these are not time-stamped as above... change if we need to!
	va_list ap;
	int i;
	char st[MAXDISPARGS][100];
	double p[MAXDISPARGS];
	void *retval;

	// this kludge dates from the olden days!
	strcpy(st[0], p0);
	p[0] = STRING_TO_DOUBLE(st[0]);
	va_start(ap, p0); // start variable list after p0
	for (i = 1; i < n_args; i++) {
		strcpy(st[i], va_arg(ap, char*));
		p[i] = STRING_TO_DOUBLE(st[i]);
	}
	va_end(ap);

	(void) ::dispatch(name, p, n_args, &retval);

	return (Instrument *) retval;
}

#include <PFieldSet.h>
#include <PField.h>

// new PFieldSet sending command.
Instrument *
RTcmix::cmd(const char *name, const PFieldSet &pfSet)
{
	int nFields = pfSet.size();
	Arg	*arglist = new Arg[nFields];
	Arg retArg;
	
	// Copy PField pointers into arglist
	
	for (int field = 0; field < nFields; ++field) {
		arglist[field] = createPFieldHandle(&pfSet[field]);
	}

	(void) dispatch(name, arglist, nFields, &retArg);

	delete [] arglist;
	
	// Extract and return instrument pointer.
	
	RtcHandle rethandle = (RtcHandle) retArg;
	if (rethandle->type == InstrumentPtrType)
		return (Instrument *) rethandle->ptr;
	else
		return NULL;
}

// for commands with no params -- the double return val is because
// that's what these commands generally do.
double
RTcmix::cmd(const char *name)
{
	// these are not time-stamped as above... change if we need to!
	double p[MAXDISPARGS]; // for passing into dispatch only
	double retval;

	retval = ::dispatch(name, p, 0, NULL);

	return(retval);
}

// This is s duplicate of the RTcmix::cmd() function, except that
// instead of returning an Inst*, it returns the double value
// of the RTcmix command that was invoked
double
RTcmix::cmdval(const char *name, int n_args, double p0, ...)
{
	va_list ap;
	int i;
	double p[MAXDISPARGS];
	void   *retval;

	p[0] = p0;
	va_start(ap, p0); // start variable list after p0
	for (i = 1; i < n_args; i++)
		p[i] = va_arg(ap, double);
	va_end(ap);

	return ::dispatch(name, p, n_args, &retval);
}

// This is s duplicate of the RTcmix::cmd() string-passing function, except
// that instead of returning an Inst*, it returns the double value
// of the RTcmix command that was invoked
double
RTcmix::cmdval(const char *name, int n_args, const char* p0, ...)
{
	// these are not time-stamped as above... change if we need to!
	va_list ap;
	int i;
	char st[MAXDISPARGS][100];
	double p[MAXDISPARGS];
	void *retval;

	// this kludge dates from the olden days!
	strcpy(st[0], p0);
	p[0] = STRING_TO_DOUBLE(st[0]);
	va_start(ap, p0); // start variable list after p0
	for (i = 1; i < n_args; i++) {
		strcpy(st[i], va_arg(ap, char*));
		p[i] = STRING_TO_DOUBLE(st[i]);
	}
	va_end(ap);

	return ::dispatch(name, p, n_args, &retval);
}


void RTcmix::printOn()
{
	Option::print(MMP_PRINTALL);
	Option::reportClipping(true);

	/* Banner */
	char tbuf[128];
	sprintf(tbuf, "--------> %s %s <--------\n", RTCMIX_NAME, RTCMIX_VERSION);
	rtcmix_advise(NULL, tbuf);
}

void RTcmix::printOff()
{
	Option::print(MMP_FATAL);
	Option::reportClipping(false);
}

void RTcmix::panic()
{
	run_status = RT_PANIC;	// resets itself in other thread
}

// This method is called by imbedded apps only, and only those apps which
// do not write to an audio device.  It causes the audio loop to run until
// it completes, and run() blocks until it does.

void RTcmix::run()
{
	int retcode;
	if (!Option::play() && !Option::record() && rtfileit == 1) {
		/* Create scheduling/audio thread. */
#ifdef DBUG
		fprintf(stdout, "calling runMainLoop()\n");
#endif
		retcode = runMainLoop();
		if (retcode != 0) {
			fprintf(stderr, "runMainLoop() failed\n");
		}
		else {
			/* Wait for audio thread. */
#ifdef DBUG
			fprintf(stdout, "calling waitForMainLoop()\n");
#endif
			retcode = waitForMainLoop();
			if (retcode != 0) {
				fprintf(stderr, "waitForMailLoop() failed\n");
			}
		}
	}
}

bool RTcmix::isInputAudioDevice(int fdIndex)
{
    return inputFileTable[fdIndex].isAudioDevice();
}

const char * RTcmix::getInputPath(int fdIndex)
{
    return inputFileTable[fdIndex].fileName();
}

void RTcmix::close()
{
	run_status = RT_SHUTDOWN;
	delete audioDevice;
	audioDevice = NULL;
}


// This is from the RTsockfuncs.c file in ../lib.  It's such a handy
// function, I just had to have it here, too!  BGG
/* RTtimeit takes a floating point number of seconds (interval) and a pointer
   to a void-returning function and sets up a timer to call that function
   every interval seconds.  Setting interval to 0.0 should disable the
   timer */
// I notice that because this uses SIGALRM it will wake up sleep()'s
// put sleep()s in a while loop to keep the process alive...
void RTtimeit(float interval, sig_t func)
{
	struct timeval tv;
	struct itimerval itv;

	tv.tv_sec = (int)(interval);
	tv.tv_usec = (int)((interval - (float)tv.tv_sec) * 1000000.0);
	itv.it_interval = tv;
	itv.it_value = tv;
	setitimer(ITIMER_REAL, &itv, NULL);
	signal(SIGALRM, func);
}

