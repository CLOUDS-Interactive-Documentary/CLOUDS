/* RTcmix  - Copyright (C) 2004  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/

#ifndef _LOAD_UTILS_H_
#define _LOAD_UTILS_H_

#ifdef __cplusplus
extern "C" {
#endif

void *find_dso(const char *loadPath);

void *find_symbol(void *dso, const char *symbol);

int unload_dso(void *dso);

const char *get_dso_error();

#ifdef __cplusplus
}
#endif

#endif	// _LOAD_UTILS_H_
