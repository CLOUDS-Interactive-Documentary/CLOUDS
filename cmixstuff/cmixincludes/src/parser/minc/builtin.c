/* RTcmix  - Copyright (C) 2004  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/
#include "minc_internal.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <Option.h>
#include <MMPrint.h>

/* Minc builtin functions, for use only in Minc scripts.
   To add a builtin function, make an entry for it in the function ptr array
   below, make a prototype for it, and define the function in this file.
   Follow the model of the existing builtins at the bottom of the file.

   John Gibson, 1/24/2004
*/

/* builtin function prototypes */
static MincFloat _minc_print(const MincListElem args[], const int nargs);
static MincFloat _minc_printf(const MincListElem args[], const int nargs);
static MincFloat _minc_len(const MincListElem args[], const int nargs);
static MincFloat _minc_index(const MincListElem args[], const int nargs);
static MincString _minc_type(const MincListElem args[], const int nargs);

/* other prototypes */
static int _find_builtin(const char *funcname);
static void _do_print(const MincListElem args[], const int nargs);
static MincString _make_type_string(const MincDataType type);


/* list of builtin functions, searched by _find_builtin */
static struct _builtins {
   char *label;
   MincFloat (*number_return)(); /* func name for those returning MincFloat */
   MincString (*string_return)();   /* func name for those returning char * */
} builtin_funcs[] = {
   { "print",     _minc_print,   NULL },
   { "printf",    _minc_printf,  NULL },
   { "len",       _minc_len,     NULL },
   { "index",     _minc_index,   NULL },
   { "type",      NULL,          _minc_type },
   { NULL,        NULL,          NULL }         /* marks end of list */
};


/* -------------------------------------- call_builtin_function and helper -- */
static int
_find_builtin(const char *funcname)
{
   int i = 0;
   while (1) {
      if (builtin_funcs[i].label == NULL)
         break;
      if (strcmp(builtin_funcs[i].label, funcname) == 0)
         return i;
      i++;
   }
   return -1;
}

int
call_builtin_function(const char *funcname, const MincListElem arglist[],
   const int nargs, MincListElem *retval)
{
   int index = _find_builtin(funcname);
   if (index < 0)
      return -1;
   if (builtin_funcs[index].number_return) {
      retval->val.number = (MincFloat) (*(builtin_funcs[index].number_return))
                                                         (arglist, nargs);
      retval->type = MincFloatType;
   }
   else if (builtin_funcs[index].string_return) {
      retval->val.string = (MincString) (*(builtin_funcs[index].string_return))
                                                         (arglist, nargs);
      retval->type = MincStringType;
   }
   return 0;
}


/* ============================================= print, printf and friends == */

/* ----------------------------------------------------- _make_type_string -- */
static MincString
_make_type_string(const MincDataType type)
{
   char *str = NULL;

   switch (type) {
      case MincVoidType:
         str = strdup("void");
         break;
      case MincFloatType:
         str = strdup("float");
         break;
      case MincStringType:
         str = strdup("string");
         break;
      case MincHandleType:
         str = strdup("handle");
         break;
      case MincListType:
         str = strdup("list");
         break;
   }
   return (MincString) str;
}


/* ------------------------------------------------------------- _do_print -- */
static void
_do_print(const MincListElem args[], const int nargs)
{
   int i, last_arg;
   int nchars;

   last_arg = nargs - 1;
   for (i = 0; i < nargs; i++) {
      switch (args[i].type) {
#ifdef MAXMSP
         case MincFloatType:
            if (i == last_arg)
               nchars = sprintf(get_mm_print_ptr(), "%.12g", args[i].val.number);
            else
               nchars = sprintf(get_mm_print_ptr(), "%.12g, ", args[i].val.number);
            break;
         case MincStringType:
            if (i == last_arg)
               nchars = sprintf(get_mm_print_ptr(), "\"%s\"", args[i].val.string);
            else
               nchars = sprintf(get_mm_print_ptr(), "\"%s\", ", args[i].val.string);
            break;
         case MincHandleType:
            if (i == last_arg)
               nchars = sprintf(get_mm_print_ptr(), "RtcHandle:%p", args[i].val.handle);
            else
               nchars = sprintf(get_mm_print_ptr(), "RtcHandle:%p, ", args[i].val.handle);
            break;
         case MincListType:
            nchars = sprintf(get_mm_print_ptr(), "%s", "[");
            set_mm_print_ptr(nchars);
            _do_print(args[i].val.list->data, args[i].val.list->len);
            if (i == last_arg)
               nchars = sprintf(get_mm_print_ptr(), "%s", "]");
            else
               nchars = sprintf(get_mm_print_ptr(), "%s, ", "]");
            break;
         default:
            break;
      }
      set_mm_print_ptr(nchars);
#else
         case MincFloatType:
            if (i == last_arg)
               printf("%.12g", args[i].val.number);
            else
               printf("%.12g, ", args[i].val.number);
            break;
         case MincStringType:
            if (i == last_arg)
               printf("\"%s\"", args[i].val.string);
            else
               printf("\"%s\", ", args[i].val.string);
            break;
         case MincHandleType:
            if (i == last_arg)
               printf("RtcHandle:%p", args[i].val.handle);
            else
               printf("RtcHandle:%p, ", args[i].val.handle);
            break;
         case MincListType:
            putchar('[');
            _do_print(args[i].val.list->data, args[i].val.list->len);
            if (i == last_arg)
               putchar(']');
            else
               printf("], ");
            break;
         default:
            break;
      }
#endif // MAXMSP
   }
}


/* ----------------------------------------------------------------- print -- */
MincFloat
_minc_print(const MincListElem args[], const int nargs)
{
   if (get_print_option() < 1) return 0.0;

   _do_print(args, nargs);
#ifdef MAXMSP
   int nchars = sprintf(get_mm_print_ptr(), "\n");
   set_mm_print_ptr(nchars+1);
#else
   putchar('\n');
   fflush(stdout);
#endif
   return 0.0;
}


/* ---------------------------------------------------------------- printf -- */
/* A primitive version of printf, supporting some Minc-specific things.
   Conversion specifiers are:

      d      print float as integer
      f      print float
      l      print list
      s      print string
      t      print type of object
      z      print using the style appropriate for the type

   Escapes are \n for newline, \t for tab.  You must put the newline in
   if you want one.

   Example:

      a = 1.2345
      b = { -2, -1, 0, 1, 2, 99.99 }
      c = "boo!"
      printf("a=%d, a=%f, b=%l, c=%s, type of c: %t\n", a, a, b, c, c)

   This will print...

      a=1, a=1.2345, b=[-2, -1, 0, 1, 2, 99.99], c=boo!, type of c: string
*/

#ifdef MAXMSP
MincFloat
_minc_printf(const MincListElem args[], const int nargs)
{
   int n;
   const char *p;
	int nchars;

	if (get_print_option() < 1) return 0.0;

   if (args[0].type != MincStringType) {
      minc_warn("printf: first argument must be format string");
      goto err;
   }

   n = 1;
   p = args[0].val.string;
   while (*p) {
      switch (*p) {
         case '%':
            p++;
            if (n >= nargs) {
               minc_warn("printf: not enough arguments for format string");
               goto err;
            }
            switch (*p) {
               case 'd':      /* print float object as integer */
                  if (args[n].type != MincFloatType) {
                     minc_warn("printf: wrong argument type for format");
                     goto err;
                  }
                  nchars = sprintf(get_mm_print_ptr(), "%d", (int) args[n].val.number);
                  break;
               case 'f':      /* print float object */
                  if (args[n].type != MincFloatType) {
                     minc_warn("printf: wrong argument type for format");
                     goto err;
                  }
                  nchars = sprintf(get_mm_print_ptr(), "%.12g", args[n].val.number);
                  break;
               case 'l':      /* print list object */
                  if (args[n].type != MincListType) {
                     minc_warn("printf: wrong argument type for format");
                     goto err;
                  }
						nchars = sprintf(get_mm_print_ptr(), "%s", "[");
						set_mm_print_ptr(nchars);
                  _do_print(args[n].val.list->data, args[n].val.list->len);
						nchars = sprintf(get_mm_print_ptr(), "%s", "]");
						set_mm_print_ptr(nchars);
                  break;
               case 's':      /* print string object */
                  if (args[n].type != MincStringType) {
                     minc_warn("printf: wrong argument type for format");
                     goto err;
                  }
                  nchars = sprintf(get_mm_print_ptr(), "%s", args[n].val.string);
                  break;
               case 't':      /* print type of object */
                  {
                     char *tstr = (char *) _make_type_string(args[n].type);
	                  nchars = sprintf(get_mm_print_ptr(), "%s", tstr);
                     free(tstr);
                  }
                  break;
               case 'z':      /* print as appropriate for type */
                  _do_print(&args[n], 1);
                  break;
               case '\0':
                  minc_warn("printf: premature end of format string");
                  goto err;
                  break;
               default:
                  minc_warn("printf: invalid format specifier");
                  goto err;
                  break;
            }
            n++;
            p++;
            break;
         case '\\':
            p++;
            switch (*p) {
               case 'n':
						nchars = sprintf(get_mm_print_ptr(), "\n");
                  break;
               case 't':
						nchars = sprintf(get_mm_print_ptr(), "\t");
                  break;
//FIXME: currently, minc.l can't handle escaped quotes in strings
               case '\'':
						nchars = sprintf(get_mm_print_ptr(), "\'");
                  break;
               case '"':
						nchars = sprintf(get_mm_print_ptr(), "\"");
                  break;
               case '\0':
                  minc_warn("printf: premature end of format string");
                  goto err;
                  break;
               default:
                  minc_warn("printf: invalid escape character");
                  goto err;
                  break;
            }
            p++;
            break;
         default:
				nchars = sprintf(get_mm_print_ptr(), "%.1s", p);
            p++;
            break;
      }
		set_mm_print_ptr(nchars);
   }
	set_mm_print_ptr(1);
   return 0.0;
err:
	nchars = sprintf(get_mm_print_ptr(), "\n");
	set_mm_print_ptr(nchars+1);
   fflush(stdout);
   return -1.0;
}

#else
MincFloat
_minc_printf(const MincListElem args[], const int nargs)
{
   int n;
   const char *p;

   if (get_print_option() < 1) return 0.0;

   if (args[0].type != MincStringType) {
      minc_warn("printf: first argument must be format string");
      goto err;
   }

   n = 1;
   p = args[0].val.string;
   while (*p) {
      switch (*p) {
         case '%':
            p++;
            if (n >= nargs) {
               minc_warn("printf: not enough arguments for format string");
               goto err;
            }
            switch (*p) {
               case 'd':      /* print float object as integer */
                  if (args[n].type != MincFloatType) {
                     minc_warn("printf: wrong argument type for format");
                     goto err;
                  }
                  printf("%d", (int) args[n].val.number);
                  break;
               case 'f':      /* print float object */
                  if (args[n].type != MincFloatType) {
                     minc_warn("printf: wrong argument type for format");
                     goto err;
                  }
                  printf("%.12g", args[n].val.number);
                  break;
               case 'l':      /* print list object */
                  if (args[n].type != MincListType) {
                     minc_warn("printf: wrong argument type for format");
                     goto err;
                  }
                  putchar('[');
                  _do_print(args[n].val.list->data, args[n].val.list->len);
                  putchar(']');
                  break;
               case 's':      /* print string object */
                  if (args[n].type != MincStringType) {
                     minc_warn("printf: wrong argument type for format");
                     goto err;
                  }
                  fputs(args[n].val.string, stdout);
                  break;
               case 't':      /* print type of object */
                  {
                     char *tstr = (char *) _make_type_string(args[n].type);
                     fputs(tstr, stdout);
                     free(tstr);
                  }
                  break;
               case 'z':      /* print as appropriate for type */
                  _do_print(&args[n], 1);
                  break;
               case '\0':
                  minc_warn("printf: premature end of format string");
                  goto err;
                  break;
               default:
                  minc_warn("printf: invalid format specifier");
                  goto err;
                  break;
            }
            n++;
            p++;
            break;
         case '\\':
            p++;
            switch (*p) {
               case 'n':
                  putchar('\n');
                  break;
               case 't':
                  putchar('\t');
                  break;
//FIXME: currently, minc.l can't handle escaped quotes in strings
               case '\'':
                  putchar('\'');
                  break;
               case '"':
                  putchar('"');
                  break;
               case '\0':
                  minc_warn("printf: premature end of format string");
                  goto err;
                  break;
               default:
                  minc_warn("printf: invalid escape character");
                  goto err;
                  break;
            }
            p++;
            break;
         default:
            putchar(*p);
            p++;
            break;
      }
   }
   return 0.0;
err:
   putchar('\n');
   fflush(stdout);
   return -1.0;
}
#endif // MAXMSP


/* ------------------------------------------------------------------- len -- */
/* Print the length of the argument.  This is useful for getting the number
   of items in a list or the number of characters in a string.
*/
MincFloat
_minc_len(const MincListElem args[], const int nargs)
{
   int len = 0;

   if (nargs != 1)
      minc_warn("len: must have one argument");
   else {
      switch (args[0].type) {
         case MincFloatType:
            len = 1;
            break;
         case MincStringType:
            len = strlen(args[0].val.string);
            break;
         case MincHandleType:
            /* NB: To get length of a table, call tablelen(handle) */
            len = 1;
            break;
         case MincListType:
            len = args[0].val.list->len;
            break;
         default:
            minc_warn("len: invalid argument");
            break;
      }
   }
   return (MincFloat) len;
}


/* ----------------------------------------------------------------- index -- */
/* Given an item (float, string or handle), return the index of the item within
   the given list, or -1 if the item is not in the list.  Example:

      list = {1, 2, "three", 4}
      id = index(list, 2)

   <id> equals 1 after this call.
*/
MincFloat
_minc_index(const MincListElem args[], const int nargs)
{
   int i, len, index = -1;
   MincDataType argtype;
   MincListElem *data;

   if (nargs != 2) {
      minc_warn("index: must have two arguments (list, item_to_find)");
      return -1.0;
   }
   if (args[0].type != MincListType) {
      minc_warn("index: first argument must be a list");
      return -1.0;
   }
   argtype = args[1].type;
   assert(argtype == MincFloatType || argtype == MincStringType
            || argtype == MincHandleType || argtype == MincListType);

   len = args[0].val.list->len;
   data = args[0].val.list->data;

   for (i = 0; i < len; i++) {
      if (data[i].type == argtype) {
         if (argtype == MincFloatType) {
            if (data[i].val.number == args[1].val.number) {
               index = i;
               break;
            }
         }
         else if (argtype == MincStringType) {
            if (strcmp(data[i].val.string, args[1].val.string) == 0) {
               index = i;
               break;
            }
         }
//FIXME: should this recurse and match entire list contents??
         else if (argtype == MincListType) {
            if (data[i].val.list == args[1].val.list) {
               index = i;
               break;
            }
         }
         else if (argtype == MincHandleType) {
            if (data[i].val.handle == args[1].val.handle) {
               index = i;
               break;
            }
         }
      }
   }

   return (MincFloat) index;
}


/* ------------------------------------------------------------------ type -- */
/* Print the object type of the argument: float, string, handle, list.
*/
MincString
_minc_type(const MincListElem args[], const int nargs)
{
   if (nargs != 1) {
      minc_warn("type: must have one argument");
      return NULL;
   }
   return _make_type_string(args[0].type);
}


