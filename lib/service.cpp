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


    Service::Service() :
        threads(),
        mNumThreads(5),
        listenSocket(-1),
        requestSockets(),
        resourceMappings(),
        prefixMappings(),
        shuttingDown(false)
    {
    }

    Service::~Service()
    {
        shuttingDown = true;
        wait(); 
        close(listenSocket);

    }

    void Service::serve(uint16_t portNumber) 
    {
        struct sockaddr_in sockaddr;
        memset(&sockaddr, 0, sizeof(struct sockaddr_in));
        sockaddr.sin_family = AF_INET;
        sockaddr.sin_port = htons(portNumber);
        sockaddr.sin_addr.s_addr = INADDR_ANY;
        if ((listenSocket = socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC | SOCK_NONBLOCK, 0)) < 0) throw C_Error();
        if (bind(listenSocket, (struct sockaddr*)(&sockaddr), sizeof(sockaddr)) < 0) throw C_Error();
    
        startThreads();
    }


    void Service::serve(const char* socketPath) 
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
        if (bind(listenSocket, (struct sockaddr*)(&sockaddr), sizeof(sa_family_t) + strlen(socketPath) + 1) < 0) {
            throw C_Error();
        }
   
        startThreads();
    }

    void Service::wait() {
        for (int i = 0; i < threads.size(); i++) {
            threads.at(i).join();
        }
    }
    
    void Service::map(const char* path, AbstractResource* resource, bool wildcarded)
    {
        resourceMappings.add(path, std::move(resource));
    }

    void Service::unMap(const char* path)
    {
        resourceMappings.take(path);
    }

    size_t Service::mappings()
    {
        return resourceMappings.size();
    }
   

    AbstractResource* Service::mapping(const char* path, bool wildcarded)
    {
        int pos = resourceMappings.find(path); 
        if (pos < 0) {
            return NULL;
        }
        else {
            return resourceMappings.at(pos).value;
        }
    }


    void Service::startThreads()
    {
        if (listen(listenSocket, 8) < 0) throw C_Error();
        for (int i = 0; i < mNumThreads; i++) {
            threads.push_back(std::thread(&Service::worker, this));
        }
        threads.push_back(std::thread(&Service::listener, this));
    }


    void Service::listener()
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

                struct timeval tv;
                tv.tv_sec = 0;  
                tv.tv_usec = 200000;  
                setsockopt(requestSocket, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv , sizeof(struct timeval));


                requestSockets.enqueue(requestSocket);
            }
        }

    }

    void Service::worker()
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
                    int resourceIndex = resourceMappings.find(request.path);
                    if (resourceIndex < 0) {
                        throw Status::Http404;
                    }
                    AbstractResource *resource = resourceMappings.at(resourceIndex).value;
                    request.remainingPath = ""; // FIXME
                    resource->handleRequest(requestSocket, request);
                    const char* connectHeader = request.headerValue(Header::connection);
                    if (!connectHeader) connectHeader = "(null)"; 
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
