//
//  RTThread.h
//  RTcmix
//
//  Created by Douglas Scott on 8/27/12.
//
//

#ifndef _RTThread_h_
#define _RTThread_h_

#include <pthread.h>

class RTThread
{
public:
	RTThread(int inThreadIndex);
	virtual ~RTThread();
	static int	GetIndexForThread();
protected:
	virtual void run()=0;
	static void *sProcess(void *inContext);
private:
	int			GetIndex() const { return mThreadIndex; }
	static void	InitOnce();
	static void	SetIndexForThread(int inIndex);
	static void DestroyMemory(void *value);
	
	pthread_t				mThread;
	int						mThreadIndex;
	static pthread_key_t	sIndexKey;
};

#endif
