#include "sfheader.h"
#include "ugens.h"
#include <stdio.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>
static SFCODE	ampcode = {
	SF_MAXAMP,
	sizeof(SFMAXAMP) + sizeof(SFCODE)
}; 
extern SFHEADER sfdesc[NFILES];

double
sfprint(float p[], int n_args)
{
	int fno;
	fno = p[0];
	fprintf(stderr,"Header info for file number %d\n",fno);
	printsf(&sfdesc[fno]);

	return 0.0;
}
