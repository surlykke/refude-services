/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the refude-services project. 
 * It is distributed under the GPL v2 license.
 * Please refer to the LICENSE file for a copy of the license.
 */
#ifndef COLLECTIONRESOURCE_H
#define COLLECTIONRESOURCE_H
#include "json.h"
#include <set>
#include "abstractcachingresource.h"

namespace org_restfulipc 
{
    class CollectionResource : public AbstractCachingResource
    {
    public:
        typedef std::shared_ptr<CollectionResource> ptr; 
        CollectionResource();
        virtual ~CollectionResource();
        Json& getJsonArray() { return jsonArray; } 
        const char* getId() { return id; }
        void setJsonArray(Json&& json, const char* id = "Id");

    protected:
        virtual Buffer buildContent(HttpMessage& request, std::map<std::string, std::string>& headers);
        Json jsonArray;
        const char* id;
        Map<uint> indexes;
    };

    class CollectionResourceUpdater
    {
    public:
        CollectionResourceUpdater(CollectionResource::ptr collectionResource);
        void update(Json& newJsonArray, const char* idKey);
        std::set<std::string> addedResources;
        std::set<std::string> updatedResources;
        std::set<std::string> removedResources;
    
    private:
        CollectionResource::ptr collectionResource;
    };
}
#endif // COLLECTIONRESOURCE_H
