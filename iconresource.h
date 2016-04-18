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
#include <ripc/webserver.h>

#include "icontheme.h"

namespace org_restfulipc
{
    using namespace std;
    class IconResource : public WebServer
    {
    public:
        typedef std::shared_ptr<IconResource> ptr;
        IconResource(IconThemeCollection&& iconThemeCollection, map<string, IconInstance>&& usrSharePixmapIcons);
        virtual ~IconResource();
        virtual PathMimetypePair findFile(int matchedPathLength, HttpMessage& request);

    private:
        const IconInstance* findPathOfClosest(const vector<IconInstance>& instances, int size);
        string parent(string themeName);
        
        IconThemeCollection iconThemeCollection;
        map<string, IconInstance> usrSharePixmapsIcons;
    };
}
#endif /* ICONRESOURCE_H */

