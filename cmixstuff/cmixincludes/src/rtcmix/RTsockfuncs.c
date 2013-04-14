#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include <sys/types.h>
#include <netdb.h>
#include <errno.h>
#include <signal.h>
#include <sys/signal.h>
#include <sys/time.h>
#include <sys/socket.h>

#include <sockdefs.h>
#include "RTsockfuncs.h"

/* RTsockfuncs.c - a set of utility functions for real-time cmix interface
   programming */

/* RTsock takes an internet hostname and the number of an RTcmix socket
   (specified by "... -s n" when invoking an instrument -- 0 is the default)
   and returns an open socket connection.  The RTcmix sockets count up from
   MYPORT (1102), defined in sockdefs.h */

int newRTsock(char *ihost, int rtsno) {
	int s, err;
	struct sockaddr_in sss;
	struct hostent *hp;

	if( (s = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
#ifdef HARD_EXIT
		exit(1);
#endif
	}

	hp = gethostbyname(ihost);
	sss.sin_family = AF_INET;
	bcopy(hp->h_addr, &(sss.sin_addr.s_addr), hp->h_length);
	sss.sin_port = htons(MYPORT+rtsno);

	err = connect(s, (struct sockaddr *)&sss, sizeof(sss));

	if(err < 0) {
		perror("connect");
		return err;
#ifdef HARD_EXIT
		exit(1);
#endif
	}
	return s;
}

int RTsock(char *ihost, int rtsno) {
	int s;
	struct sockaddr_in sss;
	struct hostent *hp;

	if( (s = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		exit(1);
	}

	hp = gethostbyname(ihost);
	sss.sin_family = AF_INET;
	bcopy(hp->h_addr, &(sss.sin_addr.s_addr), hp->h_length);
	sss.sin_port = htons(MYPORT+rtsno);

	if(connect(s, (struct sockaddr *)&sss, sizeof(sss)) < 0) {
		perror("connect");
		exit(1);
	}
	return s;
}

/* RTopensocket - starts up the relevant cmix binary with the relevant socket 
   number (again counted up from MYPORT) and returns the process id of the
   relevant cmix process */

int RTopensocket(int socket, char *binaryname) {
	char syscall[60];
	char flags[30];
	char socknumber[2];
	char args[64];
	int pid;
	int s;
	struct sockaddr_in sss;
	struct hostent *hp;

	sprintf(syscall, "%s ", binaryname);
	sprintf(flags, "-i -n -s ");
	sprintf(socknumber, "%i", socket);
	strcat(syscall, flags);
	strcat(syscall, socknumber);
	strcat(syscall, " &>cmix.dbug");

	parse(syscall, (char**)args);
	pid = execute((char**)args);
	printf("cmix process id is %i\n", pid);

	return(pid);
}

/* Slightly different version that does not use fork() */
int RTopensocket_syscall(int socket, char *binaryname) {
	char syscall[60];
	char flags[30];
	char socknumber[2];
	char args[64];
	int pid;
	int s;
	struct sockaddr_in sss;
	struct hostent *hp;

	sprintf(syscall, "%s ", binaryname);
	sprintf(flags, "-i -n -c -o %i", socket);
	strcat(syscall, flags);
	strcat(syscall, " &>cmix.dbug &");

	pid = system(syscall);

	if (pid != 0) {
	  printf("CMIX system call failed\n");
#ifdef HARD_EXIT
	  exit(1);
#endif
	}
	
	return pid;
}

/* RTkillsocket closes the relevant cmix socket and kills off the relevant
   cmix process and its pid+1 child */

void RTkillsocket(int socket, int pid)
{
	char killcall[20];
	char process[5];

	shutdown(socket,0);

	sprintf(killcall, "kill ");
	sprintf(process, "%i", pid);
	strcat(killcall, process);
	system(killcall);

	close(socket);
}

/* Send the struct right on through */

void RTsendsockstr(int theSock, struct sockdata *sockstr)
{
  int amt;
  /* printf("Writing to socket %d\n",theSock); */
  amt = write(theSock, (void *)sockstr, sizeof(struct sockdata));
  while (amt < sizeof(struct sockdata)) 
    amt += write(theSock, (void *)sockstr, sizeof(struct sockdata));
}


/* RTsendsock takes an RTcmix scorefile command as the first arg, then
   the socket (returned by RTsock), the number of p-fields, and then
   the p-fields themselves (as doubles) */

void RTsendsock(const char *cmd, int theSock, int nargs, ...)
{
	va_list ap;
	struct sockdata ssend;
	int i;

	/* printf("Writing to socket %d\n",theSock); */

	strcpy(ssend.name, cmd);

	va_start(ap, nargs);
		if ( (strcmp(ssend.name, "rtinput") == 0) ||
			(strcmp(ssend.name, "rtoutput") == 0) ||
			(strcmp(ssend.name,"set_option") == 0) ||
			(strcmp(ssend.name,"bus_config") == 0) ||
			(strcmp(ssend.name, "load")==0) ) {
		for (i = 0; i < nargs; i++) {
			strcpy(ssend.data.text[i], va_arg(ap, char*));
		}
	}
	else {
		for (i = 0; i < nargs; i++) {
			ssend.data.p[i] = va_arg(ap, double);
		}
	}
	va_end(ap);

	ssend.n_args = nargs;

	write(theSock, (char *)&ssend, sizeof(struct sockdata));
}

/* Modify an existing sockdata's data */

void RTsetsockstr(struct sockdata *ssend, char *cmd, int nargs, double *fields)
{
  int i;

  strcpy(ssend->name, cmd);

  for (i = 0; i < nargs; i++) {
    ssend->data.p[i] = fields[i];
  }

  ssend->n_args = nargs;
}

/* Set only 1 data element */

void RTsetsockdata(struct sockdata *ssend, int arg, double val)
{
  double tval;
  tval = (double)val;
  ssend->data.p[arg] = val;
}

void RTsetsocktext(struct sockdata *ssend, int arg, char *text)
{
  strcpy(ssend->data.text[arg],text);
}

/* Print out a sockdata's values */

void RTprintsockstr(struct sockdata *ssend)
{
  int i;
  printf("name:  %s\n",ssend->name);
  printf("nargs:  %d\n",ssend->n_args);

  for (i=0;i<ssend->n_args;i++) {
    printf("p[%d]:  %2.2f\n",i,ssend->data.p[i]);
  }

  for(i=0;i<MAXTEXTARGS;i++) {
    if (ssend->data.text[i][0] != '\0') {
      printf("text[%d]:  %s\n",i,ssend->data.text[i]);
    }
    else {
      i = MAXTEXTARGS;
    }
  }
}

/* wrapper for the following */

RTsockstr *newRTsockstr(char *name, int n_args) {
   RTsockstr *theSock;
   theSock = RTnewsockstr(name);
   theSock->n_args = n_args;
   return theSock;
}

/* Return a nice clean socket structure */

RTsockstr *RTnewsockstr(char *name)
{
  RTsockstr *theSock;
  int i;
  
  theSock = (RTsockstr *)malloc(sizeof(RTsockstr));
			     
  theSock->n_args = 0;
  strcpy(theSock->name,name);

  for(i=0;i<MAXTEXTARGS;i++) {
    theSock->data.text[i][0] = (char)'\0';
  }
  theSock->data.p[0] = 0;

  return theSock;
}

/* RTtimeit takes a floating point number of seconds (interval) and a pointer
   to a void-returning function and sets up a timer to call that function
   every interval seconds.  Setting interval to 0.0 should disable the
   timer */

void RTtimeit(float interval, void *func)
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

void parse(char *buf, char **args)
{
	while (*buf != '\0') {
		/* Strip whitespace.  Use nulls, so that the previous argument
		    is terminated automatically. */
		while ((*buf == ' ') || (*buf == '\t')) 
			*buf++ = '\0';

		/* Save the argument */
		*args++ = buf;

		/* Skip over the argument. */
		while ((*buf != '\0') && (*buf != ' ') && (*buf != '\t')) 
			buf++;
	}

	*args = NULL;
}

int execute(char **args)
{
	int status;
	int pid;

	/* fork */
	if ((pid = fork()) < 0) {
		perror("fork");
		exit(1);
	}

	if (pid == 0) {
		execvp(*args, args);
		perror(*args);
		return(pid);
	}
	return 0;
}
