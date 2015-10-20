#include "resourcemap.h"

namespace org_restfulipc
{
    struct ResourceMapping
    {
        ResourceMapping(const char* path, AbstractResource* resource): path(path), resource(resource) {}
        const char* path;
        AbstractResource *resource;
    };

    struct ResourceMappings
    {
        ResourceMappings(): mappings(NULL), capacity(0), numElements(0) {}

        ResourceMapping* mappings;
        int capacity;
        int numElements;
        
        void addMapping(const char* path, AbstractResource* resource)
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

    ResourceMap::ResourceMap() : m(), mResourceMappings(new ResourceMappings)
    {
    }

    ResourceMap::~ResourceMap()
    {
        delete mResourceMappings;
    }

    void ResourceMap::map(const char* path, AbstractResource* resource)
    {
        std::unique_lock<std::shared_timed_mutex> lock(m);
        mResourceMappings->addMapping(path, resource);
    }

    void ResourceMap::unMap(AbstractResource* resource)
    {
        std::unique_lock<std::shared_timed_mutex> lock(m);
        mResourceMappings->remove(resource);
    }


    AbstractResource* ResourceMap::resource(const char* path)
    {
        std::shared_lock<std::shared_timed_mutex> lock(m);
        return mResourceMappings->resource(path);
    }

}



