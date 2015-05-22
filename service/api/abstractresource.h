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
#include <vector>

#include "../../common/httpprotocol.h"
#include "../../common/httpmessage.h"

class AbstractResource
{
public:
	AbstractResource() {};
	virtual ~AbstractResource() {};
	virtual void handleRequest(int socket, const HttpMessage& request) = 0;
};

class GenericResource : public AbstractResource
{
public:
	GenericResource(const char* json = "{}");
	virtual ~GenericResource();

	virtual void handleRequest(int socket, const HttpMessage& request);
	virtual void doGet(int socket, const HttpMessage& request);
	virtual void doStreamUpgrade(int socket, const HttpMessage& request);
	virtual void doPatch(int socket, const HttpMessage& request);

protected:
	void update(const char* data);

private:
	void notifyClients();	
	void writeData(int socket, const char *data, int nBytes);


	char _response[8192];
	char* _respPtr;
	int _responseLength;
	pthread_rwlock_t _lock;	

	std::vector<int> _webSockets;
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

