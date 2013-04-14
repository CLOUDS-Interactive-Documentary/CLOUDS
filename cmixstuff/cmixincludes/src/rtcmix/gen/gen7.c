#include <ugens.h>

double
gen7(struct gen *gen)
{
   double amp2, amp1 = 0.0;
   int j, k, l;
   int i = 0;
   amp2 = gen->pvals[0];
   for (k = 1; k < gen->nargs; k += 2) {
      amp1 = amp2;
      amp2 = gen->pvals[k + 1];
      j = i + 1;
      i = j + gen->pvals[k] - 1;
      for (l = j; l <= i; l++) {
         if (l <= gen->size)
            gen->array[l - 1] = amp1 +
                (amp2 - amp1) * (double) (l - j) / (i - j + 1);
      }
   }
   fnscl(gen);

   return 0.0;
}

