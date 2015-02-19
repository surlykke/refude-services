/* 
 * File:   RequestHandler.cpp
 * Author: Christian Surlykke <christian@surlykke.dk>
 * 
 * Created on 15. februar 2015, 19:44
 */

#include <QIODevice>
#include <QDebug>

#include "requesthandler.h"
#include "requestqueue.h"

RequestHandler::RequestHandler(int num, RequestQueue *requestQueue) : 
	QThread(), mRequestQueue(requestQueue), mInOut(0), mNum(num)
{
}

RequestHandler::~RequestHandler()
{

}

void RequestHandler::run()
{
    for (;;)
    {
		mInOut = mRequestQueue->dequeue();
		qDebug() << mNum << "Look for request" << this;
		mInOut->moveToThread(this);
		qDebug() << mNum << "Got one" << this;

        const char* response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json+vnd.rfds; charset=utf-8\r\n"
            "Content-Length: 17\r\n"
            "\r\n"
            "{\"key\" : \"value\"}";

        while (mInOut->waitForReadyRead(-1))
        {
            mInOut->read(incoming, 8192);
            qDebug() << mNum << "Request:";
            qDebug() << mNum << incoming;
            qDebug() << mNum << "Svarer:";
            qDebug() << mNum << response;
            mInOut->write(response);
        }

        qDebug() << mNum << mInOut->errorString();

        qDebug() << mNum << "Done";
    }
}
