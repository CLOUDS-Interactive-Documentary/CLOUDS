/*
 *	merror.c: text of mix error messages
 */
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include	"mixerr.h"

int	mixerr;
char	*mix_errlist[] = {
	"Error 0",
	"This name already defined",
	"No subroutine by that name",
	"No more memory",
	"Instrument initialization error",
	0
};

void
merror(const char *s)
{
	char *c;
	int n;

	c = "Unknown error";
	if (mixerr < MX_NERR)
		c = mix_errlist[mixerr];
	n = strlen(s);
	if(n) {
		write(2, s, n);
		write(2, ": ", 2);
	}
	write(2, c, strlen(c));
	write(2, "\n", 1);
}
