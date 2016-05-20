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
            std::cout << "actual_format_return: " << actual_format_return << ", bytesPrItem: " << bytesPrItem << "\n";
            void* result = malloc(nitems_return * bytesPrItem + 1);
            memcpy(result, prop_return, nitems_return * bytesPrItem);
            memset((char*) result + nitems_return*bytesPrItem, 0, bytesPrItem);
            XFree(prop_return);
            return result;
        }

    }

    Json getApplist()
    {
        Json runningApps = JsonConst::EmptyArray; 
        unsigned long len;
        Display *disp = XOpenDisplay(NULL);
        Window *list;
        char *name;

        if (!disp) {
            puts("no display!");
            return -1;
        }

        unsigned long nitems;

        Window *clients = (Window*) getProp(disp, XDefaultRootWindow(disp), "_NET_CLIENT_LIST", nitems);

        for (Window *client = clients; *client; client++) {
            Json runningApp = JsonConst::EmptyObject;
            unsigned long dummy;
            runningApp["Name"] = (char*) getProp(disp, *client, "_NET_WM_VISIBLE_NAME", dummy);
            runningApps.append(std::move(runningApp));
        }

        XCloseDisplay(disp);

        return runningApps;
    }

    RunningApplicationsResource::RunningApplicationsResource() :
        AbstractResource()
    {
    }

    RunningApplicationsResource::~RunningApplicationsResource()
    {
    }

    void RunningApplicationsResource::doGET(int& socket, HttpMessage& request, const char* remainingPath)
    {
        Buffer response;
        std::map<std::string, std::string> headers;
        Json applist = getApplist();
        Buffer content = JsonWriter(applist).buffer;
        buildResponse(response, std::move(content), headers);
        sendFully(socket, response.data(), response.size());
    }
}