/*
* Copyright (c) 2015, 2016 Christian Surlykke
*
* This file is part of the Restful Inter Process Communication (Ripc) project. 
* It is distributed under the LGPL 2.1 license.
* Please refer to the LICENSE file for a copy of the license.
*/

#ifndef GENERICRESOURCE_H
#define GENERICRESOURCE_H

#include <shared_mutex>

#include "abstractresource.h"
#include "notifierresource.h"
#include "abstractcachingresource.h"

namespace org_restfulipc
{
    class GenericResource : public AbstractCachingResource
    {
    public:
        typedef std::shared_ptr<GenericResource> ptr;
        GenericResource(const char* doc = "", NotifierResource::ptr notifierResource = 0);
        virtual ~GenericResource();

        void update(const char* data);

    private:
        NotifierResource::ptr notifierResource;
        std::string doc;
    };
}


#endif /* GENERICRESOURCE_H */

