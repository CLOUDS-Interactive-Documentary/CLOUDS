#include <stdio.h>
#include <string.h>
#include <strings.h>

#ifdef NETAUDIO

#define DEFAULT_SOCKNUM "9999"

extern char globalNetworkPath[];	// sys/audio_devices.cpp

int setnetplay(char *hname, char *snum)
{
	char netpath[128];

	if (hname[0] == '\0') {
		fprintf(stderr, "Null hostname\n");
		return(-1);
	}

	strcpy(netpath, hname);
	strcat(netpath, ":");

	if (snum[0] == '\0')
		strcat(netpath, DEFAULT_SOCKNUM); 
	else
		strcat(netpath, snum); 

	strcpy(globalNetworkPath, netpath);

	return 0;
}

#endif	// NETAUDIO
