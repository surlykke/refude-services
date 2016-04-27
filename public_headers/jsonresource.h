/*
* Copyright (c) 2015, 2016 Christian Surlykke
*
* This file is part of the Restful Inter Process Communication (Ripc) project. 
* It is distributed under the LGPL 2.1 license.
* Please refer to the LICENSE file for a copy of the license.
*/

#ifndef JSONRESOURCE_H
#define JSONRESOURCE_H

#include <shared_mutex>
#include "map.h"
#include "abstractresource.h"
#include "json.h"
#include "abstractcachingresource.h"

namespace org_restfulipc
{
    using namespace std;

    class JsonResource : public AbstractCachingResource
    {
    public:
        typedef std::shared_ptr<JsonResource> ptr;
        JsonResource();
        virtual ~JsonResource();
        const Json& getJson(); 
        void setJson(Json&& json);

    protected:
        Buffer buildContent(HttpMessage& request, map<string, string>& headers) override;
        Json json;

    };

    class LocalizedJsonResource : public AbstractCachingResource
    {
    public:
        typedef std::shared_ptr<LocalizedJsonResource> ptr; 
        LocalizedJsonResource();
        virtual ~LocalizedJsonResource();
        const Json& getJson(); 
        void setJson(Json&& json);

    protected:
        virtual Buffer buildContent(HttpMessage& request, map<string, string>& headers) override;
        Json json;

    private:
        string getLocaleToServe(const char* acceptLanguageHeader);

    };

     

}
#endif // JSONRESOURCE_H
