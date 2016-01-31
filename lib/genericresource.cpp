#include <unistd.h>
#include <sys/socket.h>
#include "errorhandling.h"
#include "genericresource.h"

using namespace std;

namespace org_restfulipc
{

    GenericResource::GenericResource(const char* doc, NotifierResource* notifierResource) :
        AbstractResource(),
        notifierResource(notifierResource),
        _response(),
        _respPtr(_response),
        _responseLength(0),
        responseMutex()
    {
        update(doc);
    }

    GenericResource::~GenericResource()
    {
    }

    void GenericResource::handleRequest(int &socket, const HttpMessage& request)
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

    void GenericResource::doGet(int socket, const HttpMessage& request)
    {
        std::shared_lock<std::shared_timed_mutex> lock(responseMutex);
        int bytesWritten = 0;
        do
        {
            int nbytes = send(socket, _response + bytesWritten, _responseLength - bytesWritten, MSG_NOSIGNAL);
            if (nbytes < 0) throw C_Error();
            bytesWritten += nbytes;
        }
        while (bytesWritten < _responseLength);
    }


    void GenericResource::doPatch(int socket, const HttpMessage& request)
    {
        throw Status::Http405; // FIXME
    }


    void GenericResource::update(const char* data)
    {
        static const char* responseTemplate =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json; charset=UTF-8\r\n"
            "Content-Length: %d\r\n"
            "\r\n"
            "%s";

        int contentLength = strlen(data);
        
        {
            std::unique_lock<std::shared_timed_mutex> lock(responseMutex);
            _responseLength = sprintf(_response, responseTemplate, contentLength, data);
        }

        if (notifierResource) {
            notifierResource->notifyClients(NotifierResource::Event::Updated, path);
        }
    }
}
