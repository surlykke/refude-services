/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the Refude Services project, library
 * It is distributed under the LGPL 2.1 license.
 * Please refer to the LGPL21 file for a copy of the license.
 */

#include <string.h>
#include <unistd.h>
#include <iterator>
#include <sys/socket.h>
#include "errorhandling.h"
#include "abstractresource.h"

namespace refude
{
    void AbstractResource::handleRequest(Descriptor& socket, HttpMessage& request, const char* remainingPath)
    {
        switch (request.method) {
        case Method::GET: doGET(socket, request, remainingPath); break;
        case Method::PATCH: doPATCH(socket, request, remainingPath); break;
        case Method::POST: doPOST(socket, request, remainingPath); break;
        case Method::DELETE: doDELETE(socket, request, remainingPath); break;
        case Method::PUT: doPUT(socket, request, remainingPath); break;
        case Method::HEAD: doHEAD(socket, request, remainingPath); break;
        case Method::TRACE: doTRACE(socket, request, remainingPath); break;
        case Method::OPTIONS: doOPTIONS(socket, request, remainingPath); break;
        case Method::CONNECT: doCONNECT(socket, request, remainingPath); break;
        case Method::UNKNOWN: throw HttpCode::Http406; // FIXME Is this the right one
        }
    }

    void AbstractResource::doGET(Descriptor& socket, HttpMessage& request, const char* remainingPath)
    {
        throw HttpCode::Http405;
    }

    void AbstractResource::doPATCH(Descriptor& socket, HttpMessage& request, const char* remainingPath)
    {
        throw HttpCode::Http405;
    }

    void AbstractResource::doPOST(Descriptor& socket, HttpMessage& request, const char* remainingPath)
    {
        throw HttpCode::Http405;
    }

    void AbstractResource::doDELETE(Descriptor& socket, HttpMessage& request, const char* remainingPath)
    {
        throw HttpCode::Http405;
    }

    void AbstractResource::doPUT(Descriptor& socket, HttpMessage& request, const char* remainingPath)
    {
        throw HttpCode::Http405;
    }

    void AbstractResource::doHEAD(Descriptor& socket, HttpMessage& request, const char* remainingPath)
    {
        throw HttpCode::Http405;
    }

    void AbstractResource::doTRACE(Descriptor& socket, HttpMessage& request, const char* remainingPath)
    {
        throw HttpCode::Http405;
    }

    void AbstractResource::doOPTIONS(Descriptor& socket, HttpMessage& request, const char* remainingPath)
    {
        throw HttpCode::Http405;
    }

    void AbstractResource::doCONNECT(Descriptor& socket, HttpMessage& request, const char* remainingPath)
    {
        throw HttpCode::Http405;
    }

    std::vector<std::string> AbstractResource::getAcceptedLocales(HttpMessage& request)
    {
        std::vector<std::string> result;
        if (request.headers.find("accept-language") < 0) {
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
}
