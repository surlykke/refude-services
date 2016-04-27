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

#include "icontheme.h"

namespace org_restfulipc
{
    using namespace std;
    class IconResource : public WebServer
    {
    public:
        typedef std::shared_ptr<IconResource> ptr;
        IconResource(Json&& themes, Json&& usrSharePixmapIcons);
        virtual ~IconResource();
        virtual PathMimetypePair findFile(HttpMessage& request, const char* remainingPath) override;

    private:
        Json* findPathOfClosest(Json& iconList, int size);
        string parent(string themeName);
        
        Json themes;
        Json usrSharePixmapsIcons;
    };
}
#endif /* ICONRESOURCE_H */

