#ifndef APPLICATIONCOLLECTOR_H
#define APPLICATIONCOLLECTOR_H
#include <set>
#include <ripc/map.h>
#include <ripc/json.h>

namespace org_restfulipc
{
    class ApplicationCollector
    {
    public:
        ApplicationCollector();
        virtual ~ApplicationCollector();
        void collect();
        Map<std::vector<std::string>> defaultApplications;
        Json applicationJsons;
        Json filehandlerJsons;
        Json urlhandlerJsons;

    private:
        void readDesktopFiles(std::vector<std::string> applicationsDirs);
        void readMimeappsListFile(std::string dir);

    };
}
#endif /* APPLICATIONCOLLECTOR_H */

