/* 
 * File:   RequestHandler.cpp
 * Author: Christian Surlykke <christian@surlykke.dk>
 * 
 * Created on 15. februar 2015, 19:44
 */

#include <QIODevice>
#include <QDebug>

#include "request_handler.h"

QWaitCondition RequestHandler::mWait;
QMutex RequestHandler::mLock;

void RequestHandler::handleRequest(QIODevice* inOut)
{
	static const int numHandlers = 20;	
	static RequestHandler handlers[numHandlers];

	mLock.lock();
	while (true) 
	{
		RequestHandler *handler = 0;	
		for (int i = 0; i < numHandlers; i++)
		{
			if (handlers[i].mIdle)
			{
				handler = handlers + i;
				break;
			}
		}
		
		if (handler) 
		{
			handler->mInOut = inOut;
			handler->mIdle = false;
			handler->mInOut->moveToThread(handler);
			handler->start();
			break;
		}
		else
		{
			mWait.wait(&mLock);
		}
	}
	mLock.unlock();
}


RequestHandler::RequestHandler() : QThread(), mInOut(0), mIdle(true)
{
}

RequestHandler::~RequestHandler()
{

}

void RequestHandler::run() 
{
	char* request = new char[4096];
	const char* response =
		"HTTP/1.1 200 OK\r\n"
		"Content-Type: application/json+vnd.rfds; charset=utf-8\r\n"
		"Content-Length: 17\r\n"
		"\r\n"
	    "{\"key\" : \"value\"}";

	while (mInOut->waitForReadyRead(-1)) 
	{
		mInOut->read(request, 4096);
		qDebug() << "Request:";
		qDebug() << request;
		qDebug() << "Svarer:";
		qDebug() << response;
		//mInOut->write(response); 
	}

	qDebug() << mInOut->errorString();

	qDebug() << "Done";

	mLock.lock();
	delete mInOut;
	mIdle = true;
	mWait.wakeAll();
	mLock.unlock();

}
