#ifndef DESKTOPENTRYRESOURCE_H
#define DESKTOPENTRYRESOURCE_H

#include <ripc/abstractcachingresource.h>
#include <ripc/notifierresource.h>

namespace org_restfulipc
{

    class DesktopEntryResource : public AbstractCachingResource
    {
    public:
        typedef std::shared_ptr<DesktopEntryResource> ptr;
        DesktopEntryResource(Map<Json>&& desktopJsons);
        virtual ~DesktopEntryResource();
        void setDesktopJsons(Map<Json>&& desktopJsons, NotifierResource::ptr notifier);    
        virtual Buffer buildContent(HttpMessage& request, const char* remainingPath, map<string, string>& headers);

    private:
        Map<Json> desktopJsons;
    };
}
#endif /* DESKTOPENTRYRESOURCE_H */

