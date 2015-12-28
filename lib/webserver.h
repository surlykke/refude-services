#ifndef WEBSERVER_H
#define WEBSERVER_H
#include "abstractresource.h"

namespace org_restfulipc {

    class WebServer : public AbstractResource
    {
    public:
        WebServer(const char* html_root);

        virtual void handleRequest(int &socket, const HttpMessage& request);

    private:
        int rootFd;
    };
}
#endif // WEBSERVER_H
