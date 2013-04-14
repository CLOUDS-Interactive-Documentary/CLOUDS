/* infile.c -- datafile name-setting command for use with gen1. D.A.S. 9/89
*/

#include <stdio.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ugens.h>
#include "globals.h"

double
m_infile(float *p, short n_args, double *pp) 
{
    FILE *descrip;
    char  *name;
    int   fno;

    name = DOUBLE_TO_STRING(pp[0]);
    fno = p[1];
    /* Reject fno = 0, because that's indicates stdin to gen2. */
    if (fno < 1 || fno > MAX_INFILE_DESC)
		die("infile", "File number must be between 1 and %d.",
						MAX_INFILE_DESC);

    descrip = fopen(name,"r");
    if (descrip == NULL)
		die("infile", "Cannot find %s ... not opened.", name);
    else {
		infile_desc[fno] = descrip;
		rtcmix_advise("infile", "Datafile %s opened as file %d.", name, fno);
    }
    return fno;
}

