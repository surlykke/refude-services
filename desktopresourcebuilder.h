#ifndef DESKTOPRESOURCEBUILDER_H
#define DESKTOPRESOURCEBUILDER_H

#include <string>
#include <list>
#include <set>

#include "service.h"
#include "mimeappslistreader.h"

namespace org_restfulipc 
{
    using namespace std;
    class JsonResource;
    class DesktopResourceBuilder
    {
    public:
        DesktopResourceBuilder(Service* service, MimeappsListReader& mimeappsListReader);
        virtual ~DesktopResourceBuilder();
        void build();
        MimeappsListReader& mimeappsListReader;
    private:
        void findDirs(); 
        void findDesktopEntriesAndSubdirs(string dir, vector<string>& entries, vector<string>& subdirs);
        void build(string applicationsDir, string subDir);

        vector<string> desktopEnvNames; 
        vector<string> configDirs; 
        vector<string> applicationsDirs;

        Service* service;

        set<string> processedEntries;
        JsonResource* fileHandlers;
        JsonResource* urlHandlers;
    };

}
#endif /* DESKTOPRESOURCEBUILDER_H */

