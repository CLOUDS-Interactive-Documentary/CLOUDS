/* This file contains prototypes for functions used by the RTcmix core,
   not by instruments or utility programs.   -JGG
*/
#ifndef _PROTOTYPES_H_ 
#define PROTOTYPES_H_ 1

#include "buffers.h"    /* for BufPtr */
#include "rtcmix_types.h"

#ifdef __cplusplus

// These functions are declared as C++ functions.

class AudioDevice;

/* dispatch.cpp */

double dispatch(const char *str, double *pp, int n_args, void **inst);

/* Note that C++ functions prototyped below really are defined within
   extern "C" braces in their files.
*/
extern "C" {

#endif /* __cplusplus */

/* buffers.c:  prototypes in buffers.h */

/* checkfuncs.c */
double checkfuncs(const char *fname, double *pp, int n_args);

/* this is just a wrapper for C files to call */
double parse_dispatch(const char *str, double *pp, int n_args, void **inst);

/* parseit.C */
void *parseit(void *);

/* rtdispatch.C */
double rtdispatch(char *fname, double *pp, int n_args, void **inst);

/* rtgetin.C */
int read_samps(int fd, int data_format, int file_chans, BufPtr dest,
                                    int dest_chans, int dest_frames);

/* rtinput.c */
int get_last_input_index(void);
int open_sound_file(const char *funcname, const char *sfname, int *header_type,
                    int *data_format, int *data_location, double *srate,
                    int *nchans, long *nsamps);

/* audio_devices.cpp */
void stop_audio_devices();
void destroy_audio_devices();

/* rtcmix_wrappers.c */

int rtsetparams_was_called();
float SR();
void set_SR(float);

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif /* _PROTOTYPES_H_ */
