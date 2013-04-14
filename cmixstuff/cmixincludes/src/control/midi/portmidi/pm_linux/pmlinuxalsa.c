/*
 * pmlinuxalsa.c -- system specific definitions
 * 
 * written by:
 *  Roger Dannenberg (port to Alsa 0.9.x)
 *  Clemens Ladisch (provided code examples and invaluable consulting)
 *  Jason Cohen, Rico Colon, Matt Filippone (Alsa 0.5.x implementation)
 */ 

#include "stdlib.h"
#include "portmidi.h"
#include "pminternal.h"
#include "pmlinuxalsa.h"
#include "string.h"
#include "porttime.h"
#include "pmlinux.h"

#include <alsa/asoundlib.h>

/* I used many print statements to debug this code. I left them in the
 * source, and you can turn them on by changing false to true below:
 */
#define VERBOSE_ON 0
#define VERBOSE if (VERBOSE_ON)

#define MIDI_SYSEX      0xf0
#define MIDI_EOX        0xf7

#if SND_LIB_MAJOR == 0 && SND_LIB_MINOR < 9
#error needs ALSA 0.9.0 or later
#endif

/* to store client/port in the device descriptor */
#define MAKE_DESCRIPTOR(client, port) ((void*)(((client) << 8) | (port)))
#define GET_DESCRIPTOR_CLIENT(info) ((((int)(info)) >> 8) & 0xff)
#define GET_DESCRIPTOR_PORT(info) (((int)(info)) & 0xff)

#define BYTE unsigned char
#define UINT unsigned long

extern pm_fns_node pm_linuxalsa_in_dictionary;
extern pm_fns_node pm_linuxalsa_out_dictionary;

static snd_seq_t *seq; // all input comes here, output queue allocated on seq
static int queue, queue_used; /* one for all ports, reference counted */

typedef struct alsa_descriptor_struct {
    int client;
    int port;
    int this_port;
    int in_sysex;
    snd_midi_event_t *parser;
    int error; /* host error code */
} alsa_descriptor_node, *alsa_descriptor_type;


/* get_alsa_error_text -- copy error text to potentially short string */
/**/
static void get_alsa_error_text(char *msg, int len, int err)
{
    int errlen = strlen(snd_strerror(err));
    if (errlen < len) {
        strcpy(msg, snd_strerror(err));
    } else if (len > 20) {
        sprintf(msg, "Alsa error %d", err);
    } else if (len > 4) {
        strcpy(msg, "Alsa");
    } else {
        msg[0] = 0;
    }
}


/* queue is shared by both input and output, reference counted */
static PmError alsa_use_queue(void)
{
    if (queue_used == 0) {
        snd_seq_queue_tempo_t *tempo;

        queue = snd_seq_alloc_queue(seq);
        if (queue < 0) {
            pm_hosterror = queue;
            return pmHostError;
        }
        snd_seq_queue_tempo_alloca(&tempo);
        snd_seq_queue_tempo_set_tempo(tempo, 480000);
        snd_seq_queue_tempo_set_ppq(tempo, 480);
        pm_hosterror = snd_seq_set_queue_tempo(seq, queue, tempo);
        if (pm_hosterror < 0)
            return pmHostError;

        snd_seq_start_queue(seq, queue, NULL);
        snd_seq_drain_output(seq);
    }
    ++queue_used;
    return pmNoError;
}


static void alsa_unuse_queue(void)
{
    if (--queue_used == 0) {
        snd_seq_stop_queue(seq, queue, NULL);
        snd_seq_drain_output(seq);
        snd_seq_free_queue(seq, queue);
        VERBOSE printf("queue freed\n");
    }
}


/* midi_message_length -- how many bytes in a message? */
static int midi_message_length(PmMessage message)
{
    message &= 0xff;
    if (message < 0x80) {
        return 0;
    } else if (message < 0xf0) {
        static const int length[] = {3, 3, 3, 3, 2, 2, 3};
        return length[(message - 0x80) >> 4];
    } else {
        static const int length[] = {
            -1, 2, 3, 2, 0, 0, 1, -1, 1, 0, 1, 1, 1, 0, 1, 1};
        return length[message - 0xf0];
    }
}


static PmError alsa_out_open(PmInternal *midi, void *driverInfo) 
{
    void *client_port = descriptors[midi->device_id].descriptor;
    alsa_descriptor_type desc = (alsa_descriptor_type) 
        pm_alloc(sizeof(alsa_descriptor_node));
    snd_seq_port_info_t *info;
    int err;

    if (!desc) return pmInsufficientMemory;
    
    snd_seq_port_info_alloca(&info);
    snd_seq_port_info_set_port(info, midi->device_id);
    snd_seq_port_info_set_capability(info, SND_SEQ_PORT_CAP_WRITE |
                                     SND_SEQ_PORT_CAP_READ);
    snd_seq_port_info_set_type(info, SND_SEQ_PORT_TYPE_MIDI_GENERIC | 
                                     SND_SEQ_PORT_TYPE_APPLICATION);
    snd_seq_port_info_set_port_specified(info, 1);
    err = snd_seq_create_port(seq, info);
    if (err < 0) goto free_desc;

    /* fill in fields of desc, which is passed to pm_write routines */
    midi->descriptor = desc;
    desc->client = GET_DESCRIPTOR_CLIENT(client_port);
    desc->port = GET_DESCRIPTOR_PORT(client_port);
    desc->this_port = midi->device_id;
    desc->in_sysex = 0;

    desc->error = 0;

    err = snd_midi_event_new(PM_DEFAULT_SYSEX_BUFFER_SIZE, &desc->parser);
    if (err < 0) goto free_this_port;

    if (midi->latency > 0) { /* must delay output using a queue */
        err = alsa_use_queue();
        if (err < 0) goto free_parser;

        err = snd_seq_connect_to(seq, desc->this_port, desc->client, desc->port);
        if (err < 0) goto unuse_queue;  /* clean up and return on error */
    } else {
        err = snd_seq_connect_to(seq, desc->this_port, desc->client, desc->port);
        if (err < 0) goto free_parser;  /* clean up and return on error */
    }        
    return pmNoError;

 unuse_queue:
    alsa_unuse_queue();
 free_parser:
    snd_midi_event_free(desc->parser);
 free_this_port:
    snd_seq_delete_port(seq, desc->this_port);
 free_desc:
    pm_free(desc);
    pm_hosterror = err;
    if (err < 0) {
        get_alsa_error_text(pm_hosterror_text, PM_HOST_ERROR_MSG_LEN, err);
    }
    return pmHostError;
}
    

static PmError alsa_write_byte(PmInternal *midi, unsigned char byte, 
                        PmTimestamp timestamp)
{
    alsa_descriptor_type desc = (alsa_descriptor_type) midi->descriptor;
    snd_seq_event_t ev;
    int err;

    snd_seq_ev_clear(&ev);
    if (snd_midi_event_encode_byte(desc->parser, byte, &ev) == 1) {
        snd_seq_ev_set_dest(&ev, desc->client, desc->port);
        snd_seq_ev_set_source(&ev, desc->this_port);
        if (midi->latency > 0) {
            /* compute relative time of event = timestamp - now + latency */
            PmTimestamp now = (midi->time_proc ? 
                               midi->time_proc(midi->time_info) : 
                               Pt_Time(NULL));
            int when = timestamp;
            /* if timestamp is zero, send immediately */
            /* otherwise compute time delay and use delay if positive */
            if (when == 0) when = now;
            when = (when - now) + midi->latency;
            if (when < 0) when = 0;
            VERBOSE printf("timestamp %ld now %ld latency %ld, ", 
                           timestamp, now, midi->latency);
            VERBOSE printf("scheduling event after %d\n", when);
            /* message is sent in relative ticks, where 1 tick = 1 ms */
            snd_seq_ev_schedule_tick(&ev, queue, 1, when);
            /* NOTE: for cases where the user does not supply a time function,
               we could optimize the code by not starting Pt_Time and using
               the alsa tick time instead. I didn't do this because it would
               entail changing the queue management to start the queue tick
               count when PortMidi is initialized and keep it running until
               PortMidi is terminated. (This should be simple, but it's not
               how the code works now.) -RBD */
        } else { /* send event out without queueing */
            VERBOSE printf("direct\n");
            /* ev.queue = SND_SEQ_QUEUE_DIRECT;
               ev.dest.client = SND_SEQ_ADDRESS_SUBSCRIBERS; */
            snd_seq_ev_set_direct(&ev);
        }
        VERBOSE printf("sending event\n");
        err = snd_seq_event_output(seq, &ev);
        if (err < 0) {
            desc->error = err;
            return pmHostError;
        }
    }
    return pmNoError;
}


static PmError alsa_out_close(PmInternal *midi)
{
    int err;
    alsa_descriptor_type desc = (alsa_descriptor_type) midi->descriptor;
    if (!desc) return pmBadPtr;

    pm_hosterror = snd_seq_disconnect_to(seq, desc->this_port, 
                                             desc->client, desc->port);
    if (pm_hosterror) {
        // if there's an error, try to delete the port anyway, but don't
        // change the pm_hosterror value so we retain the first error
        snd_seq_delete_port(seq, desc->this_port);
    } else { // if there's no error, delete the port and retain any error
        pm_hosterror = snd_seq_delete_port(seq, desc->this_port);
    }
    if (midi->latency > 0) alsa_unuse_queue();
    snd_midi_event_free(desc->parser);
    pm_free(desc);
    if (pm_hosterror) {
        get_alsa_error_text(pm_hosterror_text, PM_HOST_ERROR_MSG_LEN, 
                            pm_hosterror);
        return pmHostError;
    }
    return pmNoError;
}


static PmError alsa_in_open(PmInternal *midi, void *driverInfo)
{
    void *client_port = descriptors[midi->device_id].descriptor;
    alsa_descriptor_type desc = (alsa_descriptor_type) 
        pm_alloc(sizeof(alsa_descriptor_node));
    snd_seq_port_info_t *info;
    snd_seq_port_subscribe_t *sub;
    snd_seq_addr_t addr;
    int err;

    if (!desc) return pmInsufficientMemory;
    
    err = alsa_use_queue();
    if (err < 0) goto free_desc;

    snd_seq_port_info_alloca(&info);
    snd_seq_port_info_set_port(info, midi->device_id);
    snd_seq_port_info_set_capability(info, SND_SEQ_PORT_CAP_WRITE |
                                     SND_SEQ_PORT_CAP_READ);
    snd_seq_port_info_set_type(info, SND_SEQ_PORT_TYPE_MIDI_GENERIC | 
                                     SND_SEQ_PORT_TYPE_APPLICATION);
    snd_seq_port_info_set_port_specified(info, 1);
    err = snd_seq_create_port(seq, info);
    if (err < 0) goto free_queue;

    /* fill in fields of desc, which is passed to pm_write routines */
    midi->descriptor = desc;
    desc->client = GET_DESCRIPTOR_CLIENT(client_port);
    desc->port = GET_DESCRIPTOR_PORT(client_port);
    desc->this_port = midi->device_id;
    desc->in_sysex = 0;

    desc->error = 0;

    VERBOSE printf("snd_seq_connect_from: %d %d %d\n", 
                   desc->this_port, desc->client, desc->port);
    snd_seq_port_subscribe_alloca(&sub);
    addr.client = snd_seq_client_id(seq);
    addr.port = desc->this_port;
    snd_seq_port_subscribe_set_dest(sub, &addr);
    addr.client = desc->client;
    addr.port = desc->port;
    snd_seq_port_subscribe_set_sender(sub, &addr);
    snd_seq_port_subscribe_set_time_update(sub, 1);
    /* this doesn't seem to work: messages come in with real timestamps */
    snd_seq_port_subscribe_set_time_real(sub, 0);
    err = snd_seq_subscribe_port(seq, sub);
    /* err = 
       snd_seq_connect_from(seq, desc->this_port, desc->client, desc->port); */
    if (err < 0) goto free_this_port;  /* clean up and return on error */
    return pmNoError;

 free_this_port:
    snd_seq_delete_port(seq, desc->this_port);
 free_queue:
    alsa_unuse_queue();
 free_desc:
    pm_free(desc);
    pm_hosterror = err;
    if (err < 0) {
        get_alsa_error_text(pm_hosterror_text, PM_HOST_ERROR_MSG_LEN, err);
    }
    return pmHostError;
}

static PmError alsa_in_close(PmInternal *midi)
{
    int err;
    alsa_descriptor_type desc = (alsa_descriptor_type) midi->descriptor;
    if (!desc) return pmBadPtr;
    pm_hosterror = snd_seq_disconnect_from(seq, desc->this_port, 
                                               desc->client, desc->port);
    if (pm_hosterror) {
        snd_seq_delete_port(seq, desc->this_port); /* try to close port */
    } else {
        pm_hosterror = snd_seq_delete_port(seq, desc->this_port);
    }
    alsa_unuse_queue();
    pm_free(desc);
    if (pm_hosterror) {
        get_alsa_error_text(pm_hosterror_text, PM_HOST_ERROR_MSG_LEN, 
                            pm_hosterror);
        return pmHostError;
    }
    return pmNoError;
}
        

static PmError alsa_abort(PmInternal *midi)
{
    alsa_descriptor_type desc = (alsa_descriptor_type) midi->descriptor;
    /* This is supposed to flush any pending output. */
    printf("WARNING: alsa_abort not implemented\n");
    return pmNoError;
}


#ifdef GARBAGE
This is old code here temporarily for reference
static PmError alsa_write(PmInternal *midi, PmEvent *buffer, long length)
{
    alsa_descriptor_type desc = (alsa_descriptor_type) midi->descriptor;
    int i, bytes;
    unsigned char byte;
    long msg;

    desc->error = 0;
    for (; length > 0; length--, buffer++) {
        VERBOSE printf("message 0x%x\n", buffer->message);
        if (Pm_MessageStatus(buffer->message) == MIDI_SYSEX)
            desc->in_sysex = TRUE;
        if (desc->in_sysex) {
            msg = buffer->message;
            for (i = 0; i < 4; i++) {
                byte = msg;  /* extract next byte to send */
                alsa_write_byte(midi, byte, buffer->timestamp);
                if (byte == MIDI_EOX) {
                    desc->in_sysex = FALSE;
                    break;
                }
                if (desc->error < 0) break;
                msg >>= 8; /* shift next byte into position */
            }
        } else {
            bytes = midi_message_length(buffer->message);
            msg = buffer->message;
            for (i = 0; i < bytes; i++) {
                byte = msg; /* extract next byte to send */
                VERBOSE printf("sending 0x%x\n", byte);
                alsa_write_byte(midi, byte, buffer->timestamp);
                if (desc->error < 0) break;
                msg >>= 8; /* shift next byte into position */
            }
        }
    }
    if (desc->error < 0) return pmHostError;

    VERBOSE printf("snd_seq_drain_output: 0x%x\n", seq);
    desc->error = snd_seq_drain_output(seq);
    if (desc->error < 0) return pmHostError;

    desc->error = pmNoError;
    return pmNoError;
}
#endif


static PmError alsa_write_flush(PmInternal *midi)
{
    alsa_descriptor_type desc = (alsa_descriptor_type) midi->descriptor;
    VERBOSE printf("snd_seq_drain_output: %p\n", seq);
    desc->error = snd_seq_drain_output(seq);
    if (desc->error < 0) return pmHostError;

    desc->error = pmNoError;
    return pmNoError;
}


static PmError alsa_write_short(PmInternal *midi, PmEvent *event)
{
    int bytes = midi_message_length(event->message);
    long msg = event->message;
    int i;
    alsa_descriptor_type desc = (alsa_descriptor_type) midi->descriptor;
    for (i = 0; i < bytes; i++) {
        unsigned char byte = msg;
        VERBOSE printf("sending 0x%x\n", byte);
        alsa_write_byte(midi, byte, event->timestamp);
        if (desc->error < 0) break;
        msg >>= 8; /* shift next byte into position */
    }
    if (desc->error < 0) return pmHostError;
    desc->error = pmNoError;
    return pmNoError;
}


/* alsa_sysex -- implements begin_sysex and end_sysex */
PmError alsa_sysex(PmInternal *midi, PmTimestamp timestamp) {
    return pmNoError;
}


static PmTimestamp alsa_synchronize(PmInternal *midi)
{
    return 0; /* linux implementation does not use this synchronize function */
    /* Apparently, Alsa data is relative to the time you send it, and there
       is no reference. If this is true, this is a serious shortcoming of
       Alsa. If not true, then PortMidi has a serious shortcoming -- it 
       should be scheduling relative to Alsa's time reference. */
}


static void handle_event(snd_seq_event_t *ev)
{
    int device_id = ev->dest.port;
    PmInternal *midi = descriptors[device_id].internalDescriptor;
    PmEvent pm_ev;
    PmTimeProcPtr time_proc = midi->time_proc;
    PmTimestamp timestamp;

    /* time stamp should be in ticks, using our queue where 1 tick = 1ms */
    assert((ev->flags & SND_SEQ_TIME_STAMP_MASK) == SND_SEQ_TIME_STAMP_TICK);

    /* if no time_proc, just return "native" ticks (ms) */
    if (time_proc == NULL) {
        timestamp = ev->time.tick;
    } else { /* translate time to time_proc basis */
        snd_seq_queue_status_t *queue_status;
        snd_seq_queue_status_alloca(&queue_status);
        snd_seq_get_queue_status(seq, queue, queue_status);
        /* return (now - alsa_now) + alsa_timestamp */
        timestamp = (*time_proc)(midi->time_info) + ev->time.tick -
                    snd_seq_queue_status_get_tick_time(queue_status);
    }
    pm_ev.timestamp = timestamp;
    switch (ev->type) {
    case SND_SEQ_EVENT_NOTEON:
        pm_ev.message = Pm_Message(0x90 | ev->data.note.channel,
                                   ev->data.note.note & 0x7f,
                                   ev->data.note.velocity & 0x7f);
        pm_read_short(midi, &pm_ev);
        break;
    case SND_SEQ_EVENT_NOTEOFF:
        pm_ev.message = Pm_Message(0x80 | ev->data.note.channel,
                                   ev->data.note.note & 0x7f,
                                   ev->data.note.velocity & 0x7f);
        pm_read_short(midi, &pm_ev);
        break;
    case SND_SEQ_EVENT_KEYPRESS:
        pm_ev.message = Pm_Message(0xa0 | ev->data.note.channel,
                                   ev->data.note.note & 0x7f,
                                   ev->data.note.velocity & 0x7f);
        pm_read_short(midi, &pm_ev);
        break;
    case SND_SEQ_EVENT_CONTROLLER:
        pm_ev.message = Pm_Message(0xb0 | ev->data.note.channel,
                                   ev->data.control.param & 0x7f,
                                   ev->data.control.value & 0x7f);
        pm_read_short(midi, &pm_ev);
        break;
    case SND_SEQ_EVENT_PGMCHANGE:
        pm_ev.message = Pm_Message(0xc0 | ev->data.note.channel,
                                   ev->data.control.value & 0x7f, 0);
        pm_read_short(midi, &pm_ev);
        break;
    case SND_SEQ_EVENT_CHANPRESS:
        pm_ev.message = Pm_Message(0xd0 | ev->data.note.channel,
                                   ev->data.control.value & 0x7f, 0);
        pm_read_short(midi, &pm_ev);
        break;
    case SND_SEQ_EVENT_PITCHBEND:
        pm_ev.message = Pm_Message(0xe0 | ev->data.note.channel,
                            (ev->data.control.value + 0x2000) & 0x7f,
                            ((ev->data.control.value + 0x2000) >> 7) & 0x7f);
        pm_read_short(midi, &pm_ev);
        break;
    case SND_SEQ_EVENT_CONTROL14:
        if (ev->data.control.param < 0x20) {
            pm_ev.message = Pm_Message(0xb0 | ev->data.note.channel,
                                       ev->data.control.param,
                                       (ev->data.control.value >> 7) & 0x7f);
            pm_read_short(midi, &pm_ev);
            pm_ev.message = Pm_Message(0xb0 | ev->data.note.channel,
                                       ev->data.control.param + 0x20,
                                       ev->data.control.value & 0x7f);
            pm_read_short(midi, &pm_ev);
        } else {
            pm_ev.message = Pm_Message(0xb0 | ev->data.note.channel,
                                       ev->data.control.param & 0x7f,
                                       ev->data.control.value & 0x7f);

            pm_read_short(midi, &pm_ev);
        }
        break;
    case SND_SEQ_EVENT_SONGPOS:
        pm_ev.message = Pm_Message(0xf2,
                                   ev->data.control.value & 0x7f,
                                   (ev->data.control.value >> 7) & 0x7f);
        pm_read_short(midi, &pm_ev);
        break;
    case SND_SEQ_EVENT_SONGSEL:
        pm_ev.message = Pm_Message(0xf3,
                                   ev->data.control.value & 0x7f, 0);
        pm_read_short(midi, &pm_ev);
        break;
    case SND_SEQ_EVENT_QFRAME:
        pm_ev.message = Pm_Message(0xf1,
                                   ev->data.control.value & 0x7f, 0);
        pm_read_short(midi, &pm_ev);
        break;
    case SND_SEQ_EVENT_START:
        pm_ev.message = Pm_Message(0xfa, 0, 0);
        pm_read_short(midi, &pm_ev);
        break;
    case SND_SEQ_EVENT_CONTINUE:
        pm_ev.message = Pm_Message(0xfb, 0, 0);
        pm_read_short(midi, &pm_ev);
        break;
    case SND_SEQ_EVENT_STOP:
        pm_ev.message = Pm_Message(0xfc, 0, 0);
        pm_read_short(midi, &pm_ev);
        break;
    case SND_SEQ_EVENT_CLOCK:
        pm_ev.message = Pm_Message(0xf8, 0, 0);
        pm_read_short(midi, &pm_ev);
        break;
    case SND_SEQ_EVENT_TUNE_REQUEST:
        pm_ev.message = Pm_Message(0xf6, 0, 0);
        pm_read_short(midi, &pm_ev);
        break;
    case SND_SEQ_EVENT_RESET:
        pm_ev.message = Pm_Message(0xff, 0, 0);
        pm_read_short(midi, &pm_ev);
        break;
    case SND_SEQ_EVENT_SENSING:
        pm_ev.message = Pm_Message(0xfe, 0, 0);
        pm_read_short(midi, &pm_ev);
        break;
    case SND_SEQ_EVENT_SYSEX: {
        const BYTE *ptr = (const BYTE *) ev->data.ext.ptr;
        int i;
        long msg = 0;
        int shift = 0;
        if (!(midi->filters & PM_FILT_SYSEX)) {
            for (i = 0; i < ev->data.ext.len; i++) {
                pm_read_byte(midi, *ptr++, timestamp);
            }
        }
        break;
    }
    }
}

static PmError alsa_poll(PmInternal *midi)
{
    snd_seq_event_t *ev;
    while (snd_seq_event_input(seq, &ev) >= 0) {
        if (ev) {
            handle_event(ev);
        }
    }
    return pmNoError;
}


static unsigned int alsa_has_host_error(PmInternal *midi)
{
    alsa_descriptor_type desc = (alsa_descriptor_type) midi->descriptor;
    return desc->error;
}


static void alsa_get_host_error(PmInternal *midi, char *msg, unsigned int len)
{
    alsa_descriptor_type desc = (alsa_descriptor_type) midi->descriptor;
    int err = (pm_hosterror || desc->error);
    get_alsa_error_text(msg, len, err);
}


pm_fns_node pm_linuxalsa_in_dictionary = {
    none_write_short,
    none_sysex,
    none_sysex,
    none_write_byte,
    none_write_short,
    none_write_flush,
    alsa_synchronize,
    alsa_in_open,
    alsa_abort,
    alsa_in_close,
    alsa_poll,
    alsa_has_host_error,
    alsa_get_host_error
};

pm_fns_node pm_linuxalsa_out_dictionary = {
    alsa_write_short,
    alsa_sysex,
    alsa_sysex,
    alsa_write_byte,
    alsa_write_short, /* short realtime message */
    alsa_write_flush,
    alsa_synchronize,
    alsa_out_open, 
    alsa_abort, 
    alsa_out_close,
    none_poll,
    alsa_has_host_error,
    alsa_get_host_error
};


/* pm_strdup -- copy a string to the heap. Use this rather than strdup so 
 *    that we call pm_alloc, not malloc. This allows portmidi to avoid 
 *    malloc which might cause priority inversion. Probably ALSA is going
 *    to call malloc anyway, so this extra work here may be pointless.
 */
char *pm_strdup(const char *s)
{
    int len = strlen(s);
    char *dup = (char *) pm_alloc(len + 1);
    strcpy(dup, s);
    return dup;
}


PmError pm_linuxalsa_init( void )
{
    int  err;
    snd_seq_client_info_t *cinfo;
    snd_seq_port_info_t *pinfo;
    unsigned int caps;

    err = snd_seq_open(&seq, "default", SND_SEQ_OPEN_DUPLEX, SND_SEQ_NONBLOCK);
    if (err < 0) return pmHostError;   // FIXME: added return value -JGG
    
    snd_seq_client_info_alloca(&cinfo);
    snd_seq_port_info_alloca(&pinfo);

    snd_seq_client_info_set_client(cinfo, -1);
    while (snd_seq_query_next_client(seq, cinfo) == 0) {
        snd_seq_port_info_set_client(pinfo, snd_seq_client_info_get_client(cinfo));
        snd_seq_port_info_set_port(pinfo, -1);
        while (snd_seq_query_next_port(seq, pinfo) == 0) {
            if (snd_seq_port_info_get_client(pinfo) == SND_SEQ_CLIENT_SYSTEM)
                continue; /* ignore Timer and Announce ports on client 0 */
            caps = snd_seq_port_info_get_capability(pinfo);
            if (!(caps & (SND_SEQ_PORT_CAP_SUBS_READ | SND_SEQ_PORT_CAP_SUBS_WRITE)))
                continue; /* ignore if you cannot read or write port */
            if (caps & SND_SEQ_PORT_CAP_SUBS_WRITE) {
                if (pm_default_output_device_id == -1) 
                    pm_default_output_device_id = pm_descriptor_index;
                pm_add_device("ALSA",
                              pm_strdup(snd_seq_port_info_get_name(pinfo)),
                              FALSE,
                              MAKE_DESCRIPTOR(snd_seq_port_info_get_client(pinfo),
                                              snd_seq_port_info_get_port(pinfo)),
                              &pm_linuxalsa_out_dictionary);
            }
            if (caps & SND_SEQ_PORT_CAP_SUBS_READ) {
                if (pm_default_input_device_id == -1) 
                    pm_default_input_device_id = pm_descriptor_index;
                pm_add_device("ALSA",
                              pm_strdup(snd_seq_port_info_get_name(pinfo)),
                              TRUE,
                              MAKE_DESCRIPTOR(snd_seq_port_info_get_client(pinfo),
                                              snd_seq_port_info_get_port(pinfo)),
                              &pm_linuxalsa_in_dictionary);
            }
        }
    }
    return pmNoError;
}
    

void pm_linuxalsa_term(void)
{
    snd_seq_close(seq);
}
