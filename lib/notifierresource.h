/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the Refude Services project, library
 * It is distributed under the LGPL 2.1 license.
 * Please refer to the LGPL21 file for a copy of the license.
 */

#ifndef NOTIFIERRESOURCE_H
#define NOTIFIERRESOURCE_H

#include <functional>
#include <vector>
#include <thread>


#include "queue.h"
#include "abstractresource.h"
#include "service.h"

namespace refude 
{

    class NotifierResource : public AbstractResource
    {
    public:
        typedef std::shared_ptr<NotifierResource> ptr;
        NotifierResource();
        virtual void handleRequest(Fd& socket, HttpMessage& request, Server* server) override ;
        void notify(const char* event, std::string path);
        void notify(const char* event, const char* path);

    private:
        void addClient(int socket);

        std::vector<int> mClientSockets;
        std::mutex mMutex;
    };

}
#endif // NOTIFIERRESOURCE_H
