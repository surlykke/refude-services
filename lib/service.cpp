/* 
 * File:   service_listener.cpp
 * Author: Christian Surlykke <christian@surlykke.dk>
 * 
 * Created on 14. februar 2015, 19:10
 */

#include <poll.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <string.h>
#include <stdio.h>
#include <error.h>
#include <errno.h>
#include <unistd.h>
#include <iostream>

#include "httpmessage.h"
#include "abstractresource.h"
#include "service.h"
#include "errorhandling.h"

namespace org_restfulipc
{

    struct ResourceMapping
    {
        ResourceMapping(const char* path, AbstractResource* resource, bool wildcarded = false):
            path(path),
            pathLength(strlen(path)),
            resource(resource),
            wildcarded(wildcarded)
        {
        }

        bool matches(const char* path)
        {
            const char* p = this->path;
            while (*p) {
                if (*p++ != *path++) {
                    return NULL;
                }
            }

            return *path == '\0' || (wildcarded && *path == '/');
        }

        const char* path;
        int pathLength;
        AbstractResource *resource;
        bool wildcarded;
    };



    Service::Service(const char *socketPath, int workers) :
        threads(),
        requestSockets(),
        bufferLock(),
        bufferNotFull(),
        bufferNotEmpty(),
        shuttingDown(false),
        listenSocket(-1),
        mMappingsMutex(),
        mMappings(0),
        mNumMappings(0),
        mMappingsCapacity(0)
    {
        if (strlen(socketPath) >= UNIX_PATH_MAX - 1)
        {
            throw std::runtime_error("Socket path too long");
        }

        struct sockaddr_un sockaddr;
        memset(&sockaddr, 0, sizeof(struct sockaddr_un));
        sockaddr.sun_family = AF_UNIX;
        strncpy(&sockaddr.sun_path[0], socketPath, strlen(socketPath));
        if ((listenSocket = socket(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK, 0)) < 0) throw C_Error();
        unlink(socketPath);
        if (bind(listenSocket, (struct sockaddr*)(&sockaddr), sizeof(sa_family_t) + strlen(socketPath) + 1) < 0) throw C_Error();
        if (listen(listenSocket, 8) < 0) throw C_Error();

        threads.push_back(std::thread(&Service::listenForIncoming, this));
        for (int i = 0; i < workers; i++) {
            threads.push_back(std::thread(&Service::serveIncoming, this));
        }

        mMappings = (ResourceMapping*) calloc(8, sizeof(ResourceMapping));
        if (!mMappings) {
            throw C_Error();
        }
        mMappingsCapacity = 8;
    }

    Service::Service(uint16_t portNumber, int workers) :
         threads(),
         requestSockets(),
         bufferLock(),
         bufferNotFull(),
         bufferNotEmpty(),
         shuttingDown(false),
         listenSocket(-1),
         mMappingsMutex(),
         mMappings(0),
         mNumMappings(0),
         mMappingsCapacity(0)
  {
        struct sockaddr_in sockaddr;
        memset(&sockaddr, 0, sizeof(struct sockaddr_un));
        sockaddr.sin_family = AF_INET;
        sockaddr.sin_port = htons(portNumber);
        sockaddr.sin_addr.s_addr = INADDR_ANY;
        if ((listenSocket = socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC | SOCK_NONBLOCK, 0)) < 0) throw C_Error();
        if (bind(listenSocket, (struct sockaddr*)(&sockaddr), sizeof(sockaddr)) < 0) throw C_Error();
        if (listen(listenSocket, 8) < 0) throw C_Error();

        threads.push_back(std::thread(&Service::listenForIncoming, this));
        for (int i = 0; i < workers; i++) {
            threads.push_back(std::thread(&Service::serveIncoming, this));
        }

        mMappings = (ResourceMapping*) calloc(8, sizeof(ResourceMapping));
        if (!mMappings) {
            throw C_Error();
        }
        mMappingsCapacity = 8;
   }

    Service::~Service()
    {
        shuttingDown = true;
        for (int i = 0; i < threads.size(); i++) {
            threads.at(i).join();
        }

        close(listenSocket);
        delete mMappings;
    }


    void Service::map(const char* path, AbstractResource* resource, bool wildcarded)
    {
        std::unique_lock<std::shared_timed_mutex> lock(mMappingsMutex);
        if (mMappingsCapacity <= mNumMappings) {
            mMappings = (ResourceMapping*) realloc(mMappings, 2*mMappingsCapacity*sizeof(ResourceMapping));
            if (!mMappings) throw C_Error();
            mMappingsCapacity = 2*mMappingsCapacity;
        }


        mMappings[mNumMappings++] = ResourceMapping(path, resource, wildcarded);
    }

    void Service::unMap(const AbstractResource* resource)
    {
        std::unique_lock<std::shared_timed_mutex> lock(mMappingsMutex);
        int numRemoved = 0;
        for (int i = 0; i < mNumMappings; i++) {
            if (mMappings[i].resource == resource) {
                numRemoved++;
            }

            mMappings[i - numRemoved] = mMappings[i];
        }
        mNumMappings -= numRemoved;
    }

    ResourceMapping* Service::findMapping(const char* path)
    {
        std::shared_lock<std::shared_timed_mutex> lock(mMappingsMutex);
        for (int i = 0; i < mNumMappings; i++) {
            if (mMappings[i].matches(path)) {
                return mMappings + i;
            }
        }

        return NULL;
    }


    void Service::listenForIncoming()
    {
        struct pollfd pollfd;
        pollfd.fd = listenSocket;
        pollfd.events = POLLIN;

        for (;;) {
            int pollRes = poll(&pollfd, 1, 250);
            if (shuttingDown) {
                for (int i = 1; i < threads.size(); i++) {
                    requestSockets.enqueue(-1); // Tell workers to quit
                }

                return;
            }

            if (pollRes > 0) {
                int requestSocket = accept(listenSocket, NULL, 0);
                if (requestSocket < 0) {
                    continue;
                }

                requestSockets.enqueue(requestSocket);
            }
        }

    }

    void Service::serveIncoming()
    {
        int requestSocket;
        HttpMessage request;

        for (;;) {
            requestSocket = requestSockets.dequeue();

            if (requestSocket < 0) {
                return;
            }

            do {
                try {
                    HttpMessageReader(requestSocket, request).readRequest();

                    ResourceMapping* mapping = findMapping(request.path);
                    if (! mapping) {
                        throw Status::Http404;
                    }
                    request.remainingPath = request.path + mapping->pathLength;
                    mapping->resource->handleRequest(requestSocket, request);

                    if (requestSocket > -1 &&
                        request.headerValue(Header::connection) != 0 &&
                        strcasecmp("close", request.headerValue(Header::connection)) == 0) {
                        close(requestSocket);
                        requestSocket = -1;
                    }
                }
                catch (Status status) {
                    send(requestSocket, statusLine(status), strlen(statusLine(status)), MSG_NOSIGNAL);
                    send(requestSocket, "\r\n", 2, MSG_NOSIGNAL);
                    close(requestSocket);
                    requestSocket = -1;
                }
                catch (int errorNumber) {
                    close(requestSocket);
                    requestSocket = -1;
                }
            }
            while (requestSocket > -1);
        }
    }
}
