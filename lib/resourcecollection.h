#ifndef RESOURCECOLLECTION_H
#define RESOURCECOLLECTION_H
#include <shared_mutex>
#include <string>
#include "notifierresource.h"
#include "abstractresource.h"

namespace refude
{
    namespace ResourceCollection
    {
        extern std::shared_mutex resourceMutex;
        // These methods are not threadsafe, caller must take a lock on resourceMutex

        // write lock for these
        void mapPath(AbstractResource::ptr&& resource, const std::string& path);
        void mapPrefix(AbstractResource::ptr&& resource, const std::string& prefix);
        void unmapPath(const std::string& path);
        void unmapPrefix(const std::string& prefix);
        void bulkUpdate(Map<AbstractResource::ptr>&& addOrReplace, const std::vector<std::string>& remove);

        // read or write lock for these
        bool getResource(AbstractResource*& handler, int& matchedLength, const char* path);
        AbstractResource* mapping(const std::string& path);
        AbstractResource* prefixMapping(const std::string& path);



    }
}

#endif // RESOURCECOLLECTION_H
