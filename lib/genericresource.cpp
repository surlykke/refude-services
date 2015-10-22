#include <unistd.h>
#include <sys/socket.h>

#include "errorhandling.h"
#include "genericresource.h"

using namespace std;

namespace org_restfulipc
{

    GenericResource::GenericResource(const char* json) : 
        AbstractResource(),
        _response(""),
        _respPtr(_response),
        _responseLength(0),
        responseMutex(),
        _webSockets(),
        websocketsMutex()
    {
        update(json);
    }

    GenericResource::~GenericResource()
    {
    }

    void GenericResource::handleRequest(int &socket, const HttpMessage& request)
    {
        if (request.method == Method::GET)    
        {
            if (request.headerValue(Header::connection) != 0 &&
                strcasecmp(request.headerValue(Header::connection), "upgrade") == 0 &&
                request.headerValue(Header::upgrade) != 0 &&
                strcasecmp(request.headerValue(Header::upgrade), "socketstream") == 0)
            {
                doStreamUpgrade(socket, request);
                socket = -1;
            }
            else 
            {
                doGet(socket, request);    
            }
        }
        else if (request.method == Method::PATCH)
        {
            doPatch(socket, request);
        }
        else
        {
            throw Status::Http406;
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

    void GenericResource::doStreamUpgrade(int socket, const HttpMessage& request)
    {

        const char streamUpgradeResponse[] = 
            "HTTP/1.1 101 Switching Protocols\r\n"
            "Upgrade: socketstream\r\n"
            "Connection: Upgrade\r\n"
            "\r\n";

        writeData(socket, streamUpgradeResponse, sizeof(streamUpgradeResponse));

        {
            std::unique_lock<std::mutex> lock(websocketsMutex);
            _webSockets.push_back(socket);
        }
    }

    void GenericResource::doPatch(int socket, const HttpMessage& request)
    {
        throw Status::Http406;
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
            sprintf(_response, responseTemplate, contentLength, data);
            _responseLength = strlen(_response);
        }

        notifyClients();
    }

    void GenericResource::notifyClients()
    {

        std::unique_lock<std::mutex> lock(websocketsMutex);
        std::vector<int>::iterator it = _webSockets.begin();
        
        //    AFAIBATG send to a socket will only block - or in the case of a nonblocking, 
        //    return EAGAIN - if there's no room in the kernel buffer.
        //    In that case there are waiting 'u'`s for the client to read, so no nead to send more
            
        while (it != _webSockets.end()) {
            int res = send(*it, "u", 1, MSG_NOSIGNAL);
            int errorNumber = errno;
            if ( res < 0 && errorNumber != EAGAIN) {
                while (close(*it) < 0 && errno == EINTR);
                it = _webSockets.erase(it);
            }
            else {
                it++;
            }
        }
    }


    void GenericResource::writeData(int socket, const char* data, int nBytes)
    {
        int n = 0;
        
        do 
        {    
            int m = send(socket, data + n, nBytes - n, MSG_NOSIGNAL);
            if (m < 0) {
                throw errno;
            }
            n += m;
        }
        while (n < nBytes);
    }

}
