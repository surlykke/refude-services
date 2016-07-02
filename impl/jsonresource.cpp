/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the Restful Inter Process Communication (Ripc) project. 
 * It is distributed under the LGPL 2.1 license.
 * Please refer to the LICENSE file for a copy of the license.
 */
#include <sys/socket.h>
#include <unistd.h>
#include <algorithm>
#include <map>
#include <mutex>
#include "jsonwriter.h"
#include "jsonresource.h"
#include "utils.h"

namespace org_restfulipc
{

    JsonResource::JsonResource(Json&& json) :
        AbstractResource(),
            json(std::move(json))
    {
        buildResponse();
    }

    JsonResource::~JsonResource()
    {
    }

    void JsonResource::setJson(Json&& newJson)
    {
        std::lock_guard<std::mutex> lock(mutex);

        if (json != newJson)  {
            json = std::move(newJson); 
            buildResponse();
        }
    }

    void JsonResource::doGET(int& socket, HttpMessage& request)
    {
        std::lock_guard<std::mutex> lock(mutex);
        sendFully(socket, cannedResponse.data(), cannedResponse.size()); 
    }

    void JsonResource::buildResponse()
    {
        static const char headerTemplate[] =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json; charset=utf-8\r\n"
            "Content-Length: %d\r\n"
            "\r\n";

        char header[sizeof(headerTemplate) + 20]; 
        JsonWriter writer(json);
        sprintf(header, headerTemplate, writer.buffer.size());    
        cannedResponse.clear();
        cannedResponse.write(header);
        cannedResponse.write(writer.buffer.data());
    }
}
