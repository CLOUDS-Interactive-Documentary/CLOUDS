/* RTcmix  - Copyright (C) 2005  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
 */
// Functor.h
//
// Base classes for simple objects that perform a function on its input(s).
//

#ifndef _FUNCTOR_H_
#define _FUNCTOR_H_

class IIFunctor {
public:
	virtual int operator ()(int i1, int i2) = 0;
	virtual ~IIFunctor() {}
};

class DIFunctor {
public:
	virtual double operator ()(double d1, int i1) = 0;
	virtual ~DIFunctor() {}
};

class DDFunctor {
public:
	virtual double operator ()(double d1, double d2) = 0;
	virtual ~DDFunctor() {}
};

#endif	// _FUNCTOR_H_
