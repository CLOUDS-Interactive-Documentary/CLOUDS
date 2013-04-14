/* RTcmix - Copyright (C) 2004  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/
#ifndef _OPTION_H_
#define _OPTION_H_ 1

/* Class for storing all our run-time options.

   The class is static, containing only static members and methods.  To 
   use from C++, refer to the method you want with the class name prefix:

      if (Option::print())
         print some stuff

   From C code, you must use the get* and set* functions at the bottom
   of this file.

   You must call Option::init() early in main() so that some data members
   will be initialized.

   ---

   To add new options, you need to edit Option.cpp/h and set_option.cpp.
   Please add your option after the existing ones of the same type (bool,
   number or string), so that each block of changes has all the options
   in the same order.  This makes it easier to find things.

   1. Add a kOption* define below.  This is the key string that a user will
      write in .rtcmixrc or in the set_option script call.  Please stick to
      the naming conventions already established.

   2. Add static accessor definitions for a bool, number or string option
      below.  Note that (only) the string setter function must be declared
      here and defined in Option.cpp (search for "String option setting
      methods").  Numbers can be int or double.

   3. Add a private static member variable to hold the option state, below.

   4. Initialize the static member variable at the top of Option.cpp.
      Strings must be initialized instead in the init method, in Option.cpp.

   5. Add code to Option::readConfigFile (Option.cpp) for your option.
      Keep bool, number and string options separate, as they are now.

   6. Add code to Option::writeConfigFile (Option.cpp) for your option.

   7. Add a line to Option::dump() (Option.cpp).

   8. If your option would need to be read/written from a C file, add it
      to the appropriate get_* and set_* C functions at the bottom of 
      Option.cpp.

   9. Add a symbol to the ParamType enum at the top of set_option.cpp.
      Please keep all items in set_option.cpp in the same order that
      they appear in Option.cpp/h, for ease of maintenance.

  10. Below this enum, add an entry to the _param_list array.

  11. In set_option.cpp, write a case statement to match your enum symbol
      in _set_key_value_option, using the _str_to_* functions for string
      conversion.


                                  by John Gibson and Doug Scott, 6/27/04
*/

#include <stdlib.h>
#include <string.h>

#define CONF_FILENAME ".rtcmixrc"
#define DEFAULT_BUFFER_FRAMES 4096.0
#define DEFAULT_BUFFER_COUNT 2
#define DEFAULT_OSC_INPORT 7770

#define DEVICE_MAX   64
#define MAX_OUTPUT_DEVICES 3

// Option names.  These are the keys that appear in the .rtcmixrc file.
// They're also the <option_name> used with the get_*_option C functions.

// bool options
#define kOptionAudio            "audio"
#define kOptionPlay             "play"
#define kOptionRecord           "record"
#define kOptionClobber          "clobber"
#define kOptionReportClipping   "report_clipping"
#define kOptionCheckPeaks       "check_peaks"
#define kOptionExitOnError      "exit_on_error"
#define kOptionAutoLoad         "auto_load"
#define kOptionFastUpdate       "fast_update"

// number options
#define kOptionBufferFrames     "buffer_frames"
#define kOptionBufferCount      "buffer_count"
#define kOptionOSCInPort        "osc_inport"

// int options
#define kOptionPrint            "print"

// string options
#define kOptionDevice           "device"
#define kOptionInDevice         "indevice"
#define kOptionOutDevice        "outdevice"
#define kOptionOutDevice2       "outdevice2"
#define kOptionOutDevice3       "outdevice3"
#define kOptionMidiInDevice     "midi_indevice"
#define kOptionMidiOutDevice    "midi_outdevice"
#define kOptionOSCHost          "localhost"  // NB: for unimplemented transmit
#define kOptionDSOPath          "dso_path"
#define kOptionRCName           "rcname"
#define kOptionHomeDir          "homedir"


#ifdef __cplusplus

class Option {
public:
	Option() {};
	~Option() {};

	// must call this to initialize string members
	static void init();

	static int readConfigFile(const char *fileName);
	static int writeConfigFile(const char *fileName);

	// bool options

	static bool audio() { return _audio; }
	static bool audio(const bool setIt) { _audio = setIt; return _audio; }

	static bool play() { return _play; }
	static bool play(const bool setIt) { _play = setIt; return _play; }

	static bool record() { return _record; }
	static bool record(const bool setIt) { _record = setIt; return _record; }

	static bool clobber() { return _clobber; }
	static bool clobber(const bool setIt) { _clobber = setIt; return _clobber; }

	static bool reportClipping() { return _reportClipping; }
	static bool reportClipping(const bool setIt) { _reportClipping = setIt;
													return _reportClipping; }

	static bool checkPeaks() { return _checkPeaks; }
	static bool checkPeaks(const bool setIt) { _checkPeaks = setIt;
													return _checkPeaks; }

	static bool exitOnError() { return _exitOnError; }
	static bool exitOnError(const bool setIt) { _exitOnError = setIt;
													return _exitOnError; }

	static bool autoLoad() { return _autoLoad; }
	static bool autoLoad(const bool setIt) { _autoLoad = setIt;
													return _autoLoad; }

	static bool fastUpdate() { return _fastUpdate; }
	static bool fastUpdate(const bool setIt) { _fastUpdate = setIt;
													return _fastUpdate; }

	// number options

	static double bufferFrames() { return _bufferFrames; }
	static double bufferFrames(const double frames) { _bufferFrames = frames;
													return _bufferFrames; }

	static int bufferCount() { return _bufferCount; }
	static int bufferCount(int count) { _bufferCount = count;
													return _bufferCount; }

	static int oscInPort() { return _oscInPort; }
	static int oscInPort(int portnum) { _oscInPort = portnum;
													return _oscInPort; }

	// int options
	static int print() { return _print; }
	static int print(int setIt) { _print = setIt; return _print; }

	// string options

	// WARNING: If no string as been assigned, do not expect the get method
	// to return NULL!  Instead, check that strlen is > 0.

	static char *device() { return _device; }
	static char *device(const char *devName);

	static char *inDevice() { return _inDevice; }
	static char *inDevice(const char *devName);

	static char *outDevice(int devIndex=0) { return _outDevice[devIndex]; }
	static char *outDevice(const char *devName, int devIndex=0);

	static char *midiInDevice() { return _midiInDevice; }
	static char *midiInDevice(const char *devName);

	static char *midiOutDevice() { return _midiOutDevice; }
	static char *midiOutDevice(const char *devName);

	static char *oscHost() { return _oscHost; }
	static char *oscHost(const char *oscHost);

	static char *dsoPath() { return _dsoPath; }
	static char *dsoPath(const char *pathName);
	static char *dsoPathPrepend(const char *pathName);
	static char *dsoPathAppend(const char *pathName);

	static char *homeDir() { return _homeDir; }

	static char *rcName() { return _rcName; }
	static char *rcName(const char *rcName);

	static void dump();

private:
	static void reportError(const char *format, const char *msg1, const char *msg2);

	// bool options
	static bool _audio;
	static bool _play;
	static bool _record;
	static bool _clobber;
	static bool _reportClipping;
	static bool _checkPeaks;
	static bool _exitOnError;
	static bool _autoLoad;
	static bool _fastUpdate;

	// number options
	static double _bufferFrames;
	static int _bufferCount;
	static int _oscInPort;

	// int options
	static int _print;

	// string options
	static char _device[];
	static char _inDevice[];
	static char _outDevice[MAX_OUTPUT_DEVICES][DEVICE_MAX];
	static char _midiInDevice[];
	static char _midiOutDevice[];
	static char _oscHost[];
	static char _dsoPath[];
	static char _homeDir[];
	static char _rcName[];
};

extern "C" {
#endif // __cplusplus

// These are for C code that needs to query options.	See the top of this
// file for the supported option_name strings.

int get_print_option();
char *get_dsopath_option();
int get_bool_option(const char *option_name);
void set_bool_option(const char *option_name, int value);
double get_double_option(const char *option_name);
void set_double_option(const char *option_name, double value);
char *get_string_option(const char *option_name);
void set_string_option(const char *option_name, const char *value);

void option_dump(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* _OPTION_H_ */
