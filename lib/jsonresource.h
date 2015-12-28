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
        JsonResource(Json&& json);
        virtual ~JsonResource();

        virtual void handleRequest(int &socket, const HttpMessage& request);
        virtual void doGet(int socket, const HttpMessage& request);
        virtual void doPatch(int socket, const HttpMessage& request);
        void jsonUpdated();

        Json json;

    private:
        Buffer response;
        std::shared_timed_mutex responseMutex;
    };

}
#endif // JSONRESOURCE_H
