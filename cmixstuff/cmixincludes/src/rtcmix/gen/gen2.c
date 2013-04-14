#include <stdlib.h>
#include <stdio.h>
#include <ugens.h>
#include "../globals.h"

/* gen2 lets the user fill a function table with numbers specified in the
   score or in a text file. There are two "API's" for this: the original
   one and a newer one that fixes problems with the older one. The old
   one remains for backwards compatibility with scores, as well as for
   the data file input functionality.

   Old way:

      makegen(x, 2, tablesize, fnum)

      If fnum is 0, then the table is filled with numbers appearing on
      the next line in the score. The gen tries to grab <tablesize>
      numbers. An example...

      makegen(1, 2, 8, 0)
      1.2  4.3  9.8  4.5  6.2  8.3  1.9  1.0

      If fnum is > 0, then it identifies a text file already opened
      by the infile Minc call, which returns this id. (The id is an
      index into a table of FILE pointers.)

   New way:

      The old way has problems. When used with fnum=0, it reads from
      stdin, behind the back of the parser. This means you can't use
      variables as part of the list of numbers, and the makegen won't
      work if it appears in a code block. Also, if the line of numbers
      ends with a space char, gen2 often fails.

      The new way is meant to make gen2 more like other makegens:

      makegen(x, 2, tablesize, num1, num2 [, num3, ...] )

      Only <tablesize> numbers will go into the function table,
      regardless of how many arguments there are. If there are
      fewer than <tablesize> args, the remaining table slots
      are set to zero.

      This function returns the number of elements placed into the
      table, not including any zero-padding. This is useful if you
      want to know how many elements the array contains, but don't
      want to count them manually. Just use a larger <tablesize>
      than you're likely to need, and get the real table size from
      the value returned to Minc by makegen.

   Note that this code distinguishes between the old and new ways
   by the number of pfields. If there are only 4 pfields (i.e., 
   gen->nargs is 1), then it assumes the old way. Otherwise, it
   assumes the new way. So a new-style array of 1 element will
   be interpreted as an old-style array (taking values either
   from the next line or from a file).

                             [new way and comments by JGG, 21-Feb-00]
*/
double
gen2(struct gen *gen)
{
   int i, num_elements;

   if (gen->nargs > 1) {        /* new way */
      int slots;

      /* gen->size is size of array */
      if (gen->size < gen->nargs)
         slots = gen->size;
      else
         slots = gen->nargs;

      for (i = 0; i < slots; i++)
         gen->array[i] = (double) gen->pvals[i];

      while (i < gen->size)     /* fill remainder (if any) with zeros */
         gen->array[i++] = 0.0;

      num_elements = slots;
   }
   else {                       /* old way */
      int   fno;
      float val;
      char  inval[128];
      FILE  *in_desc = NULL;

      /* input datafile is stdin if pval[0] = 0 */
      fno = (int) gen->pvals[0];
      if (fno == 0)
         in_desc = stdin;
      else if (fno <= MAX_INFILE_DESC)
         in_desc = infile_desc[fno];
      else
         return die("gen2", "Data file number must be between 1 and %d",
                                                        MAX_INFILE_DESC);

      if (in_desc == NULL)
         return die("gen2", "Call infile() to open the data file before using gen2.");

      i = 0;
      if (in_desc == stdin) {
         while (fscanf(in_desc, "%f", &val) != EOF) {
            if (i < gen->size)
               gen->array[i] = (double) val;
            i++;
            if (getc(in_desc) == 10)
               break;
         }
      }
      else {         /* if reading from input text file specified with infile */
         while (fscanf(in_desc, "%s", inval) != EOF) {
            if (i >= gen->size)
               break;
            gen->array[i] = atof(inval);
            i++;
         }
         rewind(in_desc);
      }
      if (i > gen->size)
         rtcmix_warn("gen2", "Table not large enough to hold all the data in file.");

      num_elements = (i <= gen->size) ? i : gen->size;
      rtcmix_advise("gen2", "%d values loaded into table.", num_elements);

      i--;
      while (++i < gen->size)      /* fill remainder (if any) with zeros */
         gen->array[i] = 0.0;
   }

   return (double) num_elements;
}


