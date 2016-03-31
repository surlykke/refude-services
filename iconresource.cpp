/*
* Copyright (c) 2015, 2016 Christian Surlykke
*
* This file is part of the refude-services project. 
* It is distributed under the GPL v2 license.
* Please refer to the LICENSE file for a copy of the license.
*/

#include "iconresource.h"
namespace org_restfulipc 
{

    IconResource::IconResource() : WebServer("/")
    {
    }

    IconResource::~IconResource()
    {
    }

    const char* IconResource::filePath(int matchedPathLength, const HttpMessage& request)
    {
        // Mock
        return "usr/share/icons/oxygen/32x32/categories/preferences-desktop.png";
    }

}
