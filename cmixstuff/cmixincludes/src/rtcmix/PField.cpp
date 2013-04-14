/* RTcmix  - Copyright (C) 2000  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
 */
 
#include <PField.h>
#include <string.h>
#include <stdio.h>   // for snprintf
#include <math.h>
#include <float.h>
#include <Ougens.h>
#include "Functor.h"
#include <ugens.h>

inline int max(int x, int y) { return (x >= y) ? x : y; }
inline int min(int x, int y) { return (x < y) ? x : y; }

// PField

#ifdef DEBUG
PField::PField()
{
	this;
}
#endif

PField::~PField()
{
#ifdef DEBUG
	printf("PField::~PField (this = %p)\n", this);
#endif
}

// Return a string version of the pfield value if the pointer address seems
// to be valid, else NULL (which means the PField did not contain a string).

const char * 	
PField::stringValue(double dindex) const
{
	const size_t ivalue = intValue(dindex);
	return (const char *) ((ivalue < 0x100000) ? 0 : ivalue);
}

int PField::print(FILE *file) const
{
	int chars = 0;
	for (int n = 0; n < values(); ++n)
		chars += fprintf(file, "%.6f\n", doubleValue(n));
	return chars;
}

int PField::copyValues(double *array) const
{
	const int len = values();
	for (int n = 0; n < len; ++n)
		array[n] = doubleValue(n);
	return len;
}

// SingleValuePField

double SingleValuePField::doubleValue(double) const
{
	return _value;
}

// ConstPField

ConstPField::ConstPField(double value) : SingleValuePField(value) {}

ConstPField::~ConstPField() {}


// StringPField

StringPField::StringPField(const char *value)
	: _string(new char [strlen(value) + 1])
{
	strcpy(_string, value);
}

StringPField::~StringPField() { delete [] _string; }

// Note:  We use the same old trick here to pass the string as a double,
// but the conversion is entirely contained within the PField class system.
// PField::stringValue() casts it back to a const char *.

double StringPField::doubleValue(double) const
{
	return STRING_TO_DOUBLE(_string);
}

int StringPField::print(FILE *file) const
{
	return fprintf(file, "\"%s\"\n", _string);
}

// RTNumberPField

RTNumberPField::RTNumberPField(double value) : SingleValuePField(value) {}

double RTNumberPField::set(double value) { return setValue(value); }

// PFieldBinaryOperator

PFieldBinaryOperator::PFieldBinaryOperator(PField *pf1, PField *pf2,
										   PFieldBinaryOperator::Operator op)
	: _pfield1(pf1), _pfield2(pf2), _operator(op)
{
	_pfield1->ref();
	_pfield2->ref();
}

PFieldBinaryOperator::~PFieldBinaryOperator()
{
	_pfield2->unref();
	_pfield1->unref();
}

double PFieldBinaryOperator::doubleValue(int indx) const
{
	const int rindx = min(indx, values() - 1);
	return (*_operator)(_pfield1->doubleValue(rindx), _pfield2->doubleValue(rindx));
}

double PFieldBinaryOperator::doubleValue(double frac) const
{
	return (*_operator)(_pfield1->doubleValue(frac), _pfield2->doubleValue(frac));
}

int PFieldBinaryOperator::values() const
{
	const int len1 = _pfield1->values();
	const int len2 = _pfield2->values();
	return max(len1, len2);
}

int PFieldBinaryOperator::print(FILE *file) const
{
	const int len1 = _pfield1->values();
	const int len2 = _pfield2->values();
	int chars = 0;
	if (len1 >= len2) {
		for (int n = 0; n < len1; ++n) {
			double frac = (double) n / (len1 - 1);
			double value = (*_operator)(_pfield1->doubleValue(n), _pfield2->doubleValue(frac));
			chars += fprintf(file, "%.6f\n", value);
		}
	}
	else {
		for (int n = 0; n < len2; ++n) {
			double frac = (double) n / (len2 - 1);
			double value = (*_operator)(_pfield1->doubleValue(frac), _pfield2->doubleValue(n));
			chars += fprintf(file, "%.6f\n", value);
		}
	}
	return chars;
}

int PFieldBinaryOperator::copyValues(double *array) const
{
	const int len1 = _pfield1->values();
	const int len2 = _pfield2->values();
	if (len1 >= len2) {
		for (int n = 0; n < len1; ++n) {
			double frac = (double) n / (len1 - 1);
			double value = (*_operator)(_pfield1->doubleValue(n), _pfield2->doubleValue(frac));
			array[n] = value;
		}
		return len1;
	}
	else {
		for (int n = 0; n < len2; ++n) {
			double frac = (double) n / (len2 - 1);
			double value = (*_operator)(_pfield1->doubleValue(frac), _pfield2->doubleValue(n));
			array[n] = value;
		}
		return len2;
	}
}

// LFOPField

LFOPField::LFOPField(double krate, TablePField *tablePField,
		PField *freq, LFOPField::InterpFunction ifun)
	: SingleValuePField(0.0), _tablePF(tablePField), _freqPF(freq), _interpolator(ifun)
{
	_tablePF->ref();
	_freqPF->ref();
	double *table = (double *) *_tablePF;
	const int length = _tablePF->values();
	_oscil = new Ooscil(krate, _freqPF->doubleValue(0), table, length);
}

LFOPField::~LFOPField()
{
	delete _oscil;
	_freqPF->unref();
	_tablePF->unref();
}

double LFOPField::Truncate(Ooscil *oscil)
{
	return oscil->next();
}

double LFOPField::Interpolate1stOrder(Ooscil *oscil)
{
	return oscil->nexti();
}

double LFOPField::doubleValue(double percent) const
{
	if (percent > 1.0)
		percent = 1.0;
	_oscil->setfreq(_freqPF->doubleValue(percent));
	return (*_interpolator)(_oscil);
}

// RandomPField

#include <Random.h>

RandomPField::RandomPField(double krate, Random *generator, PField *freq,
		PField *min, PField *max, PField *mid, PField *tight)
	: SingleValuePField(0.0), _freqPF(freq), _minPF(min), _maxPF(max), _midPF(mid), _tightPF(tight)
{
	_freqPF->ref();
	_minPF->ref();
	_maxPF->ref();
	RefCounted::ref(_tightPF);
	RefCounted::ref(_midPF);
	_randOscil = new RandomOscil(generator, krate, _freqPF->doubleValue());
}

RandomPField::~RandomPField()
{
	delete _randOscil;
	RefCounted::unref(_tightPF);
	RefCounted::unref(_midPF);
	_maxPF->unref();
	_minPF->unref();
	_freqPF->unref();
}

double RandomPField::doubleValue(double percent) const
{
	if (percent > 1.0)
		percent = 1.0;
	_randOscil->setmin(_minPF->doubleValue(percent));
	_randOscil->setmax(_maxPF->doubleValue(percent));
	if (_midPF)
		_randOscil->setmid(_midPF->doubleValue(percent));
	if (_tightPF)
		_randOscil->settight(_tightPF->doubleValue(percent));
	_randOscil->setfreq(_freqPF->doubleValue(percent));
	return _randOscil->next();
}

// TablePField

TablePField::TablePField(double *tableArray,
						 int length,
						 TablePField::InterpFunction ifun)
	: _table(tableArray), _len(length), _interpolator(ifun)
{
}

TablePField::~TablePField()
{
	delete [] _table;
}

double TablePField::Truncate(double *tab, int len, double didx)
{
	const int idx = int(didx);
	return tab[idx];
}

double TablePField::Interpolate1stOrder(double *tab, int len, double didx)
{
	const int idx = int(didx);
	const int idx2 = min(idx + 1, len - 1);
	double frac = didx - idx;
	return tab[idx] + frac * (tab[idx2] - tab[idx]);
}

double TablePField::Interpolate2ndOrder(double *tab, int len, double didx)
{
	const int idx = int(didx);
	const int idx2 = min(idx + 1, len - 1);
	const int idx3 = min(idx + 2, len - 1);
	double frac = didx - idx;
	double a = tab[idx];
	double hy0 = a / 2.0;
	double hy2 = tab[idx3] / 2.0;
	double b = (-3.0 * hy0) + (2.0 * tab[idx2]) - hy2;
	double c = hy0 - tab[idx2] + hy2;
	return a + (b * frac) + (c * frac * frac);
}

double TablePField::doubleValue(int indx) const
{
	return _table[min(indx, values() - 1)];
}

double TablePField::doubleValue(double percent) const
{
	if (percent > 1.0)
		percent = 1.0;
	const int len = values();
	double didx = (len - 1) * percent;
	return (*_interpolator)(_table, len, didx);
}

int TablePField::print(FILE *file) const
{
	int chars = 0;
	for (int i = 0; i < values(); i++) {
		chars += fprintf(file, "%.6f\n", _table[i]);
	}
	return chars;
}

// Optimized version for table

int TablePField::copyValues(double *array) const
{
	const int len = values();
	for (int n = 0; n < len; ++n)
		array[n] = _table[n];
	return len;
}

// PFieldWrapper

PFieldWrapper::PFieldWrapper(PField *innerPField)
	: _pField(innerPField), _len(innerPField->values())
{
	_pField->ref();
}

PFieldWrapper::~PFieldWrapper() { _pField->unref(); }

// ModifiedIndexPFieldWrapper

ModifiedIndexPFieldWrapper::ModifiedIndexPFieldWrapper(PField *innerPField,
												       IIFunctor *iif, DIFunctor *dif)
	: PFieldWrapper(innerPField), _iifun(iif), _difun(dif)
{
}

ModifiedIndexPFieldWrapper::~ModifiedIndexPFieldWrapper()
{
	delete _iifun;
	delete _difun;
}

double
ModifiedIndexPFieldWrapper::doubleValue(double didx) const
{
	return field()->doubleValue((*_difun)(didx, values()));
}

double
ModifiedIndexPFieldWrapper::doubleValue(int idx) const
{
	return field()->doubleValue((*_iifun)(idx, values()));
}

// LoopedPField

#ifdef OLD	/* The code as it was as direct subclass of PFieldWrapper */

LoopedPField::LoopedPField(PField *innerPField, double loopFactor)
	: PFieldWrapper(innerPField), _factor(loopFactor)
{
}

double LoopedPField::doubleValue(double didx) const
{
	double dfrac = didx * _factor;
	while (dfrac > 1.0)
		dfrac -= 1.0;
	return field()->doubleValue(dfrac);
}  

double LoopedPField::doubleValue(int idx) const
{
	int nidx = int(idx * _factor);
	const int len = values();
	while (nidx >= len)
		nidx -= len;
	return field()->doubleValue(nidx);
}

#else

// These are the two Functors for LoopedPField - one for double indices,
// one for integer.

class LoopedPField::LoopIIFunctor : public IIFunctor {
public:
	LoopIIFunctor(double factor) : _factor(factor) {}
	virtual int operator ()(int i1, int i2);
private:
	double _factor;	
};

class LoopedPField::LoopDIFunctor : public DIFunctor {
public:
	LoopDIFunctor(double factor) : _factor(factor) {}
	virtual double operator ()(double d1, int i1);
private:
	double _factor;	
};

int LoopedPField::LoopIIFunctor::operator ()(int idx, int len)
{
	int nidx = int(idx * _factor);
	while (nidx >= len)
		nidx -= len;
	return nidx;
}

double LoopedPField::LoopDIFunctor::operator ()(double didx, int)
{
	double dfrac = didx * _factor;
	while (dfrac > 1.0)
		dfrac -= 1.0;
	return dfrac;
}  

// This is all we need to define for the actual LoopedPField class

LoopedPField::LoopedPField(PField *innerPField, double loopFactor)
	: ModifiedIndexPFieldWrapper(innerPField,
								 new LoopIIFunctor(loopFactor),
								 new LoopDIFunctor(loopFactor))
{
}

// ShiftPField (structured like LoopedPField)

class ShiftPField::ShiftIIFunctor : public IIFunctor {
public:
	ShiftIIFunctor(int shift) : _shift(shift) {}
	virtual int operator ()(int i1, int i2);
private:
	int _shift;
};

class ShiftPField::ShiftDIFunctor : public DIFunctor {
public:
	ShiftDIFunctor(int shift) : _shift(shift) {}
	virtual double operator ()(double d1, int i1);
private:
	int _shift;
};

int ShiftPField::ShiftIIFunctor::operator ()(int idx, int len)
{
	int nidx = idx - _shift;
	while (nidx < 0)
		nidx += len;
	while (nidx >= len)
		nidx -= len;
	return nidx;
}

double ShiftPField::ShiftDIFunctor::operator ()(double didx, int len)
{
	const double dlen = double(len);
	double index = (didx * dlen) - _shift;
	while (index < 0.0)
		index += dlen;
	while (index > dlen)
		index -= dlen;
	return index / (dlen - 1.0);
}  

ShiftPField::ShiftPField(PField *innerPField, int shift)
	: ModifiedIndexPFieldWrapper(innerPField,
								 new ShiftIIFunctor(shift),
								 new ShiftDIFunctor(shift))
{
}

#endif	// !OLD

// DrawTablePField

DrawTablePField::DrawTablePField(PField *innerPField, bool literalIndex,
	PField *indexPField, PField *valuePField, PField *widthPField)
	: PFieldWrapper(innerPField), _literalIndex(literalIndex),
	  _indexPField(indexPField), _valuePField(valuePField),
	  _widthPField(widthPField)
{
	_indexPField->ref();
	_valuePField->ref();
	_widthPField->ref();
}

DrawTablePField::~DrawTablePField()
{
	_widthPField->unref();
	_valuePField->unref();
	_indexPField->unref();
}

double DrawTablePField::doubleValue(double didx) const
{
	// get table and length; if it doesn't look like a table, do nothing
	double *table = (double *) *field();
	if (table == NULL)
		return 0.0;
	const int len = values();
	if (len < 0)
		return 0.0;
	const int lastindex = len - 1;

	// get target index
	double dindex = _indexPField->doubleValue(didx);
	int targetindex = _literalIndex ? int(dindex) : int(dindex * len);
	if (targetindex < 0)
		targetindex = 0;
	else if (targetindex > lastindex)
		targetindex = lastindex;

	// new value to place at targeted index
	const double newval = _valuePField->doubleValue(didx);
	const double curval = table[targetindex];
	if (curval == newval)
		return 0.0;		// no change to table

	// printf("[%d]\tcur=%f, new=%f\n", targetindex, curval, newval);

	// The width PField defines the range of values affected.  From it we derive
	// three points across <span> indices: start, target, stop.  Target gets
	// <newval>; start keeps its current val; then we linearly interpolate
	// between start's val and <newval> across the intervening indices.  Same
	// thing between target and stop indices.

	double dwidth = _widthPField->doubleValue(didx);
	int span = _literalIndex ? int(dwidth) : int(dwidth * len);
	if (span < 0)
		span = 0;
	if (span > 0) {
		// set up first segment, between start and target
		int startindex = targetindex - (span >> 1);
		if (startindex < 0)
			startindex = 0;
		const double startval = table[startindex];
		const int seg1numvals = targetindex - startindex;
		const double seg1incr = (seg1numvals == 0) ? 0.0
										: (newval - startval) / seg1numvals;
		// set up second segment, between target and stop
		int stopindex = targetindex + (span >> 1);
		if (stopindex > lastindex)
			stopindex = lastindex;
		const double stopval = table[stopindex];
		const int seg2numvals = stopindex - targetindex;
		const double seg2incr = (seg2numvals == 0) ? 0.0
										: (stopval - newval) / seg2numvals;

		// change table values in place
		// caution: startindex and/or stopindex might == targetindex
		for (int i = startindex; i < stopindex; i++) {
			if (i < targetindex) {
				int diff = i - startindex;
				table[i] = startval + (diff * seg1incr);
			}
			else if (i == targetindex)
				table[i] = newval;
			else if (i > targetindex) {
				int diff = seg2numvals - (stopindex - i);
				table[i] = newval + (diff * seg2incr);
			}
		}
	}
	else
		table[targetindex] = newval;

	return 1.0;		// table was changed
}

double DrawTablePField::doubleValue(int idx) const
{
	int endidx = values() - 1;
	double percent = (endidx == 0) ? 0.0 : (double) idx / endidx;
	return doubleValue(percent);
}

// ConcatTablePField

ConcatTablePField::ConcatTablePField(PField *innerPField, PField *table2)
	: PFieldWrapper(innerPField), _table2(table2)
{
	_table2->ref();
	const int len1 = field()->values();
	const int len2 = _table2->values();
	_values = len1 + len2;
	_breakpct = double(len1) / _values;
	_scale1 = 1.0 / _breakpct;
	_scale2 = 1.0 / (1.0 - _breakpct);
}

ConcatTablePField::~ConcatTablePField()
{
	_table2->unref();
}

double ConcatTablePField::doubleValue(double didx) const
{
	if (didx > 1.0)
		didx = 1.0;
	if (didx < _breakpct)
		return field()->doubleValue(didx * _scale1);
	else
		return _table2->doubleValue((didx - _breakpct) * _scale2);
}

double ConcatTablePField::doubleValue(int idx) const
{
	const int len1 = field()->values();
	if (idx < len1)
		return field()->doubleValue(idx);
	if (idx >= _values)
		idx = _values - 1;
	return _table2->doubleValue(idx - len1);
}

// ReversePField

ReversePField::ReversePField(PField *innerPField) : PFieldWrapper(innerPField)
{
}

double ReversePField::doubleValue(double didx) const
{
	if (didx > 1.0)
		didx = 1.0;
	return field()->doubleValue(1.0 - didx);
}

double ReversePField::doubleValue(int idx) const
{
	const int len = values();
	return field()->doubleValue((len - 1) - idx);
}

// InvertPField

InvertPField::InvertPField(PField *innerPField, PField *centerPField)
	: PFieldWrapper(innerPField), _centerPField(centerPField)
{
	_centerPField->ref();
}

InvertPField::~InvertPField()
{
	_centerPField->unref();
}

double InvertPField::doubleValue(double didx) const
{
	const double center = _centerPField->doubleValue(didx);
	const double diff = field()->doubleValue(didx) - center;
	return center - diff;
}

double InvertPField::doubleValue(int idx) const
{
	int endidx = values() - 1;
	double frac = (endidx == 0) ? 0.0 : (double) idx / endidx;
	const double center = _centerPField->doubleValue(frac);
	const double diff = field()->doubleValue(idx) - center;
	return center - diff;
}

// RangePField

RangePField::RangePField(PField *innerPField,
						 PField *minPField, PField *maxPField,
						 RangePField::RangeFitFunction fun)
	: PFieldWrapper(innerPField),
	  _minPField(minPField), _maxPField(maxPField), _rangefitter(fun)
{
	_minPField->ref();
	_maxPField->ref();
}

RangePField::~RangePField()
{
	_maxPField->unref();
	_minPField->unref();
}

// Assumes val is in range [0, 1]
double RangePField::UnipolarSource(const double val, const double min, const double max)
{
	return min + (val * (max - min));
}

// Assumes val is in range [-1, 1]
double RangePField::BipolarSource(const double val, const double min, const double max)
{
	return min + ((val + 1.0) * 0.5 * (max - min));
}

double RangePField::doubleValue(double didx) const
{
	const double min = _minPField->doubleValue(didx);
	const double max = _maxPField->doubleValue(didx);
	const double normval = field()->doubleValue(didx);
	return (*_rangefitter)(normval, min, max);
}

double RangePField::doubleValue(int idx) const
{
	int endidx = values() - 1;
	double frac = (endidx == 0) ? 0.0 : (double) idx / endidx;
	const double min = _minPField->doubleValue(frac);
	const double max = _maxPField->doubleValue(frac);
	const double normval = field()->doubleValue(idx);
	return (*_rangefitter)(normval, min, max);
}

// SmoothPField

SmoothPField::SmoothPField(PField *innerPField, double krate, PField *lagPField,
	double initval)
	: PFieldWrapper(innerPField), _lagPField(lagPField)
{
	_lagPField->ref();
	_filter = new OonepoleTrack(krate);
	_filter->sethist(initval);
	updateCutoffFreq();
}

SmoothPField::~SmoothPField()
{
	delete _filter;
	_lagPField->unref();
}

void SmoothPField::updateCutoffFreq(double percent) const
{
	const double lag = _lagPField->doubleValue(percent);
	_filter->setlag(lag * 0.01);
}

double SmoothPField::doubleValue(double didx) const
{
	updateCutoffFreq(didx);
	return _filter->next(field()->doubleValue(didx));
}

double SmoothPField::doubleValue(int idx) const
{
	int endidx = values() - 1;
	double frac = (endidx == 0) ? 0.0 : (double) idx / endidx;
	updateCutoffFreq(frac);
	return _filter->next(field()->doubleValue(idx));
}

// DelayPField

DelayPField::DelayPField(PField *innerPField, double krate, PField *deltimePField)
	: PFieldWrapper(innerPField), _krate(krate), _curdeltime(-DBL_MAX),
		_deltimePField(deltimePField)
{
	_deltimePField->ref();
	_delay = new Odelayi((long) _krate);	// 1 second max by default (can grow)
	_delay->fill(field()->doubleValue());
	updateDelayTime();
}

DelayPField::~DelayPField()
{
	delete _delay;
	_deltimePField->unref();
}

void DelayPField::updateDelayTime(double percent) const
{
	const double deltime = _deltimePField->doubleValue(percent);
	if (deltime != _curdeltime) {
		_delay->setdelay((deltime * _krate) + 0.5);
		_curdeltime = deltime;
	}
}

double DelayPField::doubleValue(double didx) const
{
	updateDelayTime(didx);
	return _delay->next(field()->doubleValue(didx));
}

double DelayPField::doubleValue(int idx) const
{
	int endidx = values() - 1;
	double frac = (endidx == 0) ? 0.0 : (double) idx / endidx;
	updateDelayTime(frac);
	return _delay->next(field()->doubleValue(idx));
}

// QuantizePField

QuantizePField::QuantizePField(PField *innerPField, PField *quantumPField)
	: PFieldWrapper(innerPField), _quantumPField(quantumPField)
{
	_quantumPField->ref();
}

QuantizePField::~QuantizePField()
{
	_quantumPField->unref();
}

double QuantizePField::quantizeValue(const double val, const double quantum) const
{
	const double quotient = fabs(val / quantum);
	int floor = int(quotient);
	const double remainder = quotient - double(floor);
	if (remainder >= 0.5)		// round to nearest
		floor++;
	if (val < 0.0)
		return -floor * quantum;
	return floor * quantum;
}

double QuantizePField::doubleValue(double didx) const
{
	return quantizeValue(field()->doubleValue(didx), _quantumPField->doubleValue(didx));
}

double QuantizePField::doubleValue(int idx) const
{
	int endidx = values() - 1;
	double frac = (endidx == 0) ? 0.0 : (double) idx / endidx;
	double quantum = _quantumPField->doubleValue(frac);
	return quantizeValue(field()->doubleValue(idx), quantum);
}

// ClipPField

ClipPField::ClipPField(PField *innerPField, PField *minPField, PField *maxPField)
	: PFieldWrapper(innerPField), _minPField(minPField), _maxPField(maxPField)
{
	_minPField->ref();
	if (_maxPField)
		_maxPField->ref();
}

ClipPField::~ClipPField()
{
	if (_maxPField)
		_maxPField->unref();
	_minPField->unref();
}

double ClipPField::doubleValue(double didx) const
{
	double val = field()->doubleValue(didx);
	const double min = _minPField->doubleValue(didx);
	if (val < min)
		return min;
	else if (_maxPField) {
		const double max = _maxPField->doubleValue(didx);
		if (val > max)
			return max;
	}
	return val;
}

double ClipPField::doubleValue(int idx) const
{
	double val = field()->doubleValue(idx);
	int endidx = values() - 1;
	double frac = (endidx == 0) ? 0.0 : (double) idx / endidx;
	const double min = _minPField->doubleValue(frac);
	if (val < min)
		return min;
	else if (_maxPField) {
		const double max = _maxPField->doubleValue(frac);
		if (val > max)
			return max;
	}
	return val;
}

// ConstrainPField

// helper class
Constrainer::Constrainer(const double *table, const int tableLen)
	: _table(table), _tableLen(tableLen), _lastVal(DBL_MAX), _lastTableVal(0.0)
{
}

double Constrainer::next(const double val, const double strength)
{
	if (val != _lastVal && _table) {
		double min = DBL_MAX;
		int closest = 0;
		for (int i = 0; i < _tableLen; i++) {
			const double proximity = fabs(_table[i] - val);
			if (proximity < min) {
				min = proximity;
				closest = i;
			}
		}
		_lastTableVal = _table[closest];
		_lastVal = val;
	}
	if (strength == 0.0)
		return _lastVal;
	else if (strength == 1.0)
		return _lastTableVal;
	else
		return _lastVal + ((_lastTableVal - _lastVal) * strength);
}

ConstrainPField::ConstrainPField(PField *innerPField, TablePField *tablePField,
		PField *strengthPField)
	: PFieldWrapper(innerPField),
	  _tablePField(tablePField), _strengthPField(strengthPField)
{
	_tablePField->ref();
	_strengthPField->ref();
	const double *table = (double *) *_tablePField;
	_constrainer = new Constrainer(table, _tablePField->values());
}

ConstrainPField::~ConstrainPField()
{
	delete _constrainer;
	_strengthPField->unref();
	_tablePField->unref();
}

double ConstrainPField::doubleValue(double didx) const
{
	return _constrainer->next(field()->doubleValue(didx), _strengthPField->doubleValue(didx));
}

double ConstrainPField::doubleValue(int idx) const
{
	int endidx = values() - 1;
	double frac = (endidx == 0) ? 0.0 : (double) idx / endidx;
	return _constrainer->next(field()->doubleValue(idx), _strengthPField->doubleValue(frac));
}

// MapPField

// helper class
Mapper::Mapper(TablePField *tablePF, const double inputMin, const double inputMax)
	: _tablePF(tablePF), _inputMin(inputMin), _lastVal(DBL_MAX), _lastOutput(0.0)
{
	_inputDiff = inputMax - inputMin;
}

double Mapper::next(const double val)
{
	if (val != _lastVal) {
		// normalize inputVal to [0,1] and treat as table percentage
		double percent = (val - _inputMin) / _inputDiff;
		double tabval = _tablePF->doubleValue(percent);
		// map table value to input range
		_lastOutput = _inputMin + (tabval * _inputDiff);
		_lastVal = val;
	}
	return _lastOutput;
}

MapPField::MapPField(PField *innerPField, TablePField *tablePField,
		const double inputMin, const double inputMax)
	: PFieldWrapper(innerPField), _tablePField(tablePField)
{
	_tablePField->ref();
	_mapper = new Mapper(_tablePField, inputMin, inputMax);
}

MapPField::~MapPField()
{
	delete _mapper;
	_tablePField->unref();
}

double MapPField::doubleValue(double didx) const
{
	return _mapper->next(field()->doubleValue(didx));
}

double MapPField::doubleValue(int idx) const
{
	return _mapper->next(field()->doubleValue(idx));
}

// DataFileWriterPField

#include "DataFile.h"

DataFileWriterPField::DataFileWriterPField(PField *innerPField,
		const char *fileName, const bool clobber, const int controlRate,
		const int fileRate, const int format, const bool swap)
	: PFieldWrapper(innerPField)
{
	_datafile = new DataFile(fileName, controlRate);
	if (_datafile) {
		if (_datafile->openFileWrite(clobber) == 0)
			_datafile->writeHeader(fileRate, format, swap);
		else {
			delete _datafile;
			_datafile = NULL;
		}
	}
}

DataFileWriterPField::~DataFileWriterPField()
{
	delete _datafile;
}

double DataFileWriterPField::doubleValue(double didx) const
{
	const double val = field()->doubleValue(didx);
	if (_datafile) {
		if (_datafile->writeOne(val) != 0) {
			delete _datafile;
			_datafile = NULL;
		}
	}
	return val;
}

double DataFileWriterPField::doubleValue(int idx) const
{
	const double val = field()->doubleValue(idx);
	if (_datafile) {
		if (_datafile->writeOne(val) != 0) {
			delete _datafile;
			_datafile = NULL;
		}
	}
	return val;
}

// ConverterPField

ConverterPField::ConverterPField(PField *innerPField,
					 ConverterPField::ConverterFunction cfun)
	: PFieldWrapper(innerPField), _converter(cfun)
{
}

double ConverterPField::doubleValue(double percent) const
{
	double val = field()->doubleValue(percent);
	return (*_converter)(val);
}  

double ConverterPField::doubleValue(int idx) const
{
	double val = field()->doubleValue(idx);
	return (*_converter)(val);
}  

#include <ugens.h>

double ConverterPField::ampdb(const double db)
{
	return ::ampdb(db);
}

double ConverterPField::cpsoct(const double oct)
{
	return ::cpsoct(oct);
}

double ConverterPField::octcps(const double cps)
{
	return ::octcps(cps);
}

double ConverterPField::octpch(const double pch)
{
	return ::octpch(pch);
}

double ConverterPField::cpspch(const double pch)
{
	return ::cpspch(pch);
}

double ConverterPField::pchoct(const double oct)
{
	return ::pchoct(oct);
}

double ConverterPField::pchcps(const double cps)
{
	return ::pchcps(cps);
}

double ConverterPField::midipch(const double pch)
{
	return ::midipch(pch);
}

double ConverterPField::pchmidi(const double midi)
{
	return ::pchmidi((unsigned char) midi);
}

double ConverterPField::octmidi(const double midi)
{
	return ::octmidi((unsigned char) midi);
}

// NB: returns an amp multiplier; feed this to an amp pfield, not a pan pfield
double ConverterPField::boost(const double pan)
{
	return ::boost(pan);
}

