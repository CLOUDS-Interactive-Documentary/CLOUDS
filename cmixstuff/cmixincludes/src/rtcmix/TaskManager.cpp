//
// C++ Implementation: TaskManager
//
// Description: 
//
//
// Author: Douglas Scott <netdscott@netscape.net>, (C) 2010
//
//


#include "TaskManager.h"
#include "RTSemaphore.h"
#include "RTThread.h"
#include "rt_types.h"
#include <pthread.h>
#include <stdio.h>
#include <assert.h>

#ifdef LINUX
#include <sys/time.h>
#include <sys/resource.h>
#endif

#undef DEBUG
#undef TASK_DEBUG
#undef THREAD_DEBUG
#undef POOL_DEBUG

class Notifiable {
public:
	virtual void notify(int inIndex) = 0;
};

class Notifier {
public:
	Notifier(Notifiable *inTarget, int inIndex) : mTarget(inTarget), mIndex(inIndex) {}
	void notify() { mTarget->notify(mIndex); }
#ifndef THREAD_DEBUG
private:
#endif
	Notifiable	*mTarget;
	int 		mIndex;
};

class TaskThread : public RTThread, Notifier
{
public:
	TaskThread(Notifiable *inTarget, TaskProvider *inProvider, int inIndex)
		: RTThread(inIndex), Notifier(inTarget, inIndex),
		  mStopping(false), mTaskProvider(inProvider) {}
	~TaskThread() { mStopping = true; start(); }
	inline void start();
protected:
	virtual void	run();
	Task *			getATask() { return mTaskProvider->getTask(); }
private:
	bool			mStopping;
	TaskProvider *	mTaskProvider;
	RTSemaphore		mSema;
};

inline void TaskThread::start()
{
#ifdef THREAD_DEBUG
	//	printf("TaskThread::start(%p): posting for Task %p\n", this, mTask);
#endif
	mSema.post();
}

void TaskThread::run()
{
#ifdef THREAD_DEBUG
	printf("TaskThread %p running\n", this);
#endif
	do {
#ifdef THREAD_DEBUG
		printf("TaskThread %d sleeping...\n", mIndex);
#endif
		mSema.wait();
#ifdef THREAD_DEBUG
		printf("TaskThread %d woke up -- running task loop\n", mIndex);
#endif
		Task *task;
		while ((task = getATask()) != NULL) {
#ifdef THREAD_DEBUG
            printf("TaskThread %d running task %p...\n", mIndex, task);
#endif
			task->run();
#ifdef THREAD_DEBUG
            printf("TaskThread %d task %p done\n", mIndex, task);
#endif
			delete task;
		}
		notify();
	}
	while (!mStopping);
#ifdef THREAD_DEBUG
	printf("TaskThread %p exiting\n", this);
#endif
}

class ThreadPool : private Notifiable
{
public:
	ThreadPool(TaskProvider *inProvider) : mRequestCount(0), mThreadSema(RT_THREAD_COUNT), mWaitSema(0) {
		for(int i=0; i<RT_THREAD_COUNT; ++i) {
			mThreads[i] = new TaskThread(this, inProvider, i);
		}
	}
	~ThreadPool() {
		for(int i=0; i<RT_THREAD_COUNT; ++i)
			delete mThreads[i];
	}
	virtual void notify(int inIndex);
	inline void startAndWait(int taskCount);
private:
	TaskThread		*mThreads[RT_THREAD_COUNT];
	AtomicInt		mRequestCount;
	RTSemaphore		mThreadSema;
	RTSemaphore		mWaitSema;
};

inline void ThreadPool::startAndWait(int taskCount) {
	// Dont start any more threads than we have tasks.
	mRequestCount = std::min(taskCount, RT_THREAD_COUNT);
	const int count = mRequestCount;
	for(int i=0; i<count; ++i)
		mThreads[i]->start();
#ifdef POOL_DEBUG
	printf("ThreadPool::startAndWait: waiting on %d threads\n", count);
#endif
	mWaitSema.wait();
}

// Let thread pool know that the thread at index inIndex is available

void ThreadPool::notify(int inIndex)
{
#ifdef POOL_DEBUG
	printf("ThreadPool notified for index %d\n", inIndex);
#endif
	if (mRequestCount.decrementAndTest()) {
#if defined(POOL_DEBUG) || defined(THREAD_DEBUG)
		printf("ThreadPool posting to wait semaphore\n");
#endif
		mWaitSema.post();
	}
}

TaskManagerImpl::TaskManagerImpl()
	: mThreadPool(new ThreadPool(this)), mTaskHead(NULL), mTaskTail(NULL) {}

TaskManagerImpl::~TaskManagerImpl() { delete mThreadPool; }

void TaskManagerImpl::addTask(Task *inTask)
{
#ifdef DEBUG
	printf("TaskManagerImpl::addTask: adding task %p\n", inTask);
#endif
#if 0
	// Each new task is put in front of the previous one
	if (mTaskHead == NULL)
		mTaskTail = mTaskHead = inTask;
	else {
		mTaskTail->next() = inTask;
		mTaskTail = inTask;
	}
#else
	// Each new task is put behind the previous one
	inTask->next() = mTaskHead;
	mTaskHead = inTask;
#endif
}

Task * TaskManagerImpl::getTask()
{
	Task *task = mTaskStack.pop_atomic();
#ifdef DEBUG
	printf("TaskManagerImpl::getTask: returning task %p\n", task);
#endif
	return task;
}

void TaskManagerImpl::startAndWait()
{
#ifdef DEBUG
	printf("TaskManagerImpl::startAndWait waiting on ThreadPool...\n");
#endif
	int taskCount = 0;
	// Push entire reversed linked list into stack
	for (Task *t = mTaskHead; t != NULL; ++taskCount) {
		Task *next = t->next();
		mTaskStack.push_atomic(t);
		t = next;
	}
	mTaskHead = mTaskTail = NULL;
	mThreadPool->startAndWait(taskCount);
#ifdef DEBUG
	printf("TaskManagerImpl::startAndWait done\n");
#endif
}

TaskManager::TaskManager() : mImpl(new TaskManagerImpl)
{
}

TaskManager::~TaskManager()
{
	delete mImpl;
}

