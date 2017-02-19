/*
* Copyright (c) 2015, 2016 Christian Surlykke
*
* This file is part of the refude-services project. 
* It is distributed under the GPL v2 license.
* Please refer to the GPL2 file for a copy of the license.
*/

#ifndef ICONRESOURCEBUILDER_H
#define ICONRESOURCEBUILDER_H
#include "service.h"

#include "icontheme.h"
#include "iconresource.h"

namespace refude 
{

    class IconResourceBuilder
    {
    public:
        IconResourceBuilder();
        virtual ~IconResourceBuilder();
        void buildResources();
        void mapResources();

        Json themesJson;
        Map<Json> themeJsonMap;

        ThemeIconMap themeIconMap;
        IconMap usrSharePixmapsIcons;
        InheritanceMap  inheritanceMap;
   };

}
#endif /* ICONRESOURCEBUILDER_H */

