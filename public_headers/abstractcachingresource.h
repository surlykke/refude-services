
#ifndef ABSTRACTCACHINGRESOURCE_H
#define ABSTRACTCACHINGRESOURCE_H
#include <mutex>

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
        virtual void doGET(int& socket, HttpMessage& request) override;

    protected:
        virtual Buffer buildContent(HttpMessage& request, std::map<std::string, std::string>& headers) = 0;
        
        Buffer getSignature(HttpMessage& request);
        void clearCache();

    protected:
        std::recursive_mutex m;
    
    private:
        Map<Buffer> cache;
        
    };

}

#endif /* ABSTRACTCACHINGRESOURCE_H */

