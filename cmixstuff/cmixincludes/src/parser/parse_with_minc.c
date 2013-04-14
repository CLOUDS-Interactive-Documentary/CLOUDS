/* RTcmix  - Copyright (C) 2000  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/
#include <stdio.h>
#include <stdlib.h>
#include "rtcmix_parse.h"
#include <Option.h>


#ifdef MAXMSP
	extern int mm_yyparse();

	/* <mm_yyin> is yacc's input file. If left alone, stdin will be used. */
	// BGG mm -- I don't think we use this at all in max/msp, should change
	extern FILE *mm_yyin;
#else
	extern int yyparse();

	/* <yyin> is yacc's input file. If left alone, stdin will be used. */
	extern FILE *yyin;
#endif


/* Defined in sys/command_line.c */
extern char *aargv[];
extern int aargc;


/* ---------------------------------------------------------- parse_score --- */
#ifdef MAXMSP
// BGG mm -- set this to accept a buffer from max/msp
int
parse_score(char *thebuf, int buflen)
#else
int
parse_score(int argc, char *argv[], char **env)
#endif
{
   int   i, status;

#ifndef MAXMSP
   /* Copy command-line args to make them available to the Minc-only
      functions in sys/command_line.c: f_arg, i_arg, s_arg, and n_arg.
   */
   for (i = 1; i < argc; i++)
      aargv[i - 1] = argv[i];
   aargc = argc - 1;

   status = yyparse();
#else
   status = mm_yyparse(thebuf, buflen+1);
#endif

   return status;
}


/* ------------------------------------------------------ use_script_file --- */
/* Parse file <fname> instead of stdin. */
void
use_script_file(char *fname)
{
#ifndef MAXMSP
	// BGG mm -- we don't use this in Max/MSP, and there is no yy_in var
   yyin = fopen(fname, "r+");
   if (yyin == NULL) {
      fprintf(stderr, "Can't open %s\n", fname);
      exit(1);
   }
#endif
   if (get_bool_option(kOptionPrint))
      printf("Using file %s\n", fname);
}


/* ------------------------------------------------------- destroy_parser --- */
void
destroy_parser()
{
   /* nothing to do for Minc */
}

