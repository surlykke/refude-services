/* 
 * File:   service_listener.cpp
 * Author: Christian Surlykke <christian@surlykke.dk>
 * 
 * Created on 14. februar 2015, 19:10
 */

#include <sys/types.h>
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
    Service::Service(const char *socketPath) :
        threads(0),
        requestSockets(128),
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
        assert((listenSocket = socket(AF_UNIX, SOCK_STREAM, 0)) >= 0);
        unlink(socketPath);
        assert(bind(listenSocket, (struct sockaddr*)(&sockaddr), sizeof(sa_family_t) + strlen(socketPath) + 1) >= 0);
        assert(listen(listenSocket, 8) >= 0);
    }

    Service::~Service()
    {
    }

    void Service::start(int workers)
    {
        if (threads) {
            std::cerr << "Already started..";
            return;
        }

        shuttingDown = false;
        threads = new boost::thread_group();
        threads->create_thread(Listener(this));
        for (int i = 0; i < workers; i++) {
            threads->create_thread(Worker(this));
        }
    }

    void Service::stop()
    {
        shuttingDown = true;
        bufferNotEmpty.notify_all();
        bufferNotFull.notify_all();
        std::cout << "join_all\n";
        threads->join_all();
        std::cout << "all joined\n";
        delete threads;
        threads = 0;
    }



    void Service::Listener::operator ()()
    {
        while (! service->shuttingDown) {
            int requestSocket = accept(service->listenSocket, NULL, 0);
            if (requestSocket >= 0) {
                struct timeval tv;
                tv.tv_sec = 0;
                tv.tv_usec = 200000;
                if (setsockopt(requestSocket, SOL_SOCKET, SO_RCVTIMEO, (struct timeval*) &tv, sizeof(struct timeval)) < 0) {
                    close(requestSocket);
                    continue;
                }

                {
                    boost::mutex::scoped_lock scope_lock(service->bufferLock);
                    while (service->requestSockets.full()) {
                        service->bufferNotFull.wait(scope_lock);
                        if (service->shuttingDown) {
                            return;
                        }
                    }
                    service->requestSockets.push_back(requestSocket);
                }

                service->bufferNotEmpty.notify_one();

            }
        }
    }

    void Service::Worker::operator ()()
    {
        int requestSocket;
        HttpMessage request;
        while (! service->shuttingDown) {
            {
                boost::mutex::scoped_lock scope_lock(service->bufferLock);
                while (service->requestSockets.empty()) {
                    service->bufferNotEmpty.wait(scope_lock);
                    if (service->shuttingDown) {
                        return;
                    }
                }
                requestSocket = service->requestSockets[0];
                service->requestSockets.pop_front();
            }

            service->bufferNotFull.notify_one();

            while (requestSocket > -1) {
                try {
                    HttpMessageReader(requestSocket, request).readRequest();

                    AbstractResource *resource = service->resourceMap.resource(request.path);
                    if (! resource) {
                        throw Status::Http404;
                    }

                    resource->handleRequest(requestSocket, request);

                    if (request.headerValue(Header::connection) != 0 &&
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
        }
    }

}
