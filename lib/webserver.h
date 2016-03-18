#ifndef WEBSERVER_H
#define WEBSERVER_H
#include "abstractresource.h"

namespace org_restfulipc {

    class WebServer : public AbstractResource
    {
    public:
        typedef std::shared_ptr<WebServer> ptr;
        WebServer(const char* html_root);

        virtual void handleRequest(int &socket, int matchedPathLength, const HttpMessage& request);

    private:
        int rootFd;
    };
}
#endif // WEBSERVER_H
