#include "utils.h"
#include "jsonwriter.h"

#include "localizingjsonwriter.h"

#include "localizedjsonresource.h"

namespace org_restfulipc
{

    LocalizedJsonResource::LocalizedJsonResource() :
        AbstractCachingResource(),
        json(JsonConst::EmptyObject)
    {
    }

    LocalizedJsonResource::~LocalizedJsonResource()
    {
    }

    Json& LocalizedJsonResource::getJson()
    {
        return json;
    }

    void LocalizedJsonResource::setJson(Json&& json)
    {
        this->json = std::move(json);
        clearCache();
    }

    Buffer LocalizedJsonResource::buildContent(HttpMessage& request, std::map<std::string, std::string>& headers)
    {
        return LocalizingJsonWriter(json, getAcceptedLocales(request)).buffer;
    }

    
     
   
    std::string LocalizedJsonResource::getLocaleToServe(const char* acceptLanguageHeader)
    {
        if (!acceptLanguageHeader) {
            return "";
        }
        std::vector<std::string> locales;
        std::string aLH(acceptLanguageHeader);
        aLH.erase(std::remove_if(aLH.begin(), aLH.end(), ::isspace), aLH.end());
        std::replace(aLH.begin(), aLH.end(), '-', '_');
        std::transform(aLH.begin(), aLH.end(), aLH.begin(), ::tolower);
        for (std::string part : split(aLH, ',')) {
            std::vector<std::string> langAndWeight = split(part, ';');
            if (langAndWeight.size() > 1 &&
                langAndWeight[1].size() >= 2 &&
                langAndWeight[1].substr(0, 2) == "q=") {
                langAndWeight[1].erase(0, 2);
                langAndWeight[1].resize(5, '0');

                locales.push_back(langAndWeight[1] + langAndWeight[0]);
            }
            else {
                locales.push_back(std::string("1.000") + langAndWeight[0]);
            }
        }

        sort(locales.begin(), locales.end(), std::greater<std::string>());
        for (int i = 0; i < locales.size(); i++) {
            locales[i].erase(0, 5);
        }

        for (std::string locale : locales) {
            if (json["_ripc:locales"].contains(locale)) {
                return locale;
            }
        }

        return "";
    }

}