/* 
 * File:   requestqueue.cpp
 * Author: Christian Surlykke <christian@surlykke.dk>
 * 
 * Created on 18. februar 2015, 18:46
 */

#include <QIODevice>
#include <QMutexLocker>
#include <QDebug>
#include <qt4/QtCore/qglobal.h>

#include "requestqueue.h"

RequestQueue::RequestQueue() : 
	 mNotFull(), mNotEmpty(), mLock(), queueStart(0), queueEnd(0)
{
	for (int i = 0; i < 8; i++) mQueue[i] = 0;
}

RequestQueue::~RequestQueue()
{
}

void RequestQueue::enqueue(QIODevice* ioDevice)
{	
	QMutexLocker locker(&mLock);
	
	while (mQueue[queueEnd] != 0)	
	{
		mNotFull.wait(&mLock);
	}

	mQueue[queueEnd] = ioDevice;
	increment(queueEnd);
	
	mNotEmpty.wakeOne();
}

QIODevice* RequestQueue::dequeue()
{
	QMutexLocker locker(&mLock);

	while (mQueue[queueStart] == 0)
	{
		mNotEmpty.wait(&mLock);
	}

	QIODevice* result = mQueue[queueStart];
	mQueue[queueStart] = 0;
	increment(queueStart);

	mNotFull.wakeOne();
	return result;
}

