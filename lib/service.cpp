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
        buffer(128),
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
        std::cout << "joining\n";
        threads->join_all();
        std::cout << "joined\n";
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
                    while (service->buffer.full()) {
                        service->bufferNotFull.wait(scope_lock);
                    }
                    service->buffer.push_back(requestSocket);
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
            std::cout << "Worker main loop\n";
            bool closeSocket = false;

            {
                boost::mutex::scoped_lock scope_lock(service->bufferLock);
                while (service->buffer.empty()) {
                    service->bufferNotEmpty.wait(scope_lock);
                }
                requestSocket = service->buffer[0];
                service->buffer.pop_front();
            }

            service->bufferNotFull.notify_one();


            std::cout << "Worker got socket\n";
            do {
                try {
                    HttpMessageReader(requestSocket, request).readRequest();

                    if (request.headerValue(Header::connection) != 0 &&
                        strcasecmp("close", request.headerValue(Header::connection)) == 0) {
                        closeSocket = true;
                    }
                    AbstractResource *resource = service->resourceMap.resource(request.path);
                    if (! resource) {
                        throw Status::Http404;
                    }
                    resource->handleRequest(requestSocket, request);
                }
                catch (Status status) {
                    std::cout << "Worker caught status: " << statusLine(status) << "\n";
                    send(requestSocket, statusLine(status), strlen(statusLine(status)), MSG_NOSIGNAL);
                    send(requestSocket, "\r\n", 2, MSG_NOSIGNAL);
                    closeSocket = true;
                }
                catch (int errorNumber) {
                    std::cout << "Worker caught errorNumber: " << errorNumber << "\n";
                    if (errorNumber != 0) {
                        dprintf(2, strerror(errorNumber));
                        writeBacktrace();
                    }
                    closeSocket = true;
                }

            } while(! closeSocket);

            close(requestSocket); // We don't really care about errors here..
        }
    }

}
