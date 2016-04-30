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
#include "desktopservice.h"
#include "iconcollector.h"
#include "themeTemplate.h"
#include "themesTemplate.h"

#include "iconresourcebuilder.h"

namespace org_restfulipc
{

    IconResourceBuilder::IconResourceBuilder()
    {
        themesJson << themesTemplate_json;
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
                Json& themeJson = themeJsonMap[themeDir]; 
                if (themeJson.undefined()) {
                    themeJson << themeTemplate_json;
                }
                ThemeReader(themeJson, themeIconMap[themeDir], iconsDir + '/' + themeDir);
                if (themeDir != "hicolor") {
                    if (themeJson.contains("Inherits")) {
                        inheritanceMap[themeDir] = (const char*) themeJson["Inherits"];
                    }
                    else {
                        inheritanceMap[themeDir] = "hicolor";
                    }
                }
            }
        }

        IconCollector("/usr/share/pixmaps", 0, 0, "").collectInto(usrSharePixmapsIcons);
    }

    void IconResourceBuilder::mapResources(DesktopService& desktopService)
    {
        IconResource::ptr iconResource = 
            make_shared<IconResource>(move(themeIconMap), move(usrSharePixmapsIcons), move(inheritanceMap));
        desktopService.map("/icons/icon", iconResource);

        themeJsonMap.each([&desktopService, this](const char* themeDirName, Json& themeJson){
            themesJson["themes"].append(themeDirName);
            string selfUri = string("/icons/themes/") + themeDirName;
            themeJson["_links"]["self"]["href"] = selfUri;
            LocalizedJsonResource::ptr themeResource = make_shared<LocalizedJsonResource>();
            themeResource->setJson(move(themeJson));
            desktopService.map(selfUri.data(), themeResource);
        });

        const char* themesSelfUri = "/icons/themes";
        themesJson["_links"]["self"]["href"] = themesSelfUri;
        JsonResource::ptr themesResource = make_shared<JsonResource>();
        themesResource->setJson(move(themesJson));
        desktopService.map(themesSelfUri, themesResource);
    }

}
