/* RTcmix - Copyright (C) 2004  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/

#include <RTMidiPField.h>
#include <RTcmixMIDI.h>
#include <PField.h>
#include <Ougens.h>
#include <assert.h>

extern int resetval;		// declared in src/rtcmix/minc_functions.c


RTMidiPField::RTMidiPField(
		RTcmixMIDI			*midiport,
		const double		minval,
		const double		maxval,
		const double		defaultval,
		const double		lag,				// in range [0, 1]
		const int			chan,
		const MIDIType		type,
		const MIDISubType	subtype)
	: RTNumberPField(0),
	  _midiport(midiport), _min(minval), _default(defaultval), _chan(chan),
	  _type(type), _subtype(subtype)
{
	assert(_midiport != NULL);

	_diff = maxval - minval;

	const double maxraw = (type == kMIDIPitchBendType) ? 16383.0 : 127.0;
	_factor = 1.0 / maxraw;

	// NOTE: We rely on the control rate in effect when this PField is created.
	_filter = new Oonepole(resetval);
	_filter->sethist(_default);
	_filter->setlag(lag);
}

RTMidiPField::~RTMidiPField()
{
	delete _filter;
}

double RTMidiPField::doubleValue(double dummy) const
{
	double val = computeValue();
	return _filter->next(val);
}

double RTMidiPField::computeValue() const
{
	int rawval;

	if (_type == kMIDIControlType)
		rawval = _midiport->getControl(_chan, _subtype);

	else if (_type == kMIDIPitchBendType)
		rawval = _midiport->getBend(_chan) + 8192;

	else if (_type == kMIDIChanPressType)
		rawval = _midiport->getChanPress(_chan);

	else if (_type == kMIDINoteOnType) {
		if (_subtype == kMIDINotePitchSubType)
			rawval = _midiport->getNoteOnPitch(_chan);
		else
			rawval = _midiport->getNoteOnVel(_chan);
	}

	else if (_type == kMIDINoteOffType) {
		if (_subtype == kMIDINotePitchSubType)
			rawval = _midiport->getNoteOffPitch(_chan);
		else
			rawval = _midiport->getNoteOffVel(_chan);
	}

	else if (_type == kMIDIPolyPressType)
		rawval = _midiport->getPolyPress(_chan, _subtype);

	else if (_type == kMIDIProgramType)
		rawval = _midiport->getProgram(_chan);

	else
		rawval = INVALID_MIDIVAL;

	if (rawval == INVALID_MIDIVAL)
		return _default;

	return _min + (_diff * (rawval * _factor));
}

