#include "utils.h"
#include "jsonwriter.h"

#include "localizingjsonwriter.h"

#include "jsonresource.h"

namespace refude
{

    JsonResource::JsonResource() :
        AbstractCachingResource(),
        json(JsonConst::EmptyObject)
    {
    }

    JsonResource::~JsonResource()
    {
    }

    Json& JsonResource::getJson()
    {
        return json;
    }

    void JsonResource::setJson(Json&& json)
    {
        this->json = std::move(json);
        clearCache();
    }

    Buffer JsonResource::buildContent(HttpMessage& request, std::map<std::string, std::string>& headers)
    {
        return LocalizingJsonWriter(json, getAcceptedLocales(request)).buffer;
    }

}