/* simple MIDI monitor program, based on portmidi/pm_test/midithread.c  -JGG */

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "assert.h"
#include "portmidi.h"
#include "pmutil.h"
#include "porttime.h"

/* if INPUT_BUFFER_SIZE is 0, PortMidi uses a default value */
#define INPUT_BUFFER_SIZE 0

#define STRING_MAX 80

enum {
	kNoteOff = 0x80,
	kNoteOn = 0x90,
	kPolyPress = 0xA0,
	kControl = 0xB0,
	kProgram = 0xC0,
	kChanPress = 0xD0,
	kPitchBend = 0xE0
} StatusByte;


/**********************************/
/* DATA USED ONLY BY process_midi */
/* (except during initialization) */
/**********************************/

int active = FALSE;
PmStream *midi_in;
int showchan = 0;

/****************************/
/* END OF process_midi DATA */
/****************************/


/* shared queues */
PmQueue *midi_to_main;
PmQueue *main_to_midi;

#define QUIT_MSG 1000


/* timer interrupt for processing midi data */
void process_midi(PtTimestamp timestamp, void *userData)
{
	PmError result;
	PmEvent buffer; /* just one message at a time */
	long msg;

	/* do nothing until initialization completes */
	if (!active) 
		return;

	/* check for messages */
	do {
		result = Pm_Dequeue(main_to_midi, &msg); 
		if (result) {
			if (msg == QUIT_MSG) {
				/* acknowledge receipt of quit message */
				Pm_Enqueue(midi_to_main, &msg);
				active = FALSE;
				return;
			}
		}
	} while (result);         
    
	/* see if there is any midi input to process */
	do {
		result = Pm_Poll(midi_in);
		if (result) {
			long status, chan, data1, data2;

			if (Pm_Read(midi_in, &buffer, 1) == pmBufferOverflow) 
				continue;

			/* unless there was overflow, we should have a message now */
			status = Pm_MessageStatus(buffer.message);
			data1 = Pm_MessageData1(buffer.message);
			data2 = Pm_MessageData2(buffer.message);

			chan = (status & 0x0F) + 1;
			if (showchan > 0 && chan != showchan)
				continue;

			//printf("raw: %lx %lx %lx\n", status, data1, data2);
			switch (status & 0xF0) {
				case kNoteOn:
					printf("NoteOn (chan=%ld):\tpitch=%ld vel=%ld\n",
										chan, data1, data2);
					break;
				case kNoteOff:
					printf("NoteOff (chan=%ld):\tpitch=%ld vel=%ld\n",
										chan, data1, data2);
					break;
				case kPolyPress:
					printf("PolyPress (chan=%ld):\tpitch=%ld press=%ld\n",
										chan, data1, data2);
					break;
				case kControl:
					printf("Control (chan=%ld):\tcntl=%ld val=%ld\n",
										chan, data1, data2);
					break;
				case kProgram:
					printf("Program (chan=%ld):\tnum=%ld\n", chan, data1);
					break;
				case kChanPress:
					printf("ChanPress (chan=%ld):\tpress=%ld\n", chan, data1);
					break;
				case kPitchBend:
					printf("PitchBend (chan=%ld):\tbend=%ld (%ld, %ld)\n", chan,
										((data2 << 7) + data1) - 8192, data1, data2);
					break;
				default:
					printf("0x%.2x, %ld, %ld\n", (u_char) status, data1, data2);
					break;
			}
			fflush(stdout);
		}
	} while (result);
}

void exit_with_message(char *msg)
{
	char line[STRING_MAX];
	printf("%s\n", msg);
	fgets(line, STRING_MAX, stdin);
	exit(1);
}

int main(int argc, char **argv)
{
	int i, id;
	long n;
	const PmDeviceInfo *info;
	char line[STRING_MAX];
	int spin;
	int done = FALSE;

	for (i = 1; i < argc; i++) {
		char *arg = argv[i];

		if (arg[0] == '-') {
			switch (arg[1]) {
				case 'c':
					showchan = atoi(&arg[2]);
					if (showchan < 1 || showchan > 16) {
						fprintf(stderr, "Invalid channel number %d\n", showchan);
						exit(1);
					}
					break;
			}
		}
	}

	midi_to_main = Pm_QueueCreate(32, sizeof(long));
	assert(midi_to_main != NULL);
	main_to_midi = Pm_QueueCreate(32, sizeof(long));
	assert(main_to_midi != NULL);

	/* always start the timer before you start midi */
	Pt_Start(1, &process_midi, 0); /* start a timer with millisecond accuracy */
	/* the timer will call our function, process_midi() every millisecond */

	Pm_Initialize();

	id = Pm_GetDefaultInputDeviceID();
	info = Pm_GetDeviceInfo(id);
	if (info == NULL) {
		printf("Could not open default input device (%d).", id);
		exit_with_message("");
	}
	printf("Opening input device %s %s\n", info->interf, info->name);
	Pm_OpenInput(&midi_in, id, NULL, INPUT_BUFFER_SIZE, NULL, NULL);

	/* enable processing in the midi thread -- yes, this is a shared variable
	   without synchronization, but this simple assignment is safe
	*/
	active = TRUE;

	printf("Type 'q' followed by Return to quit\n");

	while (!done) {
		long msg;
		int len;

		fgets(line, STRING_MAX, stdin);
		len = strlen(line);	/* remove the newline: */
		if (len > 0) line[len - 1] = 0; /* overwrite the newline char */

		if (strcmp(line, "q") == 0) {
			msg = QUIT_MSG;
			Pm_Enqueue(main_to_midi, &msg);
			/* wait for acknowlegement */
			do {
				spin = Pm_Dequeue(midi_to_main, &msg);
			} while (spin == 0); /* spin */
			done = TRUE; /* leave the command loop and wrap up */
		}
	}

	/* at this point, midi thread is inactive and we need to shut down
	 * the midi input and output */

	Pt_Stop(); /* stop the timer */
	Pm_QueueDestroy(midi_to_main);
	Pm_QueueDestroy(main_to_midi);

	Pm_Close(midi_in);

	return 0;
}

