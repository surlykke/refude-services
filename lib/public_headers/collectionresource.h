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
#include "notifierresource.h"

namespace refude 
{
    class CollectionResourceUpdater;

    class CollectionResource : public AbstractCachingResource
    {
    public:
        typedef std::shared_ptr<CollectionResource> ptr; 
        CollectionResource(const char* resourceIdKey);
        virtual ~CollectionResource();
        Json jsonArray;
        const char* resourceIdKey;
        Map<uint> indexes;
         
    
    protected:
        virtual Buffer buildContent(HttpMessage& request, std::map<std::string, std::string>& headers);

    friend CollectionResourceUpdater;
    };

    class CollectionResourceUpdater
    {
    public:
        CollectionResourceUpdater(CollectionResource::ptr collectionResource);
        void update(Json& newJsonArray);
        void notify(NotifierResource::ptr notifier, const char* prefix);
        std::set<std::string> addedResources;
        std::set<std::string> updatedResources;
        std::set<std::string> removedResources;
    
    private:
        CollectionResource::ptr collectionResource;
    };
}
#endif // COLLECTIONRESOURCE_H
