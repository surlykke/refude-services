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
        JsonResource(const char* selfLinkUri);
        JsonResource();
        virtual ~JsonResource();

        void addRelatedLink(const char* relatedUri, bool templated);

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

}
#endif // JSONRESOURCE_H
