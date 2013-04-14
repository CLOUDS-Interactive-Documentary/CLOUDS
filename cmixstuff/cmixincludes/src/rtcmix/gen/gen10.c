#include <math.h>
#include <ugens.h>

double
gen10(struct gen *gen)
{
   int i, j;

   for (i = 0; i < gen->size; i++)
      gen->array[i] = 0.0;
   j = gen->nargs;
   while (j--) {
      if (gen->pvals[j] != 0) {
         for (i = 0; i < gen->size; i++) {
				double val = (double) (PI2 * (double) i / (gen->size / (j + 1)));
            gen->array[i] += sin(val) * gen->pvals[j];
         }
      }
   }
   fnscl(gen);

   return 0.0;
}

