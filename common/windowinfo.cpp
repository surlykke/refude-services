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
            default: 
                return NULL; 
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

    std::vector<WindowInfo> WindowInfo::normalWindows()
    {
        std::vector<WindowInfo> result;
        DefaultDisplay disp;
        Window root = XDefaultRootWindow(disp);
        unsigned long nitems;
        Window* windows = (Window*) getProp(disp, root, "_NET_CLIENT_LIST_STACKING", nitems);
        for (Window* w = windows; *w; w++) {
            WindowInfo windowInfo(*w);
            if (windowInfo.title.size() > 0 && windowInfo.windowType == _NET_WM_WINDOW_TYPE_NORMAL) {
                result.push_back(windowInfo);
            }
        }

        return result;
         
    }

    WindowInfo::WindowInfo(Window window)
    {
        this->window = window;
        DefaultDisplay disp;
        unsigned long nitems;
        
        char* tmpTitle = (char*) getProp(disp, window, "_NET_WM_VISIBLE_NAME", nitems);
        if (tmpTitle) {
            title = tmpTitle;
            XFree(tmpTitle);
        }
        else {
            title = "";
        }
        
        Atom* tmpAtomPtr = (Atom*) getProp(disp, window, "_NET_WM_WINDOW_TYPE", nitems);
        if (tmpAtomPtr) {
            windowType = *tmpAtomPtr;
            XFree(tmpAtomPtr);
        }
        else {
            windowType = 0;
        }
       
        long frameExtents[4] = {0,0,0,0};
        long* frameExtentsTmp = (long*) getProp(disp, window, "_NET_FRAME_EXTENTS", nitems);
        if (frameExtentsTmp) {
            if (nitems >= 4) {
                frameExtents[0] = frameExtentsTmp[0];
                frameExtents[1] = frameExtentsTmp[1];
                frameExtents[2] = frameExtentsTmp[2];
                frameExtents[3] = frameExtentsTmp[3];
            }
            XFree(frameExtentsTmp);
        }

        XWindowAttributes attr;
        XGetWindowAttributes(disp, window, &attr);
        Window root, parent;
        Window *children;
        unsigned int nchildren;

        XQueryTree(disp, window, &root, &parent, &children, &nchildren);
        if (nchildren) {
            XFree(children);
        }
        
        if (parent) {
            XTranslateCoordinates(disp, parent, root, attr.x, attr.y, &x, &y, &attr.root); 
            x = x - frameExtents[0];
            y = y - frameExtents[2];
            
            width = attr.width + frameExtents[0] + frameExtents[1];
            height = attr.height + frameExtents[2] + frameExtents[3];
        }
        else {
            x = attr.x;
            y = attr.y;
            width = attr.width;
            height = attr.height;
        }
        
    }

    void WindowInfo::raiseAndFocus()
    {
        DefaultDisplay disp; 
        XRaiseWindow(disp, window);
        XSetInputFocus(disp, window, RevertToNone, CurrentTime);
    }

}
