#ifndef JSONRESOURCE_H
#define JSONRESOURCE_H

#include <shared_mutex>
#include "abstractresource.h"
#include "json.h"
#include "buffer.h"

namespace org_restfulipc
{
    class JsonResource : public AbstractResource
    {
    public:
        JsonResource();
        virtual ~JsonResource();

        virtual void handleRequest(int &socket, const HttpMessage& request);
        virtual void doGet(int socket, const HttpMessage& request);
        virtual void doPatch(int socket, const HttpMessage& request);
        Json json;
        void setResponseStale();

    private:
        bool responseIsStale;
        void buildResponse();
        Buffer response;
        std::shared_timed_mutex responseMutex;

        Buffer templateCopy;
    };

    class LocalizedJsonResource : public AbstractResource
    {
    public:
        LocalizedJsonResource();
        virtual ~LocalizedJsonResource();

        virtual void handleRequest(int &socket, const HttpMessage& request);
        virtual void doGet(string locale, int socket, const HttpMessage& request);
        Json json;
        map<string, map<string, string> > translations;

    private:
        vector<string> getLocales(const HttpMessage& request);
        void buildResponse(string locale);
        map<string, Buffer> localizedResponses; 
        std::shared_timed_mutex responseMutex;
    };

     

}
#endif // JSONRESOURCE_H
