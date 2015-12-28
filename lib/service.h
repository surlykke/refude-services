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
#include <condition_variable>

#include "resourcemapping.h"
#include "shortmtqueue.h"

namespace org_restfulipc
{
    class AbstractResource;

    class Service
    {
    public:
        Service(const char *socketPath, int numThreads = 5);
        Service(uint16_t portNumber, int numThreads = 5);
        void run();
        void runInBackground();
        virtual ~Service();

        void map(const char* path, AbstractResource* resource, bool wildcarded = false);
        void unMap(const AbstractResource* resource);

    private:
        void prepareRun();
        void listenForIncoming();
        void serveIncoming();

        std::vector<std::thread> threads;
        int mNumThreads;
        int listenSocket;
        ShortMtQueue<16> requestSockets;
        ResourceMappings mResourceMappings;
        bool shuttingDown;
    };

}



#endif    /* SERVICELISTENER_H */

