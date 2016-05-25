/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the refude-services project. 
 * It is distributed under the GPL v2 license.
 * Please refer to the LICENSE file for a copy of the license.
 */
#include <X11/Xlib.h>
#include <X11/Xatom.h>

#include <ripc/json.h>
#include <ripc/jsonwriter.h>

#include "commandsTemplate.h"
#include "runningAppCommandTemplate.h"
#include "runningapplicationsresource.h"

namespace org_restfulipc
{


    void* getProp(Display *display, Window w, const char* propName, unsigned long& nitems_return)
    {
        // Naming of these variables (and parameters above) should match whats used 
        // in XGetWindowPropterty man page  (except for '_delete' -> 'delete'). 
        // So have a look at that.
        Atom property = XInternAtom(display, propName, False);
        long long_offset = 0;
        long long_length = 1024;
        Bool _delete = False;
        Atom req_type = AnyPropertyType;
        Atom actual_type_return;
        int actual_format_return;
        unsigned long bytes_after_return;
        unsigned char* prop_return;

        if (XGetWindowProperty(display, w, property, long_offset, long_length, _delete,
                               req_type, &actual_type_return, &actual_format_return,
                               &nitems_return, &bytes_after_return, &prop_return) != Success) {
            std::cout << "Fail\n";
            return NULL;
        }
        else {
            int bytesPrItem;
            switch (actual_format_return) {
            case 32: bytesPrItem = sizeof (long);
                break;
            case 16: bytesPrItem = sizeof (short int);
                break;
            case 8: bytesPrItem = sizeof (char);
                break;
            default: return NULL; // FIXME
            }

            void* result = malloc(nitems_return * bytesPrItem + 1);
            memcpy(result, prop_return, nitems_return * bytesPrItem);
            memset((char*) result + nitems_return*bytesPrItem, 0, bytesPrItem);
            XFree(prop_return);
            return result;
        }

    }

    struct DefaultDisplay
    {
        DefaultDisplay() {
            _disp = XOpenDisplay(NULL);
        }

        ~DefaultDisplay() {
            XCloseDisplay(_disp);
        }

        operator Display*() { return _disp; }

        Display *_disp;
    };
    
    static Atom _NET_WM_WINDOW_TYPE_NORMAL = XInternAtom(DefaultDisplay(), "_NET_WM_WINDOW_TYPE_NORMAL", False);

    struct WindowInfo
    {
        WindowInfo(Window window)
        {
            DefaultDisplay disp;
            unsigned long nitems;
            title = (char*) getProp(disp, window, "_NET_WM_VISIBLE_NAME", nitems);
            windowType = (Atom*) getProp(disp, window, "_NET_WM_WINDOW_TYPE", nitems);
            clients = (Window*) getProp(disp, window, "_NET_CLIENT_LIST", nitems);

            XWindowAttributes attr;
		    XGetWindowAttributes(disp, window, &attr);
	        x = attr.x;
            y = attr.y;
            w = attr.width;
            h = attr.height;
        }

        ~WindowInfo()
        {
            if (title) {
                free(title);
            }
            if (windowType) {
                free(windowType);
            }
        }

        char* title;
        Atom* windowType;
        Window* clients; 
        int x, y, w, h;

    };


    RunningApplicationsResource::RunningApplicationsResource() :
        AbstractResource()
    {
    }

    RunningApplicationsResource::~RunningApplicationsResource()
    {
    }

    void RunningApplicationsResource::doGET(int& socket, HttpMessage& request)
    {
        Buffer response;
        std::map<std::string, std::string> headers;

        Json commands;
        commands << commandsTemplate_json;
        commands["_links"]["self"]["href"] = mappedTo;
        commands["geometry"] = JsonConst::EmptyObject;

        WindowInfo rootWindow(XDefaultRootWindow(DefaultDisplay()));
        commands["geometry"]["x"] = rootWindow.x;
        commands["geometry"]["y"] = rootWindow.y;
        commands["geometry"]["w"] = rootWindow.w;
        commands["geometry"]["h"] = rootWindow.h;

        for (Window *client = rootWindow.clients; *client; client++) {
            WindowInfo clientInfo(*client); 

            Json runningApp;
            runningApp << runningAppCommandTemplate_json;
            runningApp["Name"] = clientInfo.title ? clientInfo.title : "";
            runningApp["Comment"] = "";
            runningApp["geometry"] = JsonConst::EmptyObject;
            runningApp["geometry"]["x"] = clientInfo.x;
            runningApp["geometry"]["y"] = clientInfo.y;
            runningApp["geometry"]["w"] = clientInfo.w;
            runningApp["geometry"]["h"] = clientInfo.h;

            runningApp["_links"]["self"]["href"] = std::string(mappedTo) + "/" + std::to_string(*client);
            runningApp["_links"]["execute"]["href"] = std::string(mappedTo) + "/" + std::to_string(*client);
            runningApp["_links"]["icon"]["href"] = "/icons/icon?name=application-x-executable&size=64";

            commands["commands"].append(std::move(runningApp));
        }


        Buffer content = JsonWriter(commands).buffer;
        buildResponse(response, std::move(content), headers);
        sendFully(socket, response.data(), response.size());
    }

    void RunningApplicationsResource::doPOST(int& socket, HttpMessage& request)
    {
        if (request.remainingPath[0] == '\0') {
            throw HttpCode::Http405;
        }

        errno = 0;
        Window windowToRaise = strtoul(request.remainingPath, NULL, 0);
        if (errno != 0) throw C_Error();

        WindowInfo rootWindow(XDefaultRootWindow(DefaultDisplay()));

        for (Window *client = rootWindow.clients; *client; client++) {
            if (*client == windowToRaise) {
                std::cout << "raising..\n";
                DefaultDisplay disp; 
                XRaiseWindow(disp, *client);
                XSetInputFocus(disp, *client, RevertToNone, CurrentTime);
                throw HttpCode::Http204;
            }
        }

        std::cout << "Giving up\n";
        throw HttpCode::Http405;
    }
}