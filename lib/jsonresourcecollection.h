#ifndef JSONRESOURCECOLLECTION_H
#define JSONRESOURCECOLLECTION_H

#include <set>
#include <string>
#include "service.h"
#include "notifierresource.h"
#include "jsonresource.h"

namespace refude
{
    class JsonResourceCollection
    {
    public:
        JsonResourceCollection(Service* service, NotifierResource *notifier);
        void updateCollection(Map<JsonResource::ptr>&& pathsAndResources);

    private:
        Service* service;
        NotifierResource* notifier;
        std::vector<std::string> resourcePaths;
    };
}

#endif // JSONRESOURCECOLLECTION_H
