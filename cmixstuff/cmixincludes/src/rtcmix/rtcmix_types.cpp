#include <rtcmix_types.h>
#include "utils.h"
#include <PField.h>
#include <string.h>		// for strcmp()
#include <stdlib.h>		// for free()
#include <MMPrint.h>

// NOTE:  For now, the Arg class does not reference and dereference the
// underlying RtcHandle.  This is OK because Args are always temporary storage
// and their scope is always local within the parser.

Arg::~Arg() {
	if (_type == ArrayType) {
		if (_val.array->data)
			free(_val.array->data);
		free(_val.array);
	}
//	else if (_type == HandleType)
//		unrefHandle(_val.handle);
}

bool
Arg::operator == (const char *str) const {
	return isType(StringType) && !strcmp(_val.string, str);
}

void
Arg::operator = (const RtcHandle h) {
	_type = HandleType;
	_val.handle = h;
//	refHandle(h);
}

#ifndef MAXMSP
void 
Arg::printInline(FILE *stream) const
{
	switch (type()) {
	case DoubleType:
		fprintf(stream, "%g ", _val.number);
		break;
	case StringType:
		fprintf(stream, "\"%s\" ", _val.string);
		break;
	case HandleType:
		if (_val.handle != NULL) {
			switch (_val.handle->type) {
			case PFieldType:
			{
				// Print PField start and end values.
				PField *pf = (PField *) _val.handle->ptr;
				double start = pf->doubleValue(0);
				double end = pf->doubleValue(1.0);
				fprintf(stream, "PF:[%g,...,%g] ", start, end);
				break;
			}
			case InstrumentPtrType:
				fprintf(stream, "Inst:%p ", _val.handle->ptr);
				break;
			case AudioStreamType:
				fprintf(stream, "AudioStr:%p", _val.handle->ptr);
				break;
			default:
				fprintf(stream, "Unknown ");
				break;
			}
		}
		else
			fprintf(stream, "NULL ");
		break;
	case ArrayType:
		fprintf(stream, "[%g,...,%g] ", _val.array->data[0],
				_val.array->data[_val.array->len - 1]);
		break;
	default:
		break;
	}
}

#else // MAXMSP
void 
Arg::printInline(FILE *stream) const
{
	switch (type()) {
	case DoubleType:
		MMPrint::mm_print_ptr += sprintf(MMPrint::mm_print_ptr, "%g ", _val.number);
		break;
	case StringType:
		MMPrint::mm_print_ptr += sprintf(MMPrint::mm_print_ptr, "\"%s\" ", _val.string);
		break;
	case HandleType:
		if (_val.handle != NULL) {
			switch (_val.handle->type) {
			case PFieldType:
			{
				// Print PField start and end values.
				PField *pf = (PField *) _val.handle->ptr;
				double start = pf->doubleValue(0);
				double end = pf->doubleValue(1.0);
				MMPrint::mm_print_ptr += sprintf(MMPrint::mm_print_ptr, "PF:[%g,...,%g] ", start, end);
				break;
			}
			case InstrumentPtrType:
				MMPrint::mm_print_ptr += sprintf(MMPrint::mm_print_ptr, "Inst:0x%p ", _val.handle->ptr);
				break;
			case AudioStreamType:
				MMPrint::mm_print_ptr += sprintf(MMPrint::mm_print_ptr, "AudioStr:0x%p", _val.handle->ptr);
				break;
			default:
				MMPrint::mm_print_ptr += sprintf(MMPrint::mm_print_ptr, "Unknown ");
				break;
			}
		}
		else
			MMPrint::mm_print_ptr += sprintf(MMPrint::mm_print_ptr, "NULL ");
		break;
	case ArrayType:
		MMPrint::mm_print_ptr += sprintf(MMPrint::mm_print_ptr, "[%g,...,%g] ", _val.array->data[0],
				_val.array->data[_val.array->len - 1]);
		break;
	default:
		break;
	}
}
#endif

