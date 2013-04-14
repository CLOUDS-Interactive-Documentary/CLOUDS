/* RTcmix  - Copyright (C) 2000  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
  the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/

#ifndef _RTCMIX_LOCK_H_
#define _RTCMIX_LOCK_H_

#include <pthread.h>

// Lock class.

typedef void *LockHandle;

class Lock {
public:
	inline Lock(LockHandle h);
	inline ~Lock();
	inline void Unlock();
private:
	LockHandle _handle;
};

inline Lock::Lock(LockHandle h) : _handle(h)
{
	pthread_mutex_t *mutex = (pthread_mutex_t *) _handle;
	pthread_mutex_lock(mutex);
}

inline void Lock::Unlock()
{
	pthread_mutex_t *mutex = (pthread_mutex_t *) _handle;
	pthread_mutex_unlock(mutex);
}

inline Lock::~Lock() { Unlock(); }

class Unlock {
public:
	inline Unlock(LockHandle h);
	inline ~Unlock();
	inline void Relock();
private:
	LockHandle _handle;
};

inline Unlock::Unlock(LockHandle h) : _handle(h)
{
	pthread_mutex_t *mutex = (pthread_mutex_t *) _handle;
	pthread_mutex_unlock(mutex);
}

inline void Unlock::Relock()
{
	pthread_mutex_t *mutex = (pthread_mutex_t *) _handle;
	pthread_mutex_lock(mutex);
}

inline Unlock::~Unlock() { Relock(); }

#endif	// _RTCMIX_LOCK_H_
