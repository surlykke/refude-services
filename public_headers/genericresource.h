/*
* Copyright (c) 2015, 2016 Christian Surlykke
*
* This file is part of the Restful Inter Process Communication (Ripc) project. 
* It is distributed under the LGPL 2.1 license.
* Please refer to the LICENSE file for a copy of the license.
*/

#ifndef GENERICRESOURCE_H
#define GENERICRESOURCE_H

#include <shared_mutex>

#include "abstractresource.h"
#include "notifierresource.h"

namespace org_restfulipc
{
    class GenericResource : public AbstractResource
    {
    public:
        typedef std::shared_ptr<GenericResource> ptr;
        GenericResource(const char* doc = "", NotifierResource::ptr notifierResource = 0);
        virtual ~GenericResource();

        virtual void handleRequest(int &socket, int matchedPathLength, HttpMessage& request);
        virtual void doGet(int socket, HttpMessage& request);
        virtual void doPatch(int socket, HttpMessage& request);
        void update(const char* data);

    private:
        char _response[8192]; // FIXME
        char* _respPtr;
        int _responseLength;
        std::shared_timed_mutex responseMutex;
        NotifierResource::ptr notifierResource;
    };
}


#endif /* GENERICRESOURCE_H */

