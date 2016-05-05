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
#include <vector>
#include "map.h"
#include "buffer.h"
#include "httpprotocol.h"

namespace org_restfulipc
{
    struct HttpMessage
    {
        Method method;
        int status;
        char* path;
        Map<const char*, false> headers;
        Map<vector<const char*>, false> queryParameterMap;
        int contentLength;
        char* body;
        
        char buffer[8192];


        HttpMessage();
        virtual ~HttpMessage();
        const char* header(const char* headerName);
        vector<const char*> queryParameters(const char* parameterName);
        void clear();
        
        Buffer toBuf();
    };

    class HttpMessageReader
    {
    public:
        HttpMessageReader(int socket, HttpMessage& message, bool dumpRequest = false);
        void readRequest();
        void readResponse();
        bool dumpRequest; 
        void clear();
        void receive();

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


        int _socket;
        HttpMessage& _message;
        int _bufferEnd;
        int _currentPos;
    };
}

using namespace org_restfulipc;
std::ostream& operator<<(std::ostream& out, const HttpMessage& message);

#endif    /* HTTPMESSAGE_H */

