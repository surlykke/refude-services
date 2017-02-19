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
#include <sys/time.h>
#include <unistd.h>
#include <poll.h>
#include <mutex>
#include <limits.h>

#include "comm.h"
#include "httpmessage.h"
#include "httpmessagereader.h"
#include "abstractresource.h"
#include "notifierresource.h"
#include "socketqueue.h"
#include "resourcecollection.h"

#include "service.h"

namespace refude
{
    namespace service
    {

        bool dumpRequests;
        std::vector<std::thread> listenThreads;
        std::vector<std::thread> workerThreads;
        SocketQueue socketQueue;

        bool shuttingDown = false;

        std::shared_mutex mutex;

        void listenOnSocket(Descriptor&& socket)
        {
            struct pollfd pollfd;
            pollfd.fd = socket;
            pollfd.events = POLLIN;

            for (;;) {
                int pollRes = poll(&pollfd, 1, -1);
                if (pollRes < 0) {
                    throw C_Error();
                }

                if (shuttingDown) {
                    break;
                }

                if (pollRes > 0) {
                    struct timeval starttime;
                    gettimeofday(&starttime, NULL);
                    std::cout << "Incoming:    " << (1000000*starttime.tv_sec + starttime.tv_usec) << "\n";
                    int requestSocket = accept4(socket, NULL, NULL, SOCK_CLOEXEC);
                    if (requestSocket < 0) {
                        continue;
                    }

                    struct timeval tv;
                    tv.tv_sec = 0;
                    tv.tv_usec = 200000;
                    setsockopt(requestSocket, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv , sizeof(struct timeval));
                    socketQueue.push(requestSocket);
                }
            }
        }

        void shutDown()
        {
            shuttingDown = true;
            for (std::thread& t : listenThreads) {
                t.join();
            }
            for (std::thread& t : workerThreads) {
                t.join();
            }
        }

        void listen(uint16_t portNumber)
        {
            struct sockaddr_in sockaddr;
            memset(&sockaddr, 0, sizeof(struct sockaddr_in));
            sockaddr.sin_family = AF_INET;
            sockaddr.sin_port = htons(portNumber);
            sockaddr.sin_addr.s_addr = INADDR_ANY;
            Descriptor listenSocket = socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC | SOCK_NONBLOCK, 0);
            if (listenSocket < 0) throw C_Error();
            if (bind(listenSocket, (struct sockaddr*)(&sockaddr), sizeof(sockaddr)) < 0) throw C_Error();
            if (::listen(listenSocket, 8) < 0) throw C_Error();

            listenThreads.push_back(std::thread(&listenOnSocket, std::move(listenSocket)));
        }

        void listen(std::string socketPath)
        {
            struct sockaddr_un sockaddr;
            memset(&sockaddr, 0, sizeof(struct sockaddr_un));
            sockaddr.sun_family = AF_UNIX;
            strncpy(&sockaddr.sun_path[0], socketPath.data(), socketPath.size());
            Descriptor listenSocket = socket(AF_UNIX, SOCK_STREAM | SOCK_CLOEXEC | SOCK_NONBLOCK, 0);
            if (listenSocket < 0) throw C_Error();
            unlink(socketPath.data());
            if (bind(listenSocket, (struct sockaddr*)(&sockaddr), sizeof(sa_family_t) + socketPath.size() + 1) < 0) {
                throw C_Error();
            }
            if (::listen(listenSocket, 8) < 0) throw C_Error();

            listenThreads.push_back(std::thread(&listenOnSocket, std::move(listenSocket)));
        }


        void readRequest(Descriptor&& requestSocket)
        {
            try {

                HttpRequest request;
                request.addTimestamp("Start readRequest");
                request.read(requestSocket);

                {
                    std::shared_lock<std::shared_mutex> readLock(ResourceCollection::resourceMutex);
                    AbstractResource* handler;
                    int matchedLength;
                    if (ResourceCollection::getResource(handler, matchedLength, request.path)) {
                        handler->handleRequest(requestSocket, request, request.path + matchedLength);
                    }
                }

                request.addTimestamp("request handled");
                request.printoutTimestamps();
            }
            catch (HttpCode status) {
                sendStatus(requestSocket, status);
            }
            catch (RuntimeError re) {
                try {
                    sendStatus(requestSocket, HttpCode::Http500);
                }
                catch(RuntimeError re2) {
                }
            }
        }

        void work() {
            for(;;) {
                int socket = socketQueue.pop();
                if (socket < 0) return;
                readRequest(Descriptor(socket));
            }
        }

        void run(int numberOfWorkers)
        {
            for (; numberOfWorkers > 0; numberOfWorkers--) {
                workerThreads.push_back(std::thread(&work));
            }
        }

        void runAndWait(int numberOfWorkers)
        {
            run(numberOfWorkers);
            for (std::thread& t : workerThreads) {
                t.join();
            }
        }

    }
}


