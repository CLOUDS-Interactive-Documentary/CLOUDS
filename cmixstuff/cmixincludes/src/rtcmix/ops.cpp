/* RTcmix  - Copyright (C) 2005  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/
// Functions to use when operator overloading is not available in the
// scripting language.  Currently, we can't do op overloading in Perl,
// so we use mul, add, div and sub functions as substitutes.   -JGG

#include "rtcmix_types.h"
#include <PField.h>
#include "utils.h"
#include <ugens.h>		// for warn, die

extern "C" {
	RtcHandle opmul(const Arg args[], const int nargs);
	RtcHandle opadd(const Arg args[], const int nargs);
	RtcHandle opdiv(const Arg args[], const int nargs);
	RtcHandle opsub(const Arg args[], const int nargs);
};


// ------------------------------------------------------------------- opmul ---
RtcHandle opmul(const Arg args[], const int nargs)
{
	if (nargs == 2) {
		PField *pf0 = (PField *) args[0];
		PField *pf1 = (PField *) args[1];
		if (!pf0 && args[0].isType(DoubleType))
			pf0 = new ConstPField((double) args[0]);
		if (!pf1 && args[1].isType(DoubleType))
			pf1 = new ConstPField((double) args[1]);
		if (pf0 && pf1)
			return createPFieldHandle(new MultPField(pf0, pf1));
	}
	die("mul", "Usage: pfield = mul(arg1, arg2)\n"
					"(Args can be pfields or constants.)");
	return NULL;
}


// ------------------------------------------------------------------- opadd ---
RtcHandle opadd(const Arg args[], const int nargs)
{
	if (nargs == 2) {
		PField *pf0 = (PField *) args[0];
		PField *pf1 = (PField *) args[1];
		if (!pf0 && args[0].isType(DoubleType))
			pf0 = new ConstPField((double) args[0]);
		if (!pf1 && args[1].isType(DoubleType))
			pf1 = new ConstPField((double) args[1]);
		if (pf0 && pf1)
			return createPFieldHandle(new AddPField(pf0, pf1));
	}
	die("add", "Usage: pfield = add(arg1, arg2)\n"
					"(Args can be pfields or constants.)");
	return NULL;
}


// ------------------------------------------------------------------- opdiv ---
static double _dodiv(double x, double y)
{
	return (y != 0) ? x / y : 999999999999999999.9;
}

RtcHandle opdiv(const Arg args[], const int nargs)
{
	if (nargs == 2) {
		PField *pf0 = (PField *) args[0];
		PField *pf1 = (PField *) args[1];
		if (!pf0 && args[0].isType(DoubleType))
			pf0 = new ConstPField((double) args[0]);
		if (!pf1 && args[1].isType(DoubleType))
			pf1 = new ConstPField((double) args[1]);
		if (pf0 && pf1)
			return createPFieldHandle(new PFieldBinaryOperator(pf0, pf1, _dodiv));
	}
	die("div", "Usage: pfield = div(arg1, arg2)\n"
					"(Args can be pfields or constants.)");
	return NULL;
}


// ------------------------------------------------------------------- opsub ---
static double _dosub(double x, double y)
{
	return x - y;
}

RtcHandle opsub(const Arg args[], const int nargs)
{
	if (nargs == 2) {
		PField *pf0 = (PField *) args[0];
		PField *pf1 = (PField *) args[1];
		if (!pf0 && args[0].isType(DoubleType))
			pf0 = new ConstPField((double) args[0]);
		if (!pf1 && args[1].isType(DoubleType))
			pf1 = new ConstPField((double) args[1]);
		if (pf0 && pf1)
			return createPFieldHandle(new PFieldBinaryOperator(pf0, pf1, _dosub));
	}
	die("sub", "Usage: pfield = sub(arg1, arg2)\n"
					"(Args can be pfields or constants.)");
	return NULL;
}


