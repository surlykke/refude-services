/* 
 * File:   service_listener.cpp
 * Author: Christian Surlykke <christian@surlykke.dk>
 * 
 * Created on 14. februar 2015, 19:10
 */

#include "service_listener.h"

#include <QLocalSocket>
#include <QIODevice>
#include <QDebug>
#include <QString>
#include <QFile>
#include <qt/QtCore/qcoreapplication.h>

#define SERVICE_PATH "/home/christian/tmp/service"

ServiceListener::ServiceListener(): mServer()
{
}

void ServiceListener::run()
{
	if (QFile::exists(SERVICE_PATH) && not QFile::remove(SERVICE_PATH))
	{
		qWarning() << SERVICE_PATH << "exists and cannot be deleted";
		QCoreApplication::exit(1);
	}
	
	if (!mServer.listen(SERVICE_PATH)) 
	{
		qWarning() << "Listening on" << SERVICE_PATH << "failed";
		QCoreApplication::exit(1);
	}

	while(mServer.waitForNewConnection(-1)) 
	{
		qDebug() << "Incoming..";
		QLocalSocket *socket = mServer.nextPendingConnection();
		Service *service = new Service(socket);
		socket->moveToThread(service);
		service->start();
		qDebug() << "Service started...";
	}

	QCoreApplication::exit(0);
}



ServiceListener::~ServiceListener()
{
}

Service::Service(QIODevice* inOut) : mInOut(inOut)
{
}

Service::~Service()
{
	qDebug() << "Service destructor";
}



void Service::run() 
{
	while (true) 
	{
		if (mInOut->waitForReadyRead(-1))
		{
			qDebug() << mInOut->readAll();
		}
		else 
		{
			qDebug() << "dying..";
			return;
		}
	}
}