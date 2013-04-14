/* RTcmix  - Copyright (C) 2004  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/
#ifndef _UG_INTRO_H_ 
#define _UG_INTRO_H_ 1
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stddef.h>        /* for NULL */

void ug_intro(void);       /* called by RTcmix main and RTcmix.C */


#include "rtcmix_types.h"

typedef double (*LegacyFunction)(float *, int, double *);
typedef double (*NumberFunction)(const Arg[], int);
typedef char * (*StringFunction)(const Arg[], int);
typedef RtcHandle (*HandleFunction)(const Arg[], int);

void addfunc(const char *func_label,
			 LegacyFunction func_ptr_legacy,
			 NumberFunction func_ptr_number,
			 StringFunction func_ptr_string,
			 HandleFunction func_ptr_handle,
			 RTcmixType return_type,
			 int legacy);

#ifdef __cplusplus
} /* extern "C" */
#endif

// Macros for internal function registration.  Public macro is in ugens.h

#define UG_INTRO_DOUBLE_RETURN(flabel, func) \
   { \
      extern double func(const Arg[], int); \
      addfunc(flabel, NULL, func, NULL, NULL, DoubleType, 0); \
   }

#define UG_INTRO_STRING_RETURN(flabel, func) \
   { \
      extern char *func(const Arg[], int); \
      addfunc(flabel, NULL, NULL, func, NULL, StringType, 0); \
   }

#define UG_INTRO_HANDLE_RETURN(flabel, func) \
   { \
      extern RtcHandle func(const Arg[], int); \
      addfunc(flabel, NULL, NULL, NULL, func, HandleType, 0); \
   }

#endif /* _UG_INTRO_H_ */

