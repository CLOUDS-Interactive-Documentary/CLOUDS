/* RTcmix  - Copyright (C) 2000  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
 */
 
#ifndef _PFIELD_H_
#define _PFIELD_H_

#define DYNTABLETOKEN 0.787878    // for "dynamic" PField tables

#include <RefCounted.h>
#include <stdio.h>

// Base class for all PFields.  Value can be retrieved at any time in any
// of the 3 supported formats.

class PField : public RefCounted {
public:
#ifdef DEBUG
	PField();
#endif
	virtual double 	doubleValue(int indx = 0) const = 0;
	virtual double 	doubleValue(double dindex) const = 0;
	size_t 			intValue(double dindex) const { return (size_t) doubleValue(dindex); }
	const char * 	stringValue(double dindex) const;
	virtual int		print(FILE *) const;
	virtual operator double *() const { /* default is to */ return 0; }
	virtual int		copyValues(double *) const;
	virtual int		values() const = 0;
protected:
	virtual 		~PField();
};

// SingleValuePField is intermediate base class.

class SingleValuePField : public PField {
public:
	virtual double 	doubleValue(int indx = 0) const { return doubleValue(0.0); }
	virtual double 	doubleValue(double dindex) const;
	virtual int		values() const { return 1; }
protected:
	SingleValuePField(double val) : _value(val) {}
	double			setValue(double val) { return _value = val; }
private:
	double	_value;
};

// Constant-value PField used for all non-varying numeric parameters.

class ConstPField : public SingleValuePField {
public:
	ConstPField(double value);
protected:
	virtual 		~ConstPField();
};

// Constant-value PField used for all strings.

class StringPField : public PField {
public:
	StringPField(const char  *value);
	virtual double 	doubleValue(int indx = 0) const { return doubleValue(0.0); }
	virtual double	doubleValue(double) const;
	virtual int		print(FILE *) const;
	virtual int		values() const { return 1; }
protected:
	virtual 		~StringPField();
private:
	char	*_string;
};

// Constant-value PField used to pass Instrument pointers.

class Instrument;

class InstPField : public PField {
public:
	InstPField(Instrument *inst) : _instrument(inst) {}
	virtual double 	doubleValue(int indx = 0) const { return doubleValue(0.0); }
	virtual double	doubleValue(double) const { return 0.0; };
	virtual int		print(FILE *f) const { return fprintf(f, "Instrument %p", _instrument); }
	virtual int		values() const { return 1; }
	Instrument *	instrument() const { return _instrument; }
protected:
	virtual 		~InstPField() {}
private:
	Instrument	*_instrument;
};

// Base class for operator PFields.  These may be instantianted with an
// external Operator function.

class PFieldBinaryOperator : public PField {
public:
	typedef double (*Operator)(double, double);
	PFieldBinaryOperator(PField *pf1, PField *pf2, Operator);
	virtual double 	doubleValue(int indx = 0) const;
	virtual double	doubleValue(double) const;
	virtual int		print(FILE *) const;
	virtual int		copyValues(double *) const;
	virtual int		values() const;
protected:
	virtual 		~PFieldBinaryOperator();
private:
	PField	*_pfield1, *_pfield2;
	Operator _operator;
};

// PField which adds two other PFields

class AddPField : public PFieldBinaryOperator {
protected:
	static double Add(double x, double y) { return x + y; }
public:
	AddPField(PField *pf1, PField *pf2) : PFieldBinaryOperator(pf1, pf2, Add) {}
protected:
	virtual 		~AddPField() {}
};

// PField which multiplies two other PFields

class MultPField : public PFieldBinaryOperator {
protected:
	static double Mult(double x, double y) { return x * y; }
public:
	MultPField(PField *pf1, PField *pf2) : PFieldBinaryOperator(pf1, pf2, Mult) {}
protected:
	virtual 		~MultPField() {}
};

// Base class for all Real-Time-varying parameters.
// This may eventually hold common code needed to
// support multiple threads, etc.

class RTFieldObject {
public:
protected:
	virtual ~RTFieldObject() {}
};

// Variable PField used for simple real-time setting of params

class RTNumberPField : public SingleValuePField, public RTFieldObject {
public:
	RTNumberPField(double value);
	// Set value.  Returns same.
	virtual double	set(double);
	// Offset value by 'val'.  Returns new value.
	double offset(double val) { return set(doubleValue() + val); }
	// Overloaded operators for ease of use.
	double operator = (double value) { return set(value); }
	float operator = (float value) { return (float) set(value); }
	double operator += (double value) { return offset(value); }
	float operator += (float value) { return (float) offset(value); }
	double operator -= (double value) { return offset(-value); }
	float operator -= (float value) { return (float) offset(-value); }
protected:
	virtual 		~RTNumberPField() {}
};

// Class for low frequency control rate oscillator

class Ooscil;
class TablePField;

class LFOPField : public SingleValuePField {
public:
	typedef double (*InterpFunction)(Ooscil *);
	static double Truncate(Ooscil *);
	static double Interpolate1stOrder(Ooscil *);
public:
	LFOPField(double krate, TablePField *tablePField, PField *freq,
								InterpFunction fun=Interpolate1stOrder);
	virtual double	doubleValue(double) const;
protected:
	virtual ~LFOPField();
private:
	Ooscil *_oscil;
	TablePField *_tablePF;
	PField *_freqPF;
	InterpFunction	_interpolator;
};

// Class for random number generator

class Random;
class RandomOscil;

class RandomPField : public SingleValuePField {
public:
	// NB: RandomPField takes ownership of generator, deletes on destruction.
	RandomPField(double krate, Random *generator, PField *freq,
						PField *min, PField *max, PField *mid=NULL, PField *tight=NULL);
	virtual double	doubleValue(double) const;
protected:
	virtual ~RandomPField();
private:
	PField *_freqPF;
	PField *_minPF;
	PField *_maxPF;
	PField *_midPF;
	PField *_tightPF;
	RandomOscil *_randOscil;
};

// Class for interpolated reading of table.

class TablePField : public PField, public RTFieldObject {
public:
	typedef double (*InterpFunction)(double *, int, double);
	static double Truncate(double *, int, double);
	static double Interpolate1stOrder(double *, int, double);
	static double Interpolate2ndOrder(double *, int, double);
public:
	TablePField(double *tableArray, int length, InterpFunction fun=Interpolate1stOrder);
	virtual double 	doubleValue(int indx = 0) const;
	virtual double	doubleValue(double) const;
	virtual operator double *() const { return _table; }
	virtual int		print(FILE *) const;	// redefined
	virtual int		copyValues(double *) const;
	virtual int		values() const { return _len; }
	void setInterpFunction(InterpFunction fun) { _interpolator = fun; }
protected:
	virtual ~TablePField();
private:
	double				*_table;
	int 				_len;
	InterpFunction		_interpolator;
};

class PFieldWrapper : public PField {
public:
	virtual int		values() const { return _len; }
	virtual operator double *() const { return (double *) *_pField; }
protected:
	PFieldWrapper(PField *innerPField);
	virtual ~PFieldWrapper();
	PField *field() const { return _pField; }
private:
	PField *_pField;
	int		_len;
};

// Base class for specialized wrappers which perform transforms on the index
// passed to doubleValue() (and make no change to the actual values).

class IIFunctor;	// Takes 2 ints, and returns int.
class DIFunctor;	// Takes double, int, and returns double.

class ModifiedIndexPFieldWrapper : public PFieldWrapper {
protected:
	ModifiedIndexPFieldWrapper(PField *innerPField, IIFunctor *iif, DIFunctor *dif);
	virtual double	doubleValue(double didx) const;
	virtual double	doubleValue(int idx) const;
protected:
	virtual ~ModifiedIndexPFieldWrapper();
private:
	IIFunctor *_iifun;
	DIFunctor *_difun;
};

//#define OLD
// Class for looped reading of table.  'loopFactor' is how many times
// table will loop for doubleValue(0 through 1.0)

#ifdef OLD
class LoopedPField : public PFieldWrapper {
public:
	LoopedPField(PField *innerPField, double loopFactor);
	virtual double	doubleValue(double didx) const;
	virtual double	doubleValue(int idx) const;
private:
	double	_factor;
};
#else
class LoopedPField : public ModifiedIndexPFieldWrapper {
public:
	LoopedPField(PField *innerPField, double loopFactor);
private:
	class LoopIIFunctor;
	class LoopDIFunctor;
};

// Class for shifting table values by a number of array locations, with index
// computation performed on the fly.  Positive values of <shift> shift contents
// to the right; negative values to the left.  If a value is shifted off the end
// of the array in either direction, it reenters the other end.  -JGG

class ShiftPField : public ModifiedIndexPFieldWrapper {
public:
	ShiftPField(PField *innerPField, int shift);
private:
	class ShiftIIFunctor;
	class ShiftDIFunctor;
};
#endif

// Class for real-time rewriting of a table.

class DrawTablePField : public PFieldWrapper {
public:
	DrawTablePField(PField *innerPField, bool literalIndex, PField *indexPField,
		PField *valuePField, PField *widthPField);
	virtual double	doubleValue(double didx) const;
	virtual double	doubleValue(int idx) const;
protected:
	virtual ~DrawTablePField();
private:
	bool _literalIndex;
	PField *_indexPField;
	PField *_valuePField;
	PField *_widthPField;
};

// Class for concatenating two tables.

class ConcatTablePField : public PFieldWrapper {
public:
	ConcatTablePField(PField *innerPField, PField *table2);
	virtual double	doubleValue(double didx) const;
	virtual double	doubleValue(int idx) const;
	virtual int	values() const { return _values; }
protected:
	virtual ~ConcatTablePField();
private:
	PField *_table2;
	int _values;
	double _breakpct;
	double _scale1;
	double _scale2;
};

// Class for reverse-reading table.

class ReversePField : public PFieldWrapper {
public:
	ReversePField(PField *innerPField);
	virtual double	doubleValue(double didx) const;
	virtual double	doubleValue(int idx) const;
};

// Class for inverting PField output around a variable center of symmetry.

class InvertPField : public PFieldWrapper {
public:
	InvertPField(PField *innerPField, PField *centerPField);
	virtual double	doubleValue(double didx) const;
	virtual double	doubleValue(int idx) const;
protected:
	virtual ~InvertPField();
private:
	PField *_centerPField;
};

// Class for scaling normalized PField output into a range.

class RangePField : public PFieldWrapper {
public:
	typedef double (*RangeFitFunction)(const double, const double, const double);
	static double UnipolarSource(const double, const double, const double);
	static double BipolarSource(const double, const double, const double);
public:
	RangePField(PField *innerPField, PField *minPField, PField *maxPField,
												RangeFitFunction fun=UnipolarSource);
	virtual double	doubleValue(double didx) const;
	virtual double	doubleValue(int idx) const;
protected:
	virtual ~RangePField();
private:
	PField *_minPField;
	PField *_maxPField;
	RangeFitFunction _rangefitter;
};

// Class for smoothing a control signal.  Lag must be in range [0, 100].

class OonepoleTrack;

class SmoothPField : public PFieldWrapper {
public:
	SmoothPField(PField *innerPField, double krate, PField *lagPField,
		double initval = 0.0);
	virtual double	doubleValue(double didx) const;
	virtual double	doubleValue(int idx) const;
protected:
	virtual ~SmoothPField();
private:
	void updateCutoffFreq(double percent = 0.0) const;
	OonepoleTrack *_filter;
	PField *_lagPField;
};

// Class for delaying a control signal.

class Odelayi;

class DelayPField : public PFieldWrapper {
public:
	DelayPField(PField *innerPField, double krate, PField *deltimePField);
	virtual double	doubleValue(double didx) const;
	virtual double	doubleValue(int idx) const;
protected:
	virtual ~DelayPField();
private:
	void updateDelayTime(double percent = 0.0) const;
	double _krate;
	mutable double _curdeltime;
	Odelayi *_delay;
	PField *_deltimePField;
};

// Class for quantizing a control signal.

class QuantizePField : public PFieldWrapper {
public:
	QuantizePField(PField *innerPField, PField *quantumPField);
	virtual double	doubleValue(double didx) const;
	virtual double	doubleValue(int idx) const;
protected:
	virtual ~QuantizePField();
private:
	double quantizeValue(const double val, const double quantum) const;
	PField *_quantumPField;
};

// Class for clipping a control signal to extrema. <maxPField> can be NULL,
// in which case only the minimum is clipped.

class ClipPField : public PFieldWrapper {
public:
	ClipPField(PField *innerPField, PField *minPField, PField *maxPField);
	virtual double	doubleValue(double didx) const;
	virtual double	doubleValue(int idx) const;
protected:
	virtual ~ClipPField();
private:
	PField *_minPField;
	PField *_maxPField;
};

// Class for constraining a control signal to the nearest value in a table.
// <strength> [0,1] is the degree to which the input value approaches the
// corresponding table value.  0: not at all; 1: table value replaces input
// value; 0.5: output is halfway between input and table value; etc.   -JGG

// helper class
class Constrainer {
public:
	Constrainer(const double *table, const int tableLen);
	double next(const double val, const double strength);
private:
	const double *_table;
	const int _tableLen;
	double _lastVal, _lastTableVal;
};

class ConstrainPField : public PFieldWrapper {
public:
	ConstrainPField(PField *innerPField, TablePField *tablePField,
		PField *strengthPField);
	virtual double	doubleValue(double didx) const;
	virtual double	doubleValue(int idx) const;
protected:
	virtual ~ConstrainPField();
private:
	TablePField *_tablePField;
	PField *_strengthPField;
	Constrainer *_constrainer;
};

// Class for transforming a PField via a transfer function, supplied as
// a TablePField, which we assume to be in range [0,1].  Input range is
// fixed at construction.  -JGG

// helper class
class Mapper {
public:
	Mapper(TablePField *tablePF, const double inputMin, const double inputMax);
	double next(const double val);
private:
	TablePField *_tablePF;
	double _inputMin;
	double _inputDiff;
	double _lastVal;
	double _lastOutput;
};

class MapPField : public PFieldWrapper {
public:
	MapPField(PField *innerPField, TablePField *tablePField, 
		const double inputMin, const double inputMax);
	~MapPField();
	virtual double	doubleValue(double didx) const;
	virtual double	doubleValue(int idx) const;
private:
	TablePField *_tablePField;
	Mapper *_mapper;
};

// Class for writing PField data to a file.  The corresponding reader class
// is implemented as a plugin in ../control/datafile.   -JGG

class DataFile;

class DataFileWriterPField : public PFieldWrapper {
public:
	DataFileWriterPField(PField *innerPField, const char *fileName,
			const bool clobber, const int controlRate, const int fileRate,
			const int format = 0, const bool swap = false);
	virtual double doubleValue(double didx) const;
	virtual double doubleValue(int idx) const;
protected:
	virtual ~DataFileWriterPField();
private:
	// This is mutable to help with error-handling in doubleValue.
	mutable DataFile *_datafile;
};

// Class for converting values read from another PField.

class ConverterPField : public PFieldWrapper {
public:
	typedef double (*ConverterFunction)(double);
	static double ampdb(const double);
	static double cpsoct(const double);
	static double octcps(const double);
	static double octpch(const double);
	static double cpspch(const double);
	static double pchoct(const double);
	static double pchcps(const double);
	static double midipch(const double);
	static double pchmidi(const double);
	static double octmidi(const double);
	static double boost(const double);

	ConverterPField(PField *innerPField, ConverterFunction cfun);
	virtual double doubleValue(double percent) const;
	virtual double doubleValue(int indx = 0) const;
private:
	ConverterFunction _converter;
};


#endif	// _PFIELD_H_

