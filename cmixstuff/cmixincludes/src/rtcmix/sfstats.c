#include <stdio.h>
#include <string.h>
#include <sys/file.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <signal.h>
#include <errno.h>
#include <sfheader.h>
#include <sndlibsupport.h>


void
sfstats(int fd)
{
	int n;
	SFHEADER sfh;
	SFMAXAMP sfm;
	SFCOMMENT sfcm;
	char *cp;

	lseek(fd,0,0);
	rheader(fd,&sfh);

	printf("stats for soundfile w/ descriptor %d:\n\n",fd);
	printf("channels: %d\tsampling rate: %f",sfchans(&sfh),sfsrate(&sfh));
	if (sfclass(&sfh) == SF_SHORT)
		printf("\tinteger file\n");
	else
		printf("\tfloating point file\n");

	cp = getsfcode(&sfh,SF_MAXAMP);
	if (cp) { 
		bcopy(cp + sizeof(SFCODE), (char *) &sfm, sizeof(SFMAXAMP));
		printf("\nMAXAMP structure found, containing:\n");
		printf("\tchannel #\tmaxamp\tsample #\n");
		for (n = 0; n <= sfchans(&sfh); n++) {
			printf("\t %d\t %f\t %ld\n\n",n,sfmaxamp(&sfm,n),sfmaxamploc(&sfm,n));
		}
	}
	cp = getsfcode(&sfh,SF_COMMENT);
	if (cp) {
		bcopy(cp + sizeof(SFCODE), (char *) &sfcm, ((SFCODE *)cp)->bsize);
		printf("\nSFCOMMENT structure found, containing:\n");
		printf("  %s\n\n",&sfcomm(&sfcm,0));
	}
}
