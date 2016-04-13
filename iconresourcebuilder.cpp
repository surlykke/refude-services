/*
* Copyright (c) 2015, 2016 Christian Surlykke
*
* This file is part of the refude-services project. 
* It is distributed under the GPL v2 license.
* Please refer to the LICENSE file for a copy of the license.
*/
#include <ripc/utils.h>

#include "iconresourcebuilder.h"
#include "themereader.h"
#include "iconresource.h"
#include "desktopservice.h"
#include "iconcollector.h"

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
        vector<string> iconsDirs;
        iconsDirs.push_back(xdg::home() + "/.icons");

        for (const auto& dir : append(xdg::data_dirs(), "/icons")) {
            iconsDirs.push_back(dir);
        }

        for (string iconsDir : iconsDirs) {
            for (string themeDir : subdirectories(iconsDir)) {
                ThemeReader(iconThemeCollection[themeDir], iconsDir + '/' + themeDir);
            }
        }

        IconCollector iconCollector("/usr/share/pixmaps");
        usrSharePixmapsIcons = move(iconCollector.collectedIcons);
    }

    void IconResourceBuilder::mapResources(DesktopService& desktopService)
    {
        IconResource::ptr iconResource = make_shared<IconResource>(move(iconThemeCollection), move(usrSharePixmapsIcons));
        desktopService.map("/themeicon", iconResource);
    }

}
