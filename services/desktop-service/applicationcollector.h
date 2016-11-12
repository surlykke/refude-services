#ifndef APPLICATIONCOLLECTOR_H
#define APPLICATIONCOLLECTOR_H
#include <set>
#include <refude/map.h>
#include <refude/json.h>

namespace org_restfulipc
{
    class ApplicationCollector
    {
    public:
        ApplicationCollector();
        virtual ~ApplicationCollector();
        void collect();
        Map<std::vector<std::string>> defaultApplications;
        Json jsonArray;

    private:
        void readDesktopFiles(std::vector<std::string> applicationsDirs, Map<Json>& jsonMap);
        void readMimeappsListFile(std::string dir, Map<Json>& jsonMap);

    };
}
#endif /* APPLICATIONCOLLECTOR_H */

