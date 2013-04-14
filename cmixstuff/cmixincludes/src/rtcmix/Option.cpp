/* RTcmix - Copyright (C) 2004  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/
/* Class for storing all our run-time options,
   by John Gibson and Doug Scott, 6/27/04.
*/

#include <assert.h>
#include <limits.h>        // PATH_MAX
#include "Option.h"
#include <Config.h>
#include <string.h>
#include "MMPrint.h"
#ifndef MAXMSP
#include <iostream>
#endif

using namespace std;

#define DSOPATH_MAX  PATH_MAX * 2
#define OSCHOST_MAX  128

bool Option::_audio = true;
bool Option::_play = true;
bool Option::_record = false;
bool Option::_clobber = false;
bool Option::_reportClipping = true;
bool Option::_checkPeaks = true;
bool Option::_exitOnError = false;	// we override this in main.cpp
bool Option::_autoLoad = false;
bool Option::_fastUpdate = false;

double Option::_bufferFrames = DEFAULT_BUFFER_FRAMES;
int Option::_bufferCount = DEFAULT_BUFFER_COUNT;
int Option::_oscInPort = DEFAULT_OSC_INPORT;

// BGG see MMPrint.h for levels
#ifdef MAXMSP
int Option::_print = MMP_RTERRORS; // basic level for max/msp
#else
int Option::_print = MMP_PRINTALL; // default print everthing for regular RTcmix
#endif


char Option::_device[DEVICE_MAX];
char Option::_inDevice[DEVICE_MAX];
char Option::_outDevice[MAX_OUTPUT_DEVICES][DEVICE_MAX];
char Option::_midiInDevice[DEVICE_MAX];
char Option::_midiOutDevice[DEVICE_MAX];
char Option::_oscHost[OSCHOST_MAX];
char Option::_dsoPath[DSOPATH_MAX];
char Option::_homeDir[PATH_MAX];
char Option::_rcName[PATH_MAX];


void Option::init()
{
	_device[0] = 0;
	_inDevice[0] = 0;
	_outDevice[0][0] = 0;
	_outDevice[1][0] = 0;
	_outDevice[2][0] = 0;
	_midiInDevice[0] = 0;
	_midiOutDevice[0] = 0;
	strcpy(_oscHost, "localhost");
	_dsoPath[0] = 0;
	_homeDir[0] = 0;
	_rcName[0] = 0;

	// initialize home directory and full path of user's configuration file

	char *dir = getenv("HOME");
	if (dir == NULL)
		return;
	if (strlen(dir) > 256)     // legit HOME not likely to be longer
		return;
	strncpy(_homeDir, dir, PATH_MAX);
	_homeDir[PATH_MAX - 1] = 0;

	char *rc = new char[strlen(dir) + 1 + strlen(CONF_FILENAME) + 1];
	strcpy(rc, dir);
	strcat(rc, "/");
	strcat(rc, CONF_FILENAME);
	strncpy(_rcName, rc, PATH_MAX);
	_rcName[PATH_MAX - 1] = 0;
	delete [] rc;
}

/* Read configuration file <fileName>, which is probably the full path name
   of the user's .rtcmixrc file, and copy the settings in the file to the
   Option object.  Return 0 if successful, or if the file doesn't exist.
   Return -1 if there is a problem reading the file (such as insufficient
   privileges).
*/
int Option::readConfigFile(const char *fileName)
{
	const char *key;
	Config conf;

	assert(fileName != NULL && fileName[0] != 0);
	ConfigErrorCode result = conf.parseFile(fileName);
	if (result == kConfigFileMissingErr)
		return -1;			// user doesn't have an rc file; fail silently
	if (result != kConfigNoErr) {
		reportError("%s \"%s\"", conf.getLastErrorText(), fileName);
		return -1;
	}

	// bool options ...........................................................

	bool bval;

	key = kOptionAudio;
	result = conf.getValue(key, bval);
	if (result == kConfigNoErr)
		audio(bval);
	else if (result != kConfigNoValueForKey)
		reportError("%s: %s.", conf.getLastErrorText(), key);

	key = kOptionPlay;
	result = conf.getValue(key, bval);
	if (result == kConfigNoErr)
		play(bval);
	else if (result != kConfigNoValueForKey)
		reportError("%s: %s.", conf.getLastErrorText(), key);

	key = kOptionRecord;
	result = conf.getValue(key, bval);
	if (result == kConfigNoErr)
		record(bval);
	else if (result != kConfigNoValueForKey)
		reportError("%s: %s.", conf.getLastErrorText(), key);

	key = kOptionClobber;
	result = conf.getValue(key, bval);
	if (result == kConfigNoErr)
		clobber(bval);
	else if (result != kConfigNoValueForKey)
		reportError("%s: %s.", conf.getLastErrorText(), key);

	key = kOptionPrint;
	result = conf.getValue(key, bval);
	if (result == kConfigNoErr)
		print(bval);
	else if (result != kConfigNoValueForKey)
		reportError("%s: %s.", conf.getLastErrorText(), key);

	key = kOptionReportClipping;
	result = conf.getValue(key, bval);
	if (result == kConfigNoErr)
		reportClipping(bval);
	else if (result != kConfigNoValueForKey)
		reportError("%s: %s.", conf.getLastErrorText(), key);

	key = kOptionCheckPeaks;
	result = conf.getValue(key, bval);
	if (result == kConfigNoErr)
		checkPeaks(bval);
	else if (result != kConfigNoValueForKey)
		reportError("%s: %s.", conf.getLastErrorText(), key);

	key = kOptionExitOnError;
	result = conf.getValue(key, bval);
	if (result == kConfigNoErr)
		exitOnError(bval);
	else if (result != kConfigNoValueForKey)
		reportError("%s: %s.", conf.getLastErrorText(), key);

	key = kOptionAutoLoad;
	result = conf.getValue(key, bval);
	if (result == kConfigNoErr)
		autoLoad(bval);
	else if (result != kConfigNoValueForKey)
		reportError("%s: %s.", conf.getLastErrorText(), key);

	key = kOptionFastUpdate;
	result = conf.getValue(key, bval);
	if (result == kConfigNoErr)
		fastUpdate(bval);
	else if (result != kConfigNoValueForKey)
		reportError("%s: %s.", conf.getLastErrorText(), key);

	// number options .........................................................

	double dval;

	key = kOptionBufferFrames;
	result = conf.getValue(key, dval);
	if (result == kConfigNoErr)
		bufferFrames(dval);
	else if (result != kConfigNoValueForKey)
		reportError("%s: %s.", conf.getLastErrorText(), key);

	key = kOptionBufferCount;
	result = conf.getValue(key, dval);
	if (result == kConfigNoErr)
		bufferCount((int)dval);
	else if (result != kConfigNoValueForKey)
		reportError("%s: %s.", conf.getLastErrorText(), key);

	key = kOptionOSCInPort;
	result = conf.getValue(key, dval);
	if (result == kConfigNoErr)
		oscInPort((int)dval);
	else if (result != kConfigNoValueForKey)
		reportError("%s: %s.", conf.getLastErrorText(), key);

	// string options .........................................................

	char *sval;

	key = kOptionDevice;
	result = conf.getValue(key, sval);
	if (result == kConfigNoErr)
		device(sval);
	else if (result != kConfigNoValueForKey)
		reportError("%s: %s.", conf.getLastErrorText(), key);

	key = kOptionInDevice;
	result = conf.getValue(key, sval);
	if (result == kConfigNoErr)
		inDevice(sval);
	else if (result != kConfigNoValueForKey)
		reportError("%s: %s.", conf.getLastErrorText(), key);

	key = kOptionOutDevice;
	result = conf.getValue(key, sval);
	if (result == kConfigNoErr)
		outDevice(sval);
	else if (result != kConfigNoValueForKey)
		reportError("%s: %s.", conf.getLastErrorText(), key);

	key = kOptionOutDevice2;
	result = conf.getValue(key, sval);
	if (result == kConfigNoErr)
		outDevice(sval, 1);
	else if (result != kConfigNoValueForKey)
		reportError("%s: %s.", conf.getLastErrorText(), key);

	key = kOptionOutDevice3;
	result = conf.getValue(key, sval);
	if (result == kConfigNoErr)
		outDevice(sval, 2);
	else if (result != kConfigNoValueForKey)
		reportError("%s: %s.", conf.getLastErrorText(), key);

	key = kOptionMidiInDevice;
	result = conf.getValue(key, sval);
	if (result == kConfigNoErr)
		midiInDevice(sval);
	else if (result != kConfigNoValueForKey)
		reportError("%s: %s.", conf.getLastErrorText(), key);

	key = kOptionMidiOutDevice;
	result = conf.getValue(key, sval);
	if (result == kConfigNoErr)
		midiOutDevice(sval);
	else if (result != kConfigNoValueForKey)
		reportError("%s: %s.", conf.getLastErrorText(), key);

	key = kOptionOSCHost;
	result = conf.getValue(key, sval);
	if (result == kConfigNoErr)
		oscHost(sval);
	else if (result != kConfigNoValueForKey)
		reportError("%s: %s.", conf.getLastErrorText(), key);

	key = kOptionDSOPath;
	result = conf.getValue(key, sval);
	if (result == kConfigNoErr)
		dsoPath(sval);
	else if (result != kConfigNoValueForKey)
		reportError("%s: %s.", conf.getLastErrorText(), key);

	return 0;
}

/* This is designed to be called from a utility program that writes a new
   configuration file.
*/
#include <stdio.h>
#include <errno.h>

int Option::writeConfigFile(const char *fileName)
{
	if (fileName == NULL || fileName[0] == 0) {
		fprintf(stderr, "Config file name is NULL or empty.\n");
		return -1;
	}

	FILE *stream = fopen(fileName, "r");
	if (stream != NULL || errno != ENOENT) {
		fprintf(stderr, "Config file \"%s\" already exists. Move it out of "
		                "the way.\n", fileName);
		return -1;
	}

	stream = fopen(fileName, "w");
	if (stream == NULL) {
		fprintf(stderr, "Can't open \"%s\" for writing.\n", fileName);
		return -1;
	}

	fprintf(stream, "# Default configuration file\n");

	// write bool options
	fprintf(stream, "\n# Boolean options: key = [true | false]\n");
	fprintf(stream, "%s = %s\n", kOptionAudio, audio() ? "true" : "false");
	fprintf(stream, "%s = %s\n", kOptionPlay, play() ? "true" : "false");
	fprintf(stream, "%s = %s\n", kOptionRecord, record() ? "true" : "false");
	fprintf(stream, "%s = %s\n", kOptionClobber, clobber() ? "true" : "false");
	fprintf(stream, "%s = %s\n", kOptionReportClipping,
										reportClipping() ? "true" : "false");
	fprintf(stream, "%s = %s\n", kOptionCheckPeaks,
										checkPeaks() ? "true" : "false");
	// intentionally leaving out exitOnError
	fprintf(stream, "%s = %s\n", kOptionAutoLoad, 
										autoLoad() ? "true" : "false");
	fprintf(stream, "%s = %s\n", kOptionFastUpdate, 
										fastUpdate() ? "true" : "false");

	// write number options
	fprintf(stream, "\n# Number options: key = value\n");
	fprintf(stream, "%s = %g\n", kOptionBufferFrames, bufferFrames());
	fprintf(stream, "%s = %d\n", kOptionBufferCount, bufferCount());
	fprintf(stream, "%s = %d\n", kOptionOSCInPort, oscInPort());
	fprintf(stream, "%s = %d\n", kOptionPrint, print());

	// write string options
	fprintf(stream, "\n# String options: key = \"quoted string\"\n");
	if (device()[0])
		fprintf(stream, "%s = \"%s\"\n", kOptionDevice, device());
	else
		fprintf(stream, "# %s = \"%s\"\n", kOptionDevice, "mydevice");
	if (inDevice()[0])
		fprintf(stream, "%s = \"%s\"\n", kOptionInDevice, inDevice());
	else
		fprintf(stream, "# %s = \"%s\"\n", kOptionInDevice, "myindevice");
	if (outDevice()[0])
		fprintf(stream, "%s = \"%s\"\n", kOptionOutDevice, outDevice());
	else
		fprintf(stream, "# %s = \"%s\"\n", kOptionOutDevice, "myoutdevice");
	if (midiInDevice()[0])
		fprintf(stream, "%s = \"%s\"\n", kOptionMidiInDevice, midiInDevice());
	else
		fprintf(stream, "# %s = \"%s\"\n", kOptionMidiInDevice,
														"my midi indevice");
	if (midiOutDevice()[0])
		fprintf(stream, "%s = \"%s\"\n", kOptionMidiOutDevice, midiOutDevice());
	else
		fprintf(stream, "# %s = \"%s\"\n", kOptionMidiOutDevice,
														"my midi outdevice");
#ifdef NOTYET  // we haven't implemented OSC transmit, which would need this
	if (oscHost()[0])
		fprintf(stream, "%s = \"%s\"\n", kOptionOSCHost, oscHost());
	else
		fprintf(stream, "# %s = \"%s\"\n", kOptionOSCHost, "localhost");
#endif
#ifdef SHAREDLIBDIR
	fprintf(stream, "\n# %s is a colon-separated list of directories (full "
			"path names) to \n# search for instruments.\n", kOptionDSOPath);
	fprintf(stream, "# %s = \"%s\"\n", kOptionDSOPath, SHAREDLIBDIR);
#endif

	fprintf(stream, "\n");
	fclose(stream);

	return 0;
}


// String option setting methods

char *Option::device(const char *devName)
{
	strncpy(_device, devName, DEVICE_MAX);
	_device[DEVICE_MAX - 1] = 0;
	return _device;
}

char *Option::inDevice(const char *devName)
{
	strncpy(_inDevice, devName, DEVICE_MAX);
	_inDevice[DEVICE_MAX - 1] = 0;
	return _inDevice;
}

char *Option::outDevice(const char *devName, int devIndex)
{
	strncpy(_outDevice[devIndex], devName, DEVICE_MAX);
	_outDevice[devIndex][DEVICE_MAX - 1] = 0;
	return _outDevice[devIndex];
}

char *Option::midiInDevice(const char *devName)
{
	strncpy(_midiInDevice, devName, DEVICE_MAX);
	_midiInDevice[DEVICE_MAX - 1] = 0;
	return _midiInDevice;
}

char *Option::midiOutDevice(const char *devName)
{
	strncpy(_midiOutDevice, devName, DEVICE_MAX);
	_midiOutDevice[DEVICE_MAX - 1] = 0;
	return _midiOutDevice;
}

char *Option::oscHost(const char *oscHost)
{
	strncpy(_oscHost, oscHost, OSCHOST_MAX);
	_oscHost[OSCHOST_MAX - 1] = 0;
	return _oscHost;
}

char *Option::dsoPath(const char *pathName)
{
	strncpy(_dsoPath, pathName, DSOPATH_MAX);
	_dsoPath[DSOPATH_MAX - 1] = 0;
	return _dsoPath;
}

char *Option::dsoPathPrepend(const char *pathName)
{
	char *str = new char[strlen(pathName) + 1 + strlen(_dsoPath) + 1];
	strcpy(str, pathName);
	if (strlen(_dsoPath)) {
		strcat(str, ":");
		strcat(str, _dsoPath);
	}
	strncpy(_dsoPath, str, DSOPATH_MAX);
	_dsoPath[DSOPATH_MAX - 1] = 0;
	delete [] str;
	return _dsoPath;
}

char *Option::dsoPathAppend(const char *pathName)
{
	char *str = new char[strlen(_dsoPath) + 1 + strlen(pathName) + 1];
	strcpy(str, _dsoPath);
	if (strlen(_dsoPath))
		strcat(str, ":");
	strcat(str, pathName);
	strncpy(_dsoPath, str, DSOPATH_MAX);
	_dsoPath[DSOPATH_MAX - 1] = 0;
	delete [] str;
	return _dsoPath;
}

char *Option::rcName(const char *rcName)
{
	strncpy(_rcName, rcName, PATH_MAX);
	_rcName[PATH_MAX - 1] = 0;
	return _rcName;
}

void Option::dump()
{
#ifndef MAXMSP
	cout << kOptionAudio << ": " << _audio << endl;
	cout << kOptionPlay << ": " << _play << endl;
	cout << kOptionRecord << ": " << _record << endl;
	cout << kOptionClobber << ": " << _clobber << endl;
	cout << kOptionPrint << ": " << _print << endl;
	cout << kOptionReportClipping << ": " << _reportClipping << endl;
	cout << kOptionCheckPeaks << ": " << _checkPeaks << endl;
	cout << kOptionExitOnError << ": " << _exitOnError << endl;
	cout << kOptionAutoLoad << ": " << _autoLoad << endl;
	cout << kOptionFastUpdate << ": " << _fastUpdate << endl;
	cout << kOptionBufferFrames << ": " << _bufferFrames << endl;
	cout << kOptionBufferCount << ": " << _bufferCount << endl;
	cout << kOptionOSCInPort << ": " << _oscInPort << endl;
	cout << kOptionDevice << ": " << _device << endl;
	cout << kOptionInDevice << ": " << _inDevice << endl;
	cout << kOptionOutDevice << ": " << _outDevice[0] << endl;
	cout << kOptionOutDevice2 << ": " << _outDevice[1] << endl;
	cout << kOptionOutDevice3 << ": " << _outDevice[2] << endl;
	cout << kOptionMidiInDevice << ": " << _midiInDevice << endl;
	cout << kOptionMidiOutDevice << ": " << _midiOutDevice << endl;
	cout << kOptionOSCHost << ": " << _oscHost << endl;
	cout << kOptionDSOPath << ": " << _dsoPath << endl;
	cout << kOptionRCName << ": " << _rcName << endl;
	cout << kOptionHomeDir << ": " << _homeDir << endl;
#endif // MAXMSP
}

void Option::reportError(const char *format, const char *msg1, const char *msg2)
{
	char buf[1024];
	snprintf(buf, 1024, format, msg1, msg2);
	printf("Config file error:  %s\n", buf);
}


// ----------------------------------------------------------------------------
// These functions are for C code that needs to query options.

int get_print_option()
{
	return (int) Option::print();
}

char *get_dsopath_option()
{
	return Option::dsoPath();
}

int get_bool_option(const char *option_name)
{
	if (!strcmp(option_name, kOptionPrint))
		return (int) Option::print();
	else if (!strcmp(option_name, kOptionReportClipping))
		return (int) Option::reportClipping();
	else if (!strcmp(option_name, kOptionCheckPeaks))
		return (int) Option::checkPeaks();
	else if (!strcmp(option_name, kOptionClobber))
		return (int) Option::clobber();
	else if (!strcmp(option_name, kOptionAudio))
		return (int) Option::audio();
	else if (!strcmp(option_name, kOptionPlay))
		return (int) Option::play();
	else if (!strcmp(option_name, kOptionRecord))
		return (int) Option::record();
	else if (!strcmp(option_name, kOptionExitOnError))
		return (int) Option::exitOnError();
	else if (!strcmp(option_name, kOptionAutoLoad))
		return (int) Option::autoLoad();
	else if (!strcmp(option_name, kOptionFastUpdate))
		return (int) Option::fastUpdate();

	assert(0 && "unsupported option name");		// program error
	return 0;
}

void set_bool_option(const char *option_name, int value)
{
	if (!strcmp(option_name, kOptionPrint))
		Option::print((int) value);
	else if (!strcmp(option_name, kOptionReportClipping))
		Option::reportClipping((bool) value);
	else if (!strcmp(option_name, kOptionCheckPeaks))
		Option::checkPeaks((bool) value);
	else if (!strcmp(option_name, kOptionClobber))
		Option::clobber((bool) value);
	else if (!strcmp(option_name, kOptionAudio))
		Option::audio((bool) value);
	else if (!strcmp(option_name, kOptionPlay))
		Option::play((bool) value);
	else if (!strcmp(option_name, kOptionRecord))
		Option::record((bool) value);
	else if (!strcmp(option_name, kOptionExitOnError))
		Option::exitOnError((bool) value);
	else if (!strcmp(option_name, kOptionAutoLoad))
		Option::autoLoad((bool) value);
	else if (!strcmp(option_name, kOptionFastUpdate))
		Option::fastUpdate((bool) value);
	else
		assert(0 && "unsupported option name");
}

double get_double_option(const char *option_name)
{
	if (!strcmp(option_name, kOptionBufferFrames))
		return Option::bufferFrames();

	else if (!strcmp(option_name, kOptionBufferCount))
		return Option::bufferCount();

	assert(0 && "unsupported option name");
	return 0;
}

void set_double_option(const char *option_name, double value)
{
	if (!strcmp(option_name, kOptionBufferFrames))
		Option::bufferFrames(value);
	else if (!strcmp(option_name, kOptionBufferCount))
		Option::bufferCount((int)value);
	else
		assert(0 && "unsupported option name");
}

char *get_string_option(const char *option_name)
{
	if (!strcmp(option_name, kOptionDevice))
		return Option::device();
	else if (!strcmp(option_name, kOptionInDevice))
		return Option::inDevice();
	else if (!strcmp(option_name, kOptionOutDevice))
		return Option::outDevice();
	else if (!strcmp(option_name, kOptionOutDevice2))
		return Option::outDevice(1);
	else if (!strcmp(option_name, kOptionOutDevice3))
		return Option::outDevice(2);
	else if (!strcmp(option_name, kOptionDSOPath))
		return Option::dsoPath();

	assert(0 && "unsupported option name");
	return 0;
}

void set_string_option(const char *option_name, const char *value)
{
	if (!strcmp(option_name, kOptionDevice))
		Option::device(value);
	else if (!strcmp(option_name, kOptionInDevice))
		Option::inDevice(value);
	else if (!strcmp(option_name, kOptionOutDevice))
		Option::outDevice(value);
	else if (!strcmp(option_name, kOptionOutDevice2))
		Option::outDevice(value, 1);
	else if (!strcmp(option_name, kOptionOutDevice3))
		Option::outDevice(value, 2);
	else if (!strcmp(option_name, kOptionDSOPath))
		Option::dsoPath(value);
	else
		assert(0 && "unsupported option name");
}

// This is so we can call dump from within GDB.
void option_dump()
{
	Option::dump();
}

