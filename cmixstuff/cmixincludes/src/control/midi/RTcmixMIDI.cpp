/* RTcmix - Copyright (C) 2004  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/
#include <RTcmixMIDI.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <assert.h>
#include <Option.h>

#define DEBUG 0

// if INBUF_SIZE is 0, PortMidi uses a default value
#define INBUF_SIZE		0
#define MSG_QUEUE_SIZE	32		// NB: sets number of notes queued for scheduler

typedef enum {
	kQuitMsg = 0,
	kNoteOnMsg,
	kNoteOffMsg
} RTMQMessageType;

typedef struct {
	RTMQMessageType type;
	int data0;
	int data1;
	int data2;
} RTMQMessage;


RTcmixMIDI::RTcmixMIDI()
	: _instream(NULL), _outstream(NULL), _active(false)
{
	clear();
}

RTcmixMIDI::~RTcmixMIDI()
{
	if (_mainToMIDI) {
		RTMQMessage msg;
		msg.type = kQuitMsg;
		Pm_Enqueue(_mainToMIDI, &msg);

		// wait for acknowlegement
		int spin;
		do {
			spin = Pm_Dequeue(_MIDIToMain, &msg);
		} while (spin == 0);
	}

	Pt_Stop();	// stop timer
	if (_instream)
		Pm_Close(_instream);
	if (_outstream)
		Pm_Close(_outstream);

	Pm_QueueDestroy(_mainToMIDI);
	Pm_QueueDestroy(_MIDIToMain);

	Pm_Terminate();
}

RTcmixMIDI *createMIDIPort()
{
	RTcmixMIDI *midiport = new RTcmixMIDI();
	if (midiport) {
		if (midiport->init() == -1) {
			delete midiport;
			return NULL;
		}
	}

	return midiport;
}

int RTcmixMIDI::init()
{
	_MIDIToMain = Pm_QueueCreate(MSG_QUEUE_SIZE, sizeof(RTMQMessage));
	_mainToMIDI = Pm_QueueCreate(MSG_QUEUE_SIZE, sizeof(RTMQMessage));
	if (_mainToMIDI == NULL || _MIDIToMain == NULL) {
		fprintf(stderr, "Could not create MIDI message queues.\n");
		return -1;
	}

	// Start the timer before starting MIDI I/O.  The timer runs the callback
	// function every SLEEP_MSEC milliseconds.
	Pt_Start(SLEEP_MSEC, &_processMIDI, this);

	Pm_Initialize();

	int id = 0;
	const char *devname = Option::midiInDevice();
#if DEBUG > 0
	printf("Requested MIDI input device: \"%s\"\n", devname);
#endif
	if (strlen(devname)) {
		bool found = false;
		const int numdev = Pm_CountDevices();
		for ( ; id < numdev; id++) {
			const PmDeviceInfo *info = Pm_GetDeviceInfo(id);
			if (info->input) {
#if DEBUG > 0
				printf("Found MIDI input device: \"%s\"\n", info->name);
#endif
				if (strcmp(info->name, devname) == 0) {
					found = true;
					break;
				}
			}
		}
		if (!found) {
			fprintf(stderr, "WARNING: no match for MIDI input device \"%s\""
							" ... using default.\n", devname);
			id = Pm_GetDefaultInputDeviceID();
		}
	}
	else {
		fprintf(stderr, "WARNING: using default MIDI input device.\n");
		id = Pm_GetDefaultInputDeviceID();
	}
	const PmDeviceInfo *info = Pm_GetDeviceInfo(id);
	if (info == NULL) {
		fprintf(stderr, "Could not open MIDI input device %d.\n", id);
		return -1;
	}

	PmError err = Pm_OpenInput(&_instream, id, NULL, INBUF_SIZE, NULL, NULL);
	if (err != pmNoError) {
		fprintf(stderr, "Could not open MIDI input stream: %s.\n",
									Pm_GetErrorText(err));
		return -1;
	}

	Pm_SetFilter(_instream, PM_FILT_ACTIVE | PM_FILT_CLOCK | PM_FILT_SYSEX);

	// Empty buffer after setting filter, in case anything got through before.
	PmEvent buffer;
	while (Pm_Poll(_instream))
		Pm_Read(_instream, &buffer, 1);

	// enable processing in MIDI thread (_processMIDI)
	active(true);

	return 0;
}

void RTcmixMIDI::clear()
{
	for (int chan = 0; chan < 16; chan++) {
		_noteonpitch[chan] = INVALID_MIDIVAL;
		_noteonvel[chan] = INVALID_MIDIVAL;
		_noteoffpitch[chan] = INVALID_MIDIVAL;
		_noteoffvel[chan] = INVALID_MIDIVAL;
		_bend[chan] = INVALID_MIDIVAL;
		_program[chan] = INVALID_MIDIVAL;
		_chanpress[chan] = INVALID_MIDIVAL;
		for (int i = 0; i < 128; i++) {
			_control[chan][i] = INVALID_MIDIVAL;
			_polypress[chan][i] = INVALID_MIDIVAL;
		}
	}
}


// -------------------------------------------------------------------- dump ---
// Just for debugging.

const char *RTcmixMIDI::getValueString(const int val)
{
	if (val == INVALID_MIDIVAL)
		return "--";

	static char buf[16];
	snprintf(buf, 16, "%d", val);
	return buf;
}

void RTcmixMIDI::dump(const int chan)
{
	printf("\nDumping current MIDI state...\n");
	printf("---------------------------------------- Channel %d\n", chan + 1);
	printf("   Bend:\t%s\n", getValueString(_bend[chan]));
	printf("   Program:\t%s\n", getValueString(_program[chan]));
	printf("   ChanPress:\t%s\n", getValueString(_chanpress[chan]));
	printf("   NoteOnPitch:\t%s\n", getValueString(_noteonpitch[chan]));
	printf("   NoteOnVel:\t%s\n", getValueString(_noteonvel[chan]));
	printf("   NoteOffPitch:\t%s\n", getValueString(_noteoffpitch[chan]));
	printf("   NoteOffVel:\t%s\n", getValueString(_noteoffvel[chan]));
#if 1
	printf("   Control:\n");
	for (int i = 0; i < 128; i++)
		printf("      [%d]:\t%s\n", i, getValueString(_control[chan][i]));
#endif
#if 0
	printf("   PolyPress:\n");
	for (int i = 0; i < 128; i++)
		printf("      [%d]:\t%s\n", i, getValueString(_polypress[chan][i]));
#endif
	printf("\n");
	fflush(stdout);
}


// ------------------------------------------------------------ _processMIDI ---

static enum {
	kNoteOff = 0x80,
	kNoteOn = 0x90,
	kPolyPress = 0xA0,
	kControl = 0xB0,
	kProgram = 0xC0,
	kChanPress = 0xD0,
	kPitchBend = 0xE0,
	kSystem = 0xF0
} StatusByte;

// This is called from the MIDI worker thread.  The RTcmixMIDI object, held by
// main thread, communicates with this worker thread by passing messages
// back and forth.
void RTcmixMIDI::_processMIDI(PtTimestamp timestamp, void *context)
{
	RTcmixMIDI *obj = (RTcmixMIDI *) context;

	// Check for messages from object.
	PmError result;
	do {
		RTMQMessage msg;
		result = Pm_Dequeue(obj->mainToMIDI(), &msg);
		if (result) {
			if (msg.type == kQuitMsg) {
				// acknowledge receipt of quit message
				Pm_Enqueue(obj->MIDIToMain(), &msg);
				obj->active(false);
				return;
			}
		}
	} while (result);

	// Don't poll MIDI until initialization completes.  We still listen for
	// object messages, in case RTcmixMIDI::init fails and dtor is then called.
	if (!obj->active())
		return;

	// See if there is any MIDI input to process.
	do {
		result = Pm_Poll(obj->instream());
		if (result) {
			PmEvent buffer;

			if (Pm_Read(obj->instream(), &buffer, 1) == pmBufferOverflow)
				continue;

			// Unless there was overflow, we should have a message now.
			const long status = Pm_MessageStatus(buffer.message);
			const long data1 = Pm_MessageData1(buffer.message);
			const long data2 = Pm_MessageData2(buffer.message);

			const long chan = status & 0x0F;

			switch (status & 0xF0) {
				case kNoteOn:
					if (data2 > 0) {
						obj->setNoteOnPitch(chan, data1);
						obj->setNoteOnVel(chan, data2);
						obj->noteOnTrigger(chan, data1, data2);
					}
					else {	// note on w/ vel=0 is logically a note off
						obj->setNoteOffPitch(chan, data1);
						obj->setNoteOffVel(chan, data2);
						obj->noteOffTrigger(chan, data1, data2);
					}
					break;
				case kNoteOff:
					obj->setNoteOffPitch(chan, data1);
					obj->setNoteOffVel(chan, data2);
					obj->noteOffTrigger(chan, data1, data2);
					break;
				case kPolyPress:
					obj->setPolyPress(chan, data1, data2);
					break;
				case kControl:
					obj->setControl(chan, data1, data2);
					break;
				case kProgram:
					obj->setProgram(chan, data1);
					break;
				case kChanPress:
					obj->setChanPress(chan, data1);
					break;
				case kPitchBend:
					obj->setBend(chan, ((data2 << 7) + data1) - 8192);
					break;
				case kSystem:
				default:
#if DEBUG > 1
					printf("0x%.2x, %ld, %ld\n", (u_char) status, data1, data2);
#endif
					break;
			}
#if DEBUG > 1
			obj->dump(chan);
#endif
		}
	} while (result);
}

// These functions pass a message, from the MIDI worker thread to the main
// thread, containing the data for a new noteon or noteoff event.  If we
// were to act on that data directly here, by calling a scheduler function,
// then the note would be running within the MIDI worker thread!  Not what
// we want.

// FIXME: Presumably the scheduler would call RTcmixMIDI::checkMIDITrigger,
// which would dequeue such messages and take appropriate action, starting or
// stopping notes.  Since RTcmixMIDI is supposed to be dynamically loaded, how
// would scheduler know if and what to call?

void RTcmixMIDI::noteOnTrigger(int chan, int pitch, int velocity)
{
	RTMQMessage msg;
	msg.type = kNoteOnMsg;
	msg.data0 = chan;
	msg.data1 = pitch;
	msg.data2 = velocity;
	Pm_Enqueue(_MIDIToMain, &msg);
}

void RTcmixMIDI::noteOffTrigger(int chan, int pitch, int velocity)
{
	RTMQMessage msg;
	msg.type = kNoteOffMsg;
	msg.data0 = chan;
	msg.data1 = pitch;
	msg.data2 = velocity;
	Pm_Enqueue(_MIDIToMain, &msg);
}


