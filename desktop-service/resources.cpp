#include "runapplication.h"
#include "comm.h"
#include "mimeappslistreader.h"
#include "xdg.h"
#include "resources.h"

namespace refude
{
    ApplicationResource::ApplicationResource(Json&& application) :
        JsonResource(std::move(application))
    {
    }

    void ApplicationResource::doPOST(Descriptor& socket, HttpMessage& request, const char* remainingPath)
    {
        std::string key;
        if (request.queryParameterMap.find("action") < 0) {
            key = "_default";
        }
        else {
            key = request.queryParameterMap["action"][0];
        }

        std::shared_lock<std::shared_mutex> readLock(mutex);

        if (! getJson()["_actions"].contains(key)) throw HttpCode::Http406;

        runApplication(getJson()["_actions"][key].toString());
        sendStatus(socket, HttpCode::Http204);
    }


    MimetypeResource::MimetypeResource(Json&& mimetype) :
        JsonResource(std::move(mimetype))
    {
    }

    void MimetypeResource::doPATCH(Descriptor& socket, HttpMessage& request, const char* remainingPath)
    {
        Json json;
        json << request.body;

        if (json.type() != JsonType::Object) throw HttpCode::Http406;

        if (! (json.size() == 1 &&
               json.contains("defaultApplications") &&
               json["defaultApplications"].type() == JsonType::Array)) {
            throw HttpCode::Http422;
        }

        MimeappsList mimeappsList(xdg::config_home() + "/mimeapps.list");
        auto& defaultAppsForMime = mimeappsList.defaultApps[remainingPath];
        defaultAppsForMime.clear();
        json["defaultApplications"].eachElement([&socket, &defaultAppsForMime](Json& element) {
            if (element.type() != JsonType::String) {
                return sendStatus(socket, HttpCode::Http422);
            }

            defaultAppsForMime.push_back(element.toString());
        });

        mimeappsList.write();

        sendStatus(socket, HttpCode::Http204);
    }

}
