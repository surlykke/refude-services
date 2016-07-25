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

    struct WindowsResource : public CollectionResource
    {
        WindowsResource(Controller* controller) : 
            CollectionResource("Id"),
            controller(controller)
        {
        }

        void doGET(int& socket, HttpMessage& request) override 
        {
            controller->updateWindowsResource();
            CollectionResource::doGET(socket, request);
        }
       
        void doPOST(int& socket, HttpMessage& request) override
        {
            controller->updateWindowsResource();

            std::cout << "POST against " << request.remainingPath << "\n";
            if (! indexes.contains(request.remainingPath)) throw HttpCode::Http404;
            errno = 0;
            Window windowToRaise = strtoul(request.remainingPath, NULL, 0);
            if (errno != 0) throw C_Error();

            WindowInfo(windowToRaise).raiseAndFocus();

            throw HttpCode::Http204;
        }



        Controller* controller;
    };



    Controller::Controller() :
        notifier(std::make_shared<NotifierResource>()),
        windowsResource(std::make_shared<WindowsResource>(this)),
        displayResource(std::make_shared<JsonResource>()),
        iconsResource(std::make_shared<RunningAppsIcons>())
    {
        dispatcher.map(notifier, "notify");
        dispatcher.map(windowsResource, true, "windows");
        dispatcher.map(iconsResource, true, "icons");
        buildDisplayResource(); 
        windowsResourceStale = true;
    }

    Controller::~Controller()
    {
    }

    void Controller::run()
    {
        Display *disp = XOpenDisplay(NULL);
        XSelectInput(disp, XDefaultRootWindow(disp), SubstructureNotifyMask);
        XEvent event;
        while (true) {
           XNextEvent(disp, &event);
           if (event.type ==  ConfigureNotify) {
               if (windowsResourceStale == false) {
                   windowsResourceStale = true;
                   notifier->resourceUpdated("windows");
               }
           }
        }
    }

    void Controller::buildDisplayResource()
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
    }

    void Controller::updateWindowsResource()
    {
        if (windowsResourceStale) {
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
            windowsResourceStale = false; 
        }
    }


    /*

    */
}
