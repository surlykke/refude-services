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
#include <mutex>
#include <limits.h>

#include "httpmessage.h"
#include "abstractresource.h"
#include "service.h"

namespace refude
{

    struct ThreadSafeQueue
    {

        ThreadSafeQueue() : elements(), count(0), m() { }

        void enqueue(int s) {
           {
                std::unique_lock<std::mutex> lock(m);

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
                std::unique_lock<std::mutex> lock(m);

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

        std::mutex m;
        std::condition_variable notFull;
        std::condition_variable notEmpty;
    };

    Service::Service() :
        dumpRequests(false),
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
        if ((listenSocket = socket(AF_UNIX, SOCK_STREAM | SOCK_CLOEXEC | SOCK_NONBLOCK, 0)) < 0) throw C_Error();
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

    void Service::map(AbstractResource::ptr resource, const char* p1, const char* p2, const char* p3)
    {
        map(resource, false, p1, p2, p3);
    }

    void Service::map(AbstractResource::ptr resource, bool wildcarded, const char* p1, const char* p2, const char* p3)
    {
        std::vector<const char*> pathElements;
        for (const char*  p : {p1, p2, p3}) {
            if (p) pathElements.push_back(p);
        }
        
        map(resource, wildcarded, pathElements);
    }

    void Service::map(AbstractResource::ptr resource, bool wildcarded, std::vector<const char*> pathElements)
    {
        char path[PATH_MAX];
        int pos = 0;
        for (const char* p : pathElements) {
            pos += snprintf(path + pos, PATH_MAX - pos, "/%s", p);
            if (pos >= PATH_MAX - 1) {
                throw RuntimeError("Path too long");
            }
        }
    
        std::cout << "Mapping " << path;
        if (wildcarded) {
            std::cout << "\n";
            prefixMappings.add(path, resource);
        }
        else {
            std::cout << "\n";
            resourceMappings.add(path, resource);
        }

    }
 

   void Service::unMap(const char* p1, const char* p2, const char* p3)
    {
        std::vector<const char*> pathElements;
        for (const char* p : {p1, p2, p3}) if (p) pathElements.push_back(p);
        unMap(pathElements);
    }

    void Service::unMap(std::vector<const char*> pathElements)
    {
        char path[PATH_MAX];
        int pos = 0;
        for (const char* p: pathElements) {
            pos += snprintf(path + pos, PATH_MAX - pos, "/%s", p);
            if (pos >= PATH_MAX - 1) {
                throw RuntimeError("Path too long");
            }
        }
        if (resourceMappings.find(path) > -1) {
            resourceMappings.take(path);
        }
        
        if (prefixMappings.find(path) > -1) {
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
                int requestSocket = accept4(listenSocket, NULL, NULL, SOCK_CLOEXEC);
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
                    reader.dumpRequest = dumpRequests;
                    reader.readRequest();
                    AbstractResource::ptr handler; 
                    int resourceIndex = resourceMappings.find(request.path);
                    if (resourceIndex > -1) {
                        handler = resourceMappings.valueAt(resourceIndex);
                        request.setMatchedPathLength(strlen(resourceMappings.keyAt(resourceIndex)));
                    }
                    else { 
                        resourceIndex = prefixMappings.find_longest_prefix(request.path);
                        if (resourceIndex >= 0) {
                            const char* matchedPath = prefixMappings.keyAt(resourceIndex); 
                            const char firstCharAfterMatch = request.path[strlen(matchedPath)];
                            if ( firstCharAfterMatch == '\0' || firstCharAfterMatch == '/') {
                                request.setMatchedPathLength(strlen(matchedPath));
                                handler = prefixMappings.valueAt(resourceIndex);
                            }
                        }
                    }

                    if (! handler) {
                        throw HttpCode::Http404;
                    }
                    
                    handler->handleRequest(requestSocket, request);

                    if (requestSocket > -1 &&
                        request.header(Header::connection) != 0 &&
                        strcasecmp("close", request.header(Header::connection)) == 0) {
                        close(requestSocket);
                        requestSocket = -1;
                    }
                }
                catch (HttpCode status) {
                    send(requestSocket, statusLine(status), strlen(statusLine(status)), MSG_NOSIGNAL);
                    send(requestSocket, "\r\n", 2, MSG_NOSIGNAL);
                    close(requestSocket);
                    requestSocket = -1;
                }
                catch (C_Error c_Error) {
                    if (c_Error.errorNumber != 0 &&
                        c_Error.errorNumber != EAGAIN &&
                        c_Error.errorNumber != EWOULDBLOCK) {
                       std::cerr << "Worker caught RuntimeError: " << c_Error.what() << "\n";
                        c_Error.printStackTrace();
                        std::cerr << "\n";
                    }
                    else {
                        // We can get here 'benign' error - i.e. connection timed out, peer closed 
                        // or some such
                    }
                    close(requestSocket);
                    requestSocket = -1;
                }
                catch (RuntimeError runtimeError) {
                    close(requestSocket);
                    requestSocket = -1;
                    std::cerr << "Worker caught RuntimeError: " << runtimeError.what() << "\n";
                    runtimeError.printStackTrace();
                    std::cerr << "\n";
                }
            }
            while (requestSocket > -1);
        }
    }
}
