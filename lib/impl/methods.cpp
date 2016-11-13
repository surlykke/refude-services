/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the Refude Services project, library
 * It is distributed under the LGPL 2.1 license.
 * Please refer to the LGPL21 file for a copy of the license.
 */

#include <string.h>
#include <sys/un.h>
#include <unistd.h>
#include <errno.h>

#include "errorhandling.h"
#include "methods.h"

namespace refude
{

    HttpUrl::HttpUrl(const char* url)
    {
        if (strlen(url) > 127) {
            throw "Url too long!";
        }

        strcpy(this->url, url);
        
        char* pos = this->url;
        if (strncmp("http://", url, 7)) {
            throw "Unknown protocol";
        }

        pos += 7;
            
        if (*pos == '{')    {
            domain = AF_UNIX;    
            socketPath = pos + 1;
            pos = index(pos, '}');    
        
            if (!pos) {
                throw "Invalid url";
            }

            *pos = '\0';
            requestPath = pos + 1;        
        }
        else {
            throw "Not implemented yet";
        }
    }

    int writeSome(int socket, const char* data, int nbytes)
    {
        int written = send(socket, data, nbytes, MSG_NOSIGNAL);
        if (written  < 0) throw C_Error();
        return written;
    }


    void writeMessage(int socket, const char* data, int nbytes)
    {
        for (int i = 0; i < nbytes; i += writeSome(socket, data + i, nbytes - i));
    }

    int openConnection(const HttpUrl& url)
    {
        if (url.domain == AF_UNIX) {
            int sock = socket(AF_UNIX, SOCK_STREAM, 0);
            if (sock < 0) throw C_Error();
            struct sockaddr_un addr;
            memset(&addr, 0, sizeof(struct sockaddr_un));
            addr.sun_family = AF_UNIX;
            strncpy(addr.sun_path, url.socketPath, sizeof(addr.sun_path) - 1);
            if (connect(sock, (const sockaddr*)&addr, sizeof(struct sockaddr_un)) < 0) throw C_Error();
            return sock;
        }
        else {
            throw "Not implemented";
        }
    }

    /**
     * For one-off requests. Tells the server to close the connection when the request is served.
     * @param url
     * @param message
     */    
    void httpGet(const char* url, HttpMessage& message)
    {
        static const char* getRequestTemplate = 
            "GET %s HTTP/1.1\r\n"
            "Host: localhost\r\n"
            "Connection: close\r\n"
            "\r\n";
    
        HttpUrl httpUrl(url);
        int sock = openConnection(httpUrl);
        char request[128];        
        sprintf(request, getRequestTemplate, httpUrl.requestPath);
        writeMessage(sock, request, strlen(request));

        HttpMessageReader httpMessageReader(sock, message);
        httpMessageReader.readResponse();
        close(sock);
    }

    /**
     * @param socket
     * @param path
     * @param message
     */
    void httpGet(int sock, const HttpUrl& url, HttpMessage& message)
    {
        static const char* getRequestTemplate = 
            "GET %s HTTP/1.1\r\n"
            "Host: localhost\r\n"
            "\r\n";
    
        char request[128];        
        sprintf(request, getRequestTemplate, url.requestPath);
        writeMessage(sock, request, strlen(request));

        HttpMessageReader httpMessageReader(sock, message);
        httpMessageReader.readResponse();
    }

}
