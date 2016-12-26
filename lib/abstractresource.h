/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the Refude Services project, library
 * It is distributed under the LGPL 2.1 license.
 * Please refer to the LGPL21 file for a copy of the license.
 */

#ifndef ABSTRACTRESOURCE_H
#define    ABSTRACTRESOURCE_H

#include <map>
#include <vector>
#include <memory>
#include "map.h"
#include "fd.h"
#include "httpprotocol.h"
#include "httpmessage.h"

namespace refude
{
    class Server;
    struct  AbstractResource
    {
        typedef std::shared_ptr<AbstractResource> ptr;

        enum class Type {
            RequestResponse,
            LongRunning
        };

        AbstractResource() {}

        virtual ~AbstractResource() {}

        virtual void handleRequest(Fd& socket, HttpMessage& request, Server* server);

        virtual void     doGET(Fd& socket, HttpMessage& request, Server* server);
        virtual void   doPATCH(Fd& socket, HttpMessage& request, Server* server);
        virtual void    doPOST(Fd& socket, HttpMessage& request, Server* server);
        virtual void  doDELETE(Fd& socket, HttpMessage& request, Server* server);
        virtual void     doPUT(Fd& socket, HttpMessage& request, Server* server);
        virtual void    doHEAD(Fd& socket, HttpMessage& request, Server* server);
        virtual void   doTRACE(Fd& socket, HttpMessage& request, Server* server);
        virtual void doOPTIONS(Fd& socket, HttpMessage& request, Server* server);
        virtual void doCONNECT(Fd& socket, HttpMessage& request, Server* server);

        /**
         * Extract acceptable locales from accept-language header as an ordered  list.
         * As an example, given:
         *  accept-language: en;q=0.4,en-US;q=0.6,da-DK,da;q=0.8,nb;q=0.2,de;q=0.2
         * It should return:
         *    { "da-DK", "da", "en-US", "en", "nb", "de" }
         * However, no guarantees are given on the ordering of locales with same weight ("nb" and "de" in this case)
         * @param request
         * @return 
         */
        std::vector<std::string> getAcceptedLocales(HttpMessage& request);
    };
}


#endif    /* ABSTRACTRESOURCE_H */

