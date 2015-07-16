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
#include <unistd.h>

#include "service.h"
#include "requesthandler.h"

namespace org_restfulipc
{
	ServiceListener::ServiceListener()
	{

	}

	ServiceListener::~ServiceListener()
	{
	}

	void ServiceListener::setup(const char* socketPath)
	{
		if (strlen(socketPath) >= UNIX_PATH_MAX - 1)
		{
			error(1, errno, "socketPath to long");
		}

		struct sockaddr_un sockaddr;
		memset(&sockaddr, 0, sizeof(struct sockaddr_un));
		sockaddr.sun_family = AF_UNIX; 
		strncpy(&sockaddr.sun_path[0], socketPath, strlen(socketPath)); 

		if ((listenSocket = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)	
		{
			error(1, errno, "creation of listen socket failed");
		}

		unlink(socketPath);

		if (bind(listenSocket, (struct sockaddr*)(&sockaddr), sizeof(sa_family_t) + strlen(socketPath) + 1) < 0)
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
		for(;;)	
		{
			int requestSocket;
			if ((requestSocket = accept(listenSocket, NULL, 0)) < 0)
			{
				error(0, errno, "accept");
			}

			requestQueue.enqueue(requestSocket);
		}
	}

	void ServiceListener::map(AbstractResource* resource, const char* path)
	{
		mResourceMap.map(path, resource);
	}

	void ServiceListener::unmap(AbstractResource* resource)
	{
		mResourceMap.unMap(resource);
	}


}
