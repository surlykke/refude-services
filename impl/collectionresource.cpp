/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the refude-services project. 
 * It is distributed under the GPL v2 license.
 * Please refer to the LICENSE file for a copy of the license.
 */
#include <set>

#include "localizingjsonwriter.h"
#include "collectionresource.h"
namespace org_restfulipc
{

    CollectionResource::CollectionResource() :
        AbstractCachingResource(),
        jsonArray(JsonConst::EmptyArray),
        id("")
    {
    }

    CollectionResource::~CollectionResource()
    {
    }

    void CollectionResource::setJsonArray(Json&& json, const char* id)
    {
        if (json.type() != JsonType::Array) {
            throw RuntimeError("Json given to CollectionResource should be an array, was %s", json.typeAsString());
        }
        jsonArray = std::move(json);
        this->id = id;
        for (uint index = 0; index < jsonArray.size(); index++) {
            Json& json = jsonArray[index];
            if (json.type() == JsonType::Object && json.contains(id)) {
                indexes.add(json[id], index);
            }
        }
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

    void CollectionResourceUpdater::update(Json& newJsonArray, const char* idKey)
    {
        {
            Map<int> newJsons;
            Json& oldJsonArray = collectionResource->getJsonArray();
            for (uint i = 0; i < newJsonArray.size(); i++) {
                const char* id = newJsonArray[i][idKey];
                newJsons[id] = i;
                addedResources.insert((const char*) newJsonArray[i][idKey]);
            }

            for (uint i = 0; i < oldJsonArray.size(); i++) {
                const char* id = oldJsonArray[i][idKey];
                if (newJsons.contains(id)) {
                    addedResources.erase(id);
                    if (oldJsonArray[i] != newJsonArray[newJsons[id]]) {
                        updatedResources.insert(id);
                    }
                }
                else {
                    removedResources.insert(id);
                }

            }
        }
        collectionResource->setJsonArray(std::move(newJsonArray), idKey);
    }

}
