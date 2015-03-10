/* 
 * File:   requestqueue.h
 * Author: Christian Surlykke <christian@surlykke.dk>
 *
 * Created on 18. februar 2015, 18:46
 */

#ifndef REQUESTQUEUE_H
#define	REQUESTQUEUE_H

#include <pthread.h>

class RequestQueue {
public:
	RequestQueue();
	virtual ~RequestQueue();
	
	void enqueue(int requestSocket);
	int dequeue();
	
private:
	static const int mask = 7;	
	void increment(int& index) { index = (index + 1) & 7; }
		
	int mQueue[8];
	pthread_mutex_t mLock;
	pthread_cond_t mNotFull;	
	pthread_cond_t mNotEmpty;
	int mSize;
	int queueStart;
	int	queueEnd; 

};

#endif	/* REQUESTQUEUE_H */

