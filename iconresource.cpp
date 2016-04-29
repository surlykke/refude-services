/*
* Copyright (c) 2015, 2016 Christian Surlykke
*
* This file is part of the refude-services project. 
* It is distributed under the GPL v2 license.
* Please refer to the LICENSE file for a copy of the license.
*/

#include <ripc/map.h>
#include <ripc/jsonwriter.h>

#include "iconresource.h"
namespace org_restfulipc 
{

    IconResource::IconResource(Json&& icons, Json&& usrSharePixmapIcons, Json&& inheritance) : 
        WebServer("/"),
        icons(move(icons)),
        usrSharePixmapsIcons(move(usrSharePixmapIcons)),
        inheritance(move(inheritance))
    {
    }

    IconResource::~IconResource()
    {
    }

    PathMimetypePair IconResource::findFile(HttpMessage& request, const char* remainingPath)
    {
        vector<string> names;
        string themeName;
        int size;

        if (request.queryParameterMap["name"].size() < 1) throw Status::Http422;
        if (request.queryParameterMap["theme"].size() > 1) throw Status::Http422;
        if (request.queryParameterMap["size"].size() > 1) throw Status::Http422;

        if (request.queryParameterMap["theme"].size() == 1) {
            themeName = request.queryParameterMap["theme"][0];
        }
        else {
            themeName = "oxygen"; // FIXME
        }
        
        if (request.queryParameterMap["size"].size() == 1) {
            size = stoi(request.queryParameterMap["size"][0]);
        }
        else {
            size = 32; // FIXME
        }

        for(;;) {
            if (! icons.contains(themeName)) {
                std::cerr << "No theme '" << themeName << "'\n";
                throw Status::Http404;
            }

            Json& themeIcons = icons[themeName];
            for (const string& name : request.queryParameterMap["name"]) {
                if (themeIcons.contains(name)) {
                    Json* icon =  findPathOfClosest(themeIcons[name], size);
                    if (icon) {
                        return {(*icon)["path"], (*icon)["mimetype"]};
                    }
                }
            }

            if (! inheritance.contains(themeName)) {
                break;
            }
            themeName = (const char*)inheritance[themeName];
        }

        // So no icons in theme or it's ancestors. We look for an icon in
        // /usr/share/pixmaps, where some applicationicons can be found
        for (const string& name : request.queryParameterMap["name"]) {
            if (usrSharePixmapsIcons.contains(name)) {
                return {usrSharePixmapsIcons[name]["path"], usrSharePixmapsIcons[name]["mimetype"]};
            }
        }

        // Abandon all hope
        throw Status::Http404;
    }

    Json* IconResource::findPathOfClosest(Json& iconList, int size)
    {
        double bestDistanceSoFar = numeric_limits<double>::max();
        Json* candidate = NULL;
        for (int i = 0; i < iconList.size(); i++) {
            Json* instance = &iconList[i];
            double distance;
            double minSize = (*instance)["minSize"];
            double maxSize = (*instance)["maxSize"];
            if (size < minSize) {
                distance = minSize - size;
            }
            else if (size > maxSize) {
                distance = size - maxSize;
            }
            else {
                return instance; // No reason to look further..
            }

            if (distance < bestDistanceSoFar) {
                candidate = instance;
                bestDistanceSoFar = distance;
            }
        }
        
        return candidate;
    }

    string IconResource::parent(string themeName)
    {
        return string();
    }

}
