#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "globals.h"
#include <ugens.h>
#include <Option.h>

#include "DynamicLib.h"

typedef void (*ProfileFun)();
extern "C" double m_load(float *, int, double *);

/* Assemble path to the shared library, and pass back as <dsoPath>. */
static int
get_dso_path(double pfield, char dsoPath[])
{
    char *str;
#ifdef SHAREDLIBDIR
    const char *directory = SHAREDLIBDIR;
#else
    const char *directory = "/musr/lib";
#endif

    /* cast double to string pointer */
    str = DOUBLE_TO_STRING(pfield);

    if (!str || strlen(str) == 0) {
		die("load", "Bad argument for p[0]!");
		return -1;
    }
    /* if name contains a '/', assume it is a full or relative path */
    if (strchr(str, '/'))
		strcpy(dsoPath, str);
    /* if name does not start with "lib", add prefix and suffix */
    else if (strncmp(str, "lib", 3))
		sprintf(dsoPath, "%s/lib%s.so", directory, str);
    /* otherwise just prepend directory and use as is */
    else
		sprintf(dsoPath, "%s/%s", directory, str);

    return 0;
}

double m_load(float *p, int n_args, double *pp)
{
    char dsoPath[1024];
    int profileLoaded;
    DynamicLib theDSO;
    ProfileFun profileFun = NULL;

#ifdef MAXMSP
// BGG mm -- dynamic loading not working for scorefile in max/msp yet
// Use the "loadinst" message
	return 1.0;
#endif

    if (get_dso_path(pp[0], dsoPath) != 0)
		return 0;

    if (theDSO.load(dsoPath) != 0) {
#ifdef linux
		rtcmix_warn("load", theDSO.error());
#else
		rtcmix_warn("load", "Unable to dynamically load '%s': %s",
			 dsoPath, theDSO.error());
#endif
		return 0;
    }

    /* load & call the shared library's profile function to load its symbols */

    profileLoaded = 0;

    if (theDSO.loadFunction(&profileFun, "profile") == 0) {
      profileLoaded++;
      (*profileFun)();
#ifdef DBUG
      printf("Loaded standard profile\n");
#endif
    }

    /* if present, load & call the shared library's rtprofile function to 
     * load its symbols.  Note that we access the function via its 
     * unmangled symbol name due to its extern "C" decl in rt.h.
     */

	if (theDSO.loadFunction(&profileFun, "rtprofile") == 0) {
      profileLoaded += 2; 
      (*profileFun)(); 
#ifdef DBUG
      printf("Loaded RT profile\n"); 
#endif
     } 

    if (!profileLoaded) {
		rtcmix_warn("load", "Unable to find a profile routine in DSO '%s'", dsoPath);
		theDSO.unload();
		return 0;
    }

    if (get_print_option()) {
		printf("Loaded %s functions from shared library:\n\t'%s'.\n",
			  (profileLoaded == 3) ? "standard and RT" :
							   (profileLoaded == 2) ? "RT" : "standard",
			  dsoPath);
    }

    return 1;
}

