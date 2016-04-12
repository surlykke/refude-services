/*
* Copyright (c) 2015, 2016 Christian Surlykke
*
* This file is part of the refude-services project. 
* It is distributed under the GPL v2 license.
* Please refer to the LICENSE file for a copy of the license.
*/

#ifndef ICONRESOURCE_H
#define ICONRESOURCE_H
#include <vector>
#include <string>
#include <ripc/webserver.h>

#include "icontheme.h"

namespace org_restfulipc
{
    using namespace std;
    class IconResource : public WebServer
    {
    public:
        typedef std::shared_ptr<IconResource> ptr;
        IconResource(IconThemeCollection&& iconThemeCollection);
        virtual ~IconResource();
        virtual const char* filePath(int matchedPathLength, const HttpMessage& request);

    private:
        void parseQueryString(const HttpMessage& request, map<string, vector<string>>& queryParameters);
        string nextQueryStringToken(const HttpMessage& request, int& pos);
        const char* findPathOfClosest(const vector<IconInstance>& instances, int size);
        string parent(string themeName);
        
        IconThemeCollection iconThemeCollection;
    };
}
#endif /* ICONRESOURCE_H */

