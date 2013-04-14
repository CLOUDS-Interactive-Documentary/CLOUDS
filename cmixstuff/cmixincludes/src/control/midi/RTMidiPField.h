/* RTcmix - Copyright (C) 2004  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/
#ifndef _RTMIDIPFIELD_H_
#define _RTMIDIPFIELD_H_

#include <PField.h>

typedef enum {
	kMIDIInvalidType = -1,
	kMIDIControlType = 0,
	kMIDINoteOnType,
	kMIDINoteOffType,
	kMIDIPitchBendType,
	kMIDIProgramType,
	kMIDIChanPressType,
	kMIDIPolyPressType
} MIDIType;

typedef enum {
	kMIDIInvalidSubType = -1,
	// 0-127 reserved for literal note and controller number values
	kMIDINotePitchSubType = 128,
	kMIDINoteVelSubType,
} MIDISubType;

class RTcmixMIDI;
class Oonepole;

class RTMidiPField : public RTNumberPField {
public:
	RTMidiPField(
		RTcmixMIDI			*midiport,
		const double		minval,
		const double		maxval,
		const double		defaultval,
		const double		lag,
		const int			chan,
		const MIDIType		type,
		const MIDISubType	subtype);

	virtual double doubleValue(double dummy) const;

protected:
	virtual ~RTMidiPField();

private:
	double computeValue() const;

	RTcmixMIDI	*_midiport;
	Oonepole		*_filter;
	double		_min;
	double		_diff;
	double		_default;
	int			_chan;
	MIDIType		_type;
	MIDISubType	_subtype;
	double		_factor;
};

#endif // _RTMIDIPFIELD_H_

