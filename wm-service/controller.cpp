/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the refude-services project. 
 * It is distributed under the GPL v2 license.
 * Please refer to the LICENSE file for a copy of the license.
 */
#include <vector>
#include "windowinfo.h"
#include "controller.h"
namespace org_restfulipc 
{
    Controller::Controller() :
        notifier(std::make_shared<NotifierResource>()),
        windowsResource(std::make_shared<CollectionResource>("Id")),
        displayResource(std::make_shared<JsonResource>()),
        iconsResource(std::make_shared<RunningAppsIcons>())
    {
        dispatcher.map(notifier, "notify");
        dispatcher.map(windowsResource, true, "windows");
        dispatcher.map(iconsResource, true, "icons");
        updateResources();
    }

    Controller::~Controller()
    {
    }

    void Controller::run()
    {
    }

    void Controller::updateResources()
    {
        Json displayJson = JsonConst::EmptyObject;
        displayJson["geometry"] = JsonConst::EmptyObject;
        WindowInfo rootWindowInfo = WindowInfo::rootWindow();
        displayJson["geometry"]["x"] = rootWindowInfo.x;
        displayJson["geometry"]["y"] = rootWindowInfo.y;
        displayJson["geometry"]["h"] = rootWindowInfo.height;
        displayJson["geometry"]["w"] = rootWindowInfo.width;

        if (displayJson != displayResource->getJson()) {
            notifier->resourceUpdated("display");
            displayResource->setJson(std::move(displayJson));
        }
 
    

        std::vector<Window> windowIds = WindowInfo::windowIds();
        Json windowsJson = JsonConst::EmptyArray;

        for (Window windowId : windowIds) {
            WindowInfo window(windowId);
            Json windowJson = JsonConst::EmptyObject;
            windowJson["Id"] = std::to_string(windowId);
            windowJson["Name"] = window.title;
            windowJson["Comment"] = "";
            windowJson["geometry"] = JsonConst::EmptyObject;
            windowJson["geometry"]["x"] = window.x;
            windowJson["geometry"]["y"] = window.y;
            windowJson["geometry"]["w"] = window.width;
            windowJson["geometry"]["h"] = window.height;
            windowJson["iconName"] = window.iconName;
            windowsJson.append(std::move(windowJson));
            iconsResource->addIcon(window.iconName, window.icon, window.iconLength);
        } 

        CollectionResourceUpdater updater(windowsResource);
        updater.update(windowsJson);
 
    }


    /*

    */
}
