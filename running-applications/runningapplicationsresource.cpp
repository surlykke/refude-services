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
    const char* asStr(Atom x)
    {
        switch (x) {
        case XA_PRIMARY: return ("XA_PRIMARY");
        case XA_SECONDARY: return ("XA_SECONDARY");
        case XA_ARC: return ("XA_ARC");
        case XA_ATOM: return ("XA_ATOM");
        case XA_CARDINAL: return ("XA_CARDINAL");
        case XA_INTEGER: return ("XA_INTEGER");
        case XA_STRING: return ("XA_STRING");
        case XA_WINDOW: return ("XA_WINDOW");
        case XA_WM_HINTS: return ("XA_WM_HINTS");
        default: return ("??");
        }
    }


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

    Json getApplist()
    {
    }

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

        Display *disp = XOpenDisplay(NULL);
        Window *list;
        char *name;

        if (!disp) {
            puts("no display!");
            throw HttpCode::Http500;
        }

        unsigned long nitems;

        Window *clients = (Window*) getProp(disp, XDefaultRootWindow(disp), "_NET_CLIENT_LIST", nitems);

        for (Window *client = clients; *client; client++) {
            Json runningApp;
            runningApp << runningAppCommandTemplate_json;
            unsigned long dummy;
            std::string windowId = std::to_string(*client);
            runningApp["Name"] = (char*) getProp(disp, *client, "_NET_WM_VISIBLE_NAME", dummy);
            runningApp["Comment"] = "";
            runningApp["_links"]["self"]["href"] = std::string(mappedTo) + "/" + windowId;
            runningApp["_links"]["execute"]["href"] = std::string(mappedTo) + "/" + windowId;
            runningApp["_links"]["icon"]["href"] =  std::string(mappedTo) + "/icon/" + windowId;
            
            commands["commands"].append(std::move(runningApp));
        }

        XCloseDisplay(disp);

        Buffer content = JsonWriter(commands).buffer;
        buildResponse(response, std::move(content), headers);
        sendFully(socket, response.data(), response.size());
    }

    void RunningApplicationsResource::doPOST(int& socket, HttpMessage& request)
    {
        std::cout << "Into doPost\n";

        if (request.remainingPath[0] == '\0') {
            throw HttpCode::Http405;
        }
        
        std::cout << "Remaining path: " << request.remainingPath << "\n";
        errno = 0;
        Window windowToRaise = strtoul(request.remainingPath, NULL, 0);
        if (errno != 0) throw C_Error();
        std::cout << "windowToRaise: " << windowToRaise << "\n";

        Display *disp = XOpenDisplay(NULL);
        Window *list;
        char *name;

        if (!disp) {
            puts("no display!");
            throw HttpCode::Http500;
        }

        unsigned long nitems;

        Window *clients = (Window*) getProp(disp, XDefaultRootWindow(disp), "_NET_CLIENT_LIST", nitems);

        for (Window *client = clients; *client; client++) {
            std::cout << "Looking at: " << *client << "\n";
            if (*client == windowToRaise) {
                std::cout << "raising..\n";
                XRaiseWindow(disp, *client);
                XSetInputFocus(disp, *client, RevertToNone, CurrentTime);
                XCloseDisplay(disp); 
                throw HttpCode::Http204;
            }
        }

        XCloseDisplay(disp);
        std::cout << "Giving up\n";
        throw HttpCode::Http405;
        
    }

}