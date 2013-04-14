/* RTcmix - Copyright (C) 2005 The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/
// The set_option function, called from a script, lets the user override
// default options (and those stored in the .rtcmixrc file).  The options
// are kept in the <options> object.  See Option.h for advice on how to add
// new options.
//
// Please keep the items created in the steps above in the same order that they
// appear in Option.h -- makes this file easier to maintain.
//
// -JGG, 6/30/04, rev. 4/10/05

#include <RTcmix.h>
#include <ugens.h>		// for die()
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <Option.h>

enum ParamType {
	AUDIO,
	PLAY,
	RECORD,
	CLOBBER,
	PRINT,
	REPORT_CLIPPING,
	CHECK_PEAKS,
	FULL_DUPLEX,
	EXIT_ON_ERROR,
	AUTO_LOAD,
	FAST_UPDATE,
	BUFFER_FRAMES,
	BUFFER_COUNT,
	OSC_INPORT,
	DEVICE,
	INDEVICE,
	OUTDEVICE,
	MIDI_INDEVICE,
	MIDI_OUTDEVICE,
	OSC_HOST,
	DSOPATH,
	RCNAME
};

#define OPT_STRLEN 128

struct Param {
	char arg[OPT_STRLEN];
	ParamType type;
	bool value;		// use false if not relevant, i.e. for key=value style
};

static Param _param_list[] = {
	// These are key=value option strings.  Please list these in the order in 
	// which they appear in Option.h, to make it easier to compare.

	// bool options
	{ kOptionAudio, AUDIO, false},
	{ kOptionPlay, PLAY, false},
	{ kOptionRecord, RECORD, false},
	{ kOptionClobber, CLOBBER, false},
	{ kOptionPrint, PRINT, false},
	{ kOptionReportClipping, REPORT_CLIPPING, false},
	{ kOptionCheckPeaks, CHECK_PEAKS, false},
	{ kOptionExitOnError, EXIT_ON_ERROR, false},
	{ kOptionAutoLoad, AUTO_LOAD, false},
	{ kOptionFastUpdate, FAST_UPDATE, false},

	// number options
	{ kOptionBufferFrames, BUFFER_FRAMES, false},
	{ kOptionBufferCount, BUFFER_COUNT, false},
	{ kOptionOSCInPort, OSC_INPORT, false},

	// string options
	{ kOptionDevice, DEVICE, false},
	{ kOptionInDevice, INDEVICE, false},
	{ kOptionOutDevice, OUTDEVICE, false},
	{ kOptionMidiInDevice, MIDI_INDEVICE, false},
	{ kOptionMidiOutDevice, MIDI_OUTDEVICE, false},
	{ kOptionOSCHost, OSC_HOST, false},
	{ kOptionDSOPath, DSOPATH, false},
	{ kOptionRCName, RCNAME, false},

	// These are the deprecated single-value option strings.
	// Please don't add more.
	{ "AUDIO_ON", AUDIO, true},
	{ "AUDIO_OFF", AUDIO, false},
	{ "RECORD_ON", RECORD, true},
	{ "RECORD_OFF", RECORD, false},
	{ "PLAY_ON", AUDIO, true},
	{ "PLAY_OFF", AUDIO, false},
	{ "CLOBBER_ON", CLOBBER, true},
	{ "CLOBBER_OFF", CLOBBER, false},
	{ "REPORT_CLIPPING_ON", REPORT_CLIPPING, true},
	{ "REPORT_CLIPPING_OFF", REPORT_CLIPPING, false},
	{ "CHECK_PEAKS_ON", CHECK_PEAKS, true},
	{ "CHECK_PEAKS_OFF", CHECK_PEAKS, false},
	{ "FULL_DUPLEX_ON", FULL_DUPLEX, true},
	{ "FULL_DUPLEX_OFF", FULL_DUPLEX, false},
};
static int _num_params = sizeof(_param_list) / sizeof(Param);


//-------------------------------- _str_to_bool, _str_to_int, _str_to_double ---
static int _str_to_bool(const char *str, bool &val)
{
	if (strcasecmp(str, "yes") == 0 || strcasecmp(str, "true") == 0
			|| strcasecmp(str, "on") == 0 || strcmp(str, "1") == 0)
		val = true;
	else if (strcasecmp(str, "no") == 0 || strcasecmp(str, "false") == 0
			|| strcasecmp(str, "off") == 0 || strcmp(str, "0") == 0)
		val = false;
	else {
		val = false;
		return -1;
	}
	return 0;
}

static int _str_to_int(const char *str, int &val)
{
	char *pos = NULL;
	long num = strtol(str, &pos, 10);
	if (*pos != 0)
		return -1;
	val = (int) num;
	return 0;
}

static int _str_to_double(const char *str, double &val)
{
	char *pos = NULL;
	errno = 0;
	val = strtod(str, &pos);
	if (errno != 0 || (val == 0.0 && pos == str))
		return -1;
	return 0;
}

//--------------------------------------------------------- _set_full_duplex ---
// The full duplex state has now been broken up into the <play> and <record>
// options, used during audio setup.  rtsetparams() checks <record>.
static int _set_full_duplex(const bool full_duplex,
	const bool rtsetparams_called)
{
#ifndef MAXMSP
	if (full_duplex && rtsetparams_called)
		return die("set_option",
					"Turn on full duplex BEFORE calling rtsetparams.");
#endif

	if (full_duplex)
		Option::record(true);
	else {
		// If not play, then record.
		bool state = Option::record() && !Option::play();
		Option::record(state);
	}
	// Same check as above, for record.
#ifndef MAXMSP
	if (Option::record() && rtsetparams_called)
		return die("set_option",
					"Turn on record BEFORE calling rtsetparams.");
#endif
	return 0;
}

//---------------------------------------------------- _set_key_value_option ---
static int _set_key_value_option(const char *key, const char *sval,
	const bool rtsetparams_called)
{
	int type = -1;
	for (int j = 0; j < _num_params; j++) {
		if (strcasecmp(_param_list[j].arg, key) == 0) {
			type = _param_list[j].type;
			break;
		}
	}
	if (type == -1)
		return -1;
	if (sval == NULL)
		return die("set_option", "No value for \"%s\"", key);

	int status = 0;
	bool bval = false;
	int ival = 0;
	double dval = 0.0;

	switch (type) {

		// bool options

		case AUDIO:
			status = _str_to_bool(sval, bval);
			Option::audio(bval);
			break;
		case PLAY:
			status = _str_to_bool(sval, bval);
			Option::play(bval);
			break;
		case RECORD:
			status = _str_to_bool(sval, bval);
			Option::record(bval);
#ifndef MAXMSP
			if (Option::record() && rtsetparams_called)
				return die("set_option",
							"Turn on record BEFORE calling rtsetparams.");
#endif
			break;
		case CLOBBER:
			status = _str_to_bool(sval, bval);
			Option::clobber(bval);
			break;
		case PRINT:
			status = _str_to_int(sval, ival);
			Option::print(ival);
			break;
		case REPORT_CLIPPING:
			status = _str_to_bool(sval, bval);
			Option::reportClipping(bval);
			break;
		case CHECK_PEAKS:
			status = _str_to_bool(sval, bval);
			Option::checkPeaks(bval);
			break;
		case EXIT_ON_ERROR:
			status = _str_to_bool(sval, bval);
			Option::exitOnError(bval);
			break;
		case AUTO_LOAD:
			status = _str_to_bool(sval, bval);
			Option::autoLoad(bval);
			break;
		case FAST_UPDATE:
			status = _str_to_bool(sval, bval);
			Option::fastUpdate(bval);
			if (bval)
				rtcmix_warn("set_option", "With \"%s\" on, certain instruments run "
					"faster at the expense of reduced capabilities.\n", key);
			break;

		// number options

		case BUFFER_FRAMES:
			status = _str_to_int(sval, ival);
			if (status == 0) {
				if (ival <= 0)
					return die("set_option", "\"%s\" value must be > 0", key);
				Option::bufferFrames(ival);
			}
			break;
		case BUFFER_COUNT:
			status = _str_to_int(sval, ival);
			if (status == 0) {
				if (ival <= 0)
					return die("set_option", "\"%s\" value must be > 0", key);
				Option::bufferCount(ival);
			}
			break;
		case OSC_INPORT:
			status = _str_to_int(sval, ival);
			if (status == 0) {
				if (ival <= 0)
					return die("set_option", "\"%s\" value must be > 0", key);
				Option::oscInPort(ival);
			}
			break;

		// string options

		case DEVICE:
			Option::device(sval);
			break;
		case INDEVICE:
			Option::inDevice(sval);
			break;
		case OUTDEVICE:
			Option::outDevice(sval);
			break;
		case MIDI_INDEVICE:
			Option::midiInDevice(sval);
			break;
		case MIDI_OUTDEVICE:
			Option::midiOutDevice(sval);
			break;
		case OSC_HOST:
			Option::oscHost(sval);
			break;
		case DSOPATH:
			Option::dsoPath(sval);
			break;
		case RCNAME:
			Option::rcName(sval);
			break;
		default:
			break;
	}

	if (status == -1)
		return die("set_option", "Trouble parsing value \"%s\"", sval);
	return status;
}

//-------------------------------------------------------- _set_value_option ---
// This way of setting options is deprecated, in favor of the new key=value way.
// NOTE: Do not handle new options in this function; use _set_key_value_option.
static int _set_value_option(const char *sval, const bool rtsetparams_called)
{
	int type = -1;
	bool bval = false;
	for (int j = 0; j < _num_params; j++) {
		if (strcasecmp(_param_list[j].arg, sval) == 0) {
			type = _param_list[j].type;
			bval = _param_list[j].value;
			break;
		}
	}
	if (type == -1)
		return -1;

	switch (type) {
		case AUDIO:
			Option::play(bval);
			break;
		case RECORD:
			Option::record(bval);
#ifndef MAXMSP
			if (Option::record() && rtsetparams_called)
				return die("set_option",
							"Turn on record BEFORE calling rtsetparams.");
#endif
			break;
		case CLOBBER:
			Option::clobber(bval);
			break;
		case REPORT_CLIPPING:
			Option::reportClipping(bval);
			break;
		case CHECK_PEAKS:
			Option::checkPeaks(bval);
			break;
		case FULL_DUPLEX:
			if (_set_full_duplex(bval, rtsetparams_called) == -1)
				return -1;
			break;
		default:
			break;
	}

	return 0;
}

//--------------------------------------------------------------- _parse_arg ---
static int _parse_arg(const char *arg, const bool rtsetparams_called)
{
	// Strip white-space chars from text to the left of any '=' and between
	// the '=' and the next non-white space.  The reason we don't strip the
	// whole string is to preserve options that must include spaces, such
	// as "MOTU 828".  Store result into <opt>.

	long len = arg ? strlen(arg) : 0;
	if (len > OPT_STRLEN - 1)
		len = OPT_STRLEN - 1;

	char opt[OPT_STRLEN];
	char *p = opt;
	int space_state = 0;
	for (int j = 0; j < len; j++) {
		if (space_state > 1)
			*p++ = arg[j];
		else if (!isspace(arg[j])) {
			if (space_state == 1)
				space_state++;
			else if (arg[j] == '=')
				space_state = 1;
			*p++ = arg[j];
		}
	}
	*p = '\0';

	// Two styles of option string: a single "value" and a "key=value" pair.
	int status = 0;
	p = strchr(opt, '=');				// check for "key=value"
	if (p) {
		*p++ = '\0';						// <opt> is now key only
		if (*p == '\0')					// p now points to value string
			return die("set_option", "Missing value for key \"%s\"", opt);
		status = _set_key_value_option(opt, p, rtsetparams_called);
	}
	else										// check for single "value"
		status = _set_value_option(opt, rtsetparams_called);

	if (status == -1)
		return die("set_option", "Unrecognized option \"%s\"", opt);
	return 0;
}

//--------------------------------------------------------------- set_option ---
double RTcmix::set_option(float *p, int nargs, double pp[])
{
	for (int i = 0; i < nargs; i++) {
		char *arg = DOUBLE_TO_STRING(pp[i]);		// cast pfield to string

		if (_parse_arg(arg, rtsetparams_was_called()) == -1)
			return -1.0;
	}
	return 0.0;
}

