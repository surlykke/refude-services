#ifndef JSONRESOURCE_H
#define JSONRESOURCE_H

#include <shared_mutex>
#include "abstractresource.h"
#include "json.h"
#include "buffer.h"

namespace org_restfulipc
{
    class AbstractJsonResource : public AbstractResource
    {
    public:
        AbstractJsonResource();
        virtual ~AbstractJsonResource();
        virtual void handleRequest(int &socket, const HttpMessage& request);

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
        JsonResource();
        virtual ~JsonResource();
        void setJson(Json&& json);

        virtual bool responseReady(const HttpMessage& request) { return true; }
        virtual void prepareResponse(const HttpMessage& request) {};
        virtual Buffer& getResponse(const HttpMessage& request) { return buf; }
 
    private:
        Buffer buf;
    };

    class LocalizedJsonResource : public AbstractJsonResource
    {
    public:
        LocalizedJsonResource();
        virtual ~LocalizedJsonResource();
        void setJson(Json&& json, map<string, map<string, string> >&& translations);

    protected:
        bool responseReady(const HttpMessage& request) override;
        void prepareResponse(const HttpMessage& request) override;
        Buffer& getResponse(const HttpMessage& request) override;

    private:
        void buildResponse(string locale);
        string getLocaleToServe(const char* acceptLanguageHeader);
        Json json;
        map<string, map<string, string> > translations;

        map<string, Buffer> localizedResponses; 
    };

     

}
#endif // JSONRESOURCE_H
