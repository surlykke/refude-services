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
using namespace std;
namespace org_restfulipc 
{

    class NotifierResource : public AbstractResource
    {
    public:
        typedef shared_ptr<NotifierResource> ptr;
        NotifierResource();

        virtual void handleRequest(int &socket, int matchedPathLength, const HttpMessage& request);
        void notifyClients(const char* event, const char* resourceIdentification);

    private:
        void addClient(int socket);

        vector<int> mClientSockets;
        mutex mMutex;
    };

}
#endif // NOTIFIERRESOURCE_H
