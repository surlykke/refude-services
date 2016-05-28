#ifndef DESKTOPENTRYRESOURCE_H 
#define DESKTOPENTRYRESOURCE_H 

#include <ripc/json.h>
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
        void doPOST(int& socket, HttpMessage& request) override;

    protected:
        Buffer buildContent(HttpMessage& request, std::map<std::string, std::string>& headers) override;

    private:
        Buffer handleDesktopEntrySearch(HttpMessage& request);
        Buffer handleCommandSearch(HttpMessage& request);

        bool matchDesktopEntry(Json& desktopJson, 
                               std::vector<const char*>* searchTerms, 
                               const std::vector<std::string>& locales);

        bool matchCommand(Json& desktopJson, 
                          std::vector<const char*>* searchTerms, 
                          const std::vector<std::string>& locales);

        Map<Json> desktopJsons;
        Json commandLastUsed;
    };
}
#endif /* DESKTOPENTRYRESOURCE_H */

