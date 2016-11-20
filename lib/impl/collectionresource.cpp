/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the Refude Services project, library
 * It is distributed under the LGPL 2.1 license.
 * Please refer to the LGPL21 file for a copy of the license.
 */

#include <set>

#include "localizingjsonwriter.h"
#include "collectionresource.h"
namespace refude
{

    CollectionResource::CollectionResource(const char* resourceIdKey) :
        AbstractCachingResource(),
        jsonArray(JsonConst::EmptyArray),
        resourceIdKey(resourceIdKey)
    {
    }

    CollectionResource::~CollectionResource()
    {
    }

    Buffer CollectionResource::buildContent(HttpMessage& request, std::map<std::string, std::string>& headers)
    {
        if (*(request.remainingPath)) {
            if (indexes.contains(request.remainingPath)) {
                Json& json = jsonArray[indexes[request.remainingPath]];
                return LocalizingJsonWriter(json, getAcceptedLocales(request)).buffer;
            }
            else {
                throw HttpCode::Http404;
            }
        }
        else {
            return LocalizingJsonWriter(jsonArray, getAcceptedLocales(request)).buffer;
        }

    }

    CollectionResourceUpdater::CollectionResourceUpdater(CollectionResource::ptr collectionResource) :
        collectionResource(collectionResource)
    {
    }

    void CollectionResourceUpdater::update(Json& newJsonArray)
    {
        {
            std::unique_lock<std::recursive_mutex> lock(collectionResource->m);
            {
                const char* resourceIdKey = collectionResource->resourceIdKey;
                Map<int> newJsons;
                for (uint i = 0; i < newJsonArray.size(); i++) {
                    const char* id = newJsonArray[i][resourceIdKey];
                    newJsons[id] = i;
                    addedResources.insert((const char*) newJsonArray[i][resourceIdKey]);
                }

                for (uint i = 0; i < collectionResource->jsonArray.size(); i++) {
                    const char* id = collectionResource->jsonArray[i][resourceIdKey];
                    if (newJsons.contains(id)) {
                        addedResources.erase(id);
                        if (collectionResource->jsonArray[i] != newJsonArray[newJsons[id]]) {
                            updatedResources.insert(id);
                        }
                    }
                    else {
                        removedResources.insert(id);
                    }

                }
            }
            collectionResource->jsonArray = std::move(newJsonArray);
            collectionResource->indexes.clear();
            for (uint i = 0; i < collectionResource->jsonArray.size(); i++) {
                collectionResource->indexes[collectionResource->jsonArray[i][collectionResource->resourceIdKey]] = i;
            }
            collectionResource->clearCache();
        }

    }

    void CollectionResourceUpdater::updateSingle(Json& json)
    {
        std::unique_lock<std::recursive_mutex> lock(collectionResource->m);
        const char* resourceId = json[collectionResource->resourceIdKey];
        if (collectionResource->indexes.contains(resourceId)) {
            collectionResource->jsonArray[collectionResource->indexes[resourceId]] = json.copy();
            updatedResources.insert(resourceId);
        }
    }

    void CollectionResourceUpdater::notify(NotifierResource::ptr notifier, const char* prefix)
    {
        for (std::string id : addedResources) {
            notifier->resourceAdded(prefix, id.data());
        }
        for (std::string id : removedResources) {
            notifier->resourceRemoved(prefix, id.data());
        }
        for (std::string id : updatedResources) {
            notifier->resourceUpdated(prefix, id.data());
        }
    }
}
