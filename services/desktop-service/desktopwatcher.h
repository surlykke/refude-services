#ifndef DESKTOPWATCHER_H
#define DESKTOPWATCHER_H
#include <string>

#include "controller.h"

namespace org_restfulipc
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

