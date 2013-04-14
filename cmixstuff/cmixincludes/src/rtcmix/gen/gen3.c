#include <stdio.h>
#include <math.h>
#include <ugens.h>
#include "globals.h"

double
gen3(struct gen *gen)
{
   float val;
   int fno, i = 0;
   FILE *in_desc;

   fno = (int) gen->pvals[0];
   if (fno < 1 || fno > MAX_INFILE_DESC)
      return die("gen3", "Data file number must be between 1 and %d",
                                                             MAX_INFILE_DESC);

   in_desc = infile_desc[fno];
   if (in_desc == NULL)
      return die("gen3", "Call infile() to open the data file before using gen3.");

   /* read input file until EOF */

   while (fread(&val, sizeof(val), 1, in_desc)) {
      if (i >= gen->size)
         break;
      gen->array[i] = (double) val;
      i++;
   }
   rewind(in_desc);

   if (i > gen->size)
      rtcmix_warn("gen3", "Table not large enough to hold all the data in file.");

   rtcmix_advise("gen3", "%d values loaded into table.",
                (i <= gen->size) ? i : gen->size);

   i--;
   while (++i < gen->size)      /* fill remainder (if any) with zeros */
      gen->array[i] = 0.0;

   /* fnscl(gen); */ /* no rescaling of this gen */

   return 0.0;
}

