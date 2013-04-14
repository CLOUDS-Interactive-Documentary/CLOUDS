#ifndef _DYNAMICLIB_H_
#define _DYNAMICLIB_H_

#include <stdlib.h>
#include "load_utils.h"

class DynamicLib {
public:
	DynamicLib() : _handle(NULL) {}
	~DynamicLib() {}
	int load(const char *path) {
		return ((_handle = ::find_dso(path)) != NULL) ? 0 : -1;
	}
	bool loaded() const { return _handle != NULL; }
	int unload() { return loaded() ? ::unload_dso(_handle) : 0; }
	template <class Fun>
	int loadFunction(Fun **ppFun, const char *name) {
		return (*ppFun = (Fun *) ::find_symbol(_handle, name)) != NULL ? 0 : -1;
	}
	const char *error() { return ::get_dso_error(); }
private:
	void *_handle;
};

#endif	// _DYNAMICLIB_H_
