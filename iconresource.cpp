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

    IconResource::IconResource(Json&& themes, Json&& usrSharePixmapIcons) : 
        WebServer("/"),
        themes(move(themes)),
        usrSharePixmapsIcons(move(usrSharePixmapIcons))
    {
    }

    IconResource::~IconResource()
    {
    }

    PathMimetypePair IconResource::findFile(HttpMessage& request, const char* remainingPath)
    {
        std::cout << "Into IconResource::findFile\n";
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

        std::cout << "names: ";
        for (const string& name : request.queryParameterMap["name"]) {
            std::cout << name << " ";
        }
        std::cout << "\n";

        while (! themeName.empty()) {
            std::cout << "Looking at theme " << themeName << "\n";
            if (! themes.contains(themeName)) {
                std::cerr << "No theme '" << themeName << "'\n";
                throw Status::Http404;
            }

            Json& theme = themes[themeName];
            for (const string& name : request.queryParameterMap["name"]) {
                if (theme["Icons"].contains(name)) {
                    Json* icon =  findPathOfClosest(theme["Icons"][name], size);
                    if (icon) {
                        std::cout << "Found icon: " << JsonWriter(*icon).buffer.data() << "\n";
                        return {(*icon)["path"], (*icon)["mimetype"]};
                    }
                }
            }

            themeName = parent(themeName);
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
