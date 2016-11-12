/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the refude-services project. 
 * It is distributed under the GPL v2 license.
 * Please refer to the LICENSE file for a copy of the license.
 */
#include <refude/utils.h>
#include <refude/jsonwriter.h>
#include <refude/jsonresource.h>
#include "xdg.h"
#include "themereader.h"
#include "iconcollector.h"

#include "iconresourcebuilder.h"

namespace org_restfulipc
{

    IconResourceBuilder::IconResourceBuilder()
    {
        static const char* themesTemplate_json = R"json(
        {
            "defaultTheme" : "hicolor",
            "themes" : []
        }
        )json";

        themesJson << themesTemplate_json;
    }

    IconResourceBuilder::~IconResourceBuilder()
    {
    }

    void IconResourceBuilder::buildResources()
    {
        static const char* themeTemplate_json = R"Json(
            {
                "_ripc:localized:Name" : {},
                "_ripc:localized:Comment" : { },
                "IconDirectories" : {}, 
                "Inherits" : "hicolor",
                "Hidden" : false
            }
        )Json";

        std::vector<std::string> iconsDirs;
        for (const auto& dir : append(xdg::data_dirs(), "/icons")) {
            iconsDirs.push_back(dir);
        }
        iconsDirs.push_back(xdg::data_home() + "/icons");
        iconsDirs.push_back(xdg::home() + "/.icons");

    
        for (auto dirIterator =  iconsDirs.begin(); dirIterator != iconsDirs.end(); dirIterator++) {
            for (std::string themeDir : subdirectories(*dirIterator)) {
                Json& themeJson = themeJsonMap[themeDir]; 
                if (themeJson.undefined()) {
                    themeJson << themeTemplate_json;
                }

                ThemeReader(themeJson, *dirIterator + '/' + themeDir);
                IconMap& iconMap = themeIconMap[themeDir];
                themeJson["IconDirectories"].eachEntry([&](const char* iconDirPath, Json& iconDirJson) {
                    IconCollector(*dirIterator + "/" + themeDir + "/" + iconDirPath, iconDirJson).collectInto(iconMap);
                });

                if (themeDir == "hicolor") { 
                    if (themeJson.contains("Inherits")) {
                        // Thou shalt not loop forever
                        themeJson.take("Inherits");
                    }
                }
                else {
                    inheritanceMap[themeDir] = (const char*) themeJson["Inherits"];
                }

            }
        }
        Json dummyIconJson = JsonConst::EmptyObject;
        dummyIconJson["Size"] = 0.0;
        dummyIconJson["MinSize"] = 0.0;
        dummyIconJson["MaxSize"] = 0.0;

        IconCollector("/usr/share/pixmaps", dummyIconJson).collectInto(usrSharePixmapsIcons);
    }

    void IconResourceBuilder::mapResources(Service& service)
    {
        IconResource::ptr iconResource = std::make_shared<IconResource>(std::move(themeIconMap), 
                                                                        std::move(usrSharePixmapsIcons), 
                                                                        std::move(inheritanceMap));

        service.map(iconResource, "icons", "icon");

        themeJsonMap.each([&service, this](const char* themeDirName, Json& themeJson){
            themesJson["themes"].append(themeDirName);
            JsonResource::ptr themeResource = std::make_shared<JsonResource>();
            themeResource->setJson(std::move(themeJson));
            service.map(themeResource, "icons", "themes", themeDirName);
        });

        JsonResource::ptr themesResource = std::make_shared<JsonResource>();
        themesResource->setJson(std::move(themesJson));
        service.map(themesResource, "icons", "themes");
    }

}
