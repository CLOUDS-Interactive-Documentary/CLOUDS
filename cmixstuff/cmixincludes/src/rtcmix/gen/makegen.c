/* RTcmix  - Copyright (C) 2002  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/
#include <ugens.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>    /* for fabs */
#include <assert.h>

extern double gen1(struct gen *gen, char *sfname);
extern double gen2(struct gen *gen);
extern double gen3(struct gen *gen);
extern double gen4(struct gen *gen);
extern double gen5(struct gen *gen);
extern double gen6(struct gen *gen);
extern double gen7(struct gen *gen);
extern double gen9(struct gen *gen);
extern double gen10(struct gen *gen);
extern double gen17(struct gen *gen);
extern double gen18(struct gen *gen);
extern double gen20(struct gen *gen);
extern double gen24(struct gen *gen);
extern double gen25(struct gen *gen);


static int ngens = 1;           /* total number of gens so far */
/* Note: This was 0, but then non-existant gen funcs were not caught.  -JGG */

/* ok, TOTGENS is the absolute total number of gens we can have in
   a given run, MAXGENS is the number of simultaneous gens we can
   have.  TOTGENS should probably be replaced by an appropriate malloc */

#define TOTGENS 10000
#define MAXGENS 300

double *farrays[TOTGENS];
int sizeof_farray[TOTGENS];
int f_goto[MAXGENS];   /* this is used to guarantee unique farrays in rtcmix */


/* ---------------------------------------------------------- install_gen --- */
/* Install <table>, of <size> elements, into the cmix function table list
   as the user-visible table number <slot>.  Return 1 if okay, or 0 if the
   function table list is full.

   NOTE: makegen creates a new function for *every* call to it.  This is so
   that we can guarantee the correct version of a given function at run-time
   during RT operation.  The f_goto[] array keeps track of where to map a
   particular function table number during the queueing of RT Instruments.
*/
int
install_gen(int slot, int size, double *table)
{
   int index = ngens;
   if (index >= TOTGENS)
      return 0;
   f_goto[slot] = index;
   sizeof_farray[index] = size;
   farrays[index] = table;
   ngens++;       /* for next time */
   return 1;
}


/* -------------------------------------------------------------- makegen --- */
/* p0=storage loc, p1=gen no, p2=size, p3--> = args */
double
makegen(float p[], int n_args, double pp[])
{
   int    genslot, genno;
   double retval = -1.0;
   double *table = NULL;
   struct gen gen;

   genslot = (int) p[0];
   genno = (int) p[1];
   gen.size = (int) p[2];

   if (genslot < 0)
      genslot = -genslot;
   if (genslot >= MAXGENS) {
      die("makegen", "No more simultaneous function tables available!");
      return -1.0;
   }

   if (genno != 1) {    /* gen1 must allocate its own memory */
      table = (double *) malloc((size_t) gen.size * sizeof(double));
      if (table == NULL) {
         die("makegen", "Not enough memory for function table %d.", genslot);
         return -1.0;
      }
   }

   if (!install_gen(genslot, gen.size, table)) {
      die("makegen", "No more function tables available!");
      return -1.0;
   }

   gen.nargs = n_args - 3;
   gen.pvals = p + 3;
   gen.array = table;
   gen.slot = (int) p[0];   /* get from pfield, to preserve negative "flag" */

   switch (genno) {
      case 25:
         retval = gen25(&gen);
         break;
      case 24:
         retval = gen24(&gen);
         break;
      case 20:
         retval = gen20(&gen);
         break;
      case 18:
         retval = gen18(&gen);
         break;
      case 17:
         retval = gen17(&gen);
         break;
      case 10:
         retval = gen10(&gen);
         break;
      case 9:
         retval = gen9(&gen);
         break;
      case 7:
         retval = gen7(&gen);
         break;
      case 6:
         retval = gen6(&gen);
         break;
      case 5:
         retval = gen5(&gen);
         break;
      case 4:
         retval = gen4(&gen);
         break;
      case 3:
         retval = gen3(&gen);
         break;
      case 2:
         retval = gen2(&gen);
         break;
      case 1:
         {
            char *sfname = DOUBLE_TO_STRING(pp[3]);
            gen.pvals = p + 2;    /* gen1() needs size pfield */
            gen.nargs++;
            retval = gen1(&gen, sfname);
            if (retval != -1.0) {
               /* We don't know these before calling gen1, so get them now. */
               farrays[f_goto[genslot]] = gen.array;
               sizeof_farray[f_goto[genslot]] = gen.size;
            }
         }
         break;
      default:
         retval = (double) die("makegen", "There is no gen%d.", genno);
   }

   return retval;
}


/* --------------------------------------------------------- resample_gen --- */
/* Given a function table of a certain size <cursize>, allocate and return
   a new table of size <newsize>, filled with values resampled from the old
   table using the kind of interpolation specified by <interp>.  If the two
   sizes are equivalent, merely make a straight copy of the table.  Return
   NULL if memory allocation error.  (JGG, 11/29/02)
*/
double *
resample_gen(double table[], int cursize, int newsize, InterpolationType interp)
{
   int   i;
   double *newtable;

   assert(interp == NO_INTERP || interp == LINEAR_INTERP);

   newtable = (double *) malloc((size_t) newsize * sizeof(double));
   if (newtable == NULL)
      return NULL;

   if (newsize == cursize) {
      for (i = 0; i < newsize; i++)
         newtable[i] = table[i];
   }
   else {
      double incr = (double) cursize / (double) newsize;

      if (interp == NO_INTERP) {
         double f = 0.0;
         for (i = 0; i < newsize; i++) {
            int n = (int) f;
            newtable[i] = table[n];
            f += incr;
         }
      }
      else if (interp == LINEAR_INTERP) {
         double frac, next, diff = 0.0;
         double f = 0.0;
         for (i = 0; i < newsize; i++) {
            int n = (int) f;
            frac = f - (double) n;
            if (frac) {
               next = (n + 1 < cursize) ? table[n + 1] : table[cursize - 1];
               diff = next - table[n];
            }
            newtable[i] = table[n] + (diff * frac);
            f += incr;
         }
      }
   }
   return newtable;
}


/* --------------------------------------------------------- combine_gens --- */
/* Given two function tables occupying the gen locations <srcslot1> and
   <srcslot2>, allocate a new table, to occupy <destslot>, filled with a
   combination of corresponding elements from the source tables.  The
   type of combination is specified by <modtype>, currently addition
   or multiplication.  The new table has the same size as the larger
   of the two source tables, and this size is returned.
*/
int
combine_gens(int destslot, int srcslot1, int srcslot2, int normalize,
                                    GenModType modtype, char *funcname)
{
   int      i;
   int      srcindex1, srcindex2;
   int      destsize, srcsize1, srcsize2;
   double   *destarray, *srcarray1, *srcarray2, *tmparray = NULL;

   assert(modtype == ADD_GENS || modtype == MULT_GENS);

   if (destslot == 0 || srcslot1 == 0 || srcslot2 == 0)
      return die(funcname, "Gen number pfields cannot be zero.");
   if (srcslot1 >= MAXGENS)
      return die(funcname, "Gen number %d out of range.", srcslot1);
   if (srcslot2 >= MAXGENS)
      return die(funcname, "Gen number %d out of range.", srcslot2);
   if (destslot >= MAXGENS)
      return die(funcname, "No more simultaneous function tables available.");

   srcindex1 = f_goto[srcslot1];
   srcindex2 = f_goto[srcslot2];
   srcsize1 = sizeof_farray[srcindex1];
   srcsize2 = sizeof_farray[srcindex2];
   if (srcsize1 == 0)
      return die(funcname, "Gen number %d doesn't exist.", srcslot1);
   if (srcsize2 == 0)
      return die(funcname, "Gen number %d doesn't exist.", srcslot2);

   srcarray1 = farrays[srcindex1];
   srcarray2 = farrays[srcindex2];

   /* If necessary, resample shorter table so that it uses same number of
      slots as the longer table.
   */
   if (srcsize1 != srcsize2) {
      int   oldsize;
      double *oldarray;

      if (srcsize1 > srcsize2) {
         destsize = srcsize1;
         oldsize = srcsize2;
         oldarray = srcarray2;
      }
      else {
         destsize = srcsize2;
         oldsize = srcsize1;
         oldarray = srcarray1;
      }
      tmparray = resample_gen(oldarray, oldsize, destsize, LINEAR_INTERP);
      if (tmparray == NULL)
         return die(funcname, "Not enough memory for temporary function table.");

      if (srcsize1 > srcsize2)
         srcarray2 = tmparray;
      else
         srcarray1 = tmparray;
   }
   else
      destsize = srcsize1;

   destarray = (double *) malloc((size_t) destsize * sizeof(double));
   if (destarray == NULL)
      return die(funcname, "Not enough memory for new function table.");

   if (!install_gen(destslot, destsize, destarray))
      return die(funcname, "No more function tables available.");

   /* Fill destination array. */
   switch (modtype) {
      case ADD_GENS:
         for (i = 0; i < destsize; i++)
            destarray[i] = srcarray1[i] + srcarray2[i];
         break;
      case MULT_GENS:
         for (i = 0; i < destsize; i++)
            destarray[i] = srcarray1[i] * srcarray2[i];
         break;
      default:
         break;
   }

   if (tmparray)
      free(tmparray);

   if (normalize) {        /* cf fnscl.c */
      double max = 0.0;

      for (i = 0; i < destsize; i++) {
         double val = fabs(destarray[i]);
         if (val > max)
            max = val;
      }
      for (i = 0; i < destsize; i++)
         destarray[i] /= max;
   }

   return destsize;
}


