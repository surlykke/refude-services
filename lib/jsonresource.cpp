#include <sys/socket.h>
#include "jsonwriter.h"
#include "jsonresource.h"

namespace org_restfulipc
{

    /*JsonResource*JsonResource::fromTemplate(const char* templ, std::map<const char*, const char*> parameters)
    {
        JsonResource* jsonResource = new JsonResource();


    }*/

    JsonResource::JsonResource(const char* selfLinkUri):
        AbstractResource(),
        json(JsonConst::EmptyObject),
        response(),
        responseMutex()
    {
        json = JsonConst::EmptyObject;
        json["_links"] = JsonConst::EmptyObject;
        json["_links"]["self"] = JsonConst::EmptyObject;
        json["_links"]["self"]["href"] = selfLinkUri;
    }

    JsonResource::~JsonResource()
    {
    }

    void JsonResource::addRelatedLink(const char* relatedUri, bool templated)
    {
        if (! json["_links"].contains("related")) {
            json["_links"]["related"] = JsonConst::EmptyArray;
        }

        Json& relatedObject = json["_links"]["related"].append(JsonConst::EmptyObject);
        relatedObject["href"] = relatedUri;
        if (templated) {
            relatedObject["templated"] = JsonConst::TRUE;
        }
    }

    void JsonResource::handleRequest(int &socket, const HttpMessage& request)
    {
        std::cout << "handleRequest, path: " << request.path << ", remaining path:" << request.remainingPath << "\n";
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
            JsonWriter jsonWriter(&json);
            std::unique_lock<std::shared_timed_mutex> lock(responseMutex);
            response.used = sprintf(response.data, responseTemplate, jsonWriter.buffer.used);
            response.ensureCapacity(jsonWriter.buffer.used + 1);
            strcpy(response.data + response.used, jsonWriter.buffer.data);
            response.used += jsonWriter.buffer.used;
            responseIsStale = false;
        }
    }
    void JsonResource::setResponseStale() 
    {
        responseIsStale = true;
    }

}
