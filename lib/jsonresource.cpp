#include <sys/socket.h>
#include "jsonwriter.h"
#include "jsonresource.h"
#include "utils.h"

namespace org_restfulipc
{

   JsonResource::JsonResource() : 
        AbstractResource(), 
            json(),
            response(),
            responseMutex()

    {
    }


    JsonResource::~JsonResource()
    {
    }

    void JsonResource::handleRequest(int &socket, const HttpMessage& request)
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

    void JsonResource::doGet(int socket, const HttpMessage& request)
    {
        if (responseIsStale) 
        {
            buildResponse();
        }
        
        {
            std::shared_lock<std::shared_timed_mutex> lock(responseMutex);
            int bytesWritten = 0;
            do
            {
                int nbytes = send(socket, response.data + bytesWritten, response.used - bytesWritten, MSG_NOSIGNAL);
                if (nbytes < 0) throw C_Error();
                bytesWritten += nbytes;
            }
            while (bytesWritten < response.used);
        }
    }
    void JsonResource::doPatch(int socket, const HttpMessage& request)
    {
        throw Status::Http405; // FIXME
    }

    void JsonResource::buildResponse()
    {
        std::unique_lock<std::shared_timed_mutex> lock;
        static const char* responseTemplate =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json; charset=UTF-8\r\n"
            "Content-Length: %d\r\n"
            "\r\n";


        {
            std::unique_lock<std::shared_timed_mutex> lock(responseMutex);
            Buffer content; 
            JsonWriter jsonWriter(&content);
            jsonWriter.write(json);
            response.ensureCapacity(strlen(responseTemplate) + 10);
            response.used = sprintf(response.data, responseTemplate, jsonWriter.buffer->used);
            response.write(content.data);
            responseIsStale = false;
        }
    }
    
    void JsonResource::setResponseStale() 
    {
        responseIsStale = true;
    }
//---------------------------------------------------------------------------------------

    LocalizedJsonResource::LocalizedJsonResource() :
        json(),
        translations(),
        localizedResponses(),
        responseMutex() 
    {
    }

    LocalizedJsonResource::~LocalizedJsonResource()
    {
    }

    void LocalizedJsonResource::handleRequest(int &socket, const HttpMessage& request)
    {
        if (request.method == Method::GET)
        {
            string localeToServe = "";
            for (string locale : getLocales(request)) {
                if (translations.find(locale) != translations.end()) {
                    localeToServe = locale; 
                    break;
                }
            }
            doGet(localeToServe, socket, request);
        }
        else
        {
            throw Status::Http405;
        }
    }

    void LocalizedJsonResource::doGet(string locale, int socket, const HttpMessage& request)
    {
        if (localizedResponses.find(locale) == localizedResponses.end()) 
        {
            buildResponse(locale);
        }
       
        {
            std::shared_lock<std::shared_timed_mutex> lock(responseMutex);
            Buffer& response = localizedResponses[locale];
            int bytesWritten = 0;
            do
            {
                int nbytes = send(socket, response.data + bytesWritten, response.used - bytesWritten, MSG_NOSIGNAL);
                if (nbytes < 0) throw C_Error();
                bytesWritten += nbytes;
            }
            while (bytesWritten < response.used);
        }
    }
    
    vector<string> LocalizedJsonResource::getLocales(const HttpMessage& request)
    {
        vector<string> locales;
        if (request.headers[(int)Header::accept_language]) {
            string aLH(request.headers[(int)Header::accept_language]);
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
         }

        return locales;
    }


    void LocalizedJsonResource::buildResponse(string locale)
    {
        std::unique_lock<std::shared_timed_mutex> lock;
        static const char* responseTemplate =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json; charset=UTF-8\r\n"
            "Content-Length: %    \r\n"
            "\r\n";

        size_t contentLengthPos = strchr(responseTemplate, '%') - responseTemplate;
        
        {
            std::unique_lock<std::shared_timed_mutex> lock(responseMutex);
            localizedResponses.erase(locale);
            localizedResponses[locale].write(responseTemplate);
            size_t headerLength = localizedResponses[locale].used;
            FilteringJsonWriter jsonWriter(&(localizedResponses[locale]), &(translations[locale]));
            jsonWriter.write(json);
            if (localizedResponses[locale].used - headerLength > 99999) {
                throw RuntimeError("Content too long");
            }
            string contentLength = to_string(localizedResponses[locale].used - headerLength);
            strncpy(localizedResponses[locale].data + contentLengthPos, contentLength.data(), contentLength.size());
       }
 
    }
}
