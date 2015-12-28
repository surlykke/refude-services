#ifndef RESOURCEMAPPING_H
#define RESOURCEMAPPING_H

#include <shared_mutex>


namespace org_restfulipc
{
    class AbstractResource;
    class ResourceMapping
    {
    public:
        bool match(const char* path);
        const char* mPath;
        size_t mPathLength;
        AbstractResource *mResource;
        bool mWildcarded;
    };

    struct ResourceMappings
    {
        ResourceMappings();
        ~ResourceMappings();
        void map(const char* path, AbstractResource* resource, bool wildcarded);
        void unMap(const AbstractResource* resource);
        ResourceMapping* find(const char* path);
        ResourceMapping* mMappings;
        std::shared_timed_mutex mMappingsMutex;
        int mNumMappings;
        int mMappingsCapacity;
    };

}

#endif // RESOURCEMAPPING_H
