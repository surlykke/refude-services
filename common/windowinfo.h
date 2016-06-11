#ifndef WINDOWINFO_H
#define WINDOWINFO_H

#include <X11/Xlib.h>
#include <X11/Xatom.h>

#include <string>
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
        long* icon;
        unsigned long iconLength;
        char iconName[20];
        Window window;
    private:
        WindowInfo(Window window);

        void calculateIconName();
    };
        
}

#endif /* WINDOWINFO_H */

