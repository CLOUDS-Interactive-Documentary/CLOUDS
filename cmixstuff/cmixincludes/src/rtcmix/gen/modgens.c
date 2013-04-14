/* RTcmix  - Copyright (C) 2002  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>  /* for memcpy */
#include <float.h>   /* for DBL_MAX */
#include <math.h>
#include <ugens.h>


static double *new_table(int size)
{
   return (double *) malloc((size_t) size * sizeof(double));
}


/* -------------------------------------------------------------- addgens --- */
/* Add corresponding values of gens whose table numbers are given in
   p1 and p2, placing the result in a new gen in the table number given in
   p0.  If the input tables are not the same size, the smaller one is resampled
   to have the size of the larger one before adding corresponding values.
*/
double
m_addgens(float p[], int n_args, double pp[])
{
   int   destslot, srcslot1, srcslot2, normalize, size;

   destslot = (int) p[0];
   srcslot1 = (int) p[1];
   srcslot2 = (int) p[2];
   normalize = (int) p[3];

   size = combine_gens(destslot, srcslot1, srcslot2, normalize,
                                                ADD_GENS, "addgens");
   return (double) size;
}


/* ------------------------------------------------------------- multgens --- */
/* Multiply corresponding values of gens whose table numbers are given in
   p1 and p2, placing the result in a new gen in the table number given in
   p0.  If the input tables are not the same size, the smaller one is resampled
   to have the size of the larger one before multiplying corresponding values.
*/
double
m_multgens(float p[], int n_args, double pp[])
{
   int   destslot, srcslot1, srcslot2, normalize, size;

   destslot = (int) p[0];
   srcslot1 = (int) p[1];
   srcslot2 = (int) p[2];
   normalize = (int) p[3];

   size = combine_gens(destslot, srcslot1, srcslot2, normalize,
                                                MULT_GENS, "multgens");
   return (double) size;
}


/* -------------------------------------------------------------- copygen --- */
/* Make a copy of the gen whose table number is given in p1.  Assign the
   copy the table number given in p0.  The copied table will have the size
   given in p2.  p3 is a number specifying the type of interpolation to use
   when resampling the source table to fit the new number of locations.
   Return the size of the new table.

      p0    table number of new table
      p1    table number of original table
      p2    size of new table [optional, default is size of original]
      p3    interpolation type (0: no interpolation, 1: linear interpolation)
               [optional, default is 1]
*/
double
m_copygen(float p[], int n_args, double pp[])
{
   int   srcslot, destslot, srcsize, destsize;
   double *srctable, *desttable;
   InterpolationType interp;

   destslot = (int) p[0];
   srcslot = (int) p[1];
   srctable = floc(srcslot);
   if (srctable == NULL)
      return die("copygen", "No function table defined for slot %d.", srcslot);
   srcsize = fsize(srcslot);

   destsize = (n_args > 2) ? (int) p[2] : srcsize;
   interp = (n_args > 3) ? (InterpolationType) p[3] : LINEAR_INTERP;

   desttable = resample_gen(srctable, srcsize, destsize, interp);
   if (desttable == NULL)
      return die("copygen", "No memory to copy the gen in slot %d.", srcslot);

   if (!install_gen(destslot, destsize, desttable))
      return die("copygen", "No more function tables available.");

   return (double) destsize;
}


/* ------------------------------------------------------------ offsetgen --- */
/* Add a constant, given in p1, to the values of the gen whose table number
   is given in p0.  Note that no rescaling of the resulting table is done,
   so that values outside [-1, 1] are possible.
*/
double
m_offsetgen(float p[], int n_args, double pp[])
{
   int   i, slot, size;
   double *srctable, *desttable, offset;

   slot = (int) p[0];
   srctable = floc(slot);
   if (srctable == NULL)
      return die("offsetgen", "No function table defined for slot %d.", slot);
   size = fsize(slot);
   offset = p[1];
   desttable = new_table(size);
   if (desttable == NULL)
      return die("offsetgen", "No memory for new function table.");
   if (!install_gen(slot, size, desttable))
      return die("offsetgen", "No more function tables available.");

   for (i = 0; i < size; i++)
      desttable[i] = srctable[i] + offset;

   return (double) size;
}


/* ------------------------------------------------------------- scalegen --- */
/* Multiply by a constant, given in p1, the values of the gen whose table
   number is given in p0.  Note that no rescaling of the resulting table is
   done, so that values outside [-1, 1] are possible.
*/
double
m_scalegen(float p[], int n_args, double pp[])
{
   int   i, slot, size;
   double *srctable, *desttable, scale;

   slot = (int) p[0];
   srctable = floc(slot);
   if (srctable == NULL)
      return die("scalegen", "No function table defined for slot %d.", slot);
   size = fsize(slot);
   scale = p[1];
   desttable = new_table(size);
   if (desttable == NULL)
      return die("scalegen", "No memory for new function table.");
   if (!install_gen(slot, size, desttable))
      return die("scalegen", "No more function tables available.");

   for (i = 0; i < size; i++)
      desttable[i] = srctable[i] * scale;

   return (double) size;
}


/* ------------------------------------------------------------ invertgen --- */
/* Invert the values of the gen whose table number is given in p0.  The
   y-axis center of symmetry is a point halfway between the min and max
   table values; inversion is performed around this center of symmetry.
*/
double
m_invertgen(float p[], int n_args, double pp[])
{
   int   i, slot, size;
   double min, max, center, *srctable, *desttable;

   slot = (int) p[0];
   srctable = floc(slot);
   if (srctable == NULL)
      return die("invertgen", "No function table defined for slot %d.", slot);
   size = fsize(slot);
   desttable = new_table(size);
   if (desttable == NULL)
      return die("invertgen", "No memory for new function table.");
   if (!install_gen(slot, size, desttable))
      return die("invertgen", "No more function tables available.");

   /* determine central y-axis value */
   min = DBL_MAX;
   max = -DBL_MAX;
   for (i = 0; i < size; i++) {
      if (srctable[i] < min)
         min = srctable[i];
      if (srctable[i] > max)
         max = srctable[i];
   }
   center = min + ((max - min) / 2.0);

//rtcmix_advise("invertgen", "min: %f, max: %f, center: %f", min, max, center);

   /* invert values around center */
   for (i = 0; i < size; i++) {
      double diff = srctable[i] - center;
      desttable[i] = center - diff;
   }

   return (double) size;
}


/* ----------------------------------------------------------- reversegen --- */
/* Reverse the values of the gen whose table number is given in p0. */
double
m_reversegen(float p[], int n_args, double pp[])
{
   int   i, j, slot, size;
   double *srctable, *desttable;

   slot = (int) p[0];
   srctable = floc(slot);
   if (srctable == NULL)
      return die("reversegen", "No function table defined for slot %d.", slot);
   size = fsize(slot);
   desttable = new_table(size);
   if (desttable == NULL)
      return die("reversegen", "No memory for new function table.");
   if (!install_gen(slot, size, desttable))
      return die("reversegen", "No more function tables available.");

   for (i = 0, j = size - 1; i < size; i++, j--)
      desttable[i] = srctable[j];

   return (double) size;
}


/* ------------------------------------------------------------- shiftgen --- */
/* Shift the values of the gen whose table number is given in p0 by the number
   of array locations given in p1.  Positive values shift to the right;
   negative values to the left.  If a value is shifted off the end of the
   array in either direction, it reenters the other end of the array.

   Two examples:
      [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]      source table, size = 10
      [7, 8, 9, 0, 1, 2, 3, 4, 5, 6]      shift = 3
      [3, 4, 5, 6, 7, 8, 9, 0, 1, 2]      shift = -3
*/
double
m_shiftgen(float p[], int n_args, double pp[])
{
   int      slot, size, shift, abs_shift;
   size_t   movesize;
   double   *srctable, *desttable;

   slot = (int) p[0];
   srctable = floc(slot);
   if (srctable == NULL)
      return die("shiftgen", "No function table defined for slot %d.", slot);
   size = fsize(slot);
   shift = (int) p[1];
   abs_shift = abs(shift);
   if (abs_shift == 0 || abs_shift == size) {
      rtcmix_advise("shiftgen", "Your shift of %d has no effect on the table!", shift);
      return (double) size;
   }
   if (abs_shift > size)
      return die("shiftgen", "You can't shift by more than the table size.");

   desttable = new_table(size);
   if (desttable == NULL)
      return die("shiftgen", "No memory for new function table.");
   if (!install_gen(slot, size, desttable))
      return die("shiftgen", "No more function tables available.");

   movesize = (size_t) (size - abs_shift);  /* doubles to shift */
   if (shift > 0) {
      memcpy(desttable, srctable + movesize, (size_t) abs_shift
                                                            * sizeof(double));
      memcpy(desttable + abs_shift, srctable, movesize * sizeof(double));
   }
   else {
      memcpy(desttable, srctable + abs_shift, movesize * sizeof(double));
      memcpy(desttable + movesize, srctable, (size_t) abs_shift
                                                            * sizeof(double));
   }

   return (double) size;
}


/* ---------------------------------------------------------- quantizegen --- */
/* Quantize the values of the gen whose table number is given in p0 to the
   quantum given in p1.
*/
double
m_quantizegen(float p[], int n_args, double pp[])
{
   int      i, slot, size;
   double   quantum, *srctable, *desttable;

   slot = (int) p[0];
   srctable = floc(slot);
   if (srctable == NULL)
      return die("quantizegen", "No function table defined for slot %d.", slot);
   size = fsize(slot);
   quantum = p[1];
   if (quantum <= 0.0)
      return die("quantizegen", "Quantum must be greater than zero.");
   desttable = new_table(size);
   if (desttable == NULL)
      return die("quantizegen", "No memory for new function table.");
   if (!install_gen(slot, size, desttable))
      return die("quantizegen", "No more function tables available.");

   /* It'd be nice to let the C library do the rounding, but rintf rounds
      to the nearest even integer, and round and roundf don't even work
      on my Slackware 8.1 system.  Screw it, we'll roll our own.  -JGG
   */
   for (i = 0; i < size; i++) {
      double quotient = fabs(srctable[i] / quantum);
      int floor = (int) quotient;
      double remainder = quotient - (double) floor;
      if (remainder >= 0.5)   /* round to nearest */
         floor++;
      if (srctable[i] < 0.0)
         desttable[i] = -floor * quantum;
      else
         desttable[i] = floor * quantum;
   }

   return (double) size;
}


