/*
* Copyright (c) 2015, 2016 Christian Surlykke
*
* This file is part of the refude-services project. 
* It is distributed under the GPL v2 license.
* Please refer to the LICENSE file for a copy of the license.
*/


#ifndef DESKTOPSERVICE_H
#define DESKTOPSERVICE_H

#include <string>
#include <vector>
#include <map>
#include <ripc/notifierresource.h>
#include <ripc/service.h>
#include "mimetyperesource.h"
#include "typedefs.h"
#include "iconresource.h"

namespace org_restfulipc 
{
    using namespace std;
    class DesktopService : public Service
    {
    public:
        DesktopService();
        virtual ~DesktopService() {};
        bool setDefaultApplication(string mimetype, string desktopEntryId);

    private:
        void setupNotification();
        void findDirs();
        vector<string> directoryTree(string directory);
        vector<string> desktopFiles(string directory);
        int setupWatches();
        int addWatch(int wd, string dir);
        void watcher(int wd);
        
        void buildResources();
        
        void collectMimetypes();
        void readDesktopFiles(vector<string> applicationsDirs);
        void readMimeappsListFile(string dir);
        void addAssociationsToMimeAndDesktopResources();
        void buildFileAndUrlHandlerResource();
        void mapResources();

        NotifierResource::ptr notifier;

        string usersConfigDir;   // eg. ~/.config
        vector<string> systemConfigDirs; // eg. /etc/xdg
        string usersApplicationDirRoot;  // eg. ~/.local/share/applications
        vector<string> systemApplicationDirRoots; // eg. /usr

        std::map<string, LocalizedJsonResource::ptr > desktopResources;    
        std::map<string, MimetypeResource::ptr > mimetypeResources;
        JsonResource::ptr rootMimeResource;
        JsonResource::ptr handlerResource;
        AppSets associations;
        AppLists defaults;

        thread watchThread;

        IconResource::ptr iconResource;
    };
}
#endif /* DESKTOPSERVICE_H */

