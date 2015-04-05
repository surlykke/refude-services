/* 
 * File:   RequestHandler.h
 * Author: Christian Surlykke <christian@surlykke.dk>
 *
 * Created on 15. februar 2015, 19:44
 */

#ifndef REQUESTHANDLER_H
#define	REQUESTHANDLER_H

#include "requestqueue.h"
#include "abstractresource.h"
#include "../../common/httpmessage.h"

class RequestHandler 
{
public:
    RequestHandler(RequestQueue* requestQueue, ResourceMap* resourceMap);
	virtual ~RequestHandler();

	void start();
	
	static void* launch(void* requestHandlerPtr);

private:
	void run();
	
	void readRequestLine();
	void readHeaderLine();
	void readBody();

	char _buffer[8192];

	int _requestSocket;
	HttpMessage _request;

	RequestQueue *mRequestQueue;
	ResourceMap* mResourceMap;
};



#endif	/* REQUESTHANDLER_H */

