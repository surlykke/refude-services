/*
* Copyright (c) 2015, 2016 Christian Surlykke
*
* This file is part of the Restful Inter Process Communication (Ripc) project. 
* It is distributed under the LGPL 2.1 license.
* Please refer to the LICENSE file for a copy of the license.
*/

#ifndef NOTIFIERRESOURCE_H
#define NOTIFIERRESOURCE_H

#include <mutex>
#include <vector>

#include "abstractresource.h"

namespace refude 
{

    class NotifierResource : public AbstractResource
    {
    public:
        typedef std::shared_ptr<NotifierResource> ptr;
        NotifierResource();

        virtual void doGET(int& socket, HttpMessage& request) override;
        void resourceRemoved(const char* path);
        void resourceRemoved(const char* p1, const char* p2);
        void resourceRemoved(const char* p1, const char* p2, const char* p3);
        void resourceAdded(const char* path);
        void resourceAdded(const char* p1, const char* p2);
        void resourceAdded(const char* p1, const char* p2, const char* p3);
        void resourceUpdated(const char* path);
        void resourceUpdated(const char* p1, const char* p2);
        void resourceUpdated(const char* p1, const char* p2, const char* p3);
        void notifyClients(const char* event, const char* path);

    private:
        void addClient(int socket);

        std::vector<int> mClientSockets;
        std::mutex mMutex;
    };

}
#endif // NOTIFIERRESOURCE_H
