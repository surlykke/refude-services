/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the refude-services project. 
 * It is distributed under the GPL v2 license.
 * Please refer to the GPL2 file for a copy of the license.
 */

#include <set>
#include "service.h"
#include "jsonresource.h"
#include "jsonwriter.h"
#include "notifierresource.h"


#include "applicationcollector.h"
#include "mimetypecollector.h"
#include "mimeappslistreader.h"
#include "xdg.h"
#include "desktopwatcher.h"
#include "runapplication.h"

#include "controller.h"
#include "typedefs.h"

namespace refude
{

    struct ActionResource : public JsonResource
    {
        ActionResource(Json&& action, std::string command) :
            JsonResource(std::move(action)),
            command(command)
        {}

        void doPOST(int& socket, HttpMessage& request) override
        {
            runApplication(command.data());
            throw HttpCode::Http204;
        }

        std::string command;
    };
   
    struct MimetypeResource : public JsonResource
    {
        MimetypeResource(Json&& mimetype) : JsonResource(std::move(mimetype)) {}
        void doPATCH(int& socket, HttpMessage& request) override
        {
            Json mergeJson;
            mergeJson << request.body;
            if (mergeJson.type() != JsonType::Object) throw HttpCode::Http406;
            if (mergeJson.size() != 1) throw HttpCode::Http422;
            
            if (!mergeJson.contains("defaultApplications")) throw HttpCode::Http422;
            if (mergeJson["defaultApplications"].type() != JsonType::Array) throw HttpCode::Http422;
            MimeappsList mimeappsList(xdg::config_home() + "/mimeapps.list");
            auto& defaultAppsForMime = mimeappsList.defaultApps[request.remainingPath];
            defaultAppsForMime.clear();
            mergeJson["defaultApplications"].eachElement([&defaultAppsForMime](Json& element) { 
                if (element.type() != JsonType::String) throw HttpCode::Http422;
                defaultAppsForMime.push_back(element.toString());
            });

            mimeappsList.write();
            throw HttpCode::Http204;
        }
    };


    Controller::Controller() : 
        service(),
        notifier(std::make_unique<NotifierResource>()),
        resources(&service, notifier.get()),
        desktopWatcher(new DesktopWatcher(*this, true))        
    {
        
        service.map(std::move(notifier), "/notify");
    }

    Controller::~Controller()
    {
    }

    void Controller::setupAndRun()
    {
        desktopWatcher->start();
    }

    void Controller::update()
    {
        ApplicationCollector applicationCollector;
        applicationCollector.collect();
       
        MimetypeCollector mimetypeCollector;
        mimetypeCollector.collect();
       
        mimetypeCollector.addAssociations(applicationCollector.collectedApplications);
        mimetypeCollector.addDefaultApplications(applicationCollector.defaultApplications);

        Map<JsonResource::ptr> newResources;

        char path[1024] = {0};
        Json actions = JsonConst::EmptyArray;

        for (auto& entry : applicationCollector.collectedApplications) {
            const std::string& appId = entry.key;
            Json& app = entry.value;
            std::string path = std::string("/application/") + appId + "/launch";
            newResources[path] = buildAction(app);
            actions.append(path.substr(1));
            path = std::string("/application/") + appId;
            std::cout << "Mapping: " << path << "\n";
            newResources[path] = std::make_unique<JsonResource>(std::move(app));
        };

        newResources["/actions"] = std::make_unique<JsonResource>(std::move(actions));

        for (auto& entry: mimetypeCollector.collectedMimetypes) {
            const std::string& mimetypeStr = entry.key;
            Json& mimetype = entry.value;
            newResources[std::string("/mimetype/") + mimetypeStr] = std::make_unique<MimetypeResource>(std::move(mimetype));
        };

        resources.updateCollection(std::move(newResources));
    }

    JsonResource::ptr Controller::buildAction(Json& application) {
        Json action = JsonConst::EmptyObject;
        action[std::string("_ripc:localized:name")] = application[std::string("_ripc:localized:Name")].copy();
        action["_ripc:localized:comment"] = application["_ripc:localized:Comment"].copy();
        action["icon"] = application["Icon"].copy();
        return std::make_unique<ActionResource>(std::move(action), application["Exec"].toString());
    }

}
