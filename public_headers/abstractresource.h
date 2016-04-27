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
#include <vector>
#include <memory>

#include "map.h"

#include "httpprotocol.h"
#include "httpmessage.h"

namespace org_restfulipc
{

    class AbstractResource
    {
    public:
        typedef std::shared_ptr<AbstractResource> ptr;

        AbstractResource()
        {
        }

        virtual ~AbstractResource()
        {
        }

        virtual void handleRequest(int &socket, HttpMessage& request, const char* remainingPath);

        virtual void     doGET(int& socket, HttpMessage& request, const char* remainingPath) { throw Status::Http405; }
        virtual void   doPATCH(int& socket, HttpMessage& request, const char* remainingPath) { throw Status::Http405; }
        virtual void    doPOST(int& socket, HttpMessage& request, const char* remainingPath) { throw Status::Http405; }
        virtual void  doDELETE(int& socket, HttpMessage& request, const char* remainingPath) { throw Status::Http405; }
        virtual void     doPUT(int& socket, HttpMessage& request, const char* remainingPath) { throw Status::Http405; }
        virtual void    doHEAD(int& socket, HttpMessage& request, const char* remainingPath) { throw Status::Http405; }
        virtual void   doTRACE(int& socket, HttpMessage& request, const char* remainingPath) { throw Status::Http405; }
        virtual void doOPTIONS(int& socket, HttpMessage& request, const char* remainingPath) { throw Status::Http405; }
        virtual void doCONNECT(int& socket, HttpMessage& request, const char* remainingPath) { throw Status::Http405; }

    protected:
        void sendFully(int socket, const char* data, int nbytes);
    };

}


#endif    /* ABSTRACTRESOURCE_H */

