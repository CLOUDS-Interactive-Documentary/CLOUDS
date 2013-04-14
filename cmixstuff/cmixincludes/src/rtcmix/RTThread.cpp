//
//  RTThread.cpp
//  RTcmixTest
//
//  Created by Douglas Scott on 8/27/12.
//
//

#include "RTThread.h"
#include <sys/resource.h>
#include <stdlib.h>
#include <assert.h>

static pthread_once_t sOnceControl = PTHREAD_ONCE_INIT;

pthread_key_t	RTThread::sIndexKey;

RTThread::RTThread(int inThreadIndex) : mThreadIndex(inThreadIndex) {
	pthread_once(&sOnceControl, InitOnce);
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setschedpolicy(&attr, SCHED_RR);
	pthread_create(&mThread, &attr, sProcess, this);
	pthread_attr_destroy(&attr);
}

RTThread::~RTThread() {
	pthread_join(mThread, NULL);
}

void RTThread::InitOnce() {
	int status = pthread_key_create(&sIndexKey, RTThread::DestroyMemory);
	assert(status == 0);
}

int RTThread::GetIndexForThread() {
	void *mem = pthread_getspecific(sIndexKey);
	assert(mem != NULL);
	return *((int *) mem);
}

void RTThread::SetIndexForThread(int inIndex) {
	int *pIndex = new int;
	*pIndex = inIndex;
	int status = pthread_setspecific(sIndexKey, pIndex);
	assert(status == 0);
}

void RTThread::DestroyMemory(void *value)
{
	int *threadMem = (int *) value;
	delete threadMem;
    pthread_setspecific(sIndexKey, NULL);
}

void *RTThread::sProcess(void *inContext)
{
	RTThread *This = (RTThread *) inContext;
	SetIndexForThread(This->GetIndex());
	if (setpriority(PRIO_PROCESS, 0, -20) != 0) {
#ifdef THREAD_DEBUG
		perror("RTThread::sProcess: setpriority() failed.");
#endif
	}
	This->run();
	return NULL;
}

