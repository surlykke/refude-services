/*
* Copyright (c) 2015, 2016 Christian Surlykke
*
* This file is part of the Restful Inter Process Communication (Ripc) project. 
* It is distributed under the LGPL 2.1 license.
* Please refer to the LICENSE file for a copy of the license.
*/

#ifndef HTTPMESSAGE_H
#define    HTTPMESSAGE_H
#include <iostream>

#include "httpprotocol.h"

namespace org_restfulipc
{
    struct HttpMessage
    {
        HttpMessage();
        virtual ~HttpMessage();
        void clear();

        Method method;
        char* path;
        char* queryString;
        int status;
        const char* headers[(int) Header::unknown];
        char* body;
        int contentLength;
        
        char buffer[8192];

        inline const char* headerValue(Header h) const { return headers[(int) h]; }
    };

    class HttpMessageReader
    {
    public:
        HttpMessageReader(int socket, HttpMessage& message, bool dumpRequest = false);
        void readRequest();
        void readResponse();
        bool dumpRequest; 

    private:
        void readRequestLine();
        void readStatusLine();
        void readHeaderLines();
        bool readHeaderLine();
        void readHeaders();
        void readBody();
        char currentChar();
        char nextChar();
        bool isTChar(char c);
        void receive();

        void clear();

        int _socket;
        HttpMessage& _message;
        int _bufferEnd;
        int _currentPos;
    };
}

using namespace org_restfulipc;
std::ostream& operator<<(std::ostream& out, const HttpMessage& message);

#endif    /* HTTPMESSAGE_H */

