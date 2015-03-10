/* 
 * File:   RequestHandler.cpp
 * Author: Christian Surlykke <christian@surlykke.dk>
 * 
 * Created on 15. februar 2015, 19:44
 */

#include <unistd.h>
#include <string.h>

#include "requesthandler.h"
#include "requestqueue.h"
#include "httpprotocol.h"

RequestHandler::RequestHandler(RequestQueue *requestQueue) :
mRequestQueue(requestQueue), mRequestSocket(-1),
mBufferEnd(0), mNextLineStart(0), mState(START)
{
}

RequestHandler::~RequestHandler()
{

}

void* RequestHandler::launch(void* requestHandlerPtr)
{
	((RequestHandler*) requestHandlerPtr)->run();
	return NULL;
}


void RequestHandler::run()
{
    for (;;)
    {
        mRequestSocket = mRequestQueue->dequeue();
        mBufferEnd = mNextLineStart = 0;
        mBodyStart = -1;
        mState = START;

		for(;;)
		{
            ssize_t bytesRead = read(mRequestSocket, mBuffer + mBufferEnd, 8192 - mBufferEnd);

            if (bytesRead <= 0)
            {
                internalError();
                return;
            }

            mBufferEnd += bytesRead;
			if (mState <= HEADERS)
            {
                processHeaders();
            }
            else
            {
                //FIXME
            }

        }
    }
}

void RequestHandler::processHeaders()
{
    int p = mNextLineStart;
    while (p < mBufferEnd - 1)
    {
        if (mBuffer[p] == '\r' && mBuffer[p + 1] == '\n')
        {
            if (p == mNextLineStart) // Empty line signals end of header section
            {
                mState = BODY;
                return;
            }
            else
            {
                processHeaderLine(mNextLineStart, p);
            }
        }
    }
}

void RequestHandler::processMethod(int lineLength)
{
	if (!strncmp("GET", mBuffer, 3))
	{
		/* Handle GET */
	}
	else if (!strncmp("PATCH", mBuffer, 5))
	{
		/* Handle PATCH */
	}
	else 
	{
	}
}

void RequestHandler::processHeaderLine(int from, int to)
{
    if (mNextLineStart == 0)
    {
       if (!strncmp("GET", mBuffer, 3))
        {
            /* Handle GET */
        }
        else if (!strncmp("PATCH", mBuffer, 5))
        {
            /* Handle PATCH */
        }
		else 
		{
		}
    }
}


//void RequestHandler::cannedResponse()
//{
//    const char* response =
//        "HTTP/1.1 200 OK\r\n"
//        "Content-Type: application/json+vnd.rfds; charset=utf-8\r\n"
//        "Content-Length: 17\r\n"
//        "\r\n"
//        "{\"key\" : \"value\"}";
//
//    mSocket->write(response);
//}



void RequestHandler::internalError()
{

}
