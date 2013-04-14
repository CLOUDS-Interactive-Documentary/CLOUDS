#ifndef _RTSOCKFUNCS_H_
#define _RTSOCKFUNCS_H_ 1

/* RTsockfuncs.h - headers for a set of utility functions for real-time cmix
   interface programming (located in RTsockfuncs.c; link in RTsockfuncs.o
   to compile) */

int newRTsock(char *ihost, int rtsno);

int RTsock(char *ihost, int rtsno);

int RTopensocket(int socket, char *binaryname);

int RTopensocket_syscall(int socket, char *binaryname);

void RTkillsocket(int socket, int pid);

void RTsendsock(const char *cmd, int theSock, int nargs, ...);

void RTsetsockstr(struct sockdata *ssend, char *cmd, int nargs, double *fields);

void RTsetsockdata(struct sockdata *ssend, int arg, double val);

void RTsetsocktext(struct sockdata *ssend, int arg, char *text);

void RTprintsockstr(struct sockdata *ssend);

void RTsendsockstr(int theSock, struct sockdata *sockstr);

struct sockdata *newRTsockstr(char *name, int);

struct sockdata *RTnewsockstr(char *name);

void RTtimeit(float interval, void *func);

void parse(char *buf, char**);

int execute(char **);

#endif
