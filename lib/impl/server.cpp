/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the Refude Services project, library
 * It is distributed under the LGPL 2.1 license.
 * Please refer to the LGPL21 file for a copy of the license.
 */

#include <sys/un.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <poll.h>
#include <limits.h>

#include "httpmessage.h"
#include "httpmessagereader.h"
#include "abstractresource.h"
#include "app.h"
#include "server.h"

namespace refude
{

    Server::Server() :
        resourceMappings(),
        prefixMappings()
    {
    }

    Server::~Server()
    {
        //FIXME
    }

    void Server::serve(uint16_t portNumber)
    {
        listeners.push_back(Listener(portNumber, this));
    }

    void Server::serve(const char *socketPath)
    {
        listeners.push_back(Listener(socketPath, this));
    }

    // Should run in background
    void Server::handle(Fd&& requestSocket, HttpMessage::ptr&& httpRequest)
    {
        try {
            AbstractResource::ptr handler;
            int resourceIndex = resourceMappings.find(httpRequest->path);
            if (resourceIndex > -1) {
                handler = resourceMappings.pairAt(resourceIndex).value;
                // FIXME request->setMatchedPathLength(strlen(resourceMappings.pairAt(resourceIndex).key));
            }
            else {
                resourceIndex = prefixMappings.find_longest_prefix(httpRequest->path);
                if (resourceIndex >= 0) {
                    const char* matchedPath = prefixMappings.pairAt(resourceIndex).key;
                    const char firstCharAfterMatch = httpRequest->path[strlen(matchedPath)];
                    if ( firstCharAfterMatch == '\0' || firstCharAfterMatch == '/') {
                        // FIXME request->setMatchedPathLength(strlen(matchedPath));
                        handler = prefixMappings.pairAt(resourceIndex).value;
                    }
                }
            }

            if (! handler) {
                throw HttpCode::Http404;
            }

            handler->handleRequest(requestSocket, *httpRequest, this);
        }
        catch (HttpCode status) {
            /*sendToBackground([requestSocket, this, status] () {
                sendResponse(requestSocket, status);
            });*/
        }
        catch (RuntimeError runtimeError) {
            /*sendToBackground([requestSocket, this] () {
                sendResponse(requestSocket, HttpCode::Http500);
            });*/
        }

   }


    void Server::map(AbstractResource::ptr resource, const char* path)
    {
        resourceMappings[path] = resource;
    }
 
    void Server::mapPrefix(AbstractResource::ptr resource, const char* prefix)
    {
        prefixMappings[prefix] = resource;
    }

    void Server::unMap(const char* path)
    {
        if (resourceMappings.find(path) > -1) {
            resourceMappings.take(path);
        }
        
        if (prefixMappings.find(path) > -1) {
            prefixMappings.take(path);
        }
    }

    AbstractResource::ptr Server::mapping(const char* path, bool prefix)
    {
        Map<AbstractResource::ptr>& map = prefix ? prefixMappings : resourceMappings;
        int pos = map.find(path); 
        return pos < 0 ? NULL : map.pairAt(pos).value;
    }

    // Should be run on background threads
    void Server::readRequest(Fd&& requestSocket)
    {
        HttpMessage::ptr req = HttpMessageReader(requestSocket).readRequest();
        runOnMain(&Server::handle, this, std::move(requestSocket), std::move(req));
    }
    
    void sendResponse(int socket, Buffer::ptr buf);
    void sendResponse(int socket, HttpCode status);


}
