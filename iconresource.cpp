/*
* Copyright (c) 2015, 2016 Christian Surlykke
*
* This file is part of the refude-services project. 
* It is distributed under the GPL v2 license.
* Please refer to the LICENSE file for a copy of the license.
*/

#include <ripc/map.h>

#include "iconresource.h"
namespace org_restfulipc 
{

    IconResource::IconResource(IconThemeCollection&& iconThemeCollection, 
                               map<string, IconInstance>&& usrSharePixmapIcons) : 
        WebServer("/"),
        iconThemeCollection(iconThemeCollection),
        usrSharePixmapsIcons(usrSharePixmapIcons)
    {
    }

    IconResource::~IconResource()
    {
    }

    PathMimetypePair IconResource::findFile(int matchedPathLength, const HttpMessage& request)
    {
        map<string, vector<string>> queryParameters;
        parseQueryString(request, queryParameters); 

        vector<string> names;
        string themeName;
        int size;

        if (queryParameters["name"].size() == 0) throw Status::Http422;
        if (queryParameters["theme"].size() > 1) throw Status::Http422;
        if (queryParameters["size"].size() > 1) throw Status::Http422;

        if (queryParameters["theme"].size() == 1) {
            themeName = queryParameters["theme"][0];
        }
        else {
            themeName = "oxygen"; // FIXME
        }
        
        if (queryParameters["size"].size() == 1) {
            size = stoi(queryParameters["size"][0]);
        }
        else {
            size = 32; // FIXME
        }

        while (! themeName.empty()) {
            if (iconThemeCollection.find(themeName) == iconThemeCollection.end()) {
                std::cerr << "No theme '" << themeName << "'\n";
                throw Status::Http404;
            }

            IconTheme& iconTheme = iconThemeCollection[themeName];
            for (const string& name : queryParameters["name"]) {
                if (iconTheme.find(name) != iconTheme.end()) {
                    const IconInstance* instance =  findPathOfClosest(iconTheme[name], size);
                    if (instance) {
                        return {instance->path.data(), instance->mimetype.data()};
                    }
                }
            }

            themeName = parent(themeName);
        }

        // So no icons in theme or it's ancestors. We look for an icon in
        // /usr/share/pixmaps, where some applicationicons can be found
        for (const string& name : queryParameters["name"]) {
            if (usrSharePixmapsIcons.find(name) != usrSharePixmapsIcons.end()) {
                return {usrSharePixmapsIcons[name].path.data(), usrSharePixmapsIcons[name].mimetype.data()};
            }
        }

        // Abandon all hope
        throw Status::Http404;
    }

    void IconResource::parseQueryString(const HttpMessage& request, map<string, vector<string>>& queryParameters)
    {
        int pos = 0;
        while (request.queryString[pos]) {
            string parameterName = nextQueryStringToken(request, pos);
            if (request.queryString[pos] == '=') {
                queryParameters[parameterName].push_back(nextQueryStringToken(request, ++pos)); 
            }

            if (request.queryString[pos] == '&') {
                pos++;
            }
        }

    }

    string IconResource::nextQueryStringToken(const HttpMessage& request, int& pos)
    {
        int start = pos;
        while (request.queryString[pos] && request.queryString[pos] != '=' && request.queryString[pos] != '&') {
            pos++;
        }

        return string(request.queryString + start, pos - start);
    }

    const IconInstance* IconResource::findPathOfClosest(const vector<IconInstance>& instances, int size)
    {
        int bestDistanceSoFar = numeric_limits<int>::max();
        const IconInstance* candidate = NULL;
        for (const IconInstance& instance : instances) {
            int distance;
            if (size < instance.minSize) {
                distance = instance.minSize - size;
            }
            else if (size > instance.maxSize) {
                distance = size - instance.maxSize;
            }
            else {
                return &instance; // No reason to look further..
            }

            if (distance < bestDistanceSoFar) {
                candidate = &instance;
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
