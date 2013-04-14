#include <RTcmixMouse.h>
#include <RTMousePField.h>
#include <stdio.h>
#include <unistd.h>

#define SLEEPMSEC 50 * 1000

int resetval = 1000;

int main()
{
	RTcmixMouse *mouse = createMouseWindow();

	int prec1 = 0;
	double min1 = 10;
	double max1 = 10000;
	double default1 = min1;
	double lag1 = 0.0;
	char *prefix1 = (char *) "freq";
	char *units1 = (char *) "Hz";
	RTMousePField *pf1 = new RTMousePField(mouse, kRTMouseAxisX, min1, max1,
				default1, lag1, prefix1, units1, prec1);

	int prec2 = 4;
	double min2 = 0;
	double max2 = 2;
	double default2 = min2;
	double lag2 = 0.0;
	char *prefix2 = (char *) "amp";
	char *units2 = NULL;
	RTMousePField *pf2 = new RTMousePField(mouse, kRTMouseAxisY, min2, max2,
				default2, lag2, prefix2, units2, prec2);

	double oldval1 = min1 - 1.0;
	double oldval2 = min2 - 1.0;
	while (1) {
		double val1 = pf1->doubleValue(0);
		if (val1 != oldval1) {
			printf("%s: %f\n", prefix1, val1);
			oldval1 = val1;
		}
		double val2 = pf2->doubleValue(0);
		if (val2 != oldval2) {
			printf("%s: %f\n", prefix2, val2);
			oldval2 = val2;
		}
		usleep(SLEEPMSEC);
	}

	return 0;
}


// satisfy linker for libgen.so;  FIXME: why doesn't this work??
extern "C" {
double *floc(int foo) { return NULL; }
int fsize(int foo) { return 0; }
}
