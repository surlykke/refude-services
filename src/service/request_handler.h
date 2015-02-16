/* 
 * File:   RequestHandler.h
 * Author: Christian Surlykke <christian@surlykke.dk>
 *
 * Created on 15. februar 2015, 19:44
 */

#ifndef REQUESTHANDLER_H
#define	REQUESTHANDLER_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>

class QIODevice;

class RequestHandler : public QThread
{
public:
	static void handleRequest(QIODevice* inOut);
    virtual ~RequestHandler();

protected:	
	virtual void Q_DECL_OVERRIDE run();


private:
	static QWaitCondition mWait;	
	static QMutex mLock;
    
	RequestHandler();
	QIODevice *mInOut;
	char incoming[8192];
	bool mIdle;	
};

#endif	/* REQUESTHANDLER_H */

