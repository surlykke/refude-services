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

class ServiceListener {
public:
    ServiceListener();
    virtual ~ServiceListener();

	bool setup(const char* socketPath);

	static void* launch(void* serviceListenerPtr);

private:
	void run();
	
	int listenSocket;
	RequestQueue requestQueue;
};

#endif	/* SERVICELISTENER_H */

