#ifndef REFUDEMAINRESOURCE_H
#define REFUDEMAINRESOURCE_H

#include <ripc/abstractcachingresource.h>
#include <ripc/notifierresource.h>

namespace org_restfulipc
{

    class RefudeMainResource : public AbstractCachingResource
    {
    public:
        typedef std::shared_ptr<RefudeMainResource> ptr;

        RefudeMainResource();
        virtual ~RefudeMainResource();

    protected:
        Buffer buildContent(HttpMessage& request, const char* remainingPath, map<string, string>& headers) override;

    };
}
#endif /* REFUDEMAINRESOURCE_H */

