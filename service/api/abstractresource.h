/* 
 * File:   AbstractResource.h
 * Author: Christian Surlykke <christian@surlykke.dk>
 *
 * Created on 15. marts 2015, 09:42
 */

#ifndef ABSTRACTRESOURCE_H
#define	ABSTRACTRESOURCE_H

#include <pthread.h>
#include <map>
#include <string.h>

#include "../../common/httpprotocol.h"
#include "../../common/httpmessage.h"

class AbstractResource
{
public:
	AbstractResource();
	virtual ~AbstractResource();

	virtual void doRequest(int socket, const HttpMessage& request) = 0;
	virtual void doWebsocketRequest(int socket, const char* path, const char* queryString, const char *subprotocol) { } // FIXME
};

class StaticResource : public AbstractResource
{
public:
	StaticResource(const char* content);
	~StaticResource() { delete this->content; }

	virtual void doRequest(int socket, const HttpMessage& request);

private:
	void buildResource(const char* content);
	char* content;
	int size;
};

struct ResourceMappings;

class ResourceMap
{
public:
	ResourceMap();
	virtual ~ResourceMap();

	void map(const char* path, AbstractResource* resource);
	void unMap(AbstractResource* resource);
	AbstractResource* resource(const char* path);

private:
	pthread_rwlock_t mLock;
	ResourceMappings* mResourceMappings;
};


#endif	/* ABSTRACTRESOURCE_H */

