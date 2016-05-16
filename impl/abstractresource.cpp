/*
* Copyright (c) 2015, 2016 Christian Surlykke
*
* This file is part of the Restful Inter Process Communication (Ripc) project. 
* It is distributed under the LGPL 2.1 license.
* Please refer to the LICENSE file for a copy of the license.
*/

#include <string.h>
#include <unistd.h>
#include <iterator>
#include <sys/socket.h>
#include "errorhandling.h"
#include "abstractresource.h"

namespace org_restfulipc
{

    void AbstractResource::handleRequest(int& socket, HttpMessage& request, const char* remainingPath)
    {
        switch (request.method) 
        {
        case Method::GET:     doGET(socket, request, remainingPath); break;
        case Method::PATCH:   doPATCH(socket, request, remainingPath); break;
        case Method::POST:    doPOST(socket, request, remainingPath); break;
        case Method::DELETE:  doDELETE(socket, request, remainingPath); break;
        case Method::PUT:     doPUT(socket, request, remainingPath); break;
        case Method::HEAD:    doHEAD(socket, request, remainingPath); break;
        case Method::TRACE:   doTRACE(socket, request, remainingPath); break;
        case Method::OPTIONS: doOPTIONS(socket, request, remainingPath); break;
        case Method::CONNECT: doCONNECT(socket, request, remainingPath); break;
        case Method::UNKNOWN: throw HttpCode::Http406; // FIXME Is this the right one
        }
    }

    std::vector<std::string> AbstractResource::getAcceptedLocales(HttpMessage& request)
    {
        std::vector<std::string> result;
        if (! request.headers.contains("accept-language")) {
            return result;
        }
        
        // TODO: Maybe we should just have a generel max header length in HttpMessageReader?
        if (strlen(request.headers["accept-language"]) > 1023) {
            throw HttpCode::Http400; // FIXME Error message "Header too long"
        }

        // First, make a copy of 'accept-language', skipping whitespace, and ensuring it ends with ','
        char acceptLanguage[1024];
        char* c = acceptLanguage;
        for (const char* p = request.headers["accept-language"]; *p; p++) if (!isspace(*p)) *(c++) = *p;
        if (c > acceptLanguage && *(c - 1) != ',') *(c++) = ',';
        *c = '\0';

        std::vector<std::pair<float, std::string>> weightedLocales;
        char* currentLocale = acceptLanguage;
        char* currentWeight = NULL;
        for (char* current = acceptLanguage; *current; current++) {
            if (*current == ';') {
                *current = '\0';
                if (*(++current) != 'q' || *(++current) != '=') {
                    throw HttpCode::Http400; // FIXME Error message
                }
                currentWeight = current + 1;
            } 
            else if (*current == ',') {
                *current = '\0';
                if (currentWeight) {
                    weightedLocales.push_back({std::stof(currentWeight), currentLocale});
                }
                else {
                    weightedLocales.push_back({1.0, currentLocale});
                }
                currentLocale = current + 1;
                currentWeight = NULL;
            }
        } 
        sort(weightedLocales.rbegin(), weightedLocales.rend());
        for (auto& p : weightedLocales) {
            result.push_back(p.second);
        }
    }

    void AbstractResource::buildResponse(Buffer& response, Buffer&& content, 
                                         const std::map<std::string, std::string>& headers)
    {
            response.write("HTTP/1.1 200 OK\r\n"
                           "Content-Type: application/json; charset=UTF-8\r\n");
            for (auto headerPair : headers) {
                response.write(headerPair.first.data());
                response.write(":");
                response.write(headerPair.second.data());
                response.write("\r\n");
            }
            response.write("content-length: ");
            response.write(content.size());
            response.write("\r\n\r\n");
            response.write(content.data());

    }

 
    void AbstractResource::sendFully(int socket, const char* data, int nbytes) {
        for (int bytesWritten = 0; bytesWritten < nbytes; ) {
            int n = send(socket, data + bytesWritten, nbytes - bytesWritten, MSG_NOSIGNAL);
            if (n < 0) throw C_Error();
            bytesWritten += n;
        }
    }
}