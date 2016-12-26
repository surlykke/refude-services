/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the Refude Services project, library
 * It is distributed under the LGPL 2.1 license.
 * Please refer to the LGPL21 file for a copy of the license.
 */

#ifndef SERVER_H
#define SERVER_H

#include <vector>
#include <thread>

#include "map.h"
#include "fd.h"
#include "listener.h"
#include "httpmessage.h"
#include "abstractresource.h"

namespace refude
{
    class Server {
    public:
        Server();
        virtual ~Server();
        
        void serve(uint16_t portNumber); 
        void serve(const char *socketPath);
        void handle(Fd&& requestSocket, HttpMessage::ptr&& httpRequest);

        void map(AbstractResource::ptr resource, const char* path);
        void mapPrefix(AbstractResource::ptr resource, const char* prefix);
        void unMap(const char* path);
        AbstractResource::ptr mapping(const char* path, bool prefix = false);

        // To be run in background
        void readRequest(Fd&& requestSocket);
        void sendResponse(int socket, Buffer::ptr buf);
        void sendResponse(int socket, HttpCode status);
        // --


    private:
        Map<AbstractResource::ptr> resourceMappings;
        Map<AbstractResource::ptr> prefixMappings;
        std::vector<Listener> listeners;
    };

}

#endif    /* SERVER_H */

