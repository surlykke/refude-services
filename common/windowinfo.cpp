#include <string.h>
#include <iostream>
#include "windowinfo.h"
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

    WindowInfo WindowInfo::rootWindow()
    {
        return WindowInfo(XDefaultRootWindow(DefaultDisplay()));
    }

    WindowInfo::WindowInfo(Window window)
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

    WindowInfo::~WindowInfo()
    {
        if (title) {
            free(title);
        }
        if (windowType) {
            free(windowType);
        }
    }
    
    bool WindowInfo::isNormal()
    {
        return windowType && _NET_WM_WINDOW_TYPE_NORMAL == *windowType;
    }

    void raiseAndFocus(Window window)
    {
        DefaultDisplay disp; 
        XRaiseWindow(disp, window);
        XSetInputFocus(disp, window, RevertToNone, CurrentTime);
    }

}
