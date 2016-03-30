#include <sys/socket.h>
#include <unistd.h>
#include <algorithm>
#include "jsonwriter.h"
#include "jsonresource.h"
#include "utils.h"

namespace org_restfulipc {

AbstractJsonResource::AbstractJsonResource():
    AbstractResource(),
    responseMutex()
{
}

AbstractJsonResource::~AbstractJsonResource()
{
}

void AbstractJsonResource::handleRequest(int& socket, int matchedPathLength, const HttpMessage& request)
{
    if (request.method == Method::GET)
    {
        doGet(socket, request);
    }
    else if (request.method == Method::PATCH)
    {
        doPatch(socket, request);
    }
    else
    {
        throw Status::Http405;
    }  
}

void AbstractJsonResource::doGet(int socket, const HttpMessage& request)
{
    for (;;) { 
        {
            std::shared_lock<std::shared_timed_mutex> lock(responseMutex);
            if (responseReady(request)) {
                Buffer& buf = getResponse(request);
                sendFully(socket, buf.data, buf.used);
                return;
            }
        } 
            
        {
            std::unique_lock<std::shared_timed_mutex> lock(responseMutex);
            if (! responseReady(request)) {
                prepareResponse(request);
            }
        }
    } 
}

    void AbstractJsonResource::doPatch(int socket, const HttpMessage& request)
    {
        throw Status::Http405; 
    }


   JsonResource::JsonResource() : 
        AbstractJsonResource(), 
        buf()
    {
       setJson(JsonConst::EmptyObject);
    }


    JsonResource::~JsonResource()
    {
    }
    
    void JsonResource::setJson(Json&& json)
    {
        static const char* responseTemplate =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json; charset=UTF-8\r\n"
            "Access-Control-Allow-Methods: POST, GET, OPTIONS, DELETE, PUT, PATCH\r\n"
            "Access-Control-Allow-Origin: http://localhost:8383\r\n"; // FIXME
            

        std::unique_lock<std::shared_timed_mutex> lock(responseMutex);
        buf.clear();
        buf.write(responseTemplate);
        JsonWriter writer(json);
        buf.write("Content-Length: ");
        buf.write(writer.buffer.used);
        buf.write("\r\n\r\n");
        buf.write(writer.buffer.data);
    }

//---------------------------------------------------------------------------------------

    LocalizedJsonResource::LocalizedJsonResource() :
        AbstractJsonResource(),
        json(),
        translations(),
        localizedResponses()
    {
    }

    LocalizedJsonResource::~LocalizedJsonResource()
    {
    }

    void LocalizedJsonResource::setJson(Json&& json, Translations&& translations)
    {
        std::unique_lock<std::shared_timed_mutex> lock(responseMutex);
        localizedResponses.clear();
        this->json = std::move(json);
        this->translations = std::move(translations);
    }


    bool LocalizedJsonResource::responseReady(const HttpMessage& request)
    {
        const string localeToServe = getLocaleToServe(request.headers[(int)Header::accept_language]);
        return localizedResponses.find(localeToServe.data()) > -1;
    }

    void LocalizedJsonResource::prepareResponse(const HttpMessage& request)
    {
        static const char* responseTemplate =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json; charset=UTF-8\r\n"
            "Access-Control-Allow-Methods: GET, PATCH\r\n"
            "Access-Control-Allow-Origin: http://localhost:8383\r\n"; // FIXME

        string locale = getLocaleToServe(request.headers[(int)Header::accept_language]);
        Buffer& buf = localizedResponses[locale.data()];
        buf.write(responseTemplate);
        FilteringJsonWriter jsonWriter(json, &(translations[locale]));
        
        buf.write("Content-Length: ");
        buf.write(jsonWriter.buffer.used);
        buf.write("\r\n\r\n");
        buf.write(jsonWriter.buffer.data);

    }

    Buffer& LocalizedJsonResource::getResponse(const HttpMessage& request)
    {
        string locale = getLocaleToServe(request.headers[(int) Header::accept_language]);
        return localizedResponses[locale.data()];
    }


    string LocalizedJsonResource::getLocaleToServe(const char* acceptLanguageHeader)
    {
        if (! acceptLanguageHeader) {
            return "";
        }
        vector<string> locales;
        string aLH(acceptLanguageHeader);
        aLH.erase(remove_if(aLH.begin(), aLH.end(), ::isspace), aLH.end());
        replace(aLH.begin(), aLH.end(), '-', '_');
        transform(aLH.begin(), aLH.end(), aLH.begin(), ::tolower);
        for (string part : split(aLH, ',')) {
            vector<string> langAndWeight= split(part, ';');
            if (langAndWeight.size() > 1 && 
                langAndWeight[1].size() >= 2 && 
                langAndWeight[1].substr(0,2) == "q=") {
                langAndWeight[1].erase(0, 2);
                langAndWeight[1].resize(5, '0');

                locales.push_back(langAndWeight[1] + langAndWeight[0]);
            }
            else {
                locales.push_back(string("1.000") + langAndWeight[0]);
            }
        }
        
        sort(locales.begin(), locales.end(), std::greater<string>());
        for (int i = 0; i < locales.size(); i++) {
            locales[i].erase(0,5);
        }

        for (string locale : locales) {
            if (translations.find(locale) != translations.end()) {
                return locale;
            }
        }

        return "";
    }

}
