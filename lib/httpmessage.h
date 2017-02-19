/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the Refude Services project, library
 * It is distributed under the LGPL 2.1 license.
 * Please refer to the LGPL21 file for a copy of the license.
 */

#ifndef HTTPMESSAGE_H
#define    HTTPMESSAGE_H
#include <iostream>
#include <vector>
#include <memory>
#include "map.h"
#include "httpprotocol.h"

namespace refude
{
    struct HttpMessage
    {
        virtual void read(int socket) = 0;
        virtual ~HttpMessage();
        const char* header(const char* headerName);
        const char* parameter(const char* parameterName);
        
        Method method;
        int status;
        char* path;
        Map<std::vector<const char*>> queryParameterMap;
        Map<const char*> headers;
        int contentLength;
        char* body;
        char buffer[8192];

        void addTimestamp(const char* desc);
        void printoutTimestamps();

    protected:
        HttpMessage();
        std::pair<const char*, long> timestamps[32];
        int numtimeStamps;
    };

    struct HttpRequest : public HttpMessage
    {
        typedef std::unique_ptr<HttpRequest> ptr;
        HttpRequest() : HttpMessage() {}
        virtual void read(int socket) override;
    };

    struct HttpResponse : public HttpMessage
    {
        typedef std::unique_ptr<HttpRequest> ptr;
        HttpResponse() : HttpMessage() {}
        virtual void read(int socket) override;
    };
}

#endif    /* HTTPMESSAGE_H */

