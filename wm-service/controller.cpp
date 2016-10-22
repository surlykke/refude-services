/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the refude-services project. 
 * It is distributed under the GPL v2 license.
 * Please refer to the LICENSE file for a copy of the license.
 */
#include <vector>
#include <unistd.h>
#include <ripc/jsonwriter.h>
#include "windowinfo.h"
#include "controller.h"
namespace org_restfulipc 
{

    struct WindowsResource : public CollectionResource
    {
        WindowsResource(Controller* controller) : CollectionResource("Id") { }

      
        void doPOST(int& socket, HttpMessage& request) override
        {
            std::cout << "POST against " << request.remainingPath << "\n";
            if (! indexes.contains(request.remainingPath)) throw HttpCode::Http404;
            errno = 0;
            Window windowToRaise = strtoul(request.remainingPath, NULL, 0);
            if (errno != 0) throw C_Error();

            WindowInfo(windowToRaise).raiseAndFocus();

            throw HttpCode::Http204;
        }
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

    Controller::Controller() :
        notifier(std::make_shared<NotifierResource>()),
        windowsResource(std::make_shared<WindowsResource>(this)),
        displayResource(std::make_shared<JsonResource>()),
        iconsResource(std::make_shared<RunningAppsIcons>())
    {
        dispatcher.map(notifier, "notify");
        dispatcher.map(windowsResource, true, "windows");
        dispatcher.map(iconsResource, true, "icons");
        dispatcher.map(displayResource, "display");
        buildDisplayResource(); 
    }

    Controller::~Controller()
    {
    }

    void Controller::run()
    {
        updateWindowsResource();
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
               updateWindowsResource();
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
        Json windowsJson = JsonConst::EmptyArray;

        for (const WindowInfo& window : WindowInfo::normalWindows()) {
            Json windowJson = JsonConst::EmptyObject;
            windowJson["Id"] = std::to_string(window.window);
            windowJson["Name"] = window.title;
            windowJson["State"] = JsonConst::EmptyArray;
            for (Atom atom : window.windowState) {
                const char* state = atomToString(atom);
                if (state) {
                    windowJson["State"].append(state);
                }
            }
            
            windowJson["Comment"] = "";
            windowJson["geometry"] = JsonConst::EmptyObject;
            windowJson["geometry"]["x"] = window.x;
            windowJson["geometry"]["y"] = window.y;
            windowJson["geometry"]["w"] = window.width;
            windowJson["geometry"]["h"] = window.height;
            windowJson["windowIcon"] = window.iconName;
            windowsJson.append(std::move(windowJson));
            iconsResource->addIcon(window.iconName.data(), window.icon, window.iconLength);
        } 
        CollectionResourceUpdater updater(windowsResource);
        updater.update(windowsJson);
        updater.notify(notifier, "windows");
    }

}
