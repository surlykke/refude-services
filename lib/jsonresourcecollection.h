#ifndef JSONRESOURCECOLLECTION_H
#define JSONRESOURCECOLLECTION_H

#include <set>
#include <string>
#include "service.h"
#include "jsonresource.h"

namespace refude
{
    class JsonResourceCollection
    {
    public:
        JsonResourceCollection();
        void updateCollection(Map<JsonResource::ptr>&& pathsAndResources);

    private:
        std::vector<std::string> resourcePaths;
    };
}

#endif // JSONRESOURCECOLLECTION_H
