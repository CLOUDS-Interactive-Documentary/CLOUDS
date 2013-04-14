#include <globals.h>
#include <ugens.h>
#include <stdio.h>
#include <math.h>
#include <sys/time.h>

/* fills a gen with random numbers b/t 0 and 1 --
   the argument is the distribution type:
     0 = even distribution
     1 = low weighted linear distribution
     2 = high weighted linear distribution
     3 = triangle linear distribution
     4 = gaussian distribution
     5 = cauchy distribution
    
     by r. luke dubois - CMC/CU, 1998.
    
   distribution equations adapted from dodge and jerse.

   optional arguments added (JGG):

      makegen(x, 20, size, dist_type[, seed[, min, max]]

   If <seed> is zero, seed comes from microsecond clock,
   otherwise <seed> is used as the seed.  If no <seed>
   argument, the seed used is 1.

   <min> and <max> define the range (inclusive) for the
   random numbers output.  Both args must be present;
   otherwise the range is from 0 to 1.
*/

/* Scale <num>, which falls in range [0,1] so that it falls
   in range [min,max].  Return result.    -JGG, 12/4/01
*/
static double
fit_range(double min, double max, double num)
{
   return min + (num * (max - min));
}


double
gen20(struct gen *gen)
{
   int i, j, k, type;
   int N = 12;
   double halfN = 6;
   double scale = 1;
   double mu = 0.5;
   double sigma = .166666;
   double randnum = 0.0;
   double randnum2 = 0.0;
   struct timeval tv;
   double output;
   double alpha = .00628338;
   double min, max, tmp;
   static long randx = 1;

   type = (int) gen->pvals[0];

   if (gen->pvals[1] == 0) {           /* added optional seed  -JG */
      gettimeofday(&tv, NULL);
      randx = tv.tv_usec;
   }
   else
      randx = (int) gen->pvals[1];

   /* Set range for random numbers. */
   if (gen->nargs == 3)
      return die("gen20",
          "usage: makegen(x, 20, size, dist_type[, seed[, min, max]])");
   if (gen->nargs == 4) {
      min = gen->pvals[2];
      max = gen->pvals[3];
      if (min == max)
         return die("gen20", "<min> must be lower than <max>");
      if (min > max) {     /* make sure these are in increasing order */
         tmp = max;
         max = min;
         min = tmp;
      }
   }
   else {
      min = 0.0;
      max = 1.0;
   }

   switch (type) {
      case 0:  /* even distribution */
         for (i = 0; i < gen->size; i++) {
            k = ((randx = randx * 1103515245 + 12345) >> 16) & 077777;
            tmp = (double) k / 32768.0;
            gen->array[i] = fit_range(min, max, tmp);
         }
         break;
      case 1:  /* low weighted */
         for (i = 0; i < gen->size; i++) {
            k = ((randx = randx * 1103515245 + 12345) >> 16) & 077777;
            randnum = (double) k / 32768.0;
            k = ((randx = randx * 1103515245 + 12345) >> 16) & 077777;
            randnum2 = (double) k / 32768.0;
            if (randnum2 < randnum) {
               randnum = randnum2;
            }
            gen->array[i] = fit_range(min, max, randnum);
         }
         break;
      case 2:  /* high weighted */
         for (i = 0; i < gen->size; i++) {
            k = ((randx = randx * 1103515245 + 12345) >> 16) & 077777;
            randnum = (double) k / 32768.0;
            k = ((randx = randx * 1103515245 + 12345) >> 16) & 077777;
            randnum2 = (double) k / 32768.0;
            if (randnum2 > randnum) {
               randnum = randnum2;
            }
            gen->array[i] = fit_range(min, max, randnum);
         }
         break;
      case 3:  /* triangle */
         for (i = 0; i < gen->size; i++) {
            k = ((randx = randx * 1103515245 + 12345) >> 16) & 077777;
            randnum = (double) k / 32768.0;
            k = ((randx = randx * 1103515245 + 12345) >> 16) & 077777;
            randnum2 = (double) k / 32768.0;
            tmp = 0.5 * (randnum + randnum2);
            gen->array[i] = fit_range(min, max, tmp);
         }
         break;
      case 4:  /* gaussian */
         i = 0;
         while (i < gen->size) {
            randnum = 0.0;
            for (j = 0; j < N; j++) {
               k = ((randx = randx * 1103515245 + 12345) >> 16) & 077777;
               randnum += (double) k / 32768.0;
            }
            output = sigma * scale * (randnum - halfN) + mu;
            if ((output <= 1.0) && (output >= 0.0)) {
               gen->array[i] = fit_range(min, max, output);
               i++;
            }
         }
         break;
      case 5:  /* cauchy */
         i = 0;
         while (i < gen->size) {
            do {
               k = ((randx = randx * 1103515245 + 12345) >> 16) & 077777;
               randnum = (double) k / 32768.0;
            }
            while (randnum == 0.5);
            randnum = randnum * PI;
            output = (alpha * tan(randnum)) + 0.5;
            if ((output <= 1.0) && (output >= 0.0)) {
               gen->array[i] = fit_range(min, max, output);
               i++;
            }
         }
         break;
      default:
         return die("gen20", "don't know about distribution %d\n", type);
         break;
   }

   return 0.0;
}

