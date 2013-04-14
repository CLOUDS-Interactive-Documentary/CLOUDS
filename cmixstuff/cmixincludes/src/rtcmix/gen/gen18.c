#include <ugens.h>

double
gen18(struct gen *gen)
{
   return (double) setline(gen->pvals, gen->nargs, gen->size, gen->array);
}

