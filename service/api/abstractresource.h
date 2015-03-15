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

class AbstractResource
{
public:
	AbstractResource();
	virtual ~AbstractResource();

	virtual const char* GET(const char* path, const char* queryString) = 0;
	virtual bool PATCH(const char* patchJS) { return false; }

};

struct ResourceMappings;

class ResourceMap
{
public:
	ResourceMap();
	virtual ~ResourceMap();

	void map(const char* path, const AbstractResource* resource);
	void unMap(const AbstractResource* resource);
	const AbstractResource* resource(const char* path);

private:
	pthread_rwlock_t mLock;
	ResourceMappings* mResourceMappings;
};


#endif	/* ABSTRACTRESOURCE_H */

