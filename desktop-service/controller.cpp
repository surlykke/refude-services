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


    struct ApplicationResource : public JsonResource
    {
        ApplicationResource(Json&& application) : JsonResource(std::move(application))
        {
        }

        void doPOST(int& socket, HttpMessage& request)
        {
            std::string key;
            if (request.queryParameterMap.find("action") < 0) {
                key = "_default";
            }
            else {
                key = request.queryParameterMap["action"][0];
            }

            if (getJson()["_actions"].contains(key)) {
                execute(getJson()["_actions"][key]);
                throw HttpCode::Http204;
            }
            else {
                throw HttpCode::Http406;
            }
        }

        void execute(Json& action)
        {
            runApplication(action["exec"].toString());
        }

    };

    struct WindowResource : public JsonResource
    {
        WindowResource(Json&& action, std::string command) :
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

        Json applicationPaths = JsonConst::EmptyArray;
        Json mimetypePaths = JsonConst::EmptyArray;
        Map<JsonResource::ptr> newResources;

        for (auto& entry : applicationCollector.collectedApplications) {
            buildActions(entry.value);
            std::string path = std::string("/application/") + entry.key;
            newResources[path] =  std::make_unique<ApplicationResource>(std::move(entry.value));
            applicationPaths.append(path.substr(1));
        };

        newResources["/applications"] = std::make_unique<JsonResource>(std::move(applicationPaths));

        for (auto& entry: mimetypeCollector.collectedMimetypes) {
            std::string path = std::string("/mimetype/") + entry.key;
            newResources[path] = std::make_unique<MimetypeResource>(std::move(entry.value));
            mimetypePaths.append(path.substr(1));
        };

        newResources["/mimetypes"] = std::make_unique<JsonResource>(std::move(mimetypePaths));

        resources.updateCollection(std::move(newResources));
    }

    Json Controller::buildActions(Json& application) {
        Json actions = JsonConst::EmptyObject;
        Json action = JsonConst::EmptyObject;
        action["_ripc:localized:name"] = application["_ripc:localized:Name"].copy();
        action["_ripc:localized:comment"] = application["_ripc:localized:Comment"].copy();
        action["icon"] = application["Icon"].copy();
        action["exec"] = application["Exec"].copy();
        actions["_default"] = std::move(action);
        if (application.contains("Actions")) {
            application["Actions"].eachEntry(
                [&application, &actions](const std::string& key, Json& value) {
                    Json action = JsonConst::EmptyObject;
                    action["_ripc:localized:name"] = value["_ripc:localized:Name"].copy();
                    action["_ripc:localized:comment"] = application["_ripc:localized:Name"].copy();
                    action["icon"] = application["Icon"].copy();
                    action["exec"] = value["Exec"].copy();
                    actions[key] = std::move(action);
                }
            );
            application.erase("Actions");
        }
        application["_actions"] = std::move(actions);
    }

}
