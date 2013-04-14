/* This is set up so that all vars are defined when this file is included
   by main.C (which defines the MAIN preprocessor symbol), and declared
   extern when included by all other files.
                                                              -JGG, 2/8/00
*/
#ifndef _GLOBALS_H_ 
#define _GLOBALS_H_ 1

#include "version.h"
#include <bus.h>
#include <stdio.h>

#ifdef __cplusplus
#include <RTcmix.h>
#endif

#define MAX_INFILE_DESC 50

// contains file descriptors for data files opened with infile
// and used by gen2 and gen3

extern FILE *	infile_desc[MAX_INFILE_DESC + 1];

#endif /* _GLOBALS_H_ */
