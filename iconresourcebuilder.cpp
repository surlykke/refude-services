/*
* Copyright (c) 2015, 2016 Christian Surlykke
*
* This file is part of the refude-services project. 
* It is distributed under the GPL v2 license.
* Please refer to the LICENSE file for a copy of the license.
*/
#include <ripc/utils.h>
#include <ripc/jsonwriter.h>

#include "themereader.h"
#include "iconresource.h"
#include "desktopservice.h"
#include "iconcollector.h"
#include "themeTemplate.h"
#include "themesTemplate.h"

#include "iconresourcebuilder.h"

namespace org_restfulipc
{

    IconResourceBuilder::IconResourceBuilder()
    {
        themes << themesTemplate_json;
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
                if (!themes.contains(themeDir)) {
                    themes[themeDir] << themeTemplate_json;
                }
                ThemeReader(themes[themeDir], iconsDir + '/' + themeDir);
            }
        }

        IconCollector iconCollector("/usr/share/pixmaps");
        usrSharePixmapsIcons = move(iconCollector.collectedIcons);
    }

    void IconResourceBuilder::mapResources(DesktopService& desktopService)
    {
        IconResource::ptr iconResource = make_shared<IconResource>(move(themes), move(usrSharePixmapsIcons));
        desktopService.map("/themeicon", iconResource);
    }

}
