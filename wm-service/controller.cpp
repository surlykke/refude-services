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

    // FIXME
    struct WindowResource : public JsonResource
    {
        WindowResource(Json&& action, Window window) :
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

    void addAction(Json& window)
    {
        Json action = JsonConst::EmptyObject;

        action["id"] = "_default";
        action["name"] = window["name"].copy();
        action["comment"] = std::string("Raise and focus");
        action["iconUrl"] = window["iconUrl"].copy();

        window["_actions"] = JsonConst::EmptyObject;
        window["_actions"]["_default"] = std::move(action);
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
        windowJson["iconUrl"] = std::string("../icons/") +  window.iconName.data();
        addAction(windowJson);
        return std::make_unique<WindowResource>(std::move(windowJson), window.window);
    }

    Controller::Controller() :
        dispatcher(),
        notifier(std::make_unique<NotifierResource>()),
        jsonResources(&dispatcher, notifier.get()),
        iconsResource(std::make_unique<RunningAppsIcons>())
    {
        dispatcher.map(std::move(notifier), "/notify");
        dispatcher.mapByPrefix(std::move(iconsResource), "/icons");
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
        Json windows = JsonConst::EmptyArray;
        Map<JsonResource::ptr> newResources;
        for (const WindowInfo& windowInfo: WindowInfo::normalWindows()) {
            std::string path = "/window/" + std::to_string(windowInfo.window);

            newResources[path] = buildWindowResource(windowInfo);
            windows.append(path.substr(1));
            RunningAppsIcons* icons = dynamic_cast<RunningAppsIcons*>(dispatcher.prefixMapping("/icons"));
            icons->addIcon(windowInfo.iconName.data(), windowInfo.icon);
        }

        newResources["/display"] = std::make_unique<JsonResource>(buildDisplay());
        newResources["/windows"] = std::make_unique<JsonResource>(std::move(windows));

        jsonResources.updateCollection(std::move(newResources));
    }
}
