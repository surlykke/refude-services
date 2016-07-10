/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the refude-services project. 
 * It is distributed under the GPL v2 license.
 * Please refer to the LICENSE file for a copy of the license.
 */


#include <ripc/json.h>

#include "mimeappslistreader.h"
#include "xdg.h"
#include "mimetyperesource.h"
namespace org_restfulipc
{

    MimetypeResource::MimetypeResource() :
    LocalizedJsonResource()
    {
    }

    MimetypeResource::~MimetypeResource()
    {
    }


    void MimetypeResource::doPATCH(int& socket, HttpMessage& request)
    {
        Json mergeJson;
        mergeJson << request.body;
        if (mergeJson.type() != JsonType::Object) {
            throw HttpCode::Http406;
        }
        else if (mergeJson.size() != 1 ||
                 !mergeJson.contains("defaultApplication") ||
                 mergeJson["defaultApplication"].type() != JsonType::String) {
            throw HttpCode::Http422; // FIXME Some error message here
        }
        else {
            std::lock_guard<std::recursive_mutex> lock(m);

            std::string thisMimetype = getJson()["type"] + "/" + getJson()["subtype"];
            std::string defaultApplication = (std::string) mergeJson["defaultApplication"];
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
            mimeappsList.write();
            throw HttpCode::Http204;
        }
    }
}