/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the Refude Services project, library
 * It is distributed under the LGPL 2.1 license.
 * Please refer to the LGPL21 file for a copy of the license.
 */

#ifndef JSONRESOURCE_H
#define JSONRESOURCE_H

#include <map>
#include <string>
#include "buffer.h"
#include "json.h"
#include "httpmessage.h"
#include "abstractcachingresource.h"

namespace refude
{
    class JsonResource : public AbstractCachingResource
    {
    public:
        typedef std::unique_ptr<JsonResource> ptr; 
        JsonResource();
        JsonResource(Json&& json);
        virtual ~JsonResource();
        Json& getJson(); 
        void setJson(Json&& json);

    protected:
        virtual Buffer buildContent(HttpMessage& request, std::map<std::string, std::string>& headers);
        Json json;
    };
}

#endif /* JSONRESOURCE_H */

