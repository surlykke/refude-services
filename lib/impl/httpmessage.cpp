/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the Refude Services project, library
 * It is distributed under the LGPL 2.1 license.
 * Please refer to the LGPL21 file for a copy of the license.
 */

#include <string.h>
#include <unistd.h>
#include <iosfwd>

#include "errorhandling.h"
#include "httpmessage.h"

namespace refude
{

    HttpMessage::HttpMessage()
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

    void HttpMessage::clear()
    {
        method = Method::UNKNOWN;
        status = 0;
        path = 0;
        queryParameterMap.clear();
        headers.clear();
        contentLength = 0;
        body = 0;
        remainingPath = 0;
    }


    void HttpMessage::setMatchedPathLength(size_t matchedPathLength)
    {
        if (matchedPathLength > strlen(path)) {
            throw RuntimeError("matchedPathLength too big: %d, path length: %d", matchedPathLength, strlen(path));
        }

        remainingPath = path + matchedPathLength;
        if (*remainingPath == '/') {
            remainingPath++;
        }

    }
}

