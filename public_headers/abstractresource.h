/*
* Copyright (c) 2015, 2016 Christian Surlykke
*
* This file is part of the Restful Inter Process Communication (Ripc) project. 
* It is distributed under the LGPL 2.1 license.
* Please refer to the LICENSE file for a copy of the license.
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
        virtual void handleRequest(int &socket, int matchedPathLength, HttpMessage& request) = 0;
    
    protected:
        void sendFully(int socket, const char* data, int nbytes);
    };

}


#endif    /* ABSTRACTRESOURCE_H */

