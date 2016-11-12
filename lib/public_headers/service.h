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
#include <condition_variable>
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

        void map(AbstractResource::ptr resource, const char* p1, const char* p2 = 0, const char* p3 = 0);
        void map(AbstractResource::ptr resource, bool wildcarded, const char* p1, const char* p2 = 0, const char* p3 = 0);
        void map(AbstractResource::ptr resource, bool wildcarded, std::vector<const char*> pathElements);
        void unMap(const char* p1, const char* p2 = 0, const char* p3 = 0);
        void unMap(std::vector<const char*> pathElements);
        

        AbstractResource::ptr mapping(const char* path, bool prefix = false);
        Map<AbstractResource::ptr> resourceMappings;
        Map<AbstractResource::ptr> prefixMappings;

        // For debug
        bool dumpRequests;
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

