#ifndef _SOCKDEFS_H_
#define _SOCKDEFS_H_ 1

#include <netinet/in.h>
#include "maxdispargs.h"

#define MAXTEXTARGS ((MAXDISPARGS*sizeof(double))/512)

#define MYPORT (IPPORT_RESERVED + 78)
/* IPPORT_RESERVED is 1024, so this is socket 1102
   IPPORT_MAXPORT is 65535 on SGIs */

typedef struct sockdata {
  char name[100];		/* the cmix command name */
  union {
    double p[MAXDISPARGS];	/* the p-fields */
    char text[MAXTEXTARGS][512];     /* or text arrays  */
  } data;
  int n_args;		/* number of p-fields used */
} RTsockstr;

#endif
