/* 
 * File:   service_listener.cpp
 * Author: Christian Surlykke <christian@surlykke.dk>
 * 
 * Created on 14. februar 2015, 19:10
 */

#include <poll.h>
#include <sys/socket.h>

#include <string.h>
#include <stdio.h>
#include <error.h>
#include <errno.h>
#include <unistd.h>
#include <iostream>

#include "service.h"
#include "errorhandling.h"

namespace org_restfulipc
{
    Service::Service(const char *socketPath, int workers) :
        threads(),
        requestSockets(),
        bufferLock(),
        bufferNotFull(),
        bufferNotEmpty(),
        shuttingDown(false),
        listenSocket(-1)
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
    }

    Service::~Service()
    {
        shuttingDown = true;
        for (int i = 0; i < threads.size(); i++) {
            threads.at(i).join();
        }

        close(listenSocket);
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

                /*struct timeval tv;
                tv.tv_sec = 0;
                tv.tv_usec = 200000;
                if (setsockopt(requestSocket, SOL_SOCKET, SO_RCVTIMEO, (struct timeval*) &tv, sizeof(struct timeval)) < 0) {
                    close(requestSocket);
                    continue;
                }*/

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

                    AbstractResource *resource = resourceMap.resource(request.path);
                    if (! resource) {
                        throw Status::Http404;
                    }

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
