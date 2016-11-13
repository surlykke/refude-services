/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the Refude Services project, library
 * It is distributed under the LGPL 2.1 license.
 * Please refer to the LGPL21 file for a copy of the license.
 */

#ifndef RESOURCE_H
#define    RESOURCE_H

#include <curl/curl.h>
#include "httpmessage.h"

namespace refude
{
    struct HttpUrl
    {
        /**
         * @param url Can be of form:  http[s]://host[:port][/path]  or  http[s]://{socketpath}[/path]
         */    
        HttpUrl(const char* url);
        
        int domain; // AF_UNIX or AF_INET
        const char* hostname;
        int port;
        const char* socketPath;
        const char* requestPath;
        char url[128];
    };



    void httpGet(const char* url, HttpMessage& message);
    void httpGet(int socket, const HttpUrl& url, HttpMessage& message);    
    char waitForNotifications(int sock);
}

#endif    /* RESOURCE_H */

