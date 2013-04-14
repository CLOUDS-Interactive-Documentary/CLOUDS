/* This file contains prototypes for functions used by the RTcmix core,
   not by instruments or utility programs.   -JGG
*/
#ifndef _TABLEUTILS_H_ 
#define TABLEUTILS_H_ 1

#include "rtcmix_types.h"

class PField;

bool is_table(const PField *pf);
void get_table_bounds(const double *array, const int len, double &min, double &max);
void fill_linebrk_table(const Arg [], const int, double *, const int);
void fill_wave_table(const Arg [], const int, double *, const int);
int wavetable_from_string(const char *, double *, const int, const char *);

#endif /* _TABLEUTILS_H_ */
