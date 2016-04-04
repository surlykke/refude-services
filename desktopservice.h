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
#include "directories.h"

namespace org_restfulipc 
{
    using namespace std;
    class DesktopService : public Service
    {
    public:
        DesktopService();
        virtual ~DesktopService() {};

    private:
        void setupNotification();
        int setupWatches();
        int addWatch(int wd, string dir);
        void watcher(int wd);
        void buildResources();
        
        Directories directories;
        NotifierResource::ptr notifier;
        thread watchThread;

    };
}
#endif /* DESKTOPSERVICE_H */

