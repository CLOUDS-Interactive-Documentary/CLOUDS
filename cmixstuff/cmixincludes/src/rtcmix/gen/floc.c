#include <stdio.h>
#include <ugens.h>

/* these 3 defined in makegen.c */
extern double *farrays[];
extern int sizeof_farray[];
extern int f_goto[];

/* Returns the address of function number genno, or NULL if the
   function array doesn't exist.

   NOTE: It's the responsiblity of instruments to deal with a
   missing gen, either by using die() or supplying a default
   and alerting the user with advise().
*/
double *floc(int genno)
{
   int index;

   if (genno < 0)
      return NULL;
      
   index = f_goto[genno];
   if (sizeof_farray[index] == 0)
      return NULL;

   return farrays[index];
}

