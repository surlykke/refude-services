/* 
 * File:   RequestHandler.h
 * Author: Christian Surlykke <christian@surlykke.dk>
 *
 * Created on 15. februar 2015, 19:44
 */

#ifndef REQUESTHANDLER_H
#define	REQUESTHANDLER_H

#include "requestqueue.h"

class RequestHandler 
{
public:
    RequestHandler(RequestQueue* requestQueue);
	virtual ~RequestHandler();

	void start();
	
	static void* launch(void* requestHandlerPtr);

private:
	void run();
	
	void internalError();
	void error(const char* message);
	void processHeaders();
	void processMethod(int lineLength);	
	void processHeaderLine(int from, int to);	
	void getPath(int from, int to);

	RequestQueue *mRequestQueue;
	int mRequestSocket;
	static const int bufferCapacity = 8192;
	char mBuffer[bufferCapacity];
	int mBufferEnd;
	int mNextLineStart;
	int mBodyStart;

	enum
	{
		START,	
		HEADERS,
		ERR_METHOD_NOT_SUPPORTED,
		ERR_UNKNOWN_HEADER,
		ERR_INVALID_HEADER,
		BODY,
		DONE
	} mState;


};


#endif	/* REQUESTHANDLER_H */

