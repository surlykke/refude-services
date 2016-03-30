/* 
 * File:   service_listener.h
 * Author: Christian Surlykke <christian@surlykke.dk>
 *
 * Created on 14. februar 2015, 19:10
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
        AbstractResource::ptr mapping(const char* path, bool wildcarded = false);
        size_t mappings();
    protected:
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

