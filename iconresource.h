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

