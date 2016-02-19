/* 
 * File:   service_listener.h
 * Author: Christian Surlykke <christian@surlykke.dk>
 *
 * Created on 14. februar 2015, 19:10
 */

#ifndef SERVICELISTENER_H
#define SERVICELISTENER_H

#include <linux/un.h>
#include <thread>
#include <vector>
#include <map.h>
#include "shortmtqueue.h"

namespace org_restfulipc
{
    class AbstractResource;

    class Service
    {
    public:
        Service();
        virtual ~Service();
        
        void serve(uint16_t portNumber); 
        void serve(const char *socketPath); 
        
        void wait();

        void map(const char* path, org_restfulipc::AbstractResource* resource, bool wildcarded = false);
        void unMap(const char* path);
        AbstractResource* mapping(const char* path, bool wildcarded = false);
        size_t mappings();
    private:
        void startThreads();
        void listener();
        void worker();

        std::vector<std::thread> threads;
        int mNumThreads;
        int listenSocket;
        ShortMtQueue<16> requestSockets;
        Map<AbstractResource*> resourceMappings;
        Map<AbstractResource*> prefixMappings;
        bool shuttingDown;
    };

}



#endif    /* SERVICELISTENER_H */

