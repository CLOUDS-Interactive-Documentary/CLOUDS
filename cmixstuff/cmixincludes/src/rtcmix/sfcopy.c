/* fast file-to-file copy */
/* p0=input fno, p1=output fno, p2=inputskip, p3=output skip, p4=dur */

#include "ugens.h"
#include "sfheader.h"
#include <stdio.h>
#include <unistd.h>

/* these are all defined in sound.c */
extern int  sfd[NFILES];            /* soundfile descriptors */
extern char *sndbuf[NFILES];        /* address of buffer */
extern off_t  filepointer[NFILES];   /* to save current pointer in file */
extern int   nbytes;
extern SFHEADER      sfdesc[NFILES];


double
sfcopy(float p[], int n_args)
{
	int maxread,n,input,output,bytes,jj;

	if(!n_args) fprintf(stderr,"(sfcopy(input_fno,output_fno,input_skip,output_skip,dur)\n");
	input = (int)p[0];
	output = (int)p[1];
	if((sfclass(&sfdesc[input]) != sfclass(&sfdesc[output])) ||
	   (sfchans(&sfdesc[input]) != sfchans(&sfdesc[output]))) {
		fprintf(stderr,
		 "Input and output specifications do not match. Canot copy.\n");
		closesf();
	}
	bytes = setnote(p[2],p[4],input) * 
		 sfchans(&sfdesc[input]) * sfclass(&sfdesc[input]);

	setnote(p[3],p[4],output);
	_backup(input);
	_backup(output);

	fprintf(stderr,"Copy %d bytes\n",bytes);

	while(bytes) {
		maxread = (bytes > nbytes) ? nbytes : bytes;
		if((n = read(sfd[input],sndbuf[input],maxread)) <= 0) {
			fprintf(stderr,"Apparent eof on input\n");
			return -1.0;
		}
		if((jj = write(sfd[output],sndbuf[input],n)) <= 0) {
			fprintf(stderr,"Trouble writing output file\n");
			closesf();
		}
		bytes -= n;
		filepointer[input] += n;
		filepointer[output] += n;
	}
	if(fsync(sfd[output]) < 0 ) {
		fprintf(stderr,"trouble fsyncing file");
		closesf();
	}
	fprintf(stderr,"Copy completed\n");

	return 0.0;
}

