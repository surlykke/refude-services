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
#include <shared_mutex>
#include "descriptor.h"
#include "map.h"

#include "httpprotocol.h"
#include "httpmessage.h"

namespace refude
{

    struct  AbstractResource
    {
        typedef std::unique_ptr<AbstractResource> ptr;

        AbstractResource() {}

        virtual ~AbstractResource() {}

        virtual void handleRequest(Descriptor& socket, HttpMessage& request,  const char* remainingPath);

    protected:
        virtual void     doGET(Descriptor& socket, HttpMessage& request, const char* remainingPath);
        virtual void   doPATCH(Descriptor& socket, HttpMessage& request, const char* remainingPath);
        virtual void    doPOST(Descriptor& socket, HttpMessage& request, const char* remainingPath);
        virtual void  doDELETE(Descriptor& socket, HttpMessage& request, const char* remainingPath);
        virtual void     doPUT(Descriptor& socket, HttpMessage& request, const char* remainingPath);
        virtual void    doHEAD(Descriptor& socket, HttpMessage& request, const char* remainingPath);
        virtual void   doTRACE(Descriptor& socket, HttpMessage& request, const char* remainingPath);
        virtual void doOPTIONS(Descriptor& socket, HttpMessage& request, const char* remainingPath);
        virtual void doCONNECT(Descriptor& socket, HttpMessage& request, const char* remainingPath);

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

        std::shared_mutex mutex;
    };

}


#endif    /* ABSTRACTRESOURCE_H */

