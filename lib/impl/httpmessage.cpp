/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the Refude Services project, library
 * It is distributed under the LGPL 2.1 license.
 * Please refer to the LGPL21 file for a copy of the license.
 */
#include <sys/time.h>

#include "httpmessagereader.h"
#include "httpmessage.h"

namespace refude
{

    HttpMessage::HttpMessage() :
        method(Method::UNKNOWN),
        status(0),
        path(0),
        queryParameterMap(),
        headers(),
        contentLength(0),
        body(0),
        numtimeStamps(0)
    {
    }

    HttpMessage::~HttpMessage()
    {
    }

    const char* HttpMessage::header(const char* headerName)
    {
        int pos = headers.find(headerName);
        return pos < 0 ? NULL : headers.pairAt(pos).value;
    }

    const char* HttpMessage::parameter(const char* parameterName)
    {
        int pos = queryParameterMap.find(parameterName);
        return pos < 0 ? NULL : queryParameterMap.pairAt(pos).value[0];
    }

    void HttpMessage::addTimestamp(const char* desc)
    {
        if (numtimeStamps >= 32) throw RuntimeError("Timestamp overflow");
        struct timeval tv;
        gettimeofday(&tv, NULL);
        timestamps[numtimeStamps++] = std::pair<const char*, long>{desc, 1000000*tv.tv_sec + tv.tv_usec};
    }

    void HttpMessage::printoutTimestamps()
    {
        for (int i = 0; i < numtimeStamps; i++) {
            std::cout << timestamps[i].first << ": " << timestamps[i].second;
            if (i > 0) {
                std::cout << " (" << (timestamps[i].second - timestamps[i - 1].second) << ")";
            }
            std::cout << "\n";
        }
    }

    void HttpRequest::read(int socket)
    {
        HttpMessageReader().readRequest(this, socket);
    }

    void HttpResponse::read(int socket)
    {
        HttpMessageReader().readResponse(this, socket);
    }


}

