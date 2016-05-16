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
        virtual void doPOST(int& socket, HttpMessage& request, const char* remainingPath);

    protected:
        virtual Buffer buildContent(HttpMessage& request, const char* remainingPath, 
                                    std::map<std::string, std::string>& headers);

    private:
        bool matchCommand(Json& desktopJson, 
                          std::vector<const char*>* searchTerms, 
                          const std::vector<std::string>& locales);

        bool matchDesktopEntry(Json& desktopJson, 
                               std::vector<const char*>* searchTerms, 
                               const std::vector<std::string>& locales);

        Map<Json> desktopJsons;
    };
}
#endif /* DESKTOPENTRYRESOURCE_H */

