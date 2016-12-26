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

    Buffer::ptr CollectionResource::buildContent(HttpMessage& request)
    {
        Buffer::ptr result = std::make_shared<Buffer>();
        LocalizingJsonWriter writer(jsonArray, getAcceptedLocales(request));
        result->writeStr("HTTP/1.1 200 OK\r\n"
                         "Content-Type: application/json; charset=UTF-8\r\n")
               .writeStr("Content-Length: ").writeLong(writer.buffer.size()).writeStr("\r\n")
               .writeStr(writer.buffer.data());
        return result;
    }


    CollectionResourceUpdater::CollectionResourceUpdater(CollectionResource::ptr collectionResource) :
        collectionResource(collectionResource)
    {
    }

    void CollectionResourceUpdater::update(Json& newJsonArray)
    {
        const char* resourceIdKey = collectionResource->resourceIdKey;
        Map<int> newJsons;
        for (uint i = 0; i < newJsonArray.size(); i++) {
            const char* id = newJsonArray[i][resourceIdKey].toString();
            newJsons[id] = i;
            addedResources.insert(newJsonArray[i][resourceIdKey].toString());
        }

        for (uint i = 0; i < collectionResource->jsonArray.size(); i++) {
            const char* id = collectionResource->jsonArray[i][resourceIdKey].toString();
            if (newJsons.find(id) >= 0) {
                addedResources.erase(id);
                if (collectionResource->jsonArray[i] != newJsonArray[newJsons[id]]) {
                    updatedResources.insert(id);
                }
            }
            else {
                removedResources.insert(id);
            }

        }

        collectionResource->jsonArray = std::move(newJsonArray);
        collectionResource->indexes.clear();
        for (uint i = 0; i < collectionResource->jsonArray.size(); i++) {
            collectionResource->indexes[collectionResource->jsonArray[i][collectionResource->resourceIdKey].toString()] = i;
        }
        collectionResource->cache.clear();
    }

    void CollectionResourceUpdater::updateSingle(Json& json)
    {
        const char* resourceId = json[collectionResource->resourceIdKey].toString();
        if (collectionResource->indexes.find(resourceId) >= 0) {
            collectionResource->jsonArray[collectionResource->indexes[resourceId]] = json.copy();
            updatedResources.insert(resourceId);
        }
    }

    void CollectionResourceUpdater::notify(NotifierResource::ptr notifier, const char* prefix)
    {
        std::string p(prefix);
        for (std::string id : addedResources) {
            notifier->notify("resource-added", p + "/" + id);
        }
        for (std::string id : removedResources) {
            notifier->notify("resource-removed", p + "/" + id);
        }
        for (std::string id : updatedResources) {
            notifier->notify("resource-updated", p + "/" + id);
        }
    }
}
