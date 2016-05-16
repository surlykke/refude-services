#ifndef DESKTOPWATCHER_H
#define DESKTOPWATCHER_H
#include <string>

namespace org_restfulipc
{

    class desktopwatcher
    {
    public:
        desktopwatcher();
        virtual ~desktopwatcher();
    private:
        int setupWatches();
        int addWatch(int wd, std::string dir);
        void watcher(int wd);
    };
}
#endif /* DESKTOPWATCHER_H */

