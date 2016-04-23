/*
* Copyright (c) 2015, 2016 Christian Surlykke
*
* This file is part of the refude-services project. 
* It is distributed under the GPL v2 license.
* Please refer to the LICENSE file for a copy of the license.
*/

#ifndef ICONRESOURCEBUILDER_H
#define ICONRESOURCEBUILDER_H
#include "desktopservice.h"
#include "icontheme.h"


namespace org_restfulipc 
{

    class IconResourceBuilder
    {
    public:
        IconResourceBuilder();
        virtual ~IconResourceBuilder();
        void buildResources();
        void mapResources(DesktopService& desktopService);

        IconThemeCollection iconThemeCollection;
        map<string, IconInstance> usrSharePixmapsIcons;
    };

}
#endif /* ICONRESOURCEBUILDER_H */
