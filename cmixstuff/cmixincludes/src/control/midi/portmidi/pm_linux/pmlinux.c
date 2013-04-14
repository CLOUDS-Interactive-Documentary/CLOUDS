/* pmlinux.c -- PortMidi os-dependent code */

/* This file only needs to implement pm_init(), which calls various
   routines to register the available midi devices. This file must
   be separate from the main portmidi.c file because it is system
   dependent, and it is separate from, pmlinuxalsa.c, because it
   might need to register non-alsa devices as well.
 */

#include "stdlib.h"
#include "portmidi.h"
#ifdef PMALSA
  #include "pmlinuxalsa.h"
#endif

#ifdef PMNULL
  #include "pmlinuxnull.h"
#endif

PmError pm_init()
{
    #ifdef PMALSA
	return pm_linuxalsa_init();
    #endif
    #ifdef PMNULL
        return pm_linuxnull_init();
    #endif
}

void pm_term(void)
{
    #ifdef PMALSA
        pm_linuxalsa_term();
    #endif
}

PmDeviceID pm_default_input_device_id = -1;
PmDeviceID pm_default_output_device_id = -1;

PmDeviceID Pm_GetDefaultInputDeviceID() { 
    return pm_default_input_device_id; 
}

PmDeviceID Pm_GetDefaultOutputDeviceID() { 
    return pm_default_output_device_id; 
}

void *pm_alloc(size_t s) { return malloc(s); }

void pm_free(void *ptr) { free(ptr); }

