/* 
 * File:   AbstractResource.h
 * Author: Christian Surlykke <christian@surlykke.dk>
 *
 * Created on 15. marts 2015, 09:42
 */

#ifndef ABSTRACTRESOURCE_H
#define    ABSTRACTRESOURCE_H

#include <map>
#include <string.h>
#include <vector>
#include <memory>

#include "httpprotocol.h"
#include "httpmessage.h"

namespace org_restfulipc
{

    class AbstractResource
    {
    public:
        typedef std::shared_ptr<AbstractResource> ptr;
        AbstractResource() {}
        virtual ~AbstractResource() {}
        virtual void handleRequest(int &socket, int matchedPathLength, const HttpMessage& request) = 0;
    
    protected:
        void sendFully(int socket, const char* data, int nbytes);
    };

}


#endif    /* ABSTRACTRESOURCE_H */

