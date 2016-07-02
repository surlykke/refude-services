/*
* Copyright (c) 2015, 2016 Christian Surlykke
*
* This file is part of the Restful Inter Process Communication (Ripc) project. 
* It is distributed under the LGPL 2.1 license.
* Please refer to the LICENSE file for a copy of the license.
*/

#ifndef JSONRESOURCE_H
#define JSONRESOURCE_H

#include <mutex>
#include "map.h"
#include "abstractresource.h"
#include "abstractcachingresource.h"
#include "json.h"
#include "notifierresource.h"

namespace org_restfulipc
{

    class JsonResource : public AbstractResource
    {
    public:
        typedef std::shared_ptr<JsonResource> ptr;
        JsonResource(Json&& json);
        virtual ~JsonResource();
        void setJson(Json&& json);
        virtual void doGET(int& socket, HttpMessage& request) override;

    private:
        Json json;
        void buildResponse();
        Buffer cannedResponse;
        std::mutex mutex;
    };

     

}
#endif // JSONRESOURCE_H
