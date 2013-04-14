// RefCounted.h
//
// Base class for objects which are held by reference in multiple locations.
//

#ifndef _RT_REFCOUNTED_H_
#define _RT_REFCOUNTED_H_

class RefCounted {
public:
	int ref() { return ++_refcount; }
	int unref();
	static void ref(RefCounted *r);
	static int unref(RefCounted *r);
protected:
	RefCounted() : _refcount(0) {}
	virtual ~RefCounted();
private:
	int _refcount;
};

#endif	//	 _RT_REFCOUNTED_H_
