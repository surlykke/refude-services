/*
* Copyright (c) 2015, 2016 Christian Surlykke
*
* This file is part of the refude-services project. 
* It is distributed under the GPL v2 license.
* Please refer to the LICENSE file for a copy of the license.
*/

#ifndef DESKTOPENTRYRESOURCESBUILDER_H
#define DESKTOPENTRYRESOURCESBUILDER_H
#include <string>
#include <vector>

#include <ripc/map.h>
#include <ripc/json.h>
#include <ripc/service.h>

#include "xdg.h"
#include "typedefs.h"

namespace org_restfulipc
{
    using namespace std;
    
    class DesktopEntryResourceBuilder
    {
    public:
        DesktopEntryResourceBuilder();
        virtual ~DesktopEntryResourceBuilder();
        void build();

        Map<Json> desktopJsons;
        Map<vector<string>> defaultApplications;

    private:
        vector<string> desktopFiles(string directory);
        void readDesktopFiles(vector<string> applicationsDirs);
        void readMimeappsListFile(string dir);
   };

}
#endif /* DESKTOPENTRYRESOURCESBUILDER_H */

