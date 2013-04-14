#include <math.h>
#include <ugens.h>

double
gen5(struct gen *gen)
{
   double c, amp2, amp1;
   int j, k, l, i = 0;

   amp2 = gen->pvals[0];
   if (amp2 <= 0.0)
      amp2 = 0.00001;
   for (k = 1; k < gen->nargs; k += 2) {
      amp1 = amp2;
      amp2 = gen->pvals[k + 1];
      if (amp2 <= 0.0)
         amp2 = 0.00001;
      j = i + 1;
      gen->array[i] = amp1;
      c = pow(amp2 / amp1, 1.0 / gen->pvals[k]);
      i = (j - 1) + gen->pvals[k];
      for (l = j; l < i; l++) {
         if (l < gen->size)
            gen->array[l] = gen->array[l - 1] * c;
      }
   }
   fnscl(gen);

   return 0.0;
}

