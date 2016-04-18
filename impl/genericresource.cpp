/*
* Copyright (c) 2015, 2016 Christian Surlykke
*
* This file is part of the Restful Inter Process Communication (Ripc) project. 
* It is distributed under the LGPL 2.1 license.
* Please refer to the LICENSE file for a copy of the license.
*/

#include <unistd.h>
#include <sys/socket.h>
#include "errorhandling.h"
#include "genericresource.h"

using namespace std;

namespace org_restfulipc
{

    GenericResource::GenericResource(const char* doc, NotifierResource::ptr notifierResource) :
        AbstractResource(),
        notifierResource(notifierResource),
        _response(),
        _respPtr(_response),
        _responseLength(0),
        responseMutex()
    {
        update(doc);
    }

    GenericResource::~GenericResource()
    {
    }

    void GenericResource::handleRequest(int &socket, int matchedPathLength, HttpMessage& request)
    {
        if (request.method == Method::GET)    
        {
            doGet(socket, request);
        }
        else if (request.method == Method::PATCH)
        {
            doPatch(socket, request);
        }
        else
        {
            throw Status::Http405;
        }
    }

    void GenericResource::doGet(int socket, HttpMessage& request)
    {
        std::shared_lock<std::shared_timed_mutex> lock(responseMutex);
        int bytesWritten = 0;
        do
        {
            int nbytes = send(socket, _response + bytesWritten, _responseLength - bytesWritten, MSG_NOSIGNAL);
            if (nbytes < 0) throw C_Error();
            bytesWritten += nbytes;
        }
        while (bytesWritten < _responseLength);
    }


    void GenericResource::doPatch(int socket, HttpMessage& request)
    {
        throw Status::Http405; // FIXME
    }


    void GenericResource::update(const char* data)
    {
        static const char* responseTemplate =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json; charset=UTF-8\r\n"
            "Content-Length: %d\r\n"
            "\r\n"
            "%s";

        int contentLength = strlen(data);
        
        {
            std::unique_lock<std::shared_timed_mutex> lock(responseMutex);
            _responseLength = sprintf(_response, responseTemplate, contentLength, data);
        }

        if (notifierResource) {
            notifierResource->notifyClients("updated", "-");
        }
    }
}
