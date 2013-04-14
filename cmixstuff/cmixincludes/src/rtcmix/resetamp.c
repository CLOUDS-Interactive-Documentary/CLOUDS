#include "ugens.h"
#include "sfheader.h"
#include <stdio.h>
#include <unistd.h>
#include <sndlibsupport.h>

extern SFMAXAMP      sfm[NFILES];
extern SFHEADER	     sfdesc[NFILES];
extern off_t filepointer[NFILES];
extern int sfd[NFILES];
extern int isopen[NFILES];


static SFCODE ampcode = {
	SF_MAXAMP,
	sizeof(SFMAXAMP) + sizeof(SFCODE)
};

double
resetamp(float p[], int n_args)
{
	int i;	
	int fno;
	fno = p[0];
	if(!isopen[fno]) {
		printf("File number %d has not been opened\n",fno);
		return -1.0;
	}
	for(i = 0; i<sfchans(&sfdesc[fno]); i++) {
		sfmaxamp(&sfm[fno],i) = 0;
		sfmaxamploc(&sfm[fno],i) = 0;
	}
	if((filepointer[fno] = lseek(sfd[fno],0L,0)) < 0) {
		fprintf(stderr,"Bad lseek to beginning of file\n");
		perror("lseek");
		closesf();
	}

	putsfcode(&sfdesc[fno],(char *)&sfm[fno],&ampcode);

	if(wheader(sfd[fno],(char *)&sfdesc[fno])) {
		fprintf(stderr,"Bad header write\n");
		perror("write");
		closesf();
	}
	printf("reset header amplitudes to 0 in file %d\n",fno);

	return 0.0;
}

