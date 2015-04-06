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
    for (;;)
    {
        _requestSocket = mRequestQueue->dequeue();
		try
        {
			HttpMessageReader(_requestSocket, _request).readRequest();
			printf("Incoming, path: %s, queryString: %s, body:\n%s\n", _request.path(), _request.queryString(), _request.body());
			AbstractResource* resource = mResourceMap->resource(_request.path());
		
			if (resource == 0)
			{		
				throw Status::Http404;	
			}
	
			printf("_requestSocket: %d\n", _requestSocket);
			resource->handleRequest(_requestSocket, _request);
        }
        catch (Status status)
        {
            printf("Error\n");
            write(_requestSocket, statusLine(status), strlen(statusLine(status)));
            write(_requestSocket, "\r\n", 2);
            close(_requestSocket);
        }
    }
}

/*const char* RequestHandler::handshakeResponseTemplate =
	"HTTP/1.1 101 Switching Protocols"
	"Upgrade: websocket"
	"Connection: Upgrade"
	"Sec-WebSocket-Protocol: %s";*/
