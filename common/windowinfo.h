#ifndef WINDOWINFO_H
#define WINDOWINFO_H

#include <X11/Xlib.h>
#include <X11/Xatom.h>


namespace org_restfulipc 
{
    struct WindowInfo
    {
        static WindowInfo rootWindow();
        WindowInfo(Window window);
        WindowInfo(WindowInfo& other) = delete;
        WindowInfo(WindowInfo&& other);

        ~WindowInfo();
        bool isNormal();
        char* title;
        Atom* windowType;
        Window* clients; 
        int x;
        int y;
        unsigned int width;
        unsigned int height;

    };
        
    void raiseAndFocus(Window window);
}

#endif /* WINDOWINFO_H */

