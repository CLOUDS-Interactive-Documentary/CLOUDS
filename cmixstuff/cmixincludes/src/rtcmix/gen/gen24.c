#include <ugens.h>

double
gen24(struct gen *gen)
{
   double scaler, amp2, amp1;
   int j, k, l;
   int i = 0;

   scaler = ((double) gen->size) / gen->pvals[gen->nargs - 2];
   amp2 = gen->pvals[1];
   for (k = 1; k < gen->nargs; k += 2) {
      amp1 = amp2;
      amp2 = gen->pvals[k + 2];
      j = i + 1;
      i = gen->pvals[k + 1] * scaler + 1;
      for (l = j; l <= i; l++) {
         if (l <= gen->size)
            gen->array[l - 1] = amp1
                                + (amp2 - amp1) * (double) (l - j) / (i - j + 1);
      }
   }
   fnscl(gen);

   return 0.0;
}

