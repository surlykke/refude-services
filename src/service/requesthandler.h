/* 
 * File:   RequestHandler.h
 * Author: Christian Surlykke <christian@surlykke.dk>
 *
 * Created on 15. februar 2015, 19:44
 */

#ifndef REQUESTHANDLER_H
#define	REQUESTHANDLER_H

#include "requestqueue.h"

#include <QThread>
#include <QMutex>
#include <QWaitCondition>

class QIODevice;

class RequestHandler : public QThread
{
public:
    RequestHandler(int num, RequestQueue* requestQueue);
	virtual ~RequestHandler();

protected:	
	virtual void Q_DECL_OVERRIDE run();

private:
	RequestQueue *mRequestQueue;
	QIODevice *mInOut;
	char incoming[8192];
	int mNum;
};


#endif	/* REQUESTHANDLER_H */

