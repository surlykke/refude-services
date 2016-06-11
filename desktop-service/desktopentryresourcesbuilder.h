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
    class DesktopEntryResourceBuilder
    {
    public:
        DesktopEntryResourceBuilder();
        virtual ~DesktopEntryResourceBuilder();
        void build();

        Map<Json> desktopJsons;
        Map<std::vector<std::string>> defaultApplications;

    private:
        void readDesktopFiles(std::vector<std::string> applicationsDirs);
        void readMimeappsListFile(std::string dir);
   };

}
#endif /* DESKTOPENTRYRESOURCESBUILDER_H */

