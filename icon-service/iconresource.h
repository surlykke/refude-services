/*
* Copyright (c) 2015, 2016 Christian Surlykke
*
* This file is part of the refude-services project. 
* It is distributed under the GPL v2 license.
* Please refer to the GPL2 file for a copy of the license.
*/

#ifndef ICONRESOURCE_H
#define ICONRESOURCE_H
#include <sys/types.h>
#include <vector>
#include <string>
#include "json.h"
#include "webserver.h"

#include "types.h"

namespace refude
{
    class IconResource : public AbstractResource
    {
    public:
        typedef std::unique_ptr<IconResource> ptr;
        IconResource(ThemeIconMap&& themeIconMap, IconMap&& usrSharePixmapIcons, InheritanceMap&& inheritanceMap);
        virtual ~IconResource();
        virtual void doGET(Descriptor &socket, HttpMessage &request, const char *remainingPath);
        
    private:
        void findByPath(Descriptor& socket, const char* path);
        Json* findPathOfClosest(Json& iconList, int size);
        bool othersHavePermissions(const char* filePath, mode_t permissions);
        
        ThemeIconMap themeIconMap;
        IconMap usrSharePixmapsIcons;
        InheritanceMap inheritanceMap;

    };
}
#endif /* ICONRESOURCE_H */

