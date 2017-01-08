/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the refude-services project. 
 * It is distributed under the GPL v2 license.
 * Please refer to the GPL2 file for a copy of the license.
 */

#include <vector>
#include <unistd.h>
#include "jsonwriter.h"
#include "xdg.h"
#include "windowinfo.h"
#include "controller.h"
namespace refude 
{

    struct ActionResource : public JsonResource
    {
        ActionResource(Json&& action, Window window) :
            JsonResource(std::move(action)),
            window(window)
        {
        }
      
        void doPOST(int& socket, HttpMessage& request) override
        {

            WindowInfo(window).raiseAndFocus();
            throw HttpCode::Http204;
        }

        Window window;
    };

    std::map<Atom, const char*> buildWmStateMap() 
    {
        std::map<Atom, const char*> map;
        Display *disp = XOpenDisplay(NULL);

        map[XInternAtom(disp, "_NET_WM_STATE_MODAL", False)] = "Modal";
        map[XInternAtom(disp, "_NET_WM_STATE_STICKY", False)] = "Sticky";
        map[XInternAtom(disp, "_NET_WM_STATE_MAXIMIZED_VERT", False)] = "MaximizedVertical";
        map[XInternAtom(disp, "_NET_WM_STATE_MAXIMIZED_HORZ", False)] = "MaximizedHorizontal";
        map[XInternAtom(disp, "_NET_WM_STATE_SHADED", False)] = "Shaded";
        map[XInternAtom(disp, "_NET_WM_STATE_SKIP_TASKBAR", False)] = "SkipTaskbar";
        map[XInternAtom(disp, "_NET_WM_STATE_SKIP_PAGER", False)] = "SkipPager";
        map[XInternAtom(disp, "_NET_WM_STATE_HIDDEN", False)] = "Hidden";
        map[XInternAtom(disp, "_NET_WM_STATE_FULLSCREEN", False)] = "Fullscreen";
        map[XInternAtom(disp, "_NET_WM_STATE_ABOVE", False)] = "Above";
        map[XInternAtom(disp, "_NET_WM_STATE_BELOW", False)] = "Below";
        map[XInternAtom(disp, "_NET_WM_STATE_DEMANDS_ATTENTION", False)] = "DemandsAttention";
        XCloseDisplay(disp);
        return map;
    }

    const char* atomToString(Atom atom) 
    {
        static std::map<Atom, const char*> wmStateMap = buildWmStateMap();
        if (wmStateMap.find(atom) != wmStateMap.end()) {
            return wmStateMap[atom];
        }
        else {
            return NULL;
        }
    }

    JsonResource::ptr buildWindowResource(const WindowInfo& window)
    {
        Json windowJson = JsonConst::EmptyObject;
        windowJson["id"] = std::to_string(window.window);
        windowJson["name"] = window.title;
        windowJson["state"] = JsonConst::EmptyArray;
        for (Atom atom : window.windowState) {
            const char* state = atomToString(atom);
            if (state) {
                windowJson["state"].append(state);
            }
        }

        windowJson["comment"] = "";
        windowJson["geometry"] = JsonConst::EmptyObject;
        windowJson["geometry"]["x"] = window.x;
        windowJson["geometry"]["y"] = window.y;
        windowJson["geometry"]["w"] = window.width;
        windowJson["geometry"]["h"] = window.height;
        windowJson["windowIcon"] = window.iconName;
        return std::make_unique<JsonResource>(std::move(windowJson));
    }

    JsonResource::ptr buildActionResource(const WindowInfo& window)
    {
        Json action = JsonConst::EmptyObject;
        action["name"] = window.title;
        action["comment"] = std::string("Raise and focus");
        action["geometry"] = JsonConst::EmptyObject;
        action["geometry"]["x"] = window.x;
        action["geometry"]["y"] = window.y;
        action["geometry"]["w"] = window.width;
        action["geometry"]["h"] = window.height;
        char iconUrl[1024] = {0};
        snprintf(iconUrl, 1024, "/icons/%s", window.iconName.data());
        action["iconUrl"] = iconUrl;

        return std::make_unique<ActionResource>(std::move(action), window.window);
    }

    Controller::Controller() :
        dispatcher(),
        notifier(std::make_unique<NotifierResource>()),
        jsonResources(&dispatcher, notifier.get()),
        iconsResource(std::make_unique<RunningAppsIcons>())
    {
        dispatcher.map(std::move(notifier), "/notify");
        dispatcher.map(std::move(iconsResource), "/icons");
    }

    Controller::~Controller()
    {
    }

    void Controller::run()
    {
        update();
        std::string socketPath = xdg::runtime_dir() + "/org.refude.wm-service";
        dispatcher.serve(socketPath.data());
        Display *disp = XOpenDisplay(NULL);
        XSelectInput(disp, XDefaultRootWindow(disp), SubstructureNotifyMask);
        XEvent event;
        while (true) {
           XNextEvent(disp, &event);
           if (event.type ==  ConfigureNotify) {
               // To not float unsuspecting clients with server-sent events,
               // we send at most one event pr. 1/10 seconds. (TODO: Right value?)
               usleep(100000);
               XSync(disp, True);
               update();
           }
        }
    }

    Json Controller::buildDisplay()
    {
        WindowInfo rootWindowInfo = WindowInfo::rootWindow();

        Json json = JsonConst::EmptyObject;
        json["geometry"] = JsonConst::EmptyObject;
        json["geometry"]["x"] = rootWindowInfo.x;
        json["geometry"]["y"] = rootWindowInfo.y;
        json["geometry"]["h"] = rootWindowInfo.height;
        json["geometry"]["w"] = rootWindowInfo.width;

        return json;
    }

    void Controller::update()
    {
        Json actions = JsonConst::EmptyArray;
        Map<JsonResource::ptr> newResources;

        char path[1024] = {0};

        for (const WindowInfo& windowInfo: WindowInfo::normalWindows()) {
            snprintf(path, 1024, "/window/%lu", windowInfo.window);
            newResources[path] = buildWindowResource(windowInfo);
            iconsResource->addIcon(windowInfo.iconName.data(), windowInfo.icon);
            snprintf(path, 1024, "/window/%lu/raiseAndFocus", windowInfo.window);
            newResources[path] = buildActionResource(windowInfo);
            actions.append(path + 1);
        }

        newResources["/display"] = std::make_unique<JsonResource>(buildDisplay());
        newResources["/actions"] = std::make_unique<JsonResource>(std::move(actions));

        jsonResources.updateCollection(std::move(newResources));
    }
}
