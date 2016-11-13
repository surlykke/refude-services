/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the Refude Services project, library
 * It is distributed under the LGPL 2.1 license.
 * Please refer to the LGPL21 file for a copy of the license.
 */

#include "utils.h"
#include "jsonwriter.h"

#include "localizingjsonwriter.h"

#include "jsonresource.h"

namespace refude
{

    JsonResource::JsonResource() :
        AbstractCachingResource(),
        json(JsonConst::EmptyObject)
    {
    }

    JsonResource::~JsonResource()
    {
    }

    Json& JsonResource::getJson()
    {
        return json;
    }

    void JsonResource::setJson(Json&& json)
    {
        this->json = std::move(json);
        clearCache();
    }

    Buffer JsonResource::buildContent(HttpMessage& request, std::map<std::string, std::string>& headers)
    {
        return LocalizingJsonWriter(json, getAcceptedLocales(request)).buffer;
    }

}
