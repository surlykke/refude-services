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

namespace refude
{


    struct  AbstractResource
    {
        typedef std::shared_ptr<AbstractResource> ptr;

        AbstractResource() {}

        virtual ~AbstractResource() {}

        virtual void handleRequest(int &socket, HttpMessage& request);

        virtual void     doGET(int& socket, HttpMessage& request);
        virtual void   doPATCH(int& socket, HttpMessage& request);
        virtual void    doPOST(int& socket, HttpMessage& request);
        virtual void  doDELETE(int& socket, HttpMessage& request);
        virtual void     doPUT(int& socket, HttpMessage& request);
        virtual void    doHEAD(int& socket, HttpMessage& request);
        virtual void   doTRACE(int& socket, HttpMessage& request);
        virtual void doOPTIONS(int& socket, HttpMessage& request);
        virtual void doCONNECT(int& socket, HttpMessage& request);

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

    protected:
        void buildResponse(Buffer& response, Buffer&& content, 
                           const std::map<std::string, std::string>& headers);

        void sendFully(int socket, const char* data, int nbytes);

    };

}


#endif    /* ABSTRACTRESOURCE_H */

