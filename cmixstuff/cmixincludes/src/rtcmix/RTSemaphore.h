// RTSemaphore.h

#ifndef RT_SEMAPHORE_H
#define RT_SEMAPHORE_H

#ifdef MACOSX
#include <Availability.h>
#endif

#if defined(MACOSX) && __MAC_OS_X_VERSION_MIN_REQUIRED >= 1060

#include <dispatch/dispatch.h>

class RTSemaphore
{
public:
	RTSemaphore(unsigned inStartingValue=0) : mSema(dispatch_semaphore_create((long)inStartingValue)) {}
	~RTSemaphore() { mSema = NULL; }
	void wait() { dispatch_semaphore_wait(mSema, DISPATCH_TIME_FOREVER); }	// each thread will wait on this
	void post() { dispatch_semaphore_signal(mSema); }	// when done, each thread calls this
private:
	dispatch_semaphore_t	mSema;
};

#else

#include <semaphore.h>

class RTSemaphore
{
public:
	RTSemaphore(unsigned inStartingValue=0) { sem_init(&mSema, 0, inStartingValue); }
	~RTSemaphore() { sem_destroy(&mSema); }
	void wait() { sem_wait(&mSema); }	// each thread will wait on this
	void post() { sem_post(&mSema); }	// when done, each thread calls this
private:
	sem_t	mSema;
};

#endif	// MACOSX && 1060

#endif	// RT_SEMAPHORE_H

