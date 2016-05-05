/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the refude-services project. 
 * It is distributed under the GPL v2 license.
 * Please refer to the LICENSE file for a copy of the license.
 */


#include <ripc/json.h>
#include <ripc/jsonwriter.h>
#include <ripc/httpmessage.h>

#include "rootTemplate.h"
#include "mimeappslistreader.h"
#include "desktopservice.h"
#include "mimetyperesource.h"
namespace org_restfulipc
{

    MimetypeResource::MimetypeResource(Map<Json>&& mimetypeJsons) :
        AbstractCachingResource(),
        mimetypeJsons(move(mimetypeJsons))
    {
    }

    MimetypeResource::~MimetypeResource()
    {
    }

    void MimetypeResource::setMimetypeJsons(Map<Json>&& mimetypeJsons, NotifierResource::ptr notifier)
    {
        const char* mimetypeUpdated = "mimetype-updated";
        const char* mimetypeAdded = "mimetype-added";
        const char* mimetypeRemoved = "mimetype-removed";
        Map<const char*> affectedMimetypes;
        {
            unique_lock<recursive_mutex> lock(m);
            clearCache();
            for (int i = 0; i < this->mimetypeJsons.size(); i++) {
                if (!mimetypeJsons.contains(this->mimetypeJsons.keyAt(i))) {
                    affectedMimetypes.add(this->mimetypeJsons.keyAt(i), mimetypeRemoved);
                }
            }

            for (int i = 0; i < mimetypeJsons.size(); i++) {
                if (!this->mimetypeJsons.contains(mimetypeJsons.keyAt(i))) {
                    affectedMimetypes.add(mimetypeJsons.keyAt(i), mimetypeAdded);
                }
                else if (this->mimetypeJsons[mimetypeJsons.keyAt(i)] != mimetypeJsons.valueAt(i)) {
                    affectedMimetypes.add(mimetypeJsons.keyAt(i), mimetypeUpdated);
                }
            }
            this->mimetypeJsons = move(mimetypeJsons);
        }
        affectedMimetypes.each([&notifier](const char* key, const char* value) {
            notifier->notifyClients(value, key);
        });
    }

    /* 
     * This is called from AbstractCachingResource::doGET, which will take the lock, so no need to do that here
     */
    Buffer MimetypeResource::buildContent(HttpMessage& request, const char* remainingPath, map<string, string>& headers)
    {
        std::cout << "Into build content, queryParameterMap.size() = " << request.queryParameterMap.size()
            << ", remainingPath: '" << remainingPath << "'\n";
        if (remainingPath[0] == '\0') {
            Json mimetypes = JsonConst::EmptyObject;
            if (request.queryParameterMap.size() == 0) {
                mimetypeJsons.each([&](const char* key, Json & value)
                {
                    add(value["type"], value["subtype"], mimetypes);
                });
            }
            else if (request.queryParameterMap.size() == 1 &&
                     request.queryParameterMap.contains("search")) {
                const vector<const char*>& terms = request.queryParameterMap["search"];
                vector<string> locales = getAcceptedLocales(request);
                mimetypeJsons.each([&](const char* key, Json & value) {
                    if (match(terms, value, locales)) {
                        add(value["type"], value["subtype"], mimetypes);
                    }
                });

            }
            else {
                throw Status::Http422;
            }

            Json result; 
            result << rootTemplate_json;
            result["mimetypes"] = move(mimetypes);
            return JsonWriter(result).buffer;
        }
        else {
            if (mimetypeJsons.contains(remainingPath)) {
                return LocalizingJsonWriter(mimetypeJsons[remainingPath], getAcceptedLocales(request)).buffer;
            }
        }

        throw Status::Http404;
    }

    void MimetypeResource::add(const char* type, const char* subtype, Json& mimetypes)
    {
        if (!mimetypes.contains(type)) {
            mimetypes[type] = JsonConst::EmptyArray;
        }
        mimetypes[type].append(subtype);
    }


    bool MimetypeResource::match(const vector<const char*>& searchTerms,
                                 Json& mimetypeJson,
                                 const vector<string>& acceptableLocales)
    {
        for (const char* searchTerm : searchTerms) {
            if (strcasestr(mimetypeJson["subtype"], searchTerm)) {
                return true;
            }

            if (mimetypeJson.contains("globs")) {
                for (int j = 0; j < mimetypeJson["globs"].size(); j++) {
                    if (strcasestr(mimetypeJson["globs"][j], searchTerm)) {
                        return true;
                    }
                }
            }

            if (mimetypeJson.contains("comment")) {
                Json& commentObj = mimetypeJson["comment"];
                const char* locale = NULL;
                for (const string& acceptableLocale : acceptableLocales) {
                    if (commentObj.contains(acceptableLocale)) {
                        locale = acceptableLocale.data();
                        break;
                    }
                }
                if (locale) {
                    if (strcasestr(commentObj[locale], searchTerm)) {
                        return true;
                    }
                }
                else {
                    if (strcasestr(commentObj[""], searchTerm)) {
                        return true;
                    }
                }
            }


        }
        return false;
    }

    void MimetypeResource::doPATCH(int& socket, HttpMessage& request, const char* remainingPath)
    {
        if (!mimetypeJsons.contains(remainingPath)) {
            throw Status::Http404;
        }

        static const char* successfulResponse =
            "HTTP/1.1 204 No Content\r\n"
            "\r\n";

        Json mergeJson;
        mergeJson << request.body;
        if (mergeJson.type() != JsonType::Object) {
            throw Status::Http406;
        }
        else if (mergeJson.size() != 1 ||
                 !mergeJson.contains("defaultApplication") ||
                 mergeJson["defaultApplication"].type() != JsonType::String) {
            throw Status::Http422; // FIXME Some error message here
        }
        else {
            unique_lock<recursive_mutex> lock(m);

            string thisMimetype = remainingPath;
            string defaultApplication = (string) mergeJson["defaultApplication"];
            std::cout << "Sætter defaultApplciation for " << thisMimetype << " til " << defaultApplication << "\n";
            MimeappsList mimeappsList(xdg::config_home() + "/mimeapps.list");
            auto& defaultAppsForMime = mimeappsList.defaultApps[thisMimetype];
            for (auto it = defaultAppsForMime.begin(); it != defaultAppsForMime.end();) {
                if (*it == defaultApplication) {
                    it = defaultAppsForMime.erase(it);
                }
                else {
                    it++;
                }
            }
            defaultAppsForMime.insert(defaultAppsForMime.begin(), defaultApplication);
            std::cout << "defaultAppsForMime nu: ";
            for (auto& s : defaultAppsForMime) std::cout << s << " ";
            std::cout << "\n";
            mimeappsList.write();
        }
    }
}