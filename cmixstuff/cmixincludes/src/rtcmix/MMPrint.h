/* RTcmix - Copyright (C) 2004  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/

/* Class for accessing the print buffer used for printing messages in
	'imbedded' RTcmix apps (rtcmix~, iRTcmix, etc.)
	The class is static for printer buffer data

	the different print levels (set by optional param in print_on(lvl)):
		0 -- fatal errors (die() in message.c)
		1 -- print() and printf()'s (in minc/builtin.c)
		2 -- rterrors (rterror() in message.c)
		3 -- warn errors (rtcmix_warn() in message.c)
		4 -- advise notifications (rtcmix_advise() in message.c)
		5 -- all the rest

	Brad Garton, 12/2012
*/

#define SIZEOF_MMPRINTBUF 32768 // should move to dyn alloc at some point

#define MMP_FATAL			0
#define MMP_PRINTS		1
#define MMP_RTERRORS		2
#define MMP_WARN			3
#define MMP_ADVISE		4
#define MMP_PRINTALL		5

#ifdef __cplusplus

class MMPrint {
public:
	MMPrint() {};
	~MMPrint() {};

	static char mm_print_buf[];
	static char *mm_print_ptr;
};

extern "C" {
#endif // __cplusplus

char *get_mm_print_ptr();
void set_mm_print_ptr(int v);
#ifdef __cplusplus
} // extern "C"
#endif

