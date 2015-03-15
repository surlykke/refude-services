/* 
 * File:   RequestHandler.cpp
 * Author: Christian Surlykke <christian@surlykke.dk>
 * 
 * Created on 15. februar 2015, 19:44
 */

#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include "requesthandler.h"
#include "requestqueue.h"
#include "httpprotocol.h"

RequestHandler::RequestHandler(RequestQueue *requestQueue, ResourceMap* resourceMap) :
mRequestQueue(requestQueue),
mResourceMap(resourceMap)
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
    printf("RequestHandler::run..\n");
    for (;;)
    {
        mRequestSocket = mRequestQueue->dequeue();
        printf("Got a socket\n");
        try
        {

            mHttpRequest.clear();
            mHeadersDone = false;
            mReceived = 0;
            mNextLineStart = 0;

            mPathStart = 0;
            mQueryStringStart = 0;
            mContentLength = 0;
            mBodyStart = 0;

            for (;;)
            {
                printf("Reading..");
                ssize_t bytesRead = read(mRequestSocket, mBuffer + mReceived, 8192 - mReceived);
                printf("Read: %s", mBuffer + mReceived);
                mReceived += bytesRead;

                if (bytesRead <= 0)
                {
                    throw Status::Http500;
                }
                
				if (!mHeadersDone)
                {
                    processLines();
                }

                if (mHeadersDone && mReceived >= (mBodyStart + mContentLength))
                {
                    endOfRequest();
					close(mRequestSocket);
                    break;
                }
            }
        }
        catch (Status status)
        {
            printf("Error");
            write(mRequestSocket, statusLine(status), strlen(statusLine(status)));
            write(mRequestSocket, "\r\n", 2);
            close(mRequestSocket);
        }
    }
}

void RequestHandler::processLines()
{
    int p = mNextLineStart;
    for (; p < mReceived; p++)
    {
        if (mBuffer[p] == '\r' && mBuffer[p + 1] == '\n')
        {
            if (mNextLineStart == 0)
            {
                processMethod(p + 2);
            }
            else if (p == mNextLineStart) // Empty line signals end of header section
            {
                mBodyStart = p + 2;
                endOfHeaders();

            }
            else
            {
                processHeaderLine(mNextLineStart, p + 2);
            }
            mNextLineStart = p + 2;
        }
    }
}

void RequestHandler::processMethod(int lineLength)
{
    printf("processMethod\n");
    mHttpRequest.method = string2Method(mBuffer);

    int p = methodLength(mHttpRequest.method);
    p = skipSpace(p, lineLength - 2);

    if (p == methodLength(mHttpRequest.method)) // No space after method
    {
        throw Status::Http400;
    }

    mPathStart = p;

    for (p++; p < lineLength - 2; p++)
    {
        if (isspace(mBuffer[p]))
        {
            mBuffer[p] = '\0';
            break;
        }
        if (mBuffer[p] == '?')
        {
            mQueryStringStart = p + 1;
            mBuffer[p] = '\0';
        }
    }
    printf("path: %s\n", mBuffer + mPathStart);
    printf("queryString: %s\n", mBuffer + mQueryStringStart);
}

void RequestHandler::endOfHeaders()
{
    printf("endOfHeaders\n");
    if (mHttpRequest.method == Method::UNKNOWN)
    {
        throw Status::Http400;
    }

    mHeadersDone = true;
}

void RequestHandler::endOfRequest()
{
    mHttpRequest.path = mBuffer + mPathStart;
    mHttpRequest.queryString = mBuffer + mQueryStringStart;
    mHttpRequest.body = mBuffer + mBodyStart;


    // Mock..
    const char* response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json+vnd.rfds; charset=utf-8\r\n"
        "Content-Length: 2\r\n"
        "\r\n"
        "{}";

	printf("Writing:\n%s", response);

    write(mRequestSocket, response, strlen(response));
}

int RequestHandler::skipSpace(int from, int to)
{
    while (from < to && isspace(mBuffer[from]))
        from++;

    return from;
}

void RequestHandler::processHeaderLine(int from, int to)
{
    printf("processHeaderLine\n");
    if (strncasecmp(mBuffer + from, "content-length", 14) == 0)
    {
        if (mBuffer[from + 15] != ':')
        {
            throw Status::Http400;
        }

        mContentLength = atoi(mBuffer + 15);
    }

}