/* RTcmix  - Copyright (C) 2004  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/

#include <signal.h>
#include <stdlib.h>
#include <iostream>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "RTcmixMain.h"
#include <AudioDevice.h>
#include <Option.h>
#include "version.h"
#include <ugens.h>
#include <ug_intro.h>
#include "prototypes.h"
#include "../parser/rtcmix_parse.h"
#include <sockdefs.h>
#include <limits.h>
#include <MMPrint.h>
#include "heap.h"

using namespace std;

#ifdef NETAUDIO
extern int setnetplay(char *, char *);    // in setnetplay.cpp
#endif

/* ---------------------------------------------------------------- usage --- */
static void
usage()
{
   printf("\n"
      "Usage:  CMIX [options] [arguments] < minc_script.sco\n"
      "\n"
      "   or, to use Perl instead of Minc:\n"
      "        PCMIX [options] [arguments] < perl_script.pl\n"
      "   or:\n"
      "        PCMIX [options] perl_script.pl [arguments]\n"
      "\n"
      "   or, to use Python:\n"
      "        PYCMIX [options] [arguments] < python_script.py\n"
      "\n"
      "        options:\n"
      "           -i       run in interactive mode\n"
      "           -n       no init script (interactive mode only)\n"
      "           -o NUM   socket offset (interactive mode only)\n"
#ifdef LINUX
      "           -p NUM   set process priority to NUM (as root only)\n"
#endif
      "           -D NAME  audio device name\n"
#ifdef NETAUDIO
      "           -k NUM   socket number (netplay)\n"
      "           -r NUM   remote host ip (or name for netplay)\n"
#endif
      "          NOTE: -s, -d, and -e are not yet implemented\n"
      "           -s NUM   start time (seconds)\n"
      "           -d NUM   duration (seconds)\n"
      "           -e NUM   end time (seconds)\n"
      "           -f NAME  read score from NAME instead of stdin\n"
      "                      (Minc and Python only)\n"
      "           --debug  enter parser debugger (Perl only)\n"
      "           -q       quiet -- suppress print to screen\n"
      "           -Q       really quiet -- not even clipping or peak stats\n"
      "           -h       this help blurb\n"
      "        Other options, and arguments, passed on to parser.\n\n");
   exit(1);
}

#ifdef DEBUG_MEMORY

#include <string.h>

void *operator new(size_t size) {
	size_t memsize = (size > 4) ? size : 4;
	void *ptr = malloc(memsize);
	memset(ptr, 0xaa, memsize);
	return ptr;
}

void operator delete(void *mem) {
	if (mem) {
		memset(mem, 0xdd, 4);
		free(mem);
	}
}

#endif

extern "C" {
  int profile();
  void rtprofile();
	// I don't call the profiles here, because dead-time instruments
	// won't be compiled into the object file unless they are present at
	// the build (i.e. they aren't DSO's).  RT instruments have the
	// rtprofile() called when they get loaded.  Go Doug, go!
}

// RTcmixMain is the derived RTcmix class called by main()

// Private static state

int 			RTcmixMain::xargc;
char *			RTcmixMain::xargv[MAXARGS + 1];
char **			RTcmixMain::xenv = NULL;
int				RTcmixMain::interrupt_handler_called = 0;
int				RTcmixMain::signal_handler_called = 0;

int				RTcmixMain::noParse         = 0;
int				RTcmixMain::socknew			= 0;
#ifdef NETAUDIO
int				RTcmixMain::netplay 		= 0;	// for remote sound network playing
#endif


// ------------------------------------------------------------- makeDSOPath ---
// Assuming that our default shared lib dir is in the same directory as our
// bin dir, return the path name of the shared lib dir.  Return empty string
// if path to RTcmix executable <progPath> isn't deep enough, or if derived
// dir name doesn't exist.  Caller is responsible for deleting returned 
// string.  -JGG

#define PATH_DELIMITER  '/'
#define DSO_DIRNAME     "shlib"     // FIXME: move to site.conf?

char *RTcmixMain::makeDSOPath(const char *progPath)
{
   char *dsoPath = new char [PATH_MAX + 1];
   strncpy(dsoPath, progPath, PATH_MAX);
   dsoPath[PATH_MAX] = 0;

   // back up two delimiters (e.g., "RTcmix/bin/CMIX" to "RTcmix/")
   char *p = strrchr(dsoPath, PATH_DELIMITER);
   if (p) {
      *p = 0;
      p = strrchr(dsoPath, PATH_DELIMITER);
      if (p) {
         p++;
         strcpy(p, DSO_DIRNAME);

         // use directory only if it exists
         struct stat statbuf;
         if (stat(dsoPath, &statbuf) == 0 && S_ISDIR(statbuf.st_mode))
            return dsoPath;
      }
   }

   // give up in case there weren't two delimiters or dir is invalid
   dsoPath[0] = 0;
   return dsoPath;
}

#ifdef MAXMSP
// BGG mm -- got rid of argc and argv for max/msp
RTcmixMain::RTcmixMain() : RTcmix(0)
#else
RTcmixMain::RTcmixMain(int argc, char **argv, char **env) : RTcmix(false)
#endif
{
#ifndef MAXMSP
   set_sig_handlers();
#endif

// FIXME: should consult a makefile variable to tell us whether we should
// let dsoPath constructed at run time override SHAREDLIBDIR.

// FIXME: But ... there is no reasonable way to get the app path at
// runtime anyway...

   // From comp.lang.c FAQ list - Question 19.31
   // Q: How can my program discover the complete pathname to the executable
   //    from which it was invoked?
   // A: argv[0] may contain all or part of the pathname, or it may contain
   //    nothing. You may be able to duplicate the command language
   //    interpreter's search path logic to locate the executable if the name
   //    in argv[0] is present but incomplete.  However, there is no
   //    guaranteed solution.

#ifdef MAXMSP
	init_globals(true, NULL);			// 'true' indicates we were called from main

	for (int i = 1; i <= MAXARGS; i++) xargv[i] = NULL;
	xargc = 1;
#else
   char *dsoPath = makeDSOPath(argv[0]);
   init_globals(true, dsoPath);		// 'true' indicates we were called from main
   delete [] dsoPath;

   parseArguments(argc, argv, env);
#endif

   // Note:  What follows was done in main().  Some of it is identical
   // to RTcmix::init() for imbedded.  Factor this out.
   /* Banner */
#ifdef MAXMSP
	MMPrint::mm_print_ptr += (sprintf(MMPrint::mm_print_ptr, "--------> %s %s <--------\n", RTCMIX_NAME, RTCMIX_VERSION)+1);
#else
   if (Option::print())
      printf("--------> %s %s (%s) <--------\n",
             RTCMIX_NAME, RTCMIX_VERSION, argv[0]);
#endif

   ::ug_intro();                /* introduce standard routines */
   ::profile();                 /* introduce user-written routines etc. */
   ::rtprofile();               /* introduce real-time user-written routines */

   setbuf(stdout, NULL);        /*  Want to see stdout errors */
}

void
RTcmixMain::parseArguments(int argc, char **argv, char **env)
{
   int         i;
#ifdef LINUX
   int		   priority = 0;
#endif
   char        *infile;
#ifdef NETAUDIO
   char        rhostname[60], thesocket[8];

   rhostname[0] = thesocket[0] = '\0';
#endif

   xargv[0] = argv[0];
   for (i = 1; i <= MAXARGS; i++)
      xargv[i] = NULL;
   xargc = 1;
   xenv = env;

   /* Process command line, copying any args we don't handle into
      <xargv> for parsers to deal with.
   */
   for (i = 1; i < argc; i++) {
      char *arg = argv[i];

      if (arg[0] == '-') {
         switch (arg[1]) {
            case 'h':
               usage();
               break;
            case 'i':               /* for separate parseit thread */
               rtInteractive = 1;
               audio_config = 0;
               break;
            case 'n':               /* for use in rtInteractive mode only */
               noParse = 1;
               break;
            case 'Q':               /* really quiet */
               Option::reportClipping(false);
               Option::checkPeaks(false); /* (then fall through) */
            case 'q':               /* quiet */
               Option::print(0);
               break;
#ifdef LINUX
			case 'p':
               if (++i >= argc) {
                  fprintf(stderr, "You didn't give a priority number.\n");
                  exit(1);
               }
			   priority = atoi(argv[i]);
			   break;
#endif
            case 'D':
               if (++i >= argc) {
                  fprintf(stderr, "You didn't give an audio device name.\n");
                  exit(1);
               }
               Option::device(argv[i]);
               break;
#ifdef NETAUDIO
            case 'r':               /* set up for network playing */
              	if (++i >= argc) {
                  fprintf(stderr, "You didn't give a remote host ip.\n");
                  exit(1);
              	}
               /* host ip num */
               strcat(rhostname, "net:");
               strncat(rhostname, argv[i], 59-4);    /* safe strcat */
               rhostname[59] = '\0';
               netplay = 1;
               break;
            case 'k':               /* socket number for network playing */
                                    /* defaults to 9999 */
               if (++i >= argc) {
                  fprintf(stderr, "You didn't give a socket number.\n");
                  exit(1);
               }
               strncpy(thesocket, argv[i], 7);
               thesocket[7] = '\0';
               netplay = 1;
               break;
#endif
            case 'o':               /* NOTE NOTE NOTE: will soon replace -s */
            case 's':               /* set up a socket offset */
               if (++i >= argc) {
                  fprintf(stderr, "You didn't give a socket offset.\n");
                  exit(1);
               }
               socknew = atoi(argv[i]);
               printf("listening on socket: %d\n", MYPORT + socknew);
               break;
//            case 's':               /* start time (unimplemented) */
            case 'd':               /* duration to play for (unimplemented) */
            case 'e':               /* time to stop playing (unimplemented) */
               fprintf(stderr, "-s, -d, -e options not yet implemented\n");
               exit(1);
               break;
            case 'f':     /* use file name arg instead of stdin as score */
               if (++i >= argc) {
                  fprintf(stderr, "You didn't give a file name.\n");
                  exit(1);
               }
               infile = argv[i];
               use_script_file(infile);
               break;
            case '-':           /* accept "--debug" and pass to Perl as "-d" */
               if (strncmp(&arg[2], "debug", 10) == 0)
                  xargv[xargc++] = strdup("-d");
               break;
            default:
               xargv[xargc++] = arg;    /* copy for parser */
         }
      }
      else
         xargv[xargc++] = arg;          /* copy for parser */

      if (xargc >= MAXARGS) {
         fprintf(stderr, "Too many command-line options.\n");
         exit(1);
      }
   }
   // RtcHandle state which is set via args but not stored in RTcmix.
   // NOTE:  The way this is handled should change.
#ifdef NETAUDIO
   if (netplay) {             /* set up socket for sending audio */
      int status = ::setnetplay(rhostname, thesocket);
      if (status == -1) {
         fprintf(stderr, "Cannot establish network connection to '%s' for "
                                             "remote playing\n", rhostname);
         exit(-1);
      }
      fprintf(stderr, "Network sound playing enabled on machine '%s'\n",
                                                                  rhostname);
    }
#endif

}

void
RTcmixMain::run()
{
   pthread_t   sockitThread;
   int retcode;
   /* In rtInteractive mode, we set up RTcmix to listen for score data
      over a socket, and then parse this, schedule instruments, and play
      them concurrently. The socket listening and parsing go in one
      thread, and the scheduler and instrument code go in another.

      When not in rtInteractive mode, RTcmix parses the score, schedules
      all instruments, and then plays them -- in that order.
   */
   if (rtInteractive) {
		rtcmix_advise(NULL, "rtInteractive mode set\n");

#ifndef MAXMSP
      /* Read an initialization score. */
      if (!noParse) {
         int status;
#ifdef DBUG
         cout << "Parsing once ...\n";
#endif
         status = ::parse_score(xargc, xargv, xenv);
         if (status != 0)
            exit(1);
      }
#endif // MAXMSP

      /* Create parsing thread. */
#ifdef DBUG
      fprintf(stdout, "creating sockit() thread\n");
#endif
      retcode = pthread_create(&sockitThread, NULL, &RTcmixMain::sockit, (void *) this);
      if (retcode != 0) {
         fprintf(stderr, "sockit() thread create failed\n");
      }

      /* Create scheduling thread. */
#ifdef DBUG
      fprintf(stdout, "calling runMainLoop()\n");
#endif
      retcode = runMainLoop();
      if (retcode != 0) {
         fprintf(stderr, "runMainLoop() failed\n");
      }

      /* Join parsing thread. */
#ifdef DBUG
      fprintf(stdout, "joining sockit() thread\n");
#endif
      retcode = pthread_join(sockitThread, NULL);
      if (retcode != 0) {
         fprintf(stderr, "sockit() thread join failed\n");
      }

      /* Wait for audio thread. */
#ifdef DBUG
      fprintf(stdout, "calling waitForMainLoop()\n");
#endif
	  retcode = waitForMainLoop();
      if (retcode != 0) {
         fprintf(stderr, "waitForMailLoop() failed\n");
      }

      if (!noParse)
         destroy_parser();
   }
   else {
#ifdef MAXMSP
		int status = 0;
#else
      int status = ::parse_score(xargc, xargv, xenv);
#endif
#ifdef PYTHON
      /* Have to reinstall this after running Python interpreter. (Why?) */
	  set_sig_handlers();
#endif
      if (status == 0) {
#ifdef LINUX
//		 if (priority != 0)
//			 if (setpriority(PRIO_PROCESS, 0, priority) != 0)
//			 	perror("setpriority");
#endif
         if ((status = runMainLoop()) == 0)
			 waitForMainLoop();
	  }
      else
         exit(status);

#ifndef MAXMSP
      destroy_parser();
#endif
   }

#ifndef MAXMSP
   ::closesf_noexit();
#endif
}

/* ---------------------------------------------------- interrupt_handler --- */
void
RTcmixMain::interrupt_handler(int signo)
{
	// Dont do handler work more than once
	if (!interrupt_handler_called) {
		interrupt_handler_called = 1;
	   fprintf(stderr, "\n<<< Caught interrupt signal >>>\n");

	   // Notify rendering loop.
	   run_status = RT_SHUTDOWN;
	   if (audioDevice) {
	       audioDevice->close();
	   }
	   if (!audioLoopStarted) {
		   closesf();	// We exit if we have not yet configured audio.
	   }
	}
}

/* ------------------------------------------------------- signal_handler --- */
void
RTcmixMain::signal_handler(int signo)
{
	// Dont do handler work more than once
	if (!signal_handler_called) {
		signal_handler_called = 1;
	   fprintf(stderr, "\n<<< Caught internal signal (%d) >>>\n", signo);

	   run_status = RT_ERROR;
	   switch (signo) {
	   default:
		   fflush(stdout);
		   fflush(stderr);
  	 	   exit(1);
	       break;
	   }
	}
}

void
RTcmixMain::set_sig_handlers()
{
   /* Call interrupt_handler on cntl-C. */
   if (signal(SIGINT, interrupt_handler) == SIG_ERR) {
      fprintf(stderr, "Error installing signal handler.\n");
      exit(1);
   }
   /* Call signal_handler on segv, etc. */
   if (signal(SIGSEGV, signal_handler) == SIG_ERR) {
      fprintf(stderr, "Error installing signal handler.\n");
      exit(1);
   }
#if defined(SIGBUS)
   if (signal(SIGBUS, signal_handler) == SIG_ERR) {
      fprintf(stderr, "Error installing signal handler.\n");
      exit(1);
   }
#endif
}

void *
RTcmixMain::sockit(void *arg)
{
    char ttext[MAXTEXTARGS][512];
    int i;

    // socket stuff
    int s, ns;
    struct sockaddr_in sss;
    int err;
    struct sockdata *sinfo;
    size_t amt;
    char *sptr;
    int val,optlen;
	Bool audio_configured = NO;

    /* create the socket for listening */

#ifdef DBUG
    cout << "ENTERING sockit() FUNCTION **********\n";
#endif
    if( (s = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      perror("socket");
	  run_status = RT_ERROR;	// Notify inTraverse()
      exit(1);
    }

    /* set up the receive buffer nicely for us */
    optlen = sizeof(char);
    val = sizeof(struct sockdata);
    setsockopt(s, SOL_SOCKET, SO_RCVBUF, &val, optlen);

    /* set up the socket address */
    sss.sin_family = AF_INET;
    sss.sin_addr.s_addr = INADDR_ANY;
    // socknew is offset from MYPORT to allow more than one inst
    if (noParse) {}	
    sss.sin_port = htons(MYPORT+socknew);

    err = bind(s, (struct sockaddr *)&sss, sizeof(sss));
    if (err < 0) {
      perror("bind");
	  fflush(stdout);
	  run_status = RT_ERROR;	// Notify inTraverse()
	  sleep(1);
#ifndef MAXMSP
	  cout << "\n";
#endif
      exit(1);
    }

    listen(s, 1);

#ifdef JAGUAR
    int len = sizeof(sss);
#else
    socklen_t len = sizeof(sss);
#endif
    ns = accept(s, (struct sockaddr *)&sss, &len);
    if(ns < 0) {
      perror("accept");
	  run_status = RT_ERROR;	// Notify inTraverse()
      exit(1);
    }
    else {

      sinfo = new (struct sockdata);
      // Zero the socket structure
      sinfo->name[0] = '\0';
      for (i=0;i<MAXDISPARGS;i++) {
		sinfo->data.p[i] = 0;
      }
	  
      // we do this when the -n flag is set, it has to parse rtsetparams()
      // coming over the socket before it can access the values of RTBUFSAMPS,
      // SR, NCHANS, etc.
      if (noParse) {
		
		// Wait for the ok to go ahead
		pthread_mutex_lock(&audio_config_lock);
		if (!audio_config) {
#ifndef MAXMSP
		  if (Option::print())
			cout << "sockit():  waiting for audio_config . . . \n";
#endif
		}
		pthread_mutex_unlock(&audio_config_lock);
		
		while (!audio_configured) {
		  pthread_mutex_lock(&audio_config_lock);
		  if (audio_config) {
			audio_configured = YES;
		  }
		  pthread_mutex_unlock(&audio_config_lock);

		  sptr = (char *)sinfo;
		  amt = read(ns, (void *)sptr, sizeof(struct sockdata));
		  while (amt < sizeof(struct sockdata)) amt += read(ns, (void *)(sptr+amt), sizeof(struct sockdata)-amt);
		  if ( (strcmp(sinfo->name, "rtinput") == 0) ||
			   (strcmp(sinfo->name, "rtoutput") == 0) ||
			   (strcmp(sinfo->name,"set_option") == 0) ||
			   (strcmp(sinfo->name,"bus_config") == 0) ||
			   (strcmp(sinfo->name, "load")==0) ) {
			// these two commands use text data
			// replace the text[i] with p[i] pointers
			for (i = 0; i < sinfo->n_args; i++)
			  strcpy(ttext[i],sinfo->data.text[i]);
			for (i = 0; i < sinfo->n_args; i++) {
			  sinfo->data.p[i] = STRING_TO_DOUBLE(ttext[i]);
			}
		  }
		  (void) ::dispatch(sinfo->name, sinfo->data.p, sinfo->n_args, NULL);
		}
		
		if (audio_configured && rtInteractive) {
			if (Option::print())
				cout << "sockit():  audio set.\n";
		}
		
	  }

      // Main socket reading loop
      while(1) {

		sptr = (char *)sinfo;
		amt = read(ns, (void *)sptr, sizeof(struct sockdata));
		while (amt < sizeof(struct sockdata)) amt += read(ns, (void *)(sptr+amt), sizeof(struct sockdata)-amt);
		if ( (strcmp(sinfo->name, "rtinput") == 0) ||
			(strcmp(sinfo->name, "rtoutput") == 0) ||
			(strcmp(sinfo->name,"set_option") == 0) ||
			(strcmp(sinfo->name,"bus_config") == 0) ||
			(strcmp(sinfo->name, "load")==0) ) {
			
			// these two commands use text data
			// replace the text[i] with p[i] pointers
			for (i = 0; i < sinfo->n_args; i++)
				strcpy(ttext[i],sinfo->data.text[i]);
			for (i = 0; i < sinfo->n_args; i++) {
				sinfo->data.p[i] = STRING_TO_DOUBLE(ttext[i]);
			}
		}
		else if ( (strcmp(sinfo->name, "RTcmix_off") == 0) ) {
			printf("RTcmix termination cmd received.\n");
			run_status = RT_SHUTDOWN;	// Notify inTraverse()
 			shutdown(s,0);
			return NULL;
		}
		else if ( (strcmp(sinfo->name, "RTcmix_panic") == 0) ) {
			int count = 30;
			printf("RTcmix panic cmd received...\n");
			run_status = RT_PANIC;	// Notify inTraverse()
			while (count--) {
#ifdef linux
				usleep(1000);
#endif
			}
			printf("Resuming normal mode\n");
			run_status = RT_GOOD;	// Notify inTraverse()
		}
		else {
	
#ifdef DBUG
		  cout << "sockit(): elapsed = " << getElapsed() << endl;
		  cout << "sockit(): SR = " << SR << endl;
#endif
		  if(sinfo->name) {
#ifdef ALLBUG
			cout << "SOCKET RECIEVED\n";
			cout << "sinfo->name = " << sinfo->name << endl;
			cout << "sinfo->n_args = " << sinfo->n_args << endl;
			for (i=0;i<sinfo->n_args;i++) {
			  cout << "sinfo->data.p[" << i << "] =" << sinfo->data.p[i] << endl;
			}
#endif
			(void) ::dispatch(sinfo->name, sinfo->data.p, sinfo->n_args, NULL);
	    
		  }
		}
      }
    }
#ifdef DBUG
    cout << "EXITING sockit() FUNCTION **********\n";
#endif
}

#ifdef MAXMSP
// BGG -- called by flush_sched() in main.cpp (for the [flush] message)
void
RTcmixMain::resetQueueHeap()
{
	delete rtHeap;
	delete [] rtQueue;
	rtHeap = NULL;
	rtQueue = NULL;

	rtHeap = new heap;
	rtQueue = new RTQueue[MAXBUS*3];
}


// BGG mm -- this is the loading code for instrument development using rtcmix~
// this was mainly copied from loader.c (disabled in rtcmix~) and is called
// from the loadinst() function at the end of main.cpp

typedef void (*mm_loadFun)();

int 
RTcmixMain::doload(char *dsoPath)
{
    int profileLoaded;
    mm_loadFun mm_loadrtprof = NULL;

	// unload the dso if already present (this method checks for that)
	theDSO.unload();

    if (theDSO.load(dsoPath) != 0) {
		rtcmix_warn("load", "Unable to dynamically load '%s': %s",
			 dsoPath, theDSO.error());
		return 0;
    }

    profileLoaded = 0;

    /* if present, load & call the shared library's rtprofile function to 
     * load its symbols.  Note that we access the function via its 
     * unmangled symbol name due to its extern "C" decl in rt.h.
     */

	// BGG mm -- loading into rtcmix~ uses an auxiliary function in the
	// instrument definition called "mm_load_rtprofile()", which then
	// calls the rtprofile() [renamed mm_rtprofile()].
	if (theDSO.loadFunction(&mm_loadrtprof, "mm_load_rtprofile") == 0) {
      profileLoaded = 1; 
      (*mm_loadrtprof)(); 
#ifdef DBUG
      printf("Loaded RT profile\n"); 
#endif
     } 

    if (!profileLoaded) {
		rtcmix_warn("load", "Unable to find a profile routine in DSO '%s'", dsoPath);
		theDSO.unload();
		return 0;
    }

#ifndef MAXMSP
// BGG -- this totally cause the maxmsp compile to stop
	rtcmix_advise("loader", "Loaded %s functions from shared
		library:\n\t'%s'.\n", (profileLoaded == 3) ? "standard and RT" :
						   (profileLoaded == 2) ? "RT" : "standard", dsoPath);
#endif

	return 1;
}

void
RTcmixMain::unload()
{
	theDSO.unload();
}
#endif // MAXMSP
