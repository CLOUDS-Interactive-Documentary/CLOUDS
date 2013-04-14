/* RTcmix  - Copyright (C) 2000  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/

/* Functions for managing embedded Perl parser.    -JGG, 30-Jul-00 */

#include <assert.h>              /* put this before perl.h */
#include <EXTERN.h>
#include <perl.h>
#include <XSUB.h>
#include "rtcmix_parse.h"

extern void boot_DynaLoader (CV* cv);

static PerlInterpreter *perl_interp = NULL;
static PerlInterpreter *my_perl = NULL;
static SV *perlBuf;
static char *extra_lib_dir = "-I"SHAREDLIBDIR;


/* -------------------------------------------------------------- xs_init --- */
static void
xs_init()
{
   char *file = __FILE__;

   newXS("DynaLoader::boot_DynaLoader", boot_DynaLoader, file);

}

/* ---- special pearl parser ----- */
int perl_parse_buf (char *inBuf) {
	
	STRLEN n_a;
	char *embedding[] = { "", "-e", "" };

	
	if (!my_perl) {
		my_perl = perl_alloc();
		perl_construct( my_perl );
		perl_parse(my_perl, xs_init, 3, embedding, NULL);
		/* PL_exit_flags |= PERL_EXIT_DESTRUCT_END; */
		perl_run(my_perl);
	}

/* 	sv_setpv(text,inBuf); */
/* 	eval_sv(text,G_SCALAR); */

	perlBuf = eval_pv(inBuf, TRUE);
	
	if (0) {
		perl_destruct(my_perl);
		perl_free(my_perl);
	}

	return 0;
	
}

double get_perl_fval (char *val) {
	double retval;
	retval = SvNV(get_sv(val, FALSE));
	return retval;
}

void set_perl_var(char *string, double value) {
	sv_setpvf(perlBuf, "%f", value);
}	

double get_perl_var(char *string) {
	return SvNV(get_sv(string, FALSE));
}

/* ---------------------------------------------------------- parse_score --- */
int
parse_score(int argc, char *argv[], char **env)
{
   int   i, status, xargc;
   char  *xargv[MAXARGS + 2];

   assert(argc <= MAXARGS);

   /* Insert in arg list the lib dir containing our Perl extension. */
   xargv[0] = argv[0];
   xargv[1] = extra_lib_dir;
   for (i = 1; i < argc; i++)
      xargv[i + 1] = argv[i];
   xargv[i + 1] = NULL;
   xargc = argc + 1;
   PERL_SYS_INIT3(&xargc,&xargv,&env);
   perl_interp = perl_alloc();
   if (perl_interp) {
      perl_construct(perl_interp);
      status = perl_parse(perl_interp, xs_init, xargc, xargv, env);
      if (status == 0)
         perl_run(perl_interp);
      else {
          fprintf(stderr, "Error: perl_parse returned status %d\n", status);
      }
   }
   else {
      fprintf(stderr, "Can't create Perl interpreter.\n");
      status = -1;
   }

   return status;
}


/* ------------------------------------------------------ use_script_file --- */
/* Not supported (and not needed) for Perl. */
void
use_script_file(char *fname)
{
   fprintf(stderr, "You don't need the -f option for Perl. Try \"PCMIX %s\".\n",
           fname);
   exit(1);
}


/* ------------------------------------------------------- destroy_parser --- */
void
destroy_parser()
{
   perl_destruct(perl_interp);
   perl_free(perl_interp);
   PERL_SYS_TERM();
}

