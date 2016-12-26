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
#include <utility>
#include <memory>
#include "request.h"
#include "httpmessage.h"
#include "httpmessagereader.h"
#include "server.h"
#include "app.h"
#include "fd.h"
#include "listener.h"

namespace refude
{

    Listener::Listener(uint16_t portNumber, Server* server) :
        listenThread()
    {
        struct sockaddr_in sockaddr;
        memset(&sockaddr, 0, sizeof(struct sockaddr_in));
        sockaddr.sin_family = AF_INET;
        sockaddr.sin_port = htons(portNumber);
        sockaddr.sin_addr.s_addr = INADDR_ANY;
        int listenSocket = socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC | SOCK_NONBLOCK, 0);
        if (listenSocket < 0) throw C_Error();
        if (bind(listenSocket, (struct sockaddr*)(&sockaddr), sizeof(sockaddr)) < 0) throw C_Error();
        listenThread = std::thread(&Listener::run, this, listenSocket, server);
    }

    Listener::Listener(const char* socketPath, Server* server) :
        listenThread()
    {
        struct sockaddr_un sockaddr;
        memset(&sockaddr, 0, sizeof(struct sockaddr_un));
        sockaddr.sun_family = AF_UNIX;
        int listenSocket = socket(AF_UNIX, SOCK_STREAM | SOCK_CLOEXEC, 0);
        if (listenSocket < 0) throw strerror(errno);

        strncpy(&sockaddr.sun_path[0], socketPath, strlen(socketPath));
        unlink(socketPath);
        if (bind(listenSocket, (struct sockaddr*)(&sockaddr), sizeof(sa_family_t) + strlen(socketPath) + 1) < 0) throw strerror(errno);

        if (::listen(listenSocket, 8) < 0) throw strerror(errno);

        listenThread = std::thread(&Listener::run, this, listenSocket, server);
    }

    void Listener::shutDown()
    {
        // FIXME
    }

    void Listener::run(int listenSocket, Server* server)
    {
        for (;;) {
            Fd requestSocket = accept4(listenSocket, NULL, NULL, SOCK_CLOEXEC);
            if (requestSocket < 0) {
                std::cerr << "accept4 returned: " << strerror(errno) << "\n";
                return;
            }

            struct timeval tv;
            tv.tv_sec = 0;
            tv.tv_usec = 200000;
            setsockopt(requestSocket, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv , sizeof(struct timeval));

            runInBackground(&Server::readRequest, server, std::move(requestSocket));
        }
    }
}
