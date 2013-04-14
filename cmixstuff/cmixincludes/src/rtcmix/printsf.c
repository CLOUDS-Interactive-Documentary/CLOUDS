#include "byte_routines.h"
#include "sfheader.h"
#include "ugens.h"
#include <stdio.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>

#define MAX_TIME_CHARS  64

/* NOTE: not able to retrieve comment in sndlib version with just sfh */
void printsf(SFHEADER *sfh)
{
   int      n;
   SFMAXAMP *sfmp;
   char     timestr[MAX_TIME_CHARS];

   printf("sr: %f  nchans: %d  class: %d\n",
          sfsrate(sfh), sfchans(sfh), sfclass(sfh));

   sfmp = &(sfh->sfinfo.sf_maxamp);

   if (sfmaxamptime(sfmp)) {
      for (n = 0; n < sfchans(sfh); n++)
         printf("channel %d:  maxamp: %g  loc: %ld\n",
                n, sfmaxamp(sfmp, n), sfmaxamploc(sfmp, n));

      strftime(timestr, MAX_TIME_CHARS, "%a %b %d %H:%M:%S %Z %Y",
                                         localtime(&sfmaxamptime(sfmp)));
      printf("maxamp updated: %s\n", timestr);
   }
   else
      printf("(no maxamp stats)\n");
}

