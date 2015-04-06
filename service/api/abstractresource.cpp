/* 
 * File:   AbstractResource.cpp
 * Author: Christian Surlykke <christian@surlykke.dk>
 * 
 * Created on 15. marts 2015, 09:42
 */
#include <string.h>
#include <unistd.h>

#include "abstractresource.h"

GenericResource::GenericResource(const char* json) : 
	AbstractResource(), 
//	_response({'\0'}),
	_respPtr(_response),
	_responseLength(0)
{
	if (pthread_rwlock_init(&_lock, NULL) < 0)
	{
		throw errno;
	}
	update(json);
}

GenericResource::~GenericResource()
{
}

void GenericResource::handleRequest(int socket, const HttpMessage& request)
{
	if (request.method() == Method::GET)	
	{
		if (request.headerValue(Header::connection) != 0 &&
			strcasecmp(request.headerValue(Header::connection), "upgrade") == 0 &&
			request.headerValue(Header::upgrade) != 0 &&
			strcasecmp(request.headerValue(Header::upgrade), "websocket") == 0)
		{
			doWebsocketUpgrade(socket, request);
		}
		else 
		{
			doGet(socket, request);	
		}
	}
	else if (request.method() == Method::PATCH)
	{
		doPatch(socket, request);
	}
	else
	{
		throw Status::Http406;
	}
}

void GenericResource::doGet(int socket, const HttpMessage& request)
{
	pthread_rwlock_rdlock(&_lock);
	int bytesWritten = 0;

	do
	{
		int nbytes = write(socket, _response + bytesWritten, _responseLength - bytesWritten);
		if (nbytes < 0)
		{
			throw errno;
		}
		bytesWritten += nbytes;
	}

	while (bytesWritten < _responseLength);
}

void GenericResource::doWebsocketUpgrade(int socket, const HttpMessage& request)
{
	throw Status::Http406;
}

void GenericResource::doPatch(int socket, const HttpMessage& request)
{
	throw Status::Http406;
}


void GenericResource::update(const char* data)
{
	static const char* responseTemplate =
		"HTTP/1.1 200 OK\r\n"
		"Content-Type: application/json; charset=UTF-8\r\n"
		"Content-Length: %d\r\n"
		"\r\n"
		"%s";

	int contentLength = strlen(data);
	
	pthread_rwlock_wrlock(&_lock);
	sprintf(_response, responseTemplate, contentLength, data);
	_responseLength = strlen(_response);
	pthread_rwlock_unlock(&_lock);
}




struct ResourceMapping
{
	ResourceMapping(const char* path, AbstractResource* resource): path(path), resource(resource) {};
	const char* path;
	AbstractResource *resource;
};

struct ResourceMappings
{
	ResourceMappings(): mappings(NULL), capacity(0), numElements(0) {};

	ResourceMapping* mappings;
	int capacity;
	int numElements;
	
	void addMapping(const char* path, AbstractResource* resource)
	{
		printf("AddMapping, capacity: %d, numElements: %d", capacity, numElements);
		while (capacity <= numElements)
		{
			capacity += 8;
			mappings = (ResourceMapping*) realloc(mappings, capacity*sizeof(ResourceMapping));
		}

		mappings[numElements++] = ResourceMapping(path, resource);
	}

	void remove(const AbstractResource* resource)
	{
		int numRemoved = 0;
		for (int i = 0; i < numElements; i++)
		{
			if (mappings[i].resource == resource)
			{
				numRemoved++;
			}

			mappings[i - numRemoved] = mappings[i];
		}
		numElements -= numRemoved;
	}

	AbstractResource* resource(const char* path)
	{
		for (int i = 0; i < numElements; i++)
		{
			if (strcmp(mappings[i].path, path) == 0)
			{
				return mappings[i].resource;
			}
		}

		return NULL;
	}
};

ResourceMap::ResourceMap()
{
	mLock = PTHREAD_RWLOCK_INITIALIZER;
	mResourceMappings = new ResourceMappings;
}

ResourceMap::~ResourceMap()
{
}

void ResourceMap::map(const char* path, AbstractResource* resource)
{
	pthread_rwlock_wrlock(&mLock);
	mResourceMappings->addMapping(path, resource);		
	pthread_rwlock_unlock(&mLock);
}

void ResourceMap::unMap(AbstractResource* resource)
{
	pthread_rwlock_wrlock(&mLock);
	mResourceMappings->remove(resource);
	pthread_rwlock_unlock(&mLock);
}


AbstractResource* ResourceMap::resource(const char* path)
{
	pthread_rwlock_rdlock(&mLock);
	AbstractResource* resource = mResourceMappings->resource(path);
	pthread_rwlock_unlock(&mLock);

	return resource;
}
