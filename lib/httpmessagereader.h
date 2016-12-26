/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the Refude Services project, library
 * It is distributed under the LGPL 2.1 license.
 * Please refer to the LGPL21 file for a copy of the license.
 */

#ifndef HTTPMESSAGEREADER_H
#define HTTPMESSAGEREADER_H
#include <iostream>
#include <vector>
#include "map.h"
#include "buffer.h"
#include "httpprotocol.h"

class HttpMessage;
namespace refude
{
    class HttpMessageReader
    {
    public:
        HttpMessageReader(int socket, bool dumpRequest = false);
        ~HttpMessageReader();
        HttpMessage::ptr readRequest();
        HttpMessage::ptr readResponse();

    private:
        bool dumpRequest;
        void clear();
        void receive();
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
        HttpMessage::ptr _message;
        int _bufferEnd;
        int _currentPos;
    };
}


#endif    /* HTTPMESSAGEREADER_H */

