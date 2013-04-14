#include "RTcmix.h"

class PYRTcmix : public RTcmix
{
public:
	PYRTcmix();							// 44.1k/stereo default
	PYRTcmix(float, int);			// set SR and NCHANS
	PYRTcmix(float, int, int, const char *opt1=0, const char *opt2=0,
				const char *opt3=0);	// set SR, NCHANS, BUFSIZE, up to 3 options
	void pythonparse(char *);
	double getfval(char *);
};

