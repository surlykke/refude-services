/*
* Copyright (c) 2015, 2016 Christian Surlykke
*
* This file is part of the Restful Inter Process Communication (Ripc) project. 
* It is distributed under the LGPL 2.1 license.
* Please refer to the LICENSE file for a copy of the license.
*/

#ifndef WEBSERVER_H
#define WEBSERVER_H
#include <string>
#include "abstractresource.h"
#include "magic.h"
namespace refude 
{
    struct PathMimetypePair
    {
        const char* path;
        const char* mimetype;
    };

    class WebServer : public AbstractResource
    {
    public:
        typedef std::shared_ptr<WebServer> ptr;
        WebServer(const char* rootDir);
        void doGET(int& socket, HttpMessage& request) override;
        virtual PathMimetypePair findFile(HttpMessage& request);
    
    private:
        magic_t magic_cookie;
        const std::string rootDir;
        int rootFd;
    };
}
#endif // WEBSERVER_H
