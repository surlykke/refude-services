/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the refude-services project. 
 * It is distributed under the GPL v2 license.
 * Please refer to the GPL2 file for a copy of the license.
 */

#ifndef WINDOWINFO_H
#define WINDOWINFO_H

#include <X11/Xlib.h>
#include <X11/Xatom.h>

#include <string>
#include <vector>

namespace refude 
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
        WindowInfo(WindowInfo&& other);
        WindowInfo& operator=(WindowInfo&& other);

        void raiseAndFocus();
        
        std::string title;
        Atom windowType;
        std::vector<Atom> windowState;
        int x;
        int y;
        unsigned int width;
        unsigned int height;
        std::vector<unsigned long> icon;
        std::string iconName;
        Window window;

	private:
        void calculateIconName();
    };
        
}

#endif /* WINDOWINFO_H */

