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
