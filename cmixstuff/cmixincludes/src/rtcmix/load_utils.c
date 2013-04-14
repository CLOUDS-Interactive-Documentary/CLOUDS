/* RTcmix  - Copyright (C) 2004  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/

/* Functions for dynamically loading shared objects
 * DS, 10/2004
*/

#include "load_utils.h"

#if (defined(MACOSX) && !defined(JAGUAR)) || defined(LINUX)

#include <dlfcn.h>

void *
find_dso(const char *loadPath)
{
	dlerror();	// clear error queue
	return dlopen(loadPath, RTLD_LAZY);
}

void *
find_symbol(void *dso, const char *symbol)
{
	dlerror();	// clear error queue
	return dlsym(dso, symbol);
}

int
unload_dso(void *dso)
{
	return dlclose(dso);
}

const char *get_dso_error()
{
	return dlerror();
}

#else

/* As of Mac OS X 10.1, the standard dlopen API is not supported officially
   (though there is a compatibility library floating around that works, from
   which this code was pilfered).  We roll our own loader here using the
   native OS X API for this reason, and also because it avoids a symbol
   collision with the Perl extension mechanism when using the dlopen
   compatibility library.   -JGG, 8/10/01
*/
#include <mach-o/dyld.h>
#include <unistd.h>

static NSObjectFileImageReturnCode s_result = NSObjectFileImageSuccess;

void *
find_dso(const char *loadPath)
{
    NSObjectFileImage objectFileImage;
    s_result = NSCreateObjectFileImageFromFile(loadPath, &objectFileImage);
    if (s_result == NSObjectFileImageSuccess) {
    	unsigned long options = NSLINKMODULE_OPTION_PRIVATE | NSLINKMODULE_OPTION_BINDNOW;
		NSModule module = NSLinkModule(objectFileImage, loadPath, options);
		NSDestroyObjectFileImage(objectFileImage);
		return (void *) module;
	}
	return NULL;
}

void *
find_symbol(void *dso, const char *symbolName)
{
	NSModule module = (NSModule) dso;
	char fullSymbolName[128];
	snprintf(fullSymbolName, sizeof(fullSymbolName), "_%s", symbolName);
    NSSymbol symbol = NSLookupSymbolInModule(module, fullSymbolName);
    if (symbol != NULL) {
		return (void *) NSAddressOfSymbol(symbol);
	}
	return NULL;
}

int
unload_dso(void *dso)
{
	NSModule module = (NSModule) dso;
	return NSUnLinkModule(module, NSUNLINKMODULE_OPTION_NONE) == TRUE ? 0 : -1;
}

const char *get_dso_error()
{
	const char *err = "Unknown error";
	switch (s_result) {
	case NSObjectFileImageFailure:
		err = "";	// error printed by API
		break;
	case NSObjectFileImageInappropriateFile:
		err = "Wrong type of object";
		break;
	case NSObjectFileImageArch:
		err = "Could not find correct CPU architecture in file";
		break;
	case NSObjectFileImageFormat:
		err = "Malformed Mach-O format";
		break;
	case NSObjectFileImageAccess:
		err = "File not accessible or not found";
		break;
	default:
		break;
	}
	return err;
}

#endif
