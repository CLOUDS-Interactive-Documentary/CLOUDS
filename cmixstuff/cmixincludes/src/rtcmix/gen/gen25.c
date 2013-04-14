#include <ugens.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

double
gen25(struct gen *gen)
{
   int i;

   switch ((int) (gen->pvals[0])) {
      case 1:  /* hanning window */
         for (i = 0; i < gen->size; i++)
            gen->array[i] = -cos(2.0 * M_PI * (double) i / (double) (gen->size))
                                                                  * 0.5 + 0.5;
         break;

      case 2:  /* hamming window */
         for (i = 0; i < gen->size; i++) {
            double val = cos(2.0 * M_PI * (double) i / (double) (gen->size));
            gen->array[i] = 0.54 - 0.46 * val;
         }
         break;
      default:
         return die("gen25", "don't know about window type %d\n",
                                                     (int) (gen->pvals[0]));
   }
   fnscl(gen);

   return 0.0;
}

