#ifdef sgi
#include <sys/fpu.h>

/*
THE FOLLOWING FUNCTION
sets the special "flush zero" but (FS, bit 24) in the
Control Status Register of the FPU of R4k and beyond
so that the result of any underflowing operation will
be clamped to zero, and no exception of any kind will
be generated on the CPU.  This has no effect on
an R3000.
*/

void flush_all_underflows_to_zero()
{
	union fpc_csr f;
	f.fc_word = get_fpc_csr();
	f.fc_struct.flush = 1;
	set_fpc_csr(f.fc_word);
}
#endif

#ifdef LINUX

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

void sigfpe_handler(int sig)
{
   fprintf(stderr,
           "\nRTcmix FATAL ERROR: floating point exception halted process.\n");
   exit(1);
}

#endif

