/* 
 * File:   requestqueue.h
 * Author: Christian Surlykke <christian@surlykke.dk>
 *
 * Created on 18. februar 2015, 18:46
 */

#ifndef REQUESTQUEUE_H
#define	REQUESTQUEUE_H

#include <QMutex>
#include <QWaitCondition>
#include <QVector>

class RequestQueue {
public:
	RequestQueue();
	virtual ~RequestQueue();
	
	void enqueue(QIODevice *ioDevice);
	QIODevice *dequeue();
	
private:
	static const int mask = 7;	
	void increment(int& index) { index = (index + 1) & 7; }
		
	QIODevice* mQueue[8];
	QWaitCondition mNotFull;	
	QWaitCondition mNotEmpty;
	QMutex mLock;
	int mSize;
	int queueStart;
	int	queueEnd; 

};

#endif	/* REQUESTQUEUE_H */

