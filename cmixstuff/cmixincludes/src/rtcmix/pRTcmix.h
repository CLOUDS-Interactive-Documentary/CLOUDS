#include "RTcmix.h"

class PRTcmix : public RTcmix
{
public:
	PRTcmix();					// 44.1k/stereo default
	PRTcmix(float, int);		// set SR and NCHANS
	PRTcmix(float, int, int, const char *opt1=0, const char *opt2=0, const char *opt3=0);	// set SR, NCHANS, BUFSIZE, up to 3 options
	void perlparse(char *);
	double getfval(char *);
};
