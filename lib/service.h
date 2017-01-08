/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the Refude Services project, library
 * It is distributed under the LGPL 2.1 license.
 * Please refer to the LGPL21 file for a copy of the license.
 */

#ifndef SERVICELISTENER_H
#define SERVICELISTENER_H

#include <thread>
#include <condition_variable>
#include <vector>
#include "map.h"
#include "abstractresource.h"

namespace refude
{
    struct AbstractResource;
    class ThreadSafeQueue;

    class Service
    {
    public:
        Service();
        virtual ~Service();
        
        void serve(uint16_t portNumber); 
        void serve(const char *socketPath); 
        
        void wait();

        void map(AbstractResource::ptr&& resource, const char* path);
        void map(AbstractResource::ptr&& resource, std::string path);
        void mapByPrefix(AbstractResource::ptr&& resource, const char* path);
        void mapByPrefix(AbstractResource::ptr&& resource, std::string path);
        void unMap(const char* path);
        

        AbstractResource* mapping(const char* path);
        AbstractResource* prefixMapping(const char* path);

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

