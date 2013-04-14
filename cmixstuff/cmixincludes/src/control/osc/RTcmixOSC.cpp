// RTcmix - Copyright (C) 2005  The RTcmix Development Team
// See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
// the license to this software and for a DISCLAIMER OF ALL WARRANTIES.

#include <Option.h>
#include <RTcmixOSC.h>
#include <RTOscPField.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

#define DEBUG 0

const int kPFieldBlockSize = 8;

RTcmixOSC::RTcmixOSC()
	: _serverThread(NULL), _pfieldBlockSize(kPFieldBlockSize), _numpfields(0),
	  _ready(false)
{
	_pfields = (RTOscPField **) calloc(kPFieldBlockSize, sizeof(void *));
}

RTcmixOSC::~RTcmixOSC()
{
	if (_serverThread)
		lo_server_thread_stop(_serverThread);
	free(_pfields);
}

RTcmixOSC *createOSCServer()
{
	RTcmixOSC *oscserver = new RTcmixOSC();
	if (oscserver) {
		if (oscserver->init() == -1) {
			delete oscserver;
			return NULL;
		}
	}

	return oscserver;
}

int RTcmixOSC::init()
{
	const int portnum = Option::oscInPort();
#if DEBUG > 0
	printf("Requested OSC input port: \"%d\"\n", portnum);
#endif

	char portstr[64];
	snprintf(portstr, 64, "%d", portnum);
	portstr[63] = 0;

	_serverThread = lo_server_thread_new(portstr, oscError);
	if (_serverThread == NULL) {
		fprintf(stderr, "Could not create OSC input port.\n");
		return -1;
	}

	lo_server_thread_start(_serverThread);

	_ready = true;

	return 0;
}

void RTcmixOSC::oscError(int num, const char *msg, const char *path)
{
	fprintf(stderr, "OSC server error %d in path \"%s\": %s\n", num, path, msg);
	fflush(stderr);
}

int RTcmixOSC::registerPField(
	RTOscPField *pfield,        // client PField to register
	lo_method_handler handler)  // function to call when receiving PField's path
{
	const char *path = pfield->path();

	lo_server_thread_add_method(_serverThread, path, NULL, handler, pfield);

	// Search PFields already registered.  For any having same path as this one,
	// set their callbacks to return 1, so that all callbacks, including the one
	// we register now, will be called on receipt of <path> by the OSC server.

	for (int i = 0; i < _numpfields; i++) {
		if (_pfields[i] != NULL && strcmp(path, _pfields[i]->path()) == 0)
			_pfields[i]->callbackReturn(1);
	}

	// Add new PField to our list.

	if (_numpfields == _pfieldBlockSize) {
		_pfieldBlockSize += kPFieldBlockSize;
		_pfields = (RTOscPField **) realloc(_pfields,
		                                    sizeof(void *) *_pfieldBlockSize);
	}
	_pfields[_numpfields] = pfield;
	_numpfields++;

	return 0;
}

int RTcmixOSC::unregisterPField(
	RTOscPField *pfield)        // client PField to register
{
	const char *path = pfield->path();

	// FIXME: There is not yet an API to do this for only one method of several
	// having the same path.  lo_server_thread_del_method() does it for all
	// handlers with the same path.  That's okay for now, because we'll only
	// get here when shutting down, but that might not always be true.

	lo_server_thread_del_method(_serverThread, path, NULL);

	// Remove PField from our list.
	for (int i = 0; i < _numpfields; i++) {
		if (strcmp(path, _pfields[i]->path()) == 0)
			_pfields[i]->callbackReturn(1);
	}

	// mark this so that registerPField will ignored it
	_pfields[_numpfields] = NULL;

	return 0;
}

