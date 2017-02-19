#include <string.h>
#include "resourcecollection.h"
#include "jsonresource.h"
#include "jsonresourcecollection.h"

namespace refude
{
    JsonResourceCollection::JsonResourceCollection():
        resourcePaths()
    {
    }

    void JsonResourceCollection::updateCollection(Map<JsonResource::ptr>&& pathsAndResources)
    {
        std::shared_lock<std::shared_mutex> writeLock(ResourceCollection::resourceMutex);

        Map<AbstractResource::ptr> addOrReplace;
        std::vector<std::string> remove;

        for (const std::string& path : resourcePaths) {
            if (pathsAndResources.find(path) < 0) {
                remove.push_back(path);
            }
        }

        resourcePaths.clear();


        for (Map<JsonResource::ptr>::Entry& entry : pathsAndResources) {
            JsonResource* resource = dynamic_cast<JsonResource*>(ResourceCollection::mapping(entry.key));
            if (! (resource && entry.value->getJson() == resource->getJson())) {
                addOrReplace[entry.key] = std::move(entry.value);
            }

            resourcePaths.push_back(entry.key);
        }

        ResourceCollection::bulkUpdate(std::move(addOrReplace), remove);
    }
}
