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
#include "map.h"
#include "buffer.h"
#include "httpprotocol.h"

namespace refude
{
    struct HttpMessage
    {
        Method method;
        int status;
        char* path;
        Map<const char*> headers;
        Map<std::vector<const char*>> queryParameterMap;
        int contentLength;
        char* body;
        
        char buffer[8192];

        /**
         * This field is not really part of the Http Message. Used to 
         * communicate between service and request handlers the part of the request 
         * path that was matched (which may be different from the request path when 
         * wildcarding is in play).
         */
        char* remainingPath; 

        HttpMessage();
        virtual ~HttpMessage();
        const char* header(const char* headerName);
        const char* parameter(const char* parameterName);
        void clear();
        
        Buffer toBuf();
        
        void setMatchedPathLength(size_t matchedPathLength); 
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

std::ostream& operator<<(std::ostream& out, const refude::HttpMessage& message);

#endif    /* HTTPMESSAGE_H */

