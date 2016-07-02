#ifndef LOCALIZEDJSONRESOURCE_H
#define LOCALIZEDJSONRESOURCE_H

#include <map>
#include "buffer.h"
#include "json.h"
#include "httpmessage.h"
#include "abstractcachingresource.h"

namespace org_restfulipc
{
    class LocalizedJsonResource : public AbstractCachingResource
    {
    public:
        typedef std::shared_ptr<LocalizedJsonResource> ptr; 
        LocalizedJsonResource();
        virtual ~LocalizedJsonResource();
        const Json& getJson(); 
        void setJson(Json&& json);

    protected:
        virtual Buffer buildContent(HttpMessage& request, std::map<std::string, std::string>& headers) override;
        Json json;

    private:
        std::string getLocaleToServe(const char* acceptLanguageHeader);

    };
}

#endif /* LOCALIZEDJSONRESOURCE_H */

