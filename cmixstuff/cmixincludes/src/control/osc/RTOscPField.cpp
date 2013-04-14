// RTcmix - Copyright (C) 2005  The RTcmix Development Team
// See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
// the license to this software and for a DISCLAIMER OF ALL WARRANTIES.

#include <RTOscPField.h>
#include <RTcmixOSC.h>
#include <PField.h>
#include <Ougens.h>
#include <string.h>
#include <unistd.h>
#include <float.h>
#include <assert.h>

#define DEBUG 0

extern int resetval;		// declared in src/rtcmix/minc_functions.c

const double kInvalidValue = DBL_MAX;

RTOscPField::RTOscPField(
		RTcmixOSC			*oscserver,
		const char 			*path,
		const int			index,
		const double		inputmin,
		const double		inputmax,
		const double		outputmin,
		const double		outputmax,
		const double		defaultval,
		const double		lag)				// in range [0, 1]
	: RTNumberPField(0),
	  _oscserver(oscserver), _index(index), _badMessages(0), _lastBadArgc(0),
	  _inputmin(inputmin), _inputmax(inputmax), _outputmin(outputmin),
	  _default(defaultval), _rawvalue(kInvalidValue), _callbackReturn(0)
{
	assert(_oscserver != NULL);
	assert(_index >= 0);

	_path = new char [strlen(path) + 1];
	strcpy(_path, path);

	_factor = (outputmax - _outputmin) / (_inputmax - _inputmin);

	// NOTE: We rely on the control rate in effect when this PField is created.
	_filter = new Oonepole(resetval);
	_filter->sethist(_default);
	_filter->setlag(lag);

	while (!_oscserver->ready())
		usleep(100);
	_oscserver->registerPField(this, handler);
}

RTOscPField::~RTOscPField()
{
	// Prevent our handler from continuing to be called even after we delete
	// this PField.
	_oscserver->unregisterPField(this);

	delete [] _path;
	delete _filter;

	if (_badMessages > 0)
		fprintf(stderr, "WARNING: OSC index out of range for %d messages.\n",
		                                                       _badMessages);
}

double RTOscPField::doubleValue(double) const
{
	// map _rawvalue, clamped to input range, into output range
	double val = _rawvalue;
	if (val == kInvalidValue)
		val = _default;
	else {
		if (val < _inputmin)
			val = _inputmin;
		else if (val > _inputmax)
			val = _inputmax;
		val = ((val - _inputmin) * _factor) + _outputmin;
	}
	return _filter->next(val);
}

int RTOscPField::handler(const char *path, const char *types, lo_arg **argv,
		int argc, lo_message msg, void *context)
{
	RTOscPField *pfield = (RTOscPField *) context;
	assert(pfield != NULL);

#if DEBUG > 1
	printf("rcv OSC msg: %s, argc=%d: ", path, argc);
	for (int i = 0; i < argc; i++) {
		lo_arg_pp((lo_type) types[i], argv[i]);
		if (i == argc - 1)
			printf("\n");
		else
			printf(", ");
	}
#endif

	const int index = pfield->index();
	if (index < argc) {
		lo_type type = (lo_type) types[index];
		if (type == LO_FLOAT)	// the most common one
			pfield->rawvalue(argv[index]->f);
		else if (lo_is_numerical_type(type)) {
			double val = lo_hires_val(type, argv[index]);
			pfield->rawvalue(val);
		}
		else
			fprintf(stderr, "WARNING: incoming OSC value of type '%c' can't "
								 "be coerced to double.\n", type);
	}
	else {
		if (pfield->badMessages() == 0 || pfield->lastBadArgc() != argc) {
			fprintf(stderr, "WARNING: OSC index (%d) out of range. Message has "
		           "only %d items. (Others skipped silently.)\n", index, argc);
			pfield->lastBadArgc(argc);
		}
		pfield->incrementBadMessages();
	}

	return pfield->callbackReturn();
}

