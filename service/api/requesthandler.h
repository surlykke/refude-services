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
	
	void processLines();
	void processMethod(int lineLength);	
	void processHeaderLine(int from, int to);	
	void endOfHeaders();
	void endOfRequest();	
	int skipSpace(int from, int to);

	RequestQueue *mRequestQueue;
	ResourceMap* mResourceMap;

	int mRequestSocket;
	static const int bufferCapacity = 8192;
	char mBuffer[bufferCapacity];

	HttpRequest mHttpRequest;
	bool mHeadersDone;
	int mReceived;
	int mProcessed;
	int mNextLineStart;

	int mPathStart;
	int mQueryStringStart;
	int mContentLength;
	int mBodyStart;

};


#endif	/* REQUESTHANDLER_H */

