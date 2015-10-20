/* 
 * File:   service_listener.h
 * Author: Christian Surlykke <christian@surlykke.dk>
 *
 * Created on 14. februar 2015, 19:10
 */

#ifndef SERVICELISTENER_H
#define    SERVICELISTENER_H

#include <pthread.h>
#include <linux/un.h>
#include <boost/lockfree/queue.hpp>
#include <boost/thread/thread.hpp>
#include <boost/atomic.hpp>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "shortmtqueue.h"
#include "resourcemap.h"

namespace org_restfulipc
{
    class Service
    {
    public:
        Service(const char *socketPath, int workers = 5);
        virtual ~Service();


        ResourceMap resourceMap;

    private:
        std::vector<std::thread> threads;
        ShortMtQueue<16> requestSockets;
        std::mutex bufferLock;
        std::condition_variable bufferNotFull;
        std::condition_variable bufferNotEmpty;
        bool shuttingDown;
        int listenSocket;

        void listenForIncoming();
        void serveIncoming();
    };

}



#endif    /* SERVICELISTENER_H */

