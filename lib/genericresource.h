/* 
 * File:   genericresource.h
 * Author: Christian Surlykke <christian@surlykke.dk>
 *
 * Created on 23. juni 2015, 07:33
 */

#ifndef GENERICRESOURCE_H
#define GENERICRESOURCE_H

#include <boost/thread/locks.hpp>
#include <boost/thread/shared_mutex.hpp>

#include "abstractresource.h"

namespace org_restfulipc
{
    class GenericResource : public AbstractResource
    {
    public:
        GenericResource(const char* json = "{}");
        virtual ~GenericResource();

        virtual void handleRequest(int &socket, const HttpMessage& request);
        virtual void doGet(int socket, const HttpMessage& request);
        virtual void doStreamUpgrade(int socket, const HttpMessage& request);
        virtual void doPatch(int socket, const HttpMessage& request);
        void update(const char* data);

    private:
        void notifyClients();    
        void writeData(int socket, const char *data, int nBytes);


        char _response[8192]; // FIXME
        char* _respPtr;
        int _responseLength;
        boost::shared_mutex responseAccess;

        std::vector<int> _webSockets;
        boost::mutex websocketsAccess;
    };
}


#endif /* GENERICRESOURCE_H */

