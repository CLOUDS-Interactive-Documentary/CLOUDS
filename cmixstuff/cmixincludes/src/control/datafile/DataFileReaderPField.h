/* RTcmix - Copyright (C) 2005  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/
#ifndef _DATAFILEREADERPFIELD_H_
#define _DATAFILEREADERPFIELD_H_

#include <PField.h>
#include <DataFile.h>

class Oonepole;

class DataFileReaderPField : public RTNumberPField {
public:
	DataFileReaderPField(
		const char   *fileName,
		const double lag,
		const int    controlRate,
		const double skipTime = 0.0,     // time to skip (pre-scale) before read
		const double timeFactor = 1.0,   // scale stream duration
		// Use the following if the data file has no header.
		const int    defaultFileRate = -1,
		const int    defaultFormat = kDataFormatFloat,
		const bool   defaultSwap = false);

	virtual double doubleValue(double) const;

protected:
	virtual ~DataFileReaderPField();

private:
	DataFile *_datafile;
	Oonepole *_filter;
};

#endif // _DATAFILEREADERPFIELD_H_
