/*
* Copyright (c) 2015, 2016 Christian Surlykke
*
* This file is part of the refude-services project. 
* It is distributed under the GPL v2 license.
* Please refer to the LICENSE file for a copy of the license.
*/

#ifndef ICONRESOURCE_H
#define ICONRESOURCE_H
#include <vector>
#include <string>
#include <ripc/json.h>
#include <ripc/webserver.h>

#include "types.h"

namespace org_restfulipc
{
    class IconResource : public WebServer
    {
    public:
        typedef std::shared_ptr<IconResource> ptr;
        IconResource(ThemeIconMap&& themeIconMap, IconMap&& usrSharePixmapIcons, InheritanceMap&& inheritanceMap);
        virtual ~IconResource();
        virtual PathMimetypePair findFile(HttpMessage& request, const char* remainingPath) override;

    private:
        Json* findPathOfClosest(Json& iconList, int size);
        string parent(string themeName);
        
        ThemeIconMap themeIconMap;
        IconMap usrSharePixmapsIcons;
        InheritanceMap inheritanceMap;
    };
}
#endif /* ICONRESOURCE_H */

