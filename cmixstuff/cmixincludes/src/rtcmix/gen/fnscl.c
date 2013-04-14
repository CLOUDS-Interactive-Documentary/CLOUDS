#include <math.h>
#include <ugens.h>

void
fnscl(struct gen *gen)
{
   int j;
   double wmax, xmax = 0;

   if (gen->slot < 0)
      return;

   for (j = 0; j < gen->size; j++) {
      wmax = fabs(gen->array[j]);
      if (wmax > xmax)
         xmax = wmax;
   }
   for (j = 0; j < gen->size; j++) {
      gen->array[j] /= xmax;
   }
}

