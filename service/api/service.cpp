/* 
 * File:   service_listener.cpp
 * Author: Christian Surlykke <christian@surlykke.dk>
 * 
 * Created on 14. februar 2015, 19:10
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>

#include "service.h"
#include "requesthandler.h"


ServiceListener::ServiceListener()
{

}

ServiceListener::~ServiceListener()
{
}

bool ServiceListener::setup(const char* socketPath)
{
	if (strlen(socketPath) >= UNIX_PATH_MAX)
	{
		// Log error
		return false;
	}
		
	struct sockaddr_un sockaddr;
	sockaddr.sun_family = AF_UNIX; strcpy(sockaddr.sun_path, socketPath); 
	
	if ((listenSocket = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)	
	{
		// Log error
		return false;
	}

	if ((bind(listenSocket, (struct sockaddr*)(&sockaddr), sizeof(struct sockaddr_un))) < 0)
	{
		// Log error
		return false;
	}

	if (listen(listenSocket, 8) < 0)
	{
		// Log error
		return false;
	}

	pthread_t threads[5];	
	for (int i = 0; i < 5; i++)
	{
		pthread_create(threads + i, 0, RequestHandler::launch, new RequestHandler(&requestQueue));
	}
	
	pthread_t thread;
	if (pthread_create(&thread, 0, &ServiceListener::launch, this) < 0)
	{
		// Log error
		return false;
	}

	return true;
}

void* ServiceListener::launch(void* serviceListenerPtr)
{
	((ServiceListener*) serviceListenerPtr)->run();
	return NULL;
}


void ServiceListener::run()
{
	for(;;)	
	{
		int requestSocket;
		if ((requestSocket = accept(listenSocket, NULL, 0)) < 0)
		{
			// Log error
			return;
		}

		requestQueue.enqueue(requestSocket);
	}
}



