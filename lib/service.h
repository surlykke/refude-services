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
#include <shared_mutex>
#include <vector>
#include <condition_variable>

#include "shortmtqueue.h"

namespace org_restfulipc
{
    class ResourceMapping;
    class AbstractResource;

    class Service
    {
    public:
        Service(const char *socketPath, int workers = 5);
        Service(uint16_t portNumber, int workers = 5);
        virtual ~Service();

        void map(const char* path, AbstractResource* findMapping, bool wildcarded = false);
        void unMap(const AbstractResource* findMapping);

    private:
        void listenForIncoming();
        void serveIncoming();

        std::vector<std::thread> threads;
        ShortMtQueue<16> requestSockets;
        std::mutex bufferLock;
        std::condition_variable bufferNotFull;
        std::condition_variable bufferNotEmpty;
        bool shuttingDown;
        int listenSocket;

        ResourceMapping* findMapping(const char* path);
        std::shared_timed_mutex mMappingsMutex;
        ResourceMapping* mMappings;
        int mNumMappings;
        int mMappingsCapacity;
    };

}



#endif    /* SERVICELISTENER_H */

