#include <math.h>
#include <ugens.h>

double
gen6(struct gen *gen)
{
   if (setline(gen->pvals, gen->nargs, gen->size, gen->array) == -1)
      return -1.0;
   fnscl(gen);

   return 0.0;
}

