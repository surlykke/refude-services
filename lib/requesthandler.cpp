/* 
 * File:   RequestHandler.cpp
 * Author: Christian Surlykke <christian@surlykke.dk>
 * 
 * Created on 15. februar 2015, 19:44
 */

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <error.h>

#include "requesthandler.h"
#include "requestqueue.h"
#include "resourcemap.h"

using namespace std;

namespace org_restfulipc
{

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
			bool done = false;	
			while(!done) {
				try
				{
					HttpMessageReader(_requestSocket, _request).readRequest();
					if (_request.headerValue(Header::connection) != 0 &&
					    strcasecmp("close", _request.headerValue(Header::connection)) == 0) {
						done = true;
					}
					AbstractResource* resource = mResourceMap->resource(_request.path);
				
					if (resource == 0)
					{		
						throw Status::Http404;	
					}
					
					resource->handleRequest(_requestSocket, _request);
				}
				catch (int errorNumber) {
                    // 0 is a 'benign' error, eg. peer closed connection
                    if (errorNumber == 0) {
                        done = true;
                    }
                    else {
                        throw;
                    }
				}
				catch (Status status) {
					send(_requestSocket, statusLine(status), strlen(statusLine(status)), MSG_NOSIGNAL);
					send(_requestSocket, "\r\n", 2, MSG_NOSIGNAL);
					done = true;
				}
			}
				
			close(_requestSocket);
		}
	}
}