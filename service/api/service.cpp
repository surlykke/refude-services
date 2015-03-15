/* 
 * File:   service_listener.cpp
 * Author: Christian Surlykke <christian@surlykke.dk>
 * 
 * Created on 14. februar 2015, 19:10
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <error.h>
#include <errno.h>

#include "service.h"
#include "requesthandler.h"


ServiceListener::ServiceListener()
{

}

ServiceListener::~ServiceListener()
{
}

void ServiceListener::setup(const char* socketPath)
{
	if (strlen(socketPath) >= UNIX_PATH_MAX)
	{
		error(1, errno, "socketPath to long");
	}
		
	struct sockaddr_un sockaddr;
	sockaddr.sun_family = AF_UNIX; strcpy(sockaddr.sun_path, socketPath); 
	
	if ((listenSocket = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)	
	{
		error(1, errno, "creation of listen socket failed");
	}

	if ((bind(listenSocket, (struct sockaddr*)(&sockaddr), sizeof(struct sockaddr_un))) < 0)
	{
		error(1, errno, "binding to listen socket failed");
	}

	if (listen(listenSocket, 8) < 0)
	{
		error(1, errno, "Listen failed");
	}

	pthread_t threads[5];	
	for (int i = 0; i < 5; i++)
	{
		if (pthread_create(threads + i, 0, RequestHandler::launch, new RequestHandler(&requestQueue, &mResourceMap)) < 0)
		{
			error(1, errno, "Thread creation failed");
		}
	}
	
	pthread_t thread;
	if (pthread_create(&thread, 0, &ServiceListener::startListenThread, this) < 0)
	{
		error(1, errno, "Thread creation failed");
	}
}

void* ServiceListener::startListenThread(void* serviceListenerPtr)
{
	((ServiceListener*) serviceListenerPtr)->run();
	return NULL;
}


void ServiceListener::run()
{
	printf("ServiceListener::run.. \n");
	for(;;)	
	{
		int requestSocket;
		if ((requestSocket = accept(listenSocket, NULL, 0)) < 0)
		{
			error(0, errno, "accept");
		}

		printf("Incoming...\n");

		requestQueue.enqueue(requestSocket);
	}
}



