/* RTcmix  - Copyright (C) 2004  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "minc_internal.h"

#define BUFSIZE 1024

#ifndef vsnprintf
#define vsnprintf(str, sz, fmt, args)  vsprintf(str, fmt, args)
#endif

#ifdef MAXMSP
extern int mm_yylineno;
static int exit_on_die = 0;
#else
extern int yylineno;
static int exit_on_die = 1;
#endif

// BGG -- these in message.c.  Maybe just #include <ugens.h>?
extern void rtcmix_advise(const char *inst_name, const char *format, ...);
extern void rtcmix_warn(const char *inst_name, const char *format, ...);
extern void rterror(const char *inst_name, const char *format, ...);
extern int die(const char *inst_name, const char *format, ...);

void
sys_error(char *msg)
{
	die("parser", "%s\n", msg);

   if (exit_on_die)
      exit(EXIT_FAILURE);
}

int
configure_minc_error_handler(int exit)
{
   exit_on_die = exit;
   return 0;
}

void
minc_advise(const char *msg, ...)
{
   char buf[BUFSIZE];
   va_list args;

   va_start(args, msg);
   vsnprintf(buf, BUFSIZE, msg, args);
   va_end(args);

	rtcmix_advise("parser", buf);
}

void
minc_warn(const char *msg, ...)
{
   char buf[BUFSIZE];
   va_list args;

   va_start(args, msg);
   vsnprintf(buf, BUFSIZE, msg, args);
   va_end(args);

#ifdef MAXMSP
	rtcmix_warn("parser", "%s (near line %d)\n", buf, mm_yylineno);
#else
	rtcmix_warn("parser", "%s (near line %d)\n", buf, yylineno);
#endif
}

void
minc_die(const char *msg, ...)
{
   char buf[BUFSIZE];
   va_list args;

   va_start(args, msg);
   vsnprintf(buf, BUFSIZE, msg, args);
   va_end(args);

#ifdef MAXMSP
	rterror("parser", "%s (near line %d)\n", buf, mm_yylineno);
#else
	rterror("parser", "%s (near line %d)\n", buf, yylineno);
#endif

   if (exit_on_die)
      exit(EXIT_FAILURE);
}

void
minc_internal_error(const char *msg, ...)
{
   char buf[BUFSIZE];
   va_list args;

   va_start(args, msg);
   vsnprintf(buf, BUFSIZE, msg, args);
   va_end(args);

#ifdef MAXMSP
	rterror("parser-program", "%s (near line %d)\n", buf, mm_yylineno);
#else
	rterror("parser-program", "%s (near line %d)\n", buf, yylineno);
#endif

   if (exit_on_die)
      exit(EXIT_FAILURE);
}

#ifdef MAXMSP
void
mm_yyerror(char *msg)
{
	rterror("parser-yyerror", "near line %d: %s\n", mm_yylineno, msg);
}

#else
void
yyerror(char *msg)
{
	rterror("parser-yyerror", "near line %d: %s\n", yylineno, msg);
}
#endif // MAXMSP
