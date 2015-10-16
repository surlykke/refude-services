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
#include "errorhandling.h"

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
            throw std::runtime_error("Socket path too long");
		}

		struct sockaddr_un sockaddr;
		memset(&sockaddr, 0, sizeof(struct sockaddr_un));
		sockaddr.sun_family = AF_UNIX; 
		strncpy(&sockaddr.sun_path[0], socketPath, strlen(socketPath)); 
		assert((listenSocket = socket(AF_UNIX, SOCK_STREAM, 0)) >= 0);
		unlink(socketPath);
		assert(bind(listenSocket, (struct sockaddr*)(&sockaddr), sizeof(sa_family_t) + strlen(socketPath) + 1) >= 0);
		assert(listen(listenSocket, 8) >= 0);

		pthread_t threads[5];	
		for (int i = 0; i < 5; i++)
		{
			assert(pthread_create(threads + i, 0, RequestHandler::launch, new RequestHandler(&requestQueue, &mResourceMap)) >= 0);
		}
		
		pthread_t thread;
		assert(pthread_create(&thread, 0, &ServiceListener::startListenThread, this) <= 0);
	}

	void* ServiceListener::startListenThread(void* serviceListenerPtr)
	{
		((ServiceListener*) serviceListenerPtr)->run();
		return NULL;
	}


	void ServiceListener::run()
	{
		for(;;)	{
			int requestSocket;
			assert ((requestSocket = accept(listenSocket, NULL, 0)) >= 0);

			struct timeval tv;
			tv.tv_sec = 0;
			tv.tv_usec = 200000;
			assert (setsockopt(requestSocket, SOL_SOCKET, SO_RCVTIMEO, (struct timeval*) &tv, sizeof(struct timeval)) >= 0);
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
