/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the Refude Services project, library
 * It is distributed under the LGPL 2.1 license.
 * Please refer to the LGPL21 file for a copy of the license.
 */

#ifndef WEBSERVER_H
#define WEBSERVER_H
#include <string>
#include "abstractresource.h"
#include "magic.h"
namespace refude 
{
    class WebServer : public AbstractResource
    {
    public:
        typedef std::unique_ptr<WebServer> ptr;
        WebServer(const char* rootDir);
        void doGET(Descriptor& socket, HttpMessage& request, const char* remainingPath) override;
    
    private:
        const std::string rootDir;
    };
}
#endif // WEBSERVER_H
