#include <string.h>
#include <iostream>
#include "windowinfo.h"
#include "xdg.h"

namespace org_restfulipc
{

    int forgiving_X_error_handler(Display *d, XErrorEvent *e)
    {
        char errorMsg[80];
        XGetErrorText(d, e->error_code, errorMsg, 80);
        printf("Got error: %s\n", errorMsg);
        return 0;
    }

    void* getProp(Display *display,
                  Window w,
                  const char* propName,
                  unsigned long& nitems_return,
                  bool show = false)
    {
        // Naming of these variables (and parameters above) should match whats used 
        // in XGetWindowPropterty man page  (except for '_delete' -> 'delete'). 
        // So have a look at that.
        Atom property = XInternAtom(display, propName, False);
        long long_offset = 0;
        long long_length = 32;
        Bool _delete = False;
        Atom req_type = AnyPropertyType;
        Atom actual_type_return;
        int actual_format_return;
        unsigned long bytes_after_return;
        unsigned char* prop_return;

        void* buf = NULL;
        unsigned long bufsize = 0;

        do {
            if (XGetWindowProperty(display, w, property, long_offset, long_length, _delete,
                                   req_type, &actual_type_return, &actual_format_return,
                                   &nitems_return, &bytes_after_return, &prop_return) != Success) {
                printf("error in getProp...\n");
                if (buf) {
                    free(buf);
                }
                return NULL;

            }
            else {
                int bytesPrItem;
                switch (actual_format_return) {
                case 32:
                    bytesPrItem = sizeof (long);
                    long_offset += nitems_return;
                    break;
                case 16: bytesPrItem = sizeof (short int);
                    long_offset += nitems_return / 2;
                    break;
                case 8: bytesPrItem = sizeof (char);
                    long_offset += nitems_return / 4;
                    break;
                default:
                    return NULL;
                }
                long_length = bytes_after_return / 4;

                unsigned long newsize = bufsize + nitems_return*bytesPrItem;

                buf = realloc(buf, newsize + bytesPrItem); // Always one extra so we can zero-terminate
                memcpy((char*) buf + bufsize, prop_return, newsize - bufsize);
                memset((char*) buf + newsize, 0, bytesPrItem);
                bufsize = newsize;
                XFree(prop_return);
            }
        }
        while (long_length > 0);

        return buf;
    }

    struct DefaultDisplay
    {

        DefaultDisplay()
        {
            _disp = XOpenDisplay(NULL);
        }

        ~DefaultDisplay()
        {
            XCloseDisplay(_disp);
        }

        operator Display*()
        {
            return _disp;
        }

        Display *_disp;
    };

    static Atom _NET_WM_WINDOW_TYPE_NORMAL = XInternAtom(DefaultDisplay(), "_NET_WM_WINDOW_TYPE_NORMAL", False);

    void WindowInfo::init()
    {
        XSetErrorHandler(forgiving_X_error_handler);
    }

    WindowInfo WindowInfo::rootWindow()
    {
        return WindowInfo(XDefaultRootWindow(DefaultDisplay()));
    }

    std::vector<Window> WindowInfo::windowIds()
    {
        std::vector<Window> result;
        DefaultDisplay disp;
        Window root = XDefaultRootWindow(disp);
        unsigned long nitems;
        Window* windowlist = (Window*) getProp(disp, root, "_NET_CLIENT_LIST_STACKING", nitems);
        for (Window* w = windowlist; *w; w++) {
            result.push_back(*w);
        }
        free(windowlist);
        return result;
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
        if (windows) {
            free(windows);
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
            tmpTitle = (char*) getProp(disp, window, "_NET_WM_NAME", nitems);
            if (tmpTitle) {
                title = tmpTitle;
                XFree(tmpTitle);
            }
            else {
                title = "";
            } 
        }

        Atom* tmpAtomPtr = (Atom*) getProp(disp, window, "_NET_WM_WINDOW_TYPE", nitems);
        if (tmpAtomPtr) {
            windowType = *tmpAtomPtr;
            XFree(tmpAtomPtr);
        }
        else {
            windowType = _NET_WM_WINDOW_TYPE_NORMAL;
        }

        tmpAtomPtr = (Atom*) getProp(disp, window, "_NET_WM_STATE", nitems);
        for (unsigned long i = 0; i < nitems; i++) {
            windowState.push_back(tmpAtomPtr[i]);
        }
        if (nitems) {
            XFree(tmpAtomPtr);
        }


        long frameExtents[4] = {0, 0, 0, 0};
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

        icon = (long*) getProp(disp, window, "_NET_WM_ICON", iconLength, true);
        if (icon) {
            calculateIconName();
        }
        else {
            iconName[0] = '\0';
        }
    }

    WindowInfo::~WindowInfo() {
        if (icon) {
           free((void*)icon);
        }
    }

    void WindowInfo::raiseAndFocus()
    {
        printf("Raising window %x\n", window);
        DefaultDisplay disp;
        long mask = SubstructureRedirectMask | SubstructureNotifyMask;
        Window rootWindow = DefaultRootWindow(disp._disp);
        XEvent event;
        memset(&event, 0, sizeof(XEvent));
        Atom net_active_window = XInternAtom(disp, "_NET_ACTIVE_WINDOW", 0);
        event.xclient.type = ClientMessage;
        event.xclient.serial = 0;
        event.xclient.send_event = 1;
        event.xclient.message_type = net_active_window;
        event.xclient.window = window;
        event.xclient.format = 32;
        event.xclient.data.l[0] = 1;
        event.xclient.data.l[1] = 0;
        event.xclient.data.l[2] = 0;
        event.xclient.data.l[3] = 0;
        event.xclient.data.l[4] = 0;
        XSendEvent(disp._disp, rootWindow, 0, mask, &event);
    }

    void WindowInfo::calculateIconName()
    {
        static const char hexDigit[16] = {'0', '1', '2', '3', '4', '5', '6', '7',
            '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
        char hash[8];
        memset(hash, 0, 8);
        for (unsigned long i = 0; i < iconLength; i++) {
            hash[4 * i % 8] ^= ((icon[i] & 0xFF000000) >> 24);
            hash[(4 * i + 1) % 8] ^= ((icon[i] & 0xFF0000) >> 16);
            hash[(4 * i + 2) % 8] ^= ((icon[i] & 0xFF00) >> 8);
            hash[(4 * i + 3) % 8] ^= (icon[i] & 0xFF);
        }
        for (int i = 0; i < 8; i++) {
            iconName[2 * i + 2 * i / 4] = hexDigit[(hash[i] & 0xF0) >> 4];
            iconName[2 * i + 1 + (2 * i + 1) / 4] = hexDigit[hash[i] & 0xF];
        }
        iconName[4] = iconName[9] = iconName[14] = '-';
        iconName[19] = '\0';
    }

}
