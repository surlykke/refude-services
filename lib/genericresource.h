/* 
 * File:   genericresource.h
 * Author: Christian Surlykke <christian@surlykke.dk>
 *
 * Created on 23. juni 2015, 07:33
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
        GenericResource(const char* doc = "", NotifierResource *notifierResource = 0);
        virtual ~GenericResource();

        virtual void handleRequest(int &socket, const HttpMessage& request);
        virtual void doGet(int socket, const HttpMessage& request);
        virtual void doPatch(int socket, const HttpMessage& request);
        void update(const char* data);

    private:
        char _response[8192]; // FIXME
        char* _respPtr;
        int _responseLength;
        std::shared_timed_mutex responseMutex;
        NotifierResource *notifierResource;
    };
}


#endif /* GENERICRESOURCE_H */

