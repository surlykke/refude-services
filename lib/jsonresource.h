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
#include "abstractresource.h"

namespace refude
{
    class JsonResource : public AbstractResource
    {
    public:
        using ptr = std::unique_ptr<JsonResource>;
        JsonResource();
        JsonResource(Json&& json);
        virtual ~JsonResource();
        virtual void doGET(Descriptor& socket, HttpMessage &request, const char* remainingPath) override;

        Json& getJson();
        void setJson(Json&& json);

    private:
        Buffer buildContent(std::vector<std::string> locales);
        Map<Buffer> localizedCache; // Maps from lang to serialization
        Json json;
    };
}

#endif /* JSONRESOURCE_H */

