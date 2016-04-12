/*
* Copyright (c) 2015, 2016 Christian Surlykke
*
* This file is part of the refude-services project. 
* It is distributed under the GPL v2 license.
* Please refer to the LICENSE file for a copy of the license.
*/

#include "iconresourcebuilder.h"
#include "themereader.h"
#include "iconresource.h"
#include "desktopservice.h"

namespace org_restfulipc
{

    IconResourceBuilder::IconResourceBuilder()
    {
    }

    IconResourceBuilder::~IconResourceBuilder()
    {
    }

    void IconResourceBuilder::buildResources()
    {
        ThemeReader themeReader(iconThemeCollection["oxygen"], "/usr/share/icons/oxygen");
    }

    void IconResourceBuilder::mapResources(DesktopService& desktopService)
    {
        IconResource::ptr iconResource = make_shared<IconResource>(move(iconThemeCollection));
        desktopService.map("/themeicon", iconResource);
    }

}
