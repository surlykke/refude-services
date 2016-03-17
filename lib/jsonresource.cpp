#include <sys/socket.h>
#include <unistd.h>
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

void AbstractJsonResource::handleRequest(int& socket, const HttpMessage& request)
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
                int bytesWritten = 0; 
                while (bytesWritten < buf.used) {
                    int n = write(socket, buf.data + bytesWritten, buf.used - bytesWritten);
                    if (n < 0) throw C_Error();
                    bytesWritten += n;
                }
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
        throw Status::Http405; // FIXME
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
        Buffer body;
        JsonWriter writer(&body);
        writer.write(json);
        buf.write("Content-Length: ");
        buf.write(body.used);
        buf.write("\r\n\r\n");
        buf.write(body.data);
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

    void LocalizedJsonResource::setJson(Json&& json, map<string, map<string, string> >&& translations)
    {
        std::unique_lock<std::shared_timed_mutex> lock(responseMutex);
        localizedResponses.erase(localizedResponses.begin(), localizedResponses.end());
        this->json = std::move(json);
        this->translations = std::move(translations);
    }


    bool LocalizedJsonResource::responseReady(const HttpMessage& request)
    {
        string localeToServe = getLocaleToServe(request.headers[(int)Header::accept_language]);
        return localizedResponses.find(localeToServe) != localizedResponses.end();
    }

    void LocalizedJsonResource::prepareResponse(const HttpMessage& request)
    {
        static const char* responseTemplate =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json; charset=UTF-8\r\n"
            "Access-Control-Allow-Methods: GET, PATCH\r\n"
            "Access-Control-Allow-Origin: http://localhost:8383\r\n"; // FIXME

        string locale = getLocaleToServe(request.headers[(int)Header::accept_language]);
        localizedResponses[locale].write(responseTemplate);
        Buffer body;
        FilteringJsonWriter jsonWriter(&body, &(translations[locale]));
        jsonWriter.write(json);
        
        localizedResponses[locale].write("Content-Length: ");
        localizedResponses[locale].write(body.used);
        localizedResponses[locale].write("\r\n\r\n");
        localizedResponses[locale].write(body.data);

    }

    Buffer& LocalizedJsonResource::getResponse(const HttpMessage& request)
    {
        return localizedResponses[getLocaleToServe(request.headers[(int) Header::accept_language])];
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
