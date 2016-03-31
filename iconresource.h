/*
* Copyright (c) 2015, 2016 Christian Surlykke
*
* This file is part of the refude-services project. 
* It is distributed under the GPL v2 license.
* Please refer to the LICENSE file for a copy of the license.
*/

#ifndef ICONRESOURCE_H
#define ICONRESOURCE_H
#include <ripc/webserver.h>

namespace org_restfulipc
{
    class IconResource : public WebServer
    {
    public:
        typedef std::shared_ptr<IconResource> ptr;
        IconResource();
        virtual ~IconResource();
        virtual const char* filePath(int matchedPathLength, const HttpMessage& request);

    private:

    };
}
#endif /* ICONRESOURCE_H */

