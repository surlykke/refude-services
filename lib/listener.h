/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the Refude Services project, library
 * It is distributed under the LGPL 2.1 license.
 * Please refer to the LGPL21 file for a copy of the license.
 */

#ifndef LISTENER_H
#define LISTENER_H

#include <stdint.h>
#include <vector>
#include <thread>
#include <tuple>

#include "queue.h"
#include "fd.h"
#include "app.h"

namespace refude
{

    class Server;
    class Listener
    {
    public:
        Listener(uint16_t portNumber, Server* server);
        Listener(const char* socketPath, Server* server);
        void shutDown();
        bool dumpRequests;

    private:
        void run(int listenSocket, Server* server);
        std::thread listenThread;
    };


}



#endif

