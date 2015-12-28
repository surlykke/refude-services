#include <iostream>
#include "errorhandling.h"
#include "resourcemapping.h"

namespace org_restfulipc
{

    bool ResourceMapping::match(const char* path)
    {
        for (const char* p = mPath; *p; p++)  {
            if (*p != *path++) {
                return NULL;
            }
        }

        return (!*path) || (mWildcarded && *path == '/');
    }


    ResourceMappings::ResourceMappings() :
        mMappings(0),
        mMappingsMutex(),
        mNumMappings(0),
        mMappingsCapacity(0)

    {
        std::cout << "Into ResourceMappings()...\n";
        mMappings = (ResourceMapping*) calloc(8, sizeof(ResourceMapping));
        std::cout << "mMappings: "  << (long) mMappings << "\n";
        if (!mMappings) {
            throw C_Error();
        }
        mMappingsCapacity = 8;
        std::cout << "ResourceMappings constructor done\n";
    }

    ResourceMappings::~ResourceMappings()
    {
        if (mMappings) {
            free(mMappings);
        }
    }

    void ResourceMappings::map(const char* path, AbstractResource* resource, bool wildcarded)
    {
        std::unique_lock<std::shared_timed_mutex> lock(mMappingsMutex);
        if (mMappingsCapacity <= mNumMappings) {
            mMappings = (ResourceMapping*) realloc(mMappings, 2*mMappingsCapacity*sizeof(ResourceMapping));
            if (!mMappings) throw C_Error();
            mMappingsCapacity = 2*mMappingsCapacity;
        }

        mMappings[mNumMappings++] = {path, strlen(path), resource, wildcarded};
    }

    void ResourceMappings::unMap(const AbstractResource* resource)
    {
        std::unique_lock<std::shared_timed_mutex> lock(mMappingsMutex);
        int numRemoved = 0;
        for (int i = 0; i < mNumMappings; i++) {
            if (mMappings[i].mResource == resource) {
                numRemoved++;
            }

            mMappings[i - numRemoved] = mMappings[i];
        }
        mNumMappings -= numRemoved;
    }

    ResourceMapping*ResourceMappings::find(const char* path)
    {
        std::shared_lock<std::shared_timed_mutex> lock(mMappingsMutex);
        for (int i = 0; i < mNumMappings; i++) {
            if (mMappings[i].match(path)) {
                return mMappings + i;
            }
        }

        return NULL;
    }



}
