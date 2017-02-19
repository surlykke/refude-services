
#include "resourcecollection.h"

namespace refude
{
    namespace ResourceCollection
    {

        std::shared_mutex resourceMutex;

        Map<AbstractResource::ptr> resourceMappings;
        Map<AbstractResource::ptr> prefixMappings;
        NotificationStream notifier;

        void mapPath(AbstractResource::ptr&& resource, const std::string& path)
        {
            if (resourceMappings.find(path) > -1) {
                notifier.resourceUpdated(path);
            }
            else {
                notifier.resourceAdded(path);
            }

            resourceMappings[path] = std::move(resource);

        }


        void mapPrefix(AbstractResource::ptr&& resource, const std::string& prefix)
        {
            prefixMappings[prefix] = std::move(resource);
        }

        void unmapPath(const std::string& path)
        {
            resourceMappings.erase(path);
            notifier.resourceRemoved(path);
        }

        void unmapPrefix(const std::string& prefix)
        {
            prefixMappings.erase(prefix);
        }

        void bulkUpdate(Map<AbstractResource::ptr>&& addOrReplace, const std::vector<std::string>& remove)
        {
            for (Map<AbstractResource::ptr>::Entry& entry : addOrReplace) {
                if (resourceMappings.find(entry.key) < 0) {
                    resourceMappings[entry.key] = std::move(entry.value);
                }
                else {
                    resourceMappings[entry.key] = std::move(entry.value);
                    notifier.resourceUpdated(entry.key);
                }

            }

            for (const std::string& path : remove) {
                notifier.resourceRemoved(path);
                resourceMappings.erase(path);
            }
        }


        bool getResource(AbstractResource*& handler, int& matchedLength, const char* path)
        {
            int resourceIndex = resourceMappings.find(path);
            if (resourceIndex > -1) {
                handler = resourceMappings.pairAt(resourceIndex).value.get();
                matchedLength = strlen(path);
                return true;
            }
            else {
                resourceIndex = prefixMappings.find_longest_prefix(path);
                if (resourceIndex >= 0) {
                    int matchedLength = prefixMappings.pairAt(resourceIndex).key.size();
                    handler = prefixMappings.pairAt(resourceIndex).value.get();
                    matchedLength = prefixMappings.pairAt(resourceIndex).key.size();
                }
                return true;
            }

            return false;
        }

        AbstractResource* mapping(const std::string& path)
        {
            int pos = resourceMappings.find(path);
            return pos < 0 ? NULL : resourceMappings.pairAt(pos).value.get();
        }

        AbstractResource* prefixMapping(const std::string& path)
        {
            int pos = prefixMappings.find(path);
            return pos < 0 ? NULL : prefixMappings.pairAt(pos).value.get();
        }

        bool setupNotification()
        {
            mapPath(std::make_unique<NotifierResource>(&notifier), "/notify");
        }

        bool notificationSetup = setupNotification();
    }
}


