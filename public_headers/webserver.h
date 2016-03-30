#ifndef WEBSERVER_H
#define WEBSERVER_H
#include <string>
#include "abstractresource.h"
#include "magic.h"
namespace org_restfulipc 
{
    using namespace std;
    class WebServer : public AbstractResource
    {
    public:
        typedef std::shared_ptr<WebServer> ptr;
        WebServer(const char* rootDir);

        virtual void handleRequest(int &socket, int matchedPathLength, const HttpMessage& request);
        virtual const char* filePath(int matchedPathLength, const HttpMessage& request);
    private:
        const char* mimetype(const char* filePath);
        magic_t magic_cookie;
        const string rootDir;
        int rootFd;
    };
}
#endif // WEBSERVER_H
