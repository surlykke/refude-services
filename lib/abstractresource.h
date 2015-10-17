/* 
 * File:   AbstractResource.h
 * Author: Christian Surlykke <christian@surlykke.dk>
 *
 * Created on 15. marts 2015, 09:42
 */

#ifndef ABSTRACTRESOURCE_H
#define    ABSTRACTRESOURCE_H

#include <pthread.h>
#include <map>
#include <string.h>
#include <vector>

#include "httpprotocol.h"
#include "httpmessage.h"

namespace org_restfulipc
{

    class AbstractResource
    {
    public:
        AbstractResource() {}
        virtual ~AbstractResource() {}
        virtual void handleRequest(int socket, const HttpMessage& request) = 0;
    };

}


#endif    /* ABSTRACTRESOURCE_H */

