/* 
 * File:   httprequest.h
 * Author: Christian Surlykke <christian@surlykke.dk>
 *
 * Created on 15. marts 2015, 14:36
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
        char* remainingPath;
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

//std::ostream& operator<<(std::ostream& out, const HttpMessage& httpRequest);

#endif    /* HTTPMESSAGE_H */

