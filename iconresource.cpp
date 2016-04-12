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

    IconResource::IconResource(IconThemeCollection&& iconThemeCollection) : 
        WebServer("/"),
        iconThemeCollection(iconThemeCollection)
    {
    }

    IconResource::~IconResource()
    {
    }

    const char* IconResource::filePath(int matchedPathLength, const HttpMessage& request)
    {
        std::cout << "IconResource::filePath, queryString: " << request.queryString << "\n";
        map<string, vector<string>> queryParameters;
        parseQueryString(request, queryParameters); 

        std::cout << "queryparameters:\n";
        for (const auto& p : queryParameters) {
            std::cout << p.first << " -> ";
            for (const string& value : p.second) {
                std::cout << value << " ";
            }
            std::cout << "\n";
        }

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

        std::cout << "themeName: " << themeName << ", size: " << size << "\n";

        while (! themeName.empty()) {
            if (iconThemeCollection.find(themeName) == iconThemeCollection.end()) {
                std::cout << "theme not found";
                throw Status::Http404;
            }

            IconTheme& iconTheme = iconThemeCollection[themeName];
            for (string name : queryParameters["name"]) {
                if (iconTheme.find(name) != iconTheme.end()) {
                    return findPathOfClosest(iconTheme[name], size);
                }
            }

            themeName = parent(themeName);
        }
        std::cout << "Nothing found\n";
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

    const char* IconResource::findPathOfClosest(const vector<IconInstance>& instances, int size)
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
                return instance.path.data(); // No reason to look further..
            }

            if (distance < bestDistanceSoFar) {
                candidate = &instance;
                bestDistanceSoFar = distance;
            }
        }
        
        if (candidate) {
            return candidate->path.data();
        }
        else {
            throw Status::Http404;
        }
    }

    string IconResource::parent(string themeName)
    {
        return string();
    }

}
