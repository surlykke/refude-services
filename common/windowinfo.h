#ifndef WINDOWINFO_H
#define WINDOWINFO_H

#include <X11/Xlib.h>
#include <X11/Xatom.h>

#include <vector>

namespace org_restfulipc 
{
    struct geometry {
        int left,top;
        unsigned int width, height;
    };

    class WindowInfo
    {
    public:
        static WindowInfo rootWindow();
        static std::vector<WindowInfo> normalWindows();
    
        void raiseAndFocus();
        
        std::string title;
        Atom windowType;
        int x;
        int y;
        unsigned int width;
        unsigned int height;

        Window window;
    private:
        WindowInfo(Window window);
    };
        
}

#endif /* WINDOWINFO_H */

