/*
* Copyright (c) 2015, 2016 Christian Surlykke
*
* This file is part of the Restful Inter Process Communication (Ripc) project. 
* It is distributed under the LGPL 2.1 license.
* Please refer to the LICENSE file for a copy of the license.
*/


#include <sys/un.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <poll.h>
#include <mutex>
#include <condition_variable>

#include "httpmessage.h"
#include "abstractresource.h"
#include "service.h"

namespace org_restfulipc
{

    struct ThreadSafeQueue
    {

        ThreadSafeQueue() : elements(), count(0), m() { }

        void enqueue(int s) {
           {
                unique_lock<mutex> lock(m);

                while (count >= 16) {
                    notFull.wait(lock);
                }

                for (int i = count; i > 0; i--) {
                    elements[i] = elements[i - 1];
                }

                elements[0] = s;
                count++;
            }

            notEmpty.notify_one();
        }

        int dequeue() {
            int result;
            {
                unique_lock<mutex> lock(m);

                while (count <= 0) {
                    notEmpty.wait(lock);
                }

                result = elements[--count];
            }
            notFull.notify_one();

            return result;
        }

        int elements[16];
        int count;

        mutex m;
        condition_variable notFull;
        condition_variable notEmpty;
    };

    Service::Service() :
        threads(),
        mNumThreads(5),
        listenSocket(-1),
        requestSockets(new ThreadSafeQueue()),
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
    
    void Service::map(const char* path, AbstractResource::ptr resource, bool wildcarded)
    {
        if (wildcarded) {
            prefixMappings.add(path, resource);
        }
        else {
            resourceMappings.add(path, resource);
        }
    }

    void Service::unMap(const char* path)
    {
        if (resourceMappings.find(path)) {
            resourceMappings.take(path);
        }
        
        if (prefixMappings.find(path)) {
            prefixMappings.take(path);
        }
    }

    AbstractResource::ptr Service::mapping(const char* path, bool prefix)
    {
        Map<AbstractResource::ptr>& map = prefix ? prefixMappings : resourceMappings;
        int pos = map.find(path); 
        return pos < 0 ? NULL : map.valueAt(pos);
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
                    requestSockets->enqueue(-1); // Tell workers to quit
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
                requestSockets->enqueue(requestSocket);
            }
        }
    }

    void Service::worker()
    {
        int requestSocket;
        HttpMessage request;

        for (;;) {
            requestSocket = requestSockets->dequeue();
            if (requestSocket < 0) {
                return;
            }

            do {
                try {
                    HttpMessageReader reader(requestSocket, request);
                    //reader.dumpRequest = true;
                    reader.readRequest();
                    AbstractResource::ptr handler; 
                    uint matchedPathLength;
                    int resourceIndex = resourceMappings.find(request.path);
                    if (resourceIndex > -1) {
                        handler = resourceMappings.valueAt(resourceIndex);
                        matchedPathLength = strlen(request.path);
                    }
                    else { 
                        resourceIndex = prefixMappings.find_longest_prefix(request.path);
                        if (resourceIndex >= 0) {
                            const char* matchedPath = prefixMappings.keyAt(resourceIndex); 
                            matchedPathLength = strlen(matchedPath);
                            if (request.path[matchedPathLength] == '\0' || request.path[matchedPathLength] == '/') {
                                handler = prefixMappings.valueAt(resourceIndex);
                            }
                        }
                    }

                    if (! handler) {
                        throw Status::Http404;
                    }
                    
                    
                    handler->handleRequest(requestSocket, matchedPathLength, request);

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
                catch (C_Error c_Error) {
                    if (c_Error.errorNumber) { // We can get here with errorNumber == 0 if it's 
                                               // a 'benign' error - i.e. connection timed out
                       std::cerr << "Worker caught RuntimeError: " << c_Error.what() << "\n";
                        c_Error.printStackTrace();
                    }
                    else {
                    }
                    close(requestSocket);
                    requestSocket = -1;
                }
                catch (RuntimeError runtimeError) {
                    close(requestSocket);
                    requestSocket = -1;
                    std::cerr << "Worker caught RuntimeError: " << runtimeError.what() << "\n";
                    runtimeError.printStackTrace();
                }
            }
            while (requestSocket > -1);
        }
    }
}
