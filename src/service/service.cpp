/* 
 * File:   service_listener.cpp
 * Author: Christian Surlykke <christian@surlykke.dk>
 * 
 * Created on 14. februar 2015, 19:10
 */

#include "service.h"
#include "request_handler.h"

#include <QLocalSocket>
#include <QIODevice>
#include <QDebug>
#include <QString>
#include <QFile>
#include <QThreadPool>
#include <QCoreApplication>

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
		RequestHandler::handleRequest(socket);
	}

	QCoreApplication::exit(0);
}



ServiceListener::~ServiceListener()
{
}

