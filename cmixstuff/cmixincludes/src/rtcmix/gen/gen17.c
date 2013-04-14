#include <ugens.h>

/* Computes transfer function using Chebyshev polynomials
   First argument (after number, 17, size) is the index value for which the
   function will create the harmonics specified by the following arguments.
*/
double
gen17(struct gen *gen)
{
   int i, j;
   double Tn, Tn1, Tn2, x, d;

   d = (double) (gen->size / 2 - .5);
   for (i = 0; i < gen->size; i++) {
      x = (i / d - 1.) / gen->pvals[0];
      gen->array[i] = 0.0;
      Tn1 = 1.0;
      Tn = x;
      for (j = 1; j < gen->nargs; j++) {
         gen->array[i] = gen->pvals[j] * Tn + gen->array[i];
         Tn2 = Tn1;
         Tn1 = Tn;
         Tn = 2.0 * x * Tn1 - Tn2;
      }
   }
   /* no fnscl() done to this function */

   return 0.0;
}

