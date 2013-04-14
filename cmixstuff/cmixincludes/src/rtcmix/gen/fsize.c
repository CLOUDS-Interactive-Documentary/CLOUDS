#include <ugens.h>
#include <stdio.h>

/* these 3 defined in makegen.c */
extern double *farrays[];
extern int sizeof_farray[];
extern int f_goto[];


/*   returns the size of function number genno */
int
fsize(int genno)
{
	if (!sizeof_farray[f_goto[genno]]) {
		die("fsize", "You haven't allocated function %d yet!", genno);
		return -1;
	}
	return sizeof_farray[f_goto[genno]];
}
