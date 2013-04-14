// Lockable.h
//
// Base class for all classes which support mutex locking.
// Because the lock is not actually part of the class state,
// we set the mutex to be mutable and declare the methods const.	-- DS
// Class is entirely inline, so no .C source.
//

#ifndef _RT_LOCKABLE_H_
#define _RT_LOCKABLE_H_

#include <lock.h>

class Lockable {
public:
	Lockable() { pthread_mutex_init(&_mutex, NULL); }
	~Lockable() { pthread_mutex_destroy(&_mutex); }
	void		lock() const { pthread_mutex_lock(&_mutex); }
    bool        tryLock() const { return pthread_mutex_trylock(&_mutex) == 0; }
	void		unlock() const { pthread_mutex_unlock(&_mutex); }
protected:
	// This allows subclasses to create temp Lock class instances
	LockHandle	getLockHandle() const { return (LockHandle) &_mutex; }
private:
	mutable pthread_mutex_t _mutex;
	friend class AutoLock;
};

class AutoLock : public Lock {
public:
	AutoLock(Lockable *inLockable) : Lock(inLockable->getLockHandle()) {}
	AutoLock(Lockable &inLockable) : Lock(inLockable.getLockHandle()) {}
};
	
#endif	//	 _RT_LOCKABLE_H_

