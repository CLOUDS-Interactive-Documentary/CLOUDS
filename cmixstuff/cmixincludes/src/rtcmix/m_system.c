#include <stdlib.h>
#include <ugens.h>

double
m_system(float *p, short n_args, double *pp)
{
	char *command = DOUBLE_TO_STRING(pp[0]);

#ifdef IOS
	rtcmix_warn("system", "the system command %s not allowed on iDevices", command);
	return (-1.0);
#else
	return system(command);
#endif
}
