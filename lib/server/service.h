/* 
 * File:   service_listener.h
 * Author: Christian Surlykke <christian@surlykke.dk>
 *
 * Created on 14. februar 2015, 19:10
 */

#ifndef SERVICELISTENER_H
#define	SERVICELISTENER_H

#include <pthread.h>
#include <linux/un.h>

#include "requestqueue.h"
#include "resourcemap.h"

namespace org_restfulipc
{
	class ServiceListener 
	{
	public:
		ServiceListener();
		virtual ~ServiceListener();

		void setup(const char* socketPath);
		void map(AbstractResource* resource, const char* path);
		void unmap(AbstractResource* resource);

	private:
		
		static void* startListenThread(void* serviceListenerPtr);
		void run();
		
		int listenSocket;
		RequestQueue requestQueue;
		ResourceMap mResourceMap;
	};
}

#endif	/* SERVICELISTENER_H */

