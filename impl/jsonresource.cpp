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
#include "jsonwriter.h"
#include "jsonresource.h"
#include "utils.h"

namespace org_restfulipc
{

    JsonResource::JsonResource() :
        AbstractCachingResource(),
        json(JsonConst::EmptyObject)
    {
    }

    JsonResource::~JsonResource()
    {
    }

    const Json& JsonResource::getJson()
    {
        return json;
    }

    void JsonResource::setJson(Json&& json)
    {
        this->json = move(json);
        clearCache();
    }

    Buffer JsonResource::buildContent(HttpMessage& request, map<string, string>& headers)
    {
        return JsonWriter(json).buffer;
    }


    LocalizedJsonResource::LocalizedJsonResource() :
        AbstractCachingResource(),
        json(JsonConst::EmptyObject)
    {
    }

    LocalizedJsonResource::~LocalizedJsonResource()
    {
    }

    const Json& LocalizedJsonResource::getJson()
    {
        return json;
    }

    void LocalizedJsonResource::setJson(Json&& json)
    {
        this->json = std::move(json);
        clearCache();
    }

    Buffer LocalizedJsonResource::buildContent(HttpMessage& request, map<string, string>& headers)
    {
        string locale = getLocaleToServe(request.header(Header::accept_language));
        return LocalizingJsonWriter(json, locale).buffer;
    }
 

    string LocalizedJsonResource::getLocaleToServe(const char* acceptLanguageHeader)
    {
        if (!acceptLanguageHeader) {
            return "";
        }
        vector<string> locales;
        string aLH(acceptLanguageHeader);
        aLH.erase(remove_if(aLH.begin(), aLH.end(), ::isspace), aLH.end());
        replace(aLH.begin(), aLH.end(), '-', '_');
        transform(aLH.begin(), aLH.end(), aLH.begin(), ::tolower);
        for (string part : split(aLH, ',')) {
            vector<string> langAndWeight = split(part, ';');
            if (langAndWeight.size() > 1 &&
                langAndWeight[1].size() >= 2 &&
                langAndWeight[1].substr(0, 2) == "q=") {
                langAndWeight[1].erase(0, 2);
                langAndWeight[1].resize(5, '0');

                locales.push_back(langAndWeight[1] + langAndWeight[0]);
            }
            else {
                locales.push_back(string("1.000") + langAndWeight[0]);
            }
        }

        sort(locales.begin(), locales.end(), std::greater<string>());
        for (int i = 0; i < locales.size(); i++) {
            locales[i].erase(0, 5);
        }

        for (string locale : locales) {
            if (json["_ripc:locales"].contains(locale)) {
                return locale;
            }
        }

        return "";
    }


















}
