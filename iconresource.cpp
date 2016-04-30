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
    IconResource::IconResource(ThemeIconMap&& themeIconMap, IconMap&& usrSharePixmapIcons, InheritanceMap&& inheritanceMap):
        WebServer("/"),
        themeIconMap(move(themeIconMap)),
        usrSharePixmapsIcons(move(usrSharePixmapIcons)),
        inheritanceMap(move(inheritanceMap))
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

        if (! themeIconMap.contains(themeName)) {
            std::cerr << "No theme '" << themeName << "'\n";
            throw Status::Http404;
        }
        
        if (request.queryParameterMap["size"].size() == 1) {
            size = stoi(request.queryParameterMap["size"][0]);
        }
        else {
            size = 32; // FIXME
        }

        for(string& name = themeName; !name.empty() ; name = inheritanceMap[name]) {
            IconMap& iconMap = themeIconMap[name];
            for (const string& name : request.queryParameterMap["name"]) {
                if (iconMap.contains(name)) {
                    Json* icon =  findPathOfClosest(iconMap[name], size);
                    if (icon) {
                        return {(*icon)["path"], (*icon)["mimetype"]};
                    }
                }
            }
        }

        // So no icons in theme or it's ancestors. We look for an icon in
        // /usr/share/pixmaps, where some application icons can be found
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
