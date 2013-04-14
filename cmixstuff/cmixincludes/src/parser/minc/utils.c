/* RTcmix  - Copyright (C) 2004  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/
#include <stdlib.h>
#include "minc_internal.h"

/* Minc utilities.  By John Gibson, 1/24/2004 */


/* --------------------------------------------------------- is_float_list -- */
/* Return 1 if <list> contains only elements of type MincFloat; otherwise,
   return 0.
*/
int
is_float_list(const MincList *list)
{
   int i;

   for (i = 0; i < list->len; i++)
      if (list->data[i].type != MincFloatType)
         return 0;

   return 1;
}


/* --------------------------------------------------- float_list_to_array -- */
/* Given a MincList containing only floats, convert the data into a new array
   of MincFloat, and return this.  If the MincList contains any non-float data,
   return NULL.  If there isn't enough memory for a new array, the program will
   die before this function returns.
*/
MincFloat *
float_list_to_array(const MincList *list)
{
   int i;
   MincFloat *array;

   array = (MincFloat *) emalloc(list->len * sizeof(MincFloat));
   if (array == NULL)
      return NULL;
   for (i = 0; i < list->len; i++) {
      if (list->data[i].type != MincFloatType) {
         free(array);
         return NULL;
      }
      array[i] = list->data[i].val.number;
   }

   return array;
}


/* --------------------------------------------------- array_to_float_list -- */
/* Convert the given array of MincFloat into a new MincList, and return this.
   If there isn't enough memory for a new MincList, the program will die
   before this function returns.
*/
MincList *
array_to_float_list(const MincFloat *array, const int len)
{
   int i;
   MincList *list;

   list = (MincList *) emalloc(sizeof(MincList));
   if (list == NULL)
      return NULL;
   list->data = (MincListElem *) emalloc(len * sizeof(MincListElem));
   if (list->data == NULL) {
      free(list);
      return NULL;
   }

   for (i = 0; i < len; i++) {
      list->data[i].val.number = array[i];
      list->data[i].type = MincFloatType;
   }

   return list;
}


