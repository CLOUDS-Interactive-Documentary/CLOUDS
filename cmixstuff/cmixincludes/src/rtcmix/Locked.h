//
// C++ Interface: Locked
//
// Description: Template classes which allow thread-safe setting/retreiving of values.
//
//
// Author: Douglas Scott <netdscott@netscape.net>, (C) 2010
//
//
//

#ifndef RT_LOCKED_H_
#define RT_LOCKED_H_

#include "Lockable.h"

template <typename T>
class Locked : public Lockable {
private:
	T	mValue;
public:
	explicit Locked(T inValue = T(0)) : mValue(inValue) {}
	operator T() const { return mValue; }
	Locked & operator = (T rhs) { mValue = rhs; return *this; }
};

#endif	// RT_LOCKED_H_
