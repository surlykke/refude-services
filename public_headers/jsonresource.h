/*
* Copyright (c) 2015, 2016 Christian Surlykke
*
* This file is part of the Restful Inter Process Communication (Ripc) project. 
* It is distributed under the LGPL 2.1 license.
* Please refer to the LICENSE file for a copy of the license.
*/

#ifndef JSONRESOURCE_H
#define JSONRESOURCE_H

#include <shared_mutex>
#include "map.h"
#include "abstractresource.h"
#include "json.h"
#include "buffer.h"

namespace org_restfulipc
{
    using namespace std;
    class AbstractJsonResource : public AbstractResource
    {
    public:
        AbstractJsonResource();
        virtual ~AbstractJsonResource();
        virtual void handleRequest(int &socket, int matchedPathLength,  const HttpMessage& request);

    protected:
        virtual void doGet(int socket, const HttpMessage& request);

        virtual void doPatch(int socket, const HttpMessage& request);
        virtual bool responseReady(const HttpMessage& request) = 0;
        virtual void prepareResponse(const HttpMessage& request) = 0;
        virtual Buffer& getResponse(const HttpMessage& request) = 0;

        std::shared_timed_mutex responseMutex;

    };

    class JsonResource : public AbstractJsonResource
    {
    public:
        typedef std::shared_ptr<JsonResource> ptr;
        JsonResource();
        virtual ~JsonResource();
        void setJson(Json&& json);
        bool equal(const Json& json);
        virtual bool responseReady(const HttpMessage& request) { return true; }
        virtual void prepareResponse(const HttpMessage& request) {};
        virtual Buffer& getResponse(const HttpMessage& request) { return buf; }
 
    private:
        Json json;
        Buffer buf;
    };

    class LocalizedJsonResource : public AbstractJsonResource
    {
    public:
        typedef std::shared_ptr<LocalizedJsonResource> ptr; 
        LocalizedJsonResource();
        virtual ~LocalizedJsonResource();
        void setJson(Json&& json, Json&& translations);
        Json json;
        Json translations;

    protected:
        virtual bool responseReady(const HttpMessage& request);
        virtual void prepareResponse(const HttpMessage& request);
        virtual Buffer& getResponse(const HttpMessage& request);
        Map<Buffer> localizedResponses; 

    private:
        string getLocaleToServe(const char* acceptLanguageHeader);

    };

     

}
#endif // JSONRESOURCE_H
