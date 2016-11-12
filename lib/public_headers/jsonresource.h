#ifndef JSONRESOURCE_H
#define JSONRESOURCE_H

#include <map>
#include <string>
#include "buffer.h"
#include "json.h"
#include "httpmessage.h"
#include "abstractcachingresource.h"

namespace refude
{
    class JsonResource : public AbstractCachingResource
    {
    public:
        typedef std::shared_ptr<JsonResource> ptr; 
        JsonResource();
        virtual ~JsonResource();
        Json& getJson(); 
        void setJson(Json&& json);

    protected:
        virtual Buffer buildContent(HttpMessage& request, std::map<std::string, std::string>& headers);
        Json json;
    };
}

#endif /* JSONRESOURCE_H */

