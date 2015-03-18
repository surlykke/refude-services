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
        _requestSocket = mRequestQueue->dequeue();
		try
        {
			_currentPos = -1;
			_received = 0;
			_headersDone = false;

			_method = Method::UNKNOWN;
			_pathStart = -1;
			_queryStringStart = -1;
			_contentLength = 0;
			_bodyStart = -1;

			readRequestLine();
			while (! _headersDone) readHeaderLine();
			readBody();
			printf("Path: %s\n", _buffer + _pathStart);
			printf("Query string: %s\n", _buffer + _queryStringStart);
			printf("Body: %s\n", _buffer + _bodyStart);

			AbstractResource* resource = mResourceMap->resource(_buffer + _pathStart);
			if (! resource)
			{
				throw Status::Http404;
			}

			resource->doGET(_requestSocket, _buffer + _pathStart, _buffer + _queryStringStart);

			close(_requestSocket); // FIXME Handle keep alive
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

void RequestHandler::readRequestLine()
{
	printf("Read requestLine\n");
	while (nextChar() != ' ');

	_method = string2Method(_buffer);
	
	printf("_method: %d\n", _method);

	if (_method == Method::UNKNOWN)
	{
		throw Status::Http403;
	}
	
	_pathStart = _currentPos + 1;

	printf("Path start at:\n%s\n", _buffer + _pathStart);

	while (! isspace(nextChar()))
	{
		if (_buffer[_currentPos] == '?')
		{
			_queryStringStart = _currentPos;
		}
	};

	_buffer[_currentPos] = '\0';
	if (_queryStringStart == 0)
	{
		_queryStringStart = _currentPos;
	}

	int protocolStart = _currentPos + 1;

	while (! isspace(nextChar()));

	
	printf("Looking for protocol at\n%s\n", _buffer + protocolStart);

	if (strncmp(_buffer + protocolStart, "HTTP/1.1", 8) != 0)
	{
		throw Status::Http505;
	}

	printf("_currentPos - protocolStart = %d\n", _currentPos - protocolStart);

	printf(" current and next: %d %d\n", _buffer[_currentPos], _buffer[_currentPos + 1]);
	if (_buffer[_currentPos] != '\r' || nextChar() != '\n')
	{
		throw Status::Http403;
	}
}

/* TODO: Full implementation of spec
 *  - multiline header definitions
 *  - Illegal chars in names/values
 *  - Normalize whitespace
 */
void RequestHandler::readHeaderLine()
{
	printf("readHeaderling at:\n%s\n", _buffer + _currentPos + 1);
	int colonPos = -1;
	int lineStart = _currentPos + 1;

	for (;;)	
	{
		nextChar();
		if (_buffer[_currentPos] == '\r')
		{
			break;
		}
		else if	(_buffer[_currentPos] == '\n')
		{
			throw Status::Http403;
		}
		else if (_buffer[_currentPos] == ':')
		{
			colonPos = _currentPos;
		}
	}
	
	if (nextChar() != '\n') // TODO: Is this right?
	{
		printf("No \\n\n");
		throw Status::Http403;
	}
	

	printf("lineStart: %d, _currentPos: %d\n", lineStart, _currentPos);
	
	if (_currentPos == lineStart + 1)
	{
		_headersDone = true;
		_bodyStart = _currentPos + 1;
	}
	else 
	{	
		if (colonPos < 1) // None or empty header name
		{
			printf("No colon..\n");
			throw Status::Http403;
		}
	
		_buffer[colonPos] = '\0';
		_buffer[_currentPos - 1] = '\0';
		addHeader(_buffer + lineStart, _buffer + colonPos + 1);
	}
}


void RequestHandler::readBody()
{
	printf("readBody, _bodyStart: %d, _contentLength: %d, _received: %d\n", _bodyStart, _contentLength, _received);
	while (_bodyStart + _contentLength > _received)	
	{
		printf("readBody - receive\n");	
		receive();
	}
	_buffer[_bodyStart + _contentLength] = '\0';
}

char RequestHandler::nextChar()
{
	_currentPos++;	
	while ( _currentPos >= _received)
	{
		receive();
	}

	return _buffer[_currentPos];
}

void RequestHandler::readTo(char c)
{
	while (nextChar() != c);
}

void RequestHandler::receive()
{
	int bytesRead = read(_requestSocket + _currentPos, _buffer + _received, 8190 - _received);
	if (bytesRead < 0)	
	{
		printf("Brokent pipe\n");
		throw Status::Http403; // FIXME Broken pipe thing	
	}
	printf("Received:\n%s\n", _buffer + _received);
	_received += bytesRead;
}

void RequestHandler::addHeader(const char* name, const char* value)
{
	printf("Add header: <%s> -> <%s>", name, value);
	
	if (strcasecmp(name, "content-length") == 0)
	{
		char* valueEnd;
		_contentLength = strtoul(value, &valueEnd, 10);
		
		if (valueEnd == value) // Empty value 
		{
			throw Status::Http403;
		}
		
		for (; *valueEnd != '\0'; valueEnd++) // Non ws trailing characters
		{
			if (! isspace(*valueEnd))
			{
				throw Status::Http403;
			}
		}
	}
}
