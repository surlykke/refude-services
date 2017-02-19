/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the Refude Services project, library
 * It is distributed under the LGPL 2.1 license.
 * Please refer to the LGPL21 file for a copy of the license.
 */

#ifndef HTTPMESSAGEREADER_H
#define    HTTPMESSAGEREADER_H
#include <iostream>
#include <vector>
#include <memory>
#include "map.h"
#include "buffer.h"
#include "httpprotocol.h"
#include "httpmessage.h"

namespace refude
{
    class HttpMessageReader
    {
    public:
        HttpMessageReader(bool dumpRequest = false);
        void readRequest(HttpRequest* request, int socket);
        void readResponse(HttpResponse* response, int socket);

    private:
        void readRequestLine();
        void readQueryString();
        void readStatusLine();
        void readHeaderLines();
        void readHeaderLine();
        void readHeaders();
        void readBody();
        char currentChar();
        char nextChar();
        bool isTChar(char c);

        void receive();

        bool dumpRequest;
        HttpMessage* msg;
        int socket;
        int currentPos;
        int bufferEnd;
    };
}

std::ostream& operator<<(std::ostream& out, const refude::HttpMessage& message);

#endif    /* HTTPMESSAGEREADER_H */

