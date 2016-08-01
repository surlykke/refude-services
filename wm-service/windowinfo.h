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
        static void init();
        static WindowInfo rootWindow();
        static std::vector<Window> windowIds();
        static std::vector<WindowInfo> normalWindows();

        WindowInfo(Window window);
        void raiseAndFocus();
        
        std::string title;
        Atom windowType;
        std::vector<Atom> windowState;
        int x;
        int y;
        unsigned int width;
        unsigned int height;
        long* icon;
        unsigned long iconLength;
        char iconName[20];
        Window window;
    private:

        void calculateIconName();
    };
        
}

#endif /* WINDOWINFO_H */

