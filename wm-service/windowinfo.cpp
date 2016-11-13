#include <string.h>
#include <iostream>
#include <refude/xdg.h>
#include "getprop.h"
#include "windowinfo.h"

namespace refude
{

    int forgiving_X_error_handler(Display *d, XErrorEvent *e)
    {
        char errorMsg[80];
        XGetErrorText(d, e->error_code, errorMsg, 80);
        printf("Got error: %s\n", errorMsg);
        return 0;
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
        return getProp<Window>(disp, root, "_NET_CLIENT_LIST_STACKING");
    }

    std::vector<WindowInfo> WindowInfo::normalWindows()
    {
        DefaultDisplay disp;
        Window root = XDefaultRootWindow(disp);
        std::vector<Window> windows = getProp<Window>(disp, root, "_NET_CLIENT_LIST_STACKING");
        std::vector<WindowInfo> result;
        for (Window w : windows) {
            WindowInfo windowInfo(w);
            if (windowInfo.title.size() > 0 && windowInfo.windowType == _NET_WM_WINDOW_TYPE_NORMAL) {
                result.push_back(std::move(windowInfo));
            }
        }

        return result;
    }

    WindowInfo::WindowInfo(Window window): 
        title(),
        windowType(0),
        windowState(0),
        x(0),
        y(0),
        width(0),
        height(0),
        icon(0),
        iconName(),
        window()
    {
        this->window = window;
        DefaultDisplay disp;
        unsigned long nitems;

		std::vector<char> tmpTitle = getProp<char>(disp, window, "_NET_WM_VISIBLE_NAME");
        if (tmpTitle.size() == 0) {
            tmpTitle = getProp<char>(disp, window, "_NET_WM_NAME");
        }
		title = std::string(tmpTitle.begin(), tmpTitle.end());

		std::vector<Atom> tmpWindowType = getProp<Atom>(disp, window, "_NET_WM_WINDOW_TYPE");
        if (tmpWindowType.size() > 0) {
            windowType = tmpWindowType[0];
        }
        else {
            windowType = _NET_WM_WINDOW_TYPE_NORMAL;
        }

		windowState = getProp<Atom>(disp, window, "_NET_WM_STATE");

		std::vector<long> frameExtents = getProp<long>(disp, window, "_NET_FRAME_EXTENTS");
        if (frameExtents.size() < 4) {
			frameExtents = {0, 0, 0, 0};
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

        icon = getProp<unsigned long>(disp, window, "_NET_WM_ICON");
        calculateIconName();
    }


    WindowInfo::WindowInfo(WindowInfo&& other) 
    {
        this->title = std::move(other.title);
        this->windowType = other.windowType;
        this->windowState = other.windowState;
        this->x = other.x;
        this->y = other.y;
        this->width = other.width;
        this->height = other.height;
        this->icon = std::move(other.icon);
        this->iconName = std::move(other.iconName);
        this->window = other.window;
    }

    WindowInfo& WindowInfo::operator=(WindowInfo&& other) 
    {
        this->title = std::move(other.title);
        this->windowType = other.windowType;
        this->windowState = other.windowState;
        this->x = other.x;
        this->y = other.y;
        this->width = other.width;
        this->height = other.height;
        this->icon = std::move(other.icon);
        this->iconName = std::move(other.iconName);
        this->window = other.window;
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

    /**
     * @brief WindowInfo::calculateIconName Just a very naive hashing algorithm.
     * Should be replaced with something better, e.g. FNV-1a
     */
    void WindowInfo::calculateIconName()
    {
        static const char hexDigit[16] = {'0', '1', '2', '3', '4', '5', '6', '7',
            '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
        char hash[8];
   		iconName.clear();
		char tmpName[20];
        memset(hash, 0, 8);
        for (unsigned long i = 0; i < icon.size(); i++) {
            hash[4 * i % 8] ^= ((icon[i] & 0xFF000000) >> 24);
            hash[(4 * i + 1) % 8] ^= ((icon[i] & 0xFF0000) >> 16);
            hash[(4 * i + 2) % 8] ^= ((icon[i] & 0xFF00) >> 8);
            hash[(4 * i + 3) % 8] ^= (icon[i] & 0xFF);
        }
        for (int i = 0; i < 8; i++) {
            tmpName[2 * i + 2 * i / 4] = hexDigit[(hash[i] & 0xF0) >> 4];
            tmpName[2 * i + 1 + (2 * i + 1) / 4] = hexDigit[hash[i] & 0xF];
        }
        tmpName[4] = tmpName[9] = tmpName[14] = '-';
        tmpName[19] = '\0';
		iconName = tmpName;
    }

}
