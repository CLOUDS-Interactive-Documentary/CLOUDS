#ifndef _RTCMIXMAIN_H_
#define _RTCMIXMAIN_H_

#include <RTcmix.h>
#include "DynamicLib.h"

class RTcmixMain : public RTcmix {
public:
#ifdef MAXMSP
	RTcmixMain();  // called from main.cpp
	// BGG -- for flushing Queue/Heap frpm flush_sched() (main.cpp)
	void resetQueueHeap();
	// BGG -- experimental dynloading of RTcmix insts in max/msp
	DynamicLib theDSO;
	int doload(char *dsoPath);
	void unload();
#else
	RTcmixMain(int argc, char **argv, char **env);	// called from main.cpp
#endif
	void			run();	

protected:
	// Initialization methods.
	void			parseArguments(int argc, char **argv, char **env);
	static void		interrupt_handler(int);
	static void		signal_handler(int);
	static void		set_sig_handlers();

	static void *	sockit(void *);
	
private:
	char *			makeDSOPath(const char *progPath);
	static int 		xargc;	// local copy of arg count
	static char *	xargv[/*MAXARGS + 1*/];
	static char **	xenv;
	static int 		interrupt_handler_called;
	static int 		signal_handler_called;
	static int		noParse;
	#ifdef NETAUDIO
	static int		netplay;     // for remote sound network playing
	#endif
	/* for more than 1 socket, set by -s flag to CMIX as offset from MYPORT */
	static int		socknew;
};

#endif	// _RTCMIXMAIN_H_
