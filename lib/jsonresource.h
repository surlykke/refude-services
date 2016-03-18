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
        virtual void handleRequest(int &socket, int matchedPathLength,  const HttpMessage& request);

    protected:
        virtual void doGet(int socket, const HttpMessage& request);
        void writeFully(int socket, const char* data, int nbytes);

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

        virtual bool responseReady(const HttpMessage& request) { return true; }
        virtual void prepareResponse(const HttpMessage& request) {};
        virtual Buffer& getResponse(const HttpMessage& request) { return buf; }
 
    private:
        Buffer buf;
    };

    // Translations holds, for each locale, a map from untranslated to translated string
    typedef map<string, map<string, string> > Translations;

    class LocalizedJsonResource : public AbstractJsonResource
    {
    public:
        typedef std::shared_ptr<LocalizedJsonResource> ptr; 
        LocalizedJsonResource();
        virtual ~LocalizedJsonResource();
        void setJson(Json&& json, Translations&& translations);
        Json json;
        Translations translations;

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
