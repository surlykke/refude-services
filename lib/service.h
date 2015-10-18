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
#include <boost/circular_buffer.hpp>

#include "resourcemap.h"

namespace org_restfulipc
{
    class Service
    {
    public:
        Service(const char *socketPath);
        virtual ~Service();

        void start(int workers = 5);
        void stop();

        ResourceMap resourceMap;

    private:
        boost::thread_group *threads;
        boost::circular_buffer<int> requestSockets;
        boost::mutex bufferLock;
        boost::condition_variable bufferNotFull;
        boost::condition_variable bufferNotEmpty;
        boost::atomic<bool> shuttingDown;
        int listenSocket;

        class Listener
        {
        public:
            Listener(Service *service) : service(service) {}
            void operator()();

        private:
            Service *service;
        };

        class Worker
        {
        public:
            Worker(Service *service) : service(service) {}
            void operator()();

        private:
            Service *service;
        };
    };
}



#endif    /* SERVICELISTENER_H */

