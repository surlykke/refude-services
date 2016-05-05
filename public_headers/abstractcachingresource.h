
#ifndef ABSTRACTCACHINGRESOURCE_H
#define ABSTRACTCACHINGRESOURCE_H
#include <mutex>

#include "buffer.h"

#include "abstractresource.h"

namespace org_restfulipc
{
    using namespace std;
    class AbstractCachingResource : public AbstractResource
    {
    public:
        typedef std::shared_ptr<AbstractCachingResource> ptr;
        AbstractCachingResource();
        virtual ~AbstractCachingResource();
        virtual void doGET(int& socket, HttpMessage& request, const char* remainingPath) override;

    protected:
        virtual Buffer buildContent(HttpMessage& request, const char* remainingPath, map<string, string>& headers) = 0;
        Buffer getSignature(HttpMessage& request, const char* remainingPath);
        void clearCache();

    protected:
        recursive_mutex m;
    
    private:
        Map<Buffer> cache;
        
    };

}

#endif /* ABSTRACTCACHINGRESOURCE_H */

