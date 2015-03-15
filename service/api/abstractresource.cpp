/* 
 * File:   AbstractResource.cpp
 * Author: Christian Surlykke <christian@surlykke.dk>
 * 
 * Created on 15. marts 2015, 09:42
 */
#include <string.h>

#include "abstractresource.h"

AbstractResource::AbstractResource()
{
}

AbstractResource::~AbstractResource()
{
}


struct ResourceMapping
{
	ResourceMapping(const char* path, const AbstractResource* resource): path(path), resource(resource) {};
	const char* path;
	const AbstractResource *resource;
};

struct ResourceMappings
{
	ResourceMappings(): mappings(NULL), capacity(0), numElements(0) {};

	ResourceMapping* mappings;
	int capacity;
	int numElements;
	
	void addMapping(const char* path, const AbstractResource* resource)
	{
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

	const AbstractResource* resource(const char* path)
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
}

ResourceMap::~ResourceMap()
{
}

void ResourceMap::map(const char* path, const AbstractResource* resource)
{
	pthread_rwlock_wrlock(&mLock);
	mResourceMappings->addMapping(path, resource);		
	pthread_rwlock_unlock(&mLock);
}

void ResourceMap::unMap(const AbstractResource* resource)
{
	pthread_rwlock_wrlock(&mLock);
	mResourceMappings->remove(resource);
	pthread_rwlock_unlock(&mLock);
}


const AbstractResource* ResourceMap::resource(const char* path)
{
	pthread_rwlock_rdlock(&mLock);
	const AbstractResource* resource = mResourceMappings->resource(path);
	pthread_rwlock_unlock(&mLock);

	return resource;
}
