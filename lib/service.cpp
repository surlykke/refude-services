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


    Service::Service(const char *socketPath, int numThreads) :
        threads(),
        mNumThreads(numThreads),
        listenSocket(-1),
        requestSockets(),
        resourceMappings(),
        prefixMappings(),
        shuttingDown(false)
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

    }

    Service::Service(uint16_t portNumber, int numThreads) :
        threads(),
        mNumThreads(numThreads),
        listenSocket(-1),
        requestSockets(),
        resourceMappings(),
        prefixMappings(),
        shuttingDown(false)
    {
        std::cout << "Into Service constructor\n";
        struct sockaddr_in sockaddr;
        memset(&sockaddr, 0, sizeof(struct sockaddr_in));
        sockaddr.sin_family = AF_INET;
        sockaddr.sin_port = htons(portNumber);
        sockaddr.sin_addr.s_addr = INADDR_ANY;
        if ((listenSocket = socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC | SOCK_NONBLOCK, 0)) < 0) throw C_Error();
        if (bind(listenSocket, (struct sockaddr*)(&sockaddr), sizeof(sockaddr)) < 0) throw C_Error();
        std::cout << "Service constructor done\n";
    }

    void Service::run()
    {
        if (listen(listenSocket, 8) < 0) throw C_Error();
        for (int i = 0; i < mNumThreads; i++) {
            threads.push_back(std::thread(&Service::serveIncoming, this));
        }
        Service::listenForIncoming();
    }

    void Service::runInBackground()
    {
        if (listen(listenSocket, 8) < 0) throw C_Error();
        for (int i = 0; i < mNumThreads; i++) {
            threads.push_back(std::thread(&Service::serveIncoming, this));
        }
        threads.push_back(std::thread(&Service::listenForIncoming, this));
    }


    Service::~Service()
    {
        std::cout << "Service destructor\n";
        shuttingDown = true;
        /*for (int i = 0; i < threads.size(); i++) {
            threads.at(i).join();
        }

        close(listenSocket);*/

    }

    void Service::map(const char* path, AbstractResource* resource, bool wildcarded)
    {
        resourceMappings.add(path, std::move(resource));
    }

    void Service::unMap(const char* path)
    {
        resourceMappings.take(path);
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
                    int resourceIndex = resourceMappings.find(request.path);
                    if (resourceIndex < 0) {
                        throw Status::Http404;
                    }
                    AbstractResource *resource = resourceMappings.at(resourceIndex).value;
                    request.remainingPath = ""; // FIXME
                    resource->handleRequest(requestSocket, request);

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
