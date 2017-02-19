/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the Refude Services project, library
 * It is distributed under the LGPL 2.1 license.
 * Please refer to the LGPL21 file for a copy of the license.
 */

#include <sys/time.h>

#include "utils.h"
#include "localizingjsonwriter.h"
#include "httpprotocol.h"
#include "comm.h"

#include "jsonresource.h"

namespace refude
{

    JsonResource::JsonResource() : AbstractResource(), localizedCache(), json(JsonConst::EmptyObject)
    {
    }

    JsonResource::JsonResource(Json&& json) : AbstractResource(), localizedCache(), json(std::move(json))
    {
    }

    JsonResource::~JsonResource()
    {
    }

    void JsonResource::doGET(Descriptor& socket, HttpMessage& request, const char *remainingPath)
    {
        std::shared_lock<std::shared_mutex> readLock(mutex);

        std::vector<std::string> locales = getAcceptedLocales(request);
        std::string localeSignature;
        for (int i = 0; i < locales.size(); i++) {
            localeSignature.append(locales[i]).append(" ");
        }

        if (localizedCache.find(localeSignature) < 0) {
            readLock.unlock();
            std::lock_guard<std::shared_mutex> writeLock(mutex);
            localizedCache[localeSignature] = buildContent(locales);
            sendBuffer(socket, localizedCache[localeSignature]);
        }
        else {
            sendBuffer(socket, localizedCache[localeSignature]);
        }

    }
    
    
    Json& JsonResource::getJson()
    {
        return json;
    }


    void JsonResource::setJson(Json&& json)
    {
        this->json = std::move(json);
        localizedCache.clear();
    }

    Buffer JsonResource::buildContent(std::vector<std::string> locales)
    {
        LocalizingJsonWriter writer(json, locales);

        Buffer result;
        result.write(statusLine(HttpCode::Http200))
              .write("Content-Type: application/json; charset=utf-8\r\n")
              .write("Content-Length: ").write(writer.buffer.size()).write("\r\n")
              .write("\r\n")
              .write(writer.buffer.data());

        return result;
    }
}
