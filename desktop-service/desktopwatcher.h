/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the refude-services project. 
 * It is distributed under the GPL v2 license.
 * Please refer to the GPL2 file for a copy of the license.
 */

#ifndef DESKTOPWATCHER_H
#define DESKTOPWATCHER_H
#include <string>

#include "controller.h"

namespace refude
{

    class DesktopWatcher
    {
    public:
        DesktopWatcher(Controller& controller, bool emitEventOnStart);
        virtual ~DesktopWatcher();
        void start();
    private:
        int addWatch(int wd, std::string dir);
        void watcher(int wd);
        
        Controller& controller;
        bool emitEventOnStart;
        std::thread watchingThread; 
    };
}
#endif /* DESKTOPWATCHER_H */

