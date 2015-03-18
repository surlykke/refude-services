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
#include "httprequest.h"

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


	char nextChar();
	void readTo(char c);
	void receive();
	void addHeader(const char* name, const char* value);
	
	char _buffer[8192];

	int _requestSocket;
	int _currentPos;
	int _received;
	bool _headersDone;
	
	Method _method;
	int _pathStart;
	int _queryStringStart;
	int _contentLength;
	int _bodyStart;

	RequestQueue *mRequestQueue;
	ResourceMap* mResourceMap;

};



#endif	/* REQUESTHANDLER_H */

