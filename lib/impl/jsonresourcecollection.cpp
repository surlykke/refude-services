#include <string.h>
#include "jsonresource.h"
#include "jsonresourcecollection.h"

namespace refude
{
    JsonResourceCollection::JsonResourceCollection(Service* service, NotifierResource* notifier):
        service(service),
        notifier(notifier),
        resourcePaths()
    {
    }

    void JsonResourceCollection::updateCollection(Map<JsonResource::ptr>&& pathsAndResources)
    {
        for (const std::string& path : resourcePaths) {
            if (pathsAndResources.find(path) < 0) {
                service->unMap(path.data());
                notifier->resourceRemoved(path.data());
            }
        }

        resourcePaths.clear();

        pathsAndResources.each([this](const char* path, JsonResource::ptr& json) {
            AbstractResource* resource = service->mapping(path);
            if (resource) {
                JsonResource* jsonResource = dynamic_cast<JsonResource*>(resource);
                if (jsonResource == 0 || jsonResource->getJson() != json->getJson()) {
                    service->map(std::move(json), path);
                    notifier->resourceUpdated(path);
                }
            }
            else {
                service->map(std::move(json), path);
                notifier->resourceAdded(path);
            }

            resourcePaths.push_back(path);
        });
    }
}
