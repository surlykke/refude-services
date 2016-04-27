
#ifndef ABSTRACTCACHINGRESOURCE_H
#define ABSTRACTCACHINGRESOURCE_H

#include "buffer.h"

#include "abstractresource.h"

namespace org_restfulipc
{

    class AbstractCachingResource : public AbstractResource
    {
    public:
        typedef std::shared_ptr<AbstractCachingResource> ptr;
        AbstractCachingResource();
        virtual ~AbstractCachingResource();
        virtual void doGET(int& socket, HttpMessage& request, const char* remainingPath) override;

    protected:
        virtual Buffer buildContent(HttpMessage& request, map<string, string>& headers) = 0;
        Buffer getSignature(HttpMessage& request, const char* remainingPath);
        void clearCache();

    private:
        Map<Buffer> cache;
        
    };

}

#endif /* ABSTRACTCACHINGRESOURCE_H */

