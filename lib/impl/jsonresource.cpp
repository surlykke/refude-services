/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the Refude Services project, library
 * It is distributed under the LGPL 2.1 license.
 * Please refer to the LGPL21 file for a copy of the license.
 */

#include "utils.h"
#include "jsonwriter.h"
#include "buffer.h"
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
        cache.clear();
    }

    Buffer::ptr JsonResource::buildContent(HttpMessage& request)
    {
        Buffer::ptr result = std::make_shared<Buffer>();
        LocalizingJsonWriter writer(json, getAcceptedLocales(request));
        result->writeStr("HTTP/1.1 200 OK\r\n"
                         "Content-Type: application/json; charset=UTF-8\r\n")
               .writeStr("Content-Length: ").writeLong(writer.buffer.size()).writeStr("\r\n")
               .writeStr(writer.buffer.data());
        return result;
    }

}
