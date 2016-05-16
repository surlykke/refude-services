#ifndef DESKTOPWATCHER_H
#define DESKTOPWATCHER_H
#include <string>
using namespace std;
namespace org_restfulipc
{

    class desktopwatcher
    {
    public:
        desktopwatcher();
        virtual ~desktopwatcher();
    private:
        int setupWatches();
        int addWatch(int wd, string dir);
        void watcher(int wd);
    };
}
#endif /* DESKTOPWATCHER_H */

