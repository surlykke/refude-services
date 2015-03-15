/* 
 * File:   requestqueue.cpp
 * Author: Christian Surlykke <christian@surlykke.dk>
 * 
 * Created on 18. februar 2015, 18:46
 */

#include <pthread.h>
#include <stdio.h>

#include "requestqueue.h"

RequestQueue::RequestQueue() 
{
	mLock = PTHREAD_MUTEX_INITIALIZER;
	mNotEmpty = PTHREAD_COND_INITIALIZER;
	mNotFull = PTHREAD_COND_INITIALIZER;

	for (int i = 0; i < 8; i++) mQueue[i] = 0;
}

RequestQueue::~RequestQueue()
{
}

void RequestQueue::enqueue(int requestSocket)
{	
	pthread_mutex_lock(&mLock);
	while (mQueue[queueEnd] != 0)	
	{
		pthread_cond_wait(&mNotFull, &mLock);
	}

	mQueue[queueEnd] = requestSocket;
	increment(queueEnd);
	printf("enqueue, about to signal, queueStart: %d, queueEnd: %d\n", queueStart, queueEnd);
	pthread_cond_signal(&mNotEmpty);
	pthread_mutex_unlock(&mLock);
}

int RequestQueue::dequeue()
{
	pthread_mutex_lock(&mLock);

	while (mQueue[queueStart] == 0)
	{
		pthread_cond_wait(&mNotEmpty, &mLock);
	}

	int dequeuedSocket = mQueue[queueStart];
	mQueue[queueStart] = 0;
	increment(queueStart);

	printf("dequeue, about to signal, queueStart: %d, queueEnd: %d\n", queueStart, queueEnd);
	pthread_cond_signal(&mNotFull);
	pthread_mutex_unlock(&mLock);

	return dequeuedSocket;
}

