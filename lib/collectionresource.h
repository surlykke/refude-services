/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the Refude Services project, library
 * It is distributed under the LGPL 2.1 license.
 * Please refer to the LGPL21 file for a copy of the license.
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
        Map<int> indexes;
         
    
    protected:
        virtual Buffer::ptr buildContent(HttpMessage&);

    friend CollectionResourceUpdater;
    };

    class CollectionResourceUpdater
    {
    public:
        CollectionResourceUpdater(CollectionResource::ptr collectionResource);
        void update(Json& newJsonArray);
        void updateSingle(Json& json);
        void notify(NotifierResource::ptr notifier, const char* prefix);
        std::set<std::string> addedResources;
        std::set<std::string> updatedResources;
        std::set<std::string> removedResources;
    
    private:
        CollectionResource::ptr collectionResource;
    };
}
#endif // COLLECTIONRESOURCE_H
