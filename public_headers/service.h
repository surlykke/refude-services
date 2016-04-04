/*
* Copyright (c) 2015, 2016 Christian Surlykke
*
* This file is part of the Restful Inter Process Communication (Ripc) project. 
* It is distributed under the LGPL 2.1 license.
* Please refer to the LICENSE file for a copy of the license.
*/

#ifndef SERVICELISTENER_H
#define SERVICELISTENER_H

#include <thread>
#include <vector>
#include "map.h"
#include "abstractresource.h"

namespace org_restfulipc
{
    class AbstractResource;
    class ThreadSafeQueue;

    class Service
    {
    public:
        Service();
        virtual ~Service();
        
        void serve(uint16_t portNumber); 
        void serve(const char *socketPath); 
        
        void wait();

        void map(const char* path, AbstractResource::ptr resource, bool wildcarded = false);
        void unMap(const char* path);
        AbstractResource::ptr mapping(const char* path, bool prefix = false);
        Map<AbstractResource::ptr> resourceMappings;
        Map<AbstractResource::ptr> prefixMappings;
 
    private:
        void startThreads();
        void listener();
        void worker();

        std::vector<std::thread> threads;
        int mNumThreads;
        int listenSocket;
        ThreadSafeQueue* requestSockets;
        bool shuttingDown;
    };

}



#endif    /* SERVICELISTENER_H */

