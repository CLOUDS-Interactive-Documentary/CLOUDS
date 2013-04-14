/* RTcmix  - Copyright (C) 2004  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/
#ifndef _MINC_H_
#define _MINC_H_ 1

/* Anything here is availale for export outside of the Minc directory. */

#ifdef MAXMSP
int mm_yyparse(char *buf, int len);
#else
int yyparse(void);
#endif

int configure_minc_error_handler(int exit);

#endif /* _MINC_H_ */
