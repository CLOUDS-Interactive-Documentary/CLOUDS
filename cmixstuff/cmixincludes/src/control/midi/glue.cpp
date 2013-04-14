/* RTcmix  - Copyright (C) 2004  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <rtcmix_types.h>
#include <Option.h>
#include <PField.h>
#include <utils.h>	// in ../../rtcmix
#include <RTcmixMIDI.h>
#include <RTMidiPField.h>
#include <ugens.h>		// for warn, die

//FIXME: better to have a struct with one field a string, the other type?

#define NAME_VARIANTS	4
static const char *_midi_type_name[][NAME_VARIANTS] = {
// 	// NB: Order of these must correspond to indices given by MIDIType enum!
	{ "cntl", "control", NULL, NULL },
	{ "noteon", NULL, NULL, NULL },
	{ "noteoff", NULL, NULL, NULL },
	{ "bend", "pitchbend", NULL, NULL },
	{ "prog", "program", NULL, NULL },
	{ "chanpress", "monopress", "at", "aftertouch" },
	{ "polypress", "keypress", "polyat", "polyaftertouch" },
	{ NULL, NULL, NULL, NULL }
};

static const char *_midi_controller_name[128] = {
				"", "mod", "breath", "", "foot",
				"port time", "data", "volume", "balance", "",
/* 10 */		"pan", "expression", "fxctl1", "fxctl2", "",
				"", "gp1", "gp2", "gp3", "gp4",
/* 20 */		"", "", "", "", "",
				"", "", "", "", "",
/* 30 */		"", "", "", "", "",
				"", "", "", "", "",
/* 40 */		"", "", "", "", "",
				"", "", "", "", "",
/* 50 */		"", "", "", "", "",
				"", "", "", "", "",
/* 60 */		"", "", "", "", "sustainsw",
				"portamentosw", "sostenutosw", "softpedsw", "legatosw", "hold2sw",
/* 70 */		"sc1", "sc2", "sc3", "sc4", "sc5",
				"sc6", "sc7", "sc8", "sc9", "sc10",
/* 80 */		"gp5", "gp6", "gp7", "gp8", "portamento",
				"", "", "", "", "",
/* 90 */		"", "fx1depth", "fx2depth", "fx3depth", "fx4depth",
				"fx5depth", "dataincr", "datadecr", "nrplsb", "nrpmsb",
/* 100 */	"rplsb", "rpmsb", "", "", "",
				"", "", "", "", "",
/* 110 */	"", "", "", "", "",
				"", "", "", "", "",
/* 120 */	"sound off", "reset cntl", "local cntl", "notes off", "omni off",
				"omni on", "mono on", "poly on"
};

// --------------------------------------------------------- midi_connection ---
//
//    midi = makeconnection("midi", min, max, default, lag, chan, type,
//                                                                [subtype])
//
//    <chan>      MIDI channel (1-16)
//
//    <type>      "noteon", "noteoff", "cntl", "prog", "bend", "chanpress",
//                "polypress"
//
//    <subtype>   depends on <type>:
//                   noteon     note number or velocity
//                   noteoff    note number or velocity
//                   cntl       controller number or string symbol, such as
//                              "mod", "foot", "breath", "data", "volume", "pan"
//                   polypress  MIDI note number
//                   [no subtypes for the other types]
//

static RTNumberPField *
_midi_usage()
{
	die("makeconnection (midi)",
		"Usage: makeconnection(\"midi\", min, max, default, lag, "
		"chan, type[, subtype])");
	return NULL;
}

static MIDIType
_string_to_type(const char *type)
{
	for (int i = 0; _midi_type_name[i][0] != NULL; i++) {
		for (int j = 0; j < NAME_VARIANTS; j++) {
			const char *name = _midi_type_name[i][j];
			if (name == NULL)
				break;
			if (strcasecmp(type, name) == 0)
				return (MIDIType) i;
		}
	}
	return kMIDIInvalidType;
}

static MIDISubType
_string_to_subtype(const MIDIType type, const char *subtype)
{
	if (type == kMIDIControlType) {
		for (int i = 0; i < 128; i++) {
			const char *name = _midi_controller_name[i];
			if (strcmp(subtype, name) == 0)
				return (MIDISubType) i;
		}
	}
	else if (type == kMIDINoteOnType || type == kMIDINoteOffType) {
		if (strcmp(subtype, "pitch") == 0)
			return kMIDINotePitchSubType;
		else if (strncmp(subtype, "velocity", 3) == 0)
			return kMIDINoteVelSubType;
	}
	return kMIDIInvalidSubType;
}


// XXX NOTE NOTE NOTE: This code will go away later, in favor of making
// conf parser read string lists.  Leave it here for reference.

/* Parse MIDI control name string, a sequence of colon-separated strings,
   with line-splicing recognition and some white-space eating.  Reads this:
  
      midi_control_names = " \
         bank select: \
         modulation wheel: \
         : \
         foot: \
         ...
         last name"

   resulting in these names: "bank select", "modulation wheel", "", "foot",
   ... "last name".  All except the empty "" override the builtin names.
*/
static void
_read_config()
{
#ifdef NOTYET
	char *names = strdup(Option::midiCntlNames());
	char *p, buf[128];
	int i = 0;
	int j = 0;
	bool eatspace = true;
	for (i = 0, p = names; i < 128 && *p != 0; p++) {
		if (eatspace && isspace(*p))
			continue;
		if (*p == '\\')
			continue;
		if (*p == ':') {
			buf[j] = 0;
			if (buf[0])
				_midi_controller_name[i] = strdup(buf);
			i++;
			j = 0;
			eatspace = true;
		}
		else {
			buf[j++] = *p;
			if (j == 127) {
				warn("makeconnection (midi)",
						"Controller name in config file too long");
				break;
			}
			eatspace = false;
		}
	}
	free(names);
#endif
}

static RTNumberPField *
create_pfield(const Arg args[], const int nargs)
{
	if (nargs < 6)
		return _midi_usage();

	static bool configRead = false;

	if (configRead == false) {
		_read_config();
		configRead = true;
	}

	double minval, maxval, defaultval, lag;
	int chan;
	MIDIType type = kMIDIInvalidType;
	MIDISubType subtype = kMIDIInvalidSubType;

	if (args[0].isType(DoubleType))
		minval = args[0];
	else
		return _midi_usage();

	if (args[1].isType(DoubleType))
		maxval = args[1];
	else
		return _midi_usage();

	if (args[2].isType(DoubleType))
		defaultval = args[2];
	else
		return _midi_usage();

	if (args[3].isType(DoubleType))
		lag = args[3];
	else
		return _midi_usage();
	if (lag < 0.0 || lag > 100.0) {
		die("makeconnection (midi)", "<lag> must be between 0 and 100");
		return NULL;
	}
	lag *= 0.01;

	if (args[4].isType(DoubleType))
		chan = (int) args[4] - 1;		// convert to zero-based channel
	else
		return _midi_usage();
	if (chan < 0 || chan > 15) {
		die("makeconnection (midi)", "<chan> must be between 1 and 16");
		return NULL;
	}

	if (args[5].isType(StringType))
		type = _string_to_type(args[5]);
	else
		return _midi_usage();
	if (type == kMIDIInvalidType)
		return _midi_usage();

	if (nargs > 6) {
		if (args[6].isType(StringType))
			subtype = _string_to_subtype(type, args[6]);
		else if (args[6].isType(DoubleType))
			// NB: this can be a code or a literal int, e.g. note or controller num
			subtype = (MIDISubType) (int) args[6];
		else
			return _midi_usage();
		if (subtype == kMIDIInvalidSubType)
			return _midi_usage();
	}

	static RTcmixMIDI *midiport = NULL;
	if (midiport == NULL)				// first time, so init midi system
		midiport = createMIDIPort();
	if (midiport == NULL)
		return NULL;

	return new RTMidiPField(midiport, minval, maxval, defaultval, lag, chan,
																		type, subtype);
}

// The following functions are the publicly-visible ones called by the
// system.

extern "C" {
	RtcHandle create_handle(const Arg args[], const int nargs);
	int register_dso();
};

RtcHandle
create_handle(const Arg args[], const int nargs)
{
	PField *pField = create_pfield(args, nargs);
	RtcHandle handle = NULL;
	if (pField != NULL) {
		handle = createPFieldHandle(pField);
	}
	return handle;
}

int register_dso()
{
	 return 0;
}
